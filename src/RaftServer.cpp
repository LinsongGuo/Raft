#include "RaftServer.h"
#include <chrono>
#include <thread>

namespace Raft {
  RaftServer::RaftServer(const std::string &fileName) : 
    cluster(std::make_shared<RaftServerCluster>(fileName))
  {
    fout0.open(cluster->address + "/start");
    fout1.open(cluster->address + "/respond-put");
    fout2.open(cluster->address + "/respond-get");
    fout3.open(cluster->address + "/respond-requestvote");
    fout4.open(cluster->address + "/respond-heartbeat");
    fout5.open(cluster->address + "/respond-appendentries");
    fout6.open(cluster->address + "/respond-transform");

    //build transformer
    info = std::make_shared<RaftServerInfo>(cluster->size);
    transformer = std::make_shared<Transformer>();
    transformer->bindTransform(std::bind(&RaftServer::transform, 
      this, 
      std::placeholders::_1,
      std::placeholders::_2,
      std::placeholders::_3)
    );

    //build server.  
    externalServer = std::make_shared<External::RaftExternalServer>();
    externalServer->bindPut(std::bind(&RaftServer::put, this, std::placeholders::_1, std::placeholders::_2));
    externalServer->bindGet(std::bind(&RaftServer::get, this, std::placeholders::_1));
    externalServer->start(cluster->address, cluster->externalId);
  
    rpcServer = std::make_shared<Rpc::RaftRpcServer>();
    rpcServer->bindRespondRequestVote(std::bind(&RaftServer::respondRequestVote, this, std::placeholders::_1));
    rpcServer->bindRespondHeartbeat(std::bind(&RaftServer::respondHeartbeat, this, std::placeholders::_1));
    rpcServer->bindRespondAppendEntries(std::bind(&RaftServer::respondAppendEntries, this, std::placeholders::_1));
    rpcServer->start(cluster->address, cluster->localId);
    
    queueThread = boost::thread(std::bind(&RaftServer::executeTask, this));
   
    //build client.
    std::vector<std::shared_ptr<grpc::Channel> > channels;
    for(int i = 0; i < cluster->serverList.size(); ++i) {
      channels.emplace_back(grpc::CreateChannel(cluster->serverList[i], grpc::InsecureChannelCredentials()));
    }
    rpcClient = std::make_shared<Rpc::RaftRpcClient>(channels, cluster->broadcastTimeout, cluster->localServer, cluster->address);

    //create roles.
    roles[RaftServerRole::follower] = std::make_unique<Follower>(info, cluster, rpcClient, transformer, logScanner);
    roles[RaftServerRole::candidate] = std::make_unique<Candidate>(info, cluster, rpcClient, transformer, logScanner);
    roles[RaftServerRole::leader] = std::make_unique<Leader>(info, cluster, rpcClient, transformer, logScanner);

    roles[RaftServerRole::follower]->fout.open(cluster->address + "/follower");
    roles[RaftServerRole::candidate]->fout.open(cluster->address + "/candidate");
    roles[RaftServerRole::leader]->fout.open(cluster->address + "/leader");
    fout0 << getTime() << " construct end." << std::endl;    
  }

  bool RaftServer::put(const std::string &key, const std::string &args) {
    fout1 << getTime() << " push put " << key <<' '<< args << std::endl;
    boost::promise<bool> prm;
    boost::future<bool> fut = prm.get_future();
    {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      taskQueue.push(TaskType::put);
      putQueue.push(PutTask(key, args, prm));
    }
    queueCond.notify_one();
    return fut.get();
  }

  std::pair<bool, std::string> RaftServer::get(const std::string &key) {
    fout2 << getTime() << " push get " << key << std::endl;
    boost::promise<std::pair<bool, std::string> > prm;
    boost::future<std::pair<bool, std::string> > fut = prm.get_future();
    {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      taskQueue.push(TaskType::get);
      getQueue.push(GetTask(key, prm));
    }
    queueCond.notify_one();
    return fut.get();
  }

  RequestVoteReply RaftServer::respondRequestVote(const RequestVoteRequest &request) {
    fout3 << getTime() << " push respondRequestVote " << request.candidateId <<' ' <<request.term << ' ' 
    << request.lastLogTerm << ' ' << request.lastLogIndex << std::endl;
    boost::promise<RequestVoteReply> prm;
    boost::future<RequestVoteReply> fut = prm.get_future();
    {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      taskQueue.push(TaskType::respondRequestVote);
      respondRequestVoteQueue.push(RespondRequestVoteTask(request, prm));  
    }
    queueCond.notify_one();
    return fut.get();
  }

  AppendEntriesReply RaftServer::respondHeartbeat(const AppendEntriesRequest &request) {
    fout4 << getTime() << " push respondHeartbeat " << request.leaderId <<' ' <<request.term << ' ' 
    << request.prevLogTerm << ' ' << request.prevLogIndex << ' ' << request.leaderCommit << std::endl;
    boost::promise<AppendEntriesReply> prm;
    boost::future<AppendEntriesReply> fut = prm.get_future();
    {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      taskQueue.push(TaskType::respondHeartbeat);
      respondHeartbeatQueue.push(RespondHeartbeatTask(request, prm));  
    }
    queueCond.notify_one();
    return fut.get();
  }

  AppendEntriesReply RaftServer::respondAppendEntries(const Raft::Rpc::RpcAppendEntriesRequest *request) {
    fout5 << getTime() << " push respondAppendEntries " << request->leaderid() <<' ' <<request->term() << ' ' << 
    request->prevlogterm() << ' ' << request->prevlogindex() << ' ' << request->leadercommit() << std::endl;
    size_t siz = request->entries().size();
    fout5 << "entries:" << ' ' << siz << std::endl;
    /*if(siz > 0) { 
        for(int i = siz - 1; i >= 0; --i) {
          auto tmp = request->entries()[i];
          info->replicatedEntries.push_back(ReplicatedEntry(tmp.key(), tmp.args(), tmp.term()));
        }
    }*/

    boost::promise<AppendEntriesReply> prm;
    boost::future<AppendEntriesReply> fut = prm.get_future();
    {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      taskQueue.push(TaskType::respondAppendEntries);
      respondAppendEntriesQueue.push(RespondAppendEntriesTask(request, prm));  
    }
    queueCond.notify_one();
    return fut.get();
  }

  void RaftServer::transform(const RaftServerRole &fromRole, const RaftServerRole &toRole, const Term &term) {
    fout6 << getTime() <<" push transform "<< fromRole << ' ' << toRole << ' ' << term << std::endl;
    {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      taskQueue.push(TaskType::transform);
      transformQueue.push(TransformTask(fromRole, toRole, term));  
    }
    queueCond.notify_one();
  }

  void RaftServer::executeTask() {
    while(true) {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      queueCond.wait(lk, [this]{return !taskQueue.empty(); });
      while (!taskQueue.empty()) {
        switch(taskQueue.front()) {
          case TaskType::put : {
            auto tmp = putQueue.front();
            taskQueue.pop();
            putQueue.pop();
            lk.unlock();

            fout1 << getTime() << " pop put " << tmp.key << ' ' << tmp.args << std::endl;
            
            auto result = roles[currentRole]->put(tmp.key, tmp.args);

            fout1 << getTime() << " result " << result << std::endl;

            tmp.prm.set_value(result);
            
            break;
          }
          case TaskType::get : {
            auto tmp = getQueue.front();
            taskQueue.pop();
            getQueue.pop();
            lk.unlock();
            
            fout2 << getTime() << " pop get " << tmp.key << ' ' << std::endl;
            
            auto result = roles[currentRole]->get(tmp.key);
            
            fout2 << getTime() << " result " << result.first <<' ' << result.second << std::endl;
          
            tmp.prm.set_value(result);
            break;
          }
          case TaskType::respondRequestVote : {
            auto tmp = respondRequestVoteQueue.front();
            taskQueue.pop();
            respondRequestVoteQueue.pop();
            lk.unlock();
            
            fout3 << getTime() << " pop requestvote " << tmp.request.candidateId  << ' ' << tmp.request.term << ' ' 
            << tmp.request.lastLogTerm << ' ' << tmp.request.lastLogIndex << std::endl;

            auto result = roles[currentRole]->respondRequestVote(tmp.request);
            
            fout3 << getTime() << " result " << result.voteGranted << ' ' << result.term << std::endl;  

            tmp.prm.set_value(result);
            break;
          }
          case TaskType::respondHeartbeat : {
            auto tmp = respondHeartbeatQueue.front();
            taskQueue.pop();
            respondHeartbeatQueue.pop();
            lk.unlock();
            
            fout4 << getTime() << " pop heartbeat " << tmp.request.leaderId  << ' ' << tmp.request.term << ' ' 
            << tmp.request.prevLogTerm << ' ' << tmp.request.prevLogIndex << ' ' << tmp.request.leaderCommit << std::endl;
            
            auto result = roles[currentRole]->respondHeartbeat(tmp.request);
            
            fout4 << getTime() << " result " << result.success << ' ' << result.term << std::endl;  
            
            tmp.prm.set_value(result);
            break;
          }
          case TaskType::respondAppendEntries : {
            auto tmp = respondAppendEntriesQueue.front();
            taskQueue.pop();
            respondAppendEntriesQueue.pop();
            lk.unlock();
            
            fout5 << getTime() << " pop appendentries " << ' ' << tmp.request->leaderid() << ' ' << tmp.request->term() << ' '
            << tmp.request->prevlogterm() << ' ' << tmp.request->prevlogindex() << ' ' << tmp.request->leadercommit() << std::endl;
            
            auto result = roles[currentRole]->respondAppendEntries(tmp.request);
            
            fout5 << getTime() << " result " << ' ' <<result.success <<' '<< result.term << std::endl;  
            
            tmp.prm.set_value(result);
            break;
          }
          case TaskType::transform : {
            auto tmp = transformQueue.front();
            taskQueue.pop();
            transformQueue.pop();            
            lk.unlock();
              
            fout6 << getTime() << ' ' << "pop transform " << tmp.fromRole <<  ' '  << tmp.toRole << ' ' << tmp.term << std::endl;
           
            if(currentRole == tmp.fromRole) {
              currentRole = tmp.toRole;
              roles[currentRole]->init(tmp.term);  
            }
          
            fout6 << getTime() << " init." << std::endl;

            break;
          }  
        } 
        lk.lock();
      }     
    }
  }
  void RaftServer::start() {
    /*std::cout << "The RaftServer "  << cluster->localId << " havs been built." << std::endl;  
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm *ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm,"%X") << '\n';
    std::cout << "Waiting for the next minute to start all raftservers...\n";
    ++ptm->tm_min; ptm->tm_sec=0;
    std::this_thread::sleep_until (std::chrono::system_clock::from_time_t (mktime(ptm)));
    std::cout << std::put_time(ptm,"%X") << " reached!\n";*/
    
    logScanner.open("log/" + cluster->address, std::ios::in | std::ios::out);

    fout0 << getTime() << " The RaftServer " << cluster->localId << " starts." << std::endl;
    roles[RaftServerRole::follower]->init(1);
    fout0 << getTime() << " end init" << std::endl;
  }
  void RaftServer::restart() {
    logScanner.open("log/" + cluster->address, std::ios::in | std::ios::out);
    Term term = info->readLog(logScanner);
    roles[RaftServerRole::follower]->init(term);
  }
  void RaftServer::shutdown() {
    rpcServer->shutdown();
    externalServer->shutdown();
    queueThread.interrupt();
    queueThread.join();

    logScanner.close();

    fout0.close();
    fout1.close();
    fout2.close();
    fout3.close();
    fout4.close();
    fout5.close();
    fout6.close();
  }
}



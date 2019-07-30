#include "RaftServer.h"
#include <chrono>
#include <thread>

namespace Raft {
  RaftServer::RaftServer(const std::string &fileName) : 
    cluster(std::make_shared<RaftServerCluster>(fileName)),
    info(std::make_shared<RaftServerInfo>())
  {
    //build transformer
    transformer = std::make_shared<Transformer>();
    transformer->bindTransform(std::bind(&RaftServer::transform, 
      this, 
      std::placeholders::_1,
      std::placeholders::_2,
      std::placeholders::_3)
    );

    //build server.
    rpcServer = std::make_shared<Rpc::RaftRpcServer>();
    rpcServer->bindRespondRequestVote(std::bind(&RaftServer::respondRequestVote, this, std::placeholders::_1));
    rpcServer->bindRespondAppendEntries(std::bind(&RaftServer::respondAppendEntries, this, std::placeholders::_1));
    rpcServer->start(cluster->localId);
    
    queueThread = boost::thread(std::bind(&RaftServer::executeTask, this));
   
    //build client.
    std::vector<std::shared_ptr<grpc::Channel> > channels;
    for(int i = 0; i < cluster->serverList.size(); ++i) {
      channels.emplace_back(grpc::CreateChannel(cluster->serverList[i], grpc::InsecureChannelCredentials()));
    }
    rpcClient = std::make_shared<Rpc::RaftRpcClient>(channels, cluster->broadcastTimeout);

    //create roles.
    roles[RaftServerRole::follower] = std::make_unique<Follower>(info, cluster, rpcClient, transformer);
    roles[RaftServerRole::candidate] = std::make_unique<Candidate>(info, cluster, rpcClient, transformer);
    roles[RaftServerRole::leader] = std::make_unique<Leader>(info, cluster, rpcClient, transformer);
  }

  bool RaftServer::put(const std::string &key, const std::string &args) {
    std::cout << getTime() << " push put " << key <<' '<< args << std::endl;
    //boo
  }

  std::pair<bool, std::string> RaftServer::get(const std::string &key) {
    std::cout << getTime() << " push get " << key << std::endl;
  }

  RequestVoteReply RaftServer::respondRequestVote(const RequestVoteRequest &request) {
    std::cout << getTime() << " push respondRequestVote " << request.candidateId <<' ' <<request.term << std::endl;
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

  AppendEntriesReply RaftServer::respondAppendEntries(const AppendEntriesRequest &request) {
    std::cout << getTime() << " push respondAppendEntries " << request.leaderId <<' ' <<request.term << std::endl;
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

  void RaftServer::transform(RaftServerRole fromRole, RaftServerRole toRole, Term term) {
    std::cout <<getTime() <<" push transform "<<fromRole <<' '<< toRole << ' ' << term << std::endl;
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
          case TaskType::respondRequestVote: {
            auto tmp = respondRequestVoteQueue.front();
            auto result = roles[currentRole]->respondRequestVote(tmp.request);
            
            std::ofstream fout("votequeue-" + cluster->localId + "-" + tmp.request.candidateId + "-" + std::to_string(tmp.request.term));

            tmp.prm.set_value(result);
            respondRequestVoteQueue.pop();

            fout << getTime() << " pop respondRequestVote " <<info->currentTerm <<' '<<tmp.request.candidateId <<' ' << tmp.request.term <<' '<<result.term <<' '<< result.voteGranted << std::endl;  
            fout.close();
            
            break;
          }
          case TaskType::respondAppendEntries: {
            auto tmp = respondAppendEntriesQueue.front();
            auto result = roles[currentRole]->respondAppendEntries(tmp.request);
            
            std::ofstream fout("appendqueue-" + cluster->localId + "-" + tmp.request.leaderId + "-" + std::to_string(tmp.request.term));
            
            tmp.prm.set_value(result);
            respondAppendEntriesQueue.pop();
            
            fout << getTime() << " pop respondAppendEntries " <<info->currentTerm <<' '<<tmp.request.leaderId <<' ' << tmp.request.term <<' '<<result.term <<' '<< result.success << std::endl;  
            fout.close();
            
            break;
          }
          case TaskType::transform : {
            auto tmp = transformQueue.front();
            currentRole = tmp.toRole;
            
            std::ofstream fout("transqueue-" + cluster->localId + "-" + std::to_string(tmp.fromRole) + "-" + std::to_string(tmp.toRole) + "-" + std::to_string(tmp.term));
            fout<<getTime() <<" pop" << std::endl;
            
            transformQueue.pop();            
            roles[currentRole]->init(tmp.term);
          
            fout<<getTime() <<" break"<<std::endl;
            fout.close();
            
            break;
          }  
        } 
        taskQueue.pop();
      }     
    }
  }
  void RaftServer::start() {
    //prepare
    std::cout << "The local Address : " << cluster->localId << std::endl;
    std::cout <<"rpcServer and rpcClient have been built." << std::endl;  
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm *ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm,"%X") << '\n';
    std::cout << "Waiting for the next minute to start all raftservers...\n";
    ++ptm->tm_min; ptm->tm_sec=0;
    std::this_thread::sleep_until (std::chrono::system_clock::from_time_t (mktime(ptm)));
    std::cout << std::put_time(ptm,"%X") << " reached!\n";
    
    std::cout<<"start time "<<getTime() << std::endl;
    roles[currentRole = RaftServerRole::follower]->init(1);
  }
  void RaftServer::shutdown() {
    rpcServer->shutdown();
    queueThread.interrupt();
    queueThread.join();
  }
}



#include "RaftServer.h"
#include <chrono>
#include <thread>

namespace Raft {
  RaftServer::RaftServer(const std::string &fileName) : 
    cluster(std::make_shared<RaftServerCluster>(RaftServerCluster(fileName))),
    info(std::make_shared<RaftServerInfo>(RaftServerInfo(1))) 
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
    rpcServer->bindRespondAppendEntries(std::bind(&RaftServer::bindRespondAppendEntries, this, std::placeholders::_1));
    rpcServer->start(cluster->localId);
    
    queueThread = boost::thread(std::bind(&RaftServer::executeTask, this));
   
    //build client.
    std::vector<std::shared_ptr<grpc::Channel> > channels;
    for(int i = 0; i < cluster->serverList.size(); ++i) {
      channels.emplace_back(grpc::CreateChannel(cluster->serverList[i], grpc::InsecureChannelCredentials()));
    }
    rpcClient = std::make_shared<Rpc::RaftRpcClient>(channels);

    //create roles.
    currentRole = RaftServerRole::follower;
    roles[RaftServerRole::follower] = std::make_unique<Follower>(info, cluster, rpcClient, transformer);
    roles[RaftServerRole::candidate] = std::make_unique<Candidate>(info, cluster, rpcClient, transformer);
    roles[RaftServerRole::leader] = std::make_unique<Leader>(info, cluster, rpcClient, transformer);
  }
  /*
  void RaftServer::RequestVotes() {
    Rpc::RpcRequestVoteRequest request;
    Term x = rand() % 65536;
    request.set_term(x);
    std::cout << "The local is going to send requests " << x << " to all servers..." << std::endl;
    rpcClient->RpcRequestVotes(request);
  }*/

  RequestVoteReply RaftServer::respondRequestVote(RequestVoteRequest request) {
    std::cout << getTime() << " push respondRequestVote " << request.candidateId <<' ' <<request.term << std::endl;
    boost::promise<RequestVoteReply> prm;
    boost::future<RequestVoteReply> fut = prm.get_future();
    boost::unique_lock<boost::mutex> lk(queueMutex);
    taskQueue.push(TaskType::respondRequestVote);
    respondRequestVoteQueue.push(RespondRequestVoteTask(request, prm));
    lk.unlock();
    queueCond.notify_one();
    return fut.get();
  }

  AppendEntriesReply RaftServer::bindRespondAppendEntries(AppendEntriesRequest request) {
    std::cout << getTime() << " push respondAppendEntries " << request.leaderId <<' ' <<request.term << std::endl;
    boost::promise<AppendEntriesReply> prm;
    boost::futrue<AppendEntriesReply> fut = prm.get_future();
    boost::unique_lock<boost::mutex> lk(queueMutex);
    taskQueue.push(TaskType::respondAppendEntries);
    respondAppendEntriesQueue.push(RespondAppendEntriesTask(request, prm));
    lk.unlock();
    queueCond.notify_one();
    return fut.get();
  }

  void RaftServer::transform(RaftServerRole fromRole, RaftServerRole toRole, Term term) {
    std::cout <<getTime() <<" push transform "<<fromRole <<' '<< toRole << ' ' << term << std::endl;
    boost::unique_lock<boost::mutex> lk(queueMutex);
    taskQueue.push(TaskType::transform);
    transformQueue.push(TransformTask(fromRole, toRole, term)); 
    lk.unlock(); 
    queueCond.notify_one();
  }

  void RaftServer::executeTask() {
    std::ofstream fout(cluster->localId + "-queue");
    while(true) {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      queueCond.wait(lk, [this]{return !taskQueue.empty(); });
      while (!taskQueue.empty()) {
        switch(taskQueue.front()) {
          case TaskType::respondRequestVote: {
            auto tmp = respondRequestVoteQueue.front();
            auto result = roles[currentRole]->respondRequestVote(tmp.request);
            fout << getTime() << " pop respondRequestVote " <<currentRole <<' '<<tmp.request.candidateId <<' ' << tmp.request.term <<' '<<result.term <<' '<< result.voteGranted << std::endl;  
            tmp.prm.set_value(result);
            respondRequestVoteQueue.pop();
            break;
          }
          case TaskType::respondAppendEntries: {
            auto tmp = respondAppendEntriesQueue.front();
            auto result = roles[currentRole]->respondAppendEntries(tmp.request);
            fout << getTime() << " pop respondAppendEntries " <<currentRole <<' '<<tmp.request.leaderId <<' ' << tmp.request.term <<' '<<result.term <<' '<< result.success << std::endl;  
            tmp.prm.set_value(result);
            respondAppendEntriesQueue.pop();
          }
          case TaskType::transform : {
            auto tmp = transformQueue.front();
            currentRole = tmp.toRole;
            info->currentTerm = tmp.term;
            fout << getTime() << " pop transform " <<tmp.fromRole <<' '<< tmp.toRole << ' ' << tmp.term << std::endl;  
            roles[currentRole]->init();
            transformQueue.pop();
            break;
          }  
        } 
        taskQueue.pop();
      }
    }
    fout.close();
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
    roles[currentRole]->init();
  }
  void RaftServer::shutdown() {
    rpcServer->shutdown();
    queueThread.interrupt();
    queueThread.join();
  }
}



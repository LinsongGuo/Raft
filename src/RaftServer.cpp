#include "RaftServer.h"
#include <chrono>
#include <thread>

namespace Raft {
  RaftServer::RaftServer(const std::string &fileName) : cluster(fileName), info(1) {
    roles[RaftServerRole::Follower] = std::make_unique<Role>(Role(
      std::make_shared<RaftServerInfo>(info),
      std::make_shared<RaftServerCluster>(cluster), 
      rpcClient));
    roles[RaftServerRole::Candidate] = std::make_unique<Role>(Role(
      std::make_shared<RaftServerInfo>(info),
      std::make_shared<RaftServerCluster>(cluster), 
      rpcClient));
    roles[RaftServerRole::Leader] = std::make_unique<Role>(Role(
      std::make_shared<RaftServerInfo>(info),
      std::make_shared<RaftServerCluster>(cluster), 
      rpcClient));
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
    boost::promise<RequestVoteReply> prm;
    boost::future<RequestVoteReply> fut = prm.get_future();
    boost::unique_lock<boost::mutex> lk(queueMutex);
    taskQueue.push(TaskType::respondRequestVote);
    respondRequestVoteQueue.push(RespondRequestVoteTask(request, prm));
    queueCond.notify_one();
    return fut.get();
  }
  void RaftServer::executeTask() {
    while(true) {
      boost::unique_lock<boost::mutex> lk(queueMutex);
      queueCond.wait(lk, [this]{return !taskQueue.empty(); });
      while (!taskQueue.empty()) {
        switch(taskQueue.front()) {
          case TaskType::respondRequestVote: {
            auto tmp = respondRequestVoteQueue.front();
            tmp.prm.set_value(roles[currentRole]->respondRequestVote(tmp.request));
            respondRequestVoteQueue.pop();
            break;
          }  
        } 
        taskQueue.pop();
      }
    }
  }
  void RaftServer::start() {
    srand(time(NULL));
    currentRole = RaftServerRole::Follower;
    rpcServer = std::make_unique<Rpc::RaftRpcServer>();
    rpcServer->bindRespondRequestVote(std::bind(&RaftServer::respondRequestVote, this, std::placeholders::_1));
    std::cout << "The local Address : " << cluster.localId << std::endl;
    rpcServer->start(cluster.localId);
    queueThread = boost::thread(std::bind(&RaftServer::executeTask, this));
    // &RaftServer::executeTask ???

    std::cout <<"rpcServer has been built, rpcClient is going to be built..." << std::endl;  
    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm *ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm,"%X") << '\n';
    std::cout << "Waiting for the next minute to begin...\n";
    ++ptm->tm_min; ptm->tm_sec=0;
    std::this_thread::sleep_until (std::chrono::system_clock::from_time_t (mktime(ptm)));
    std::cout << std::put_time(ptm,"%X") << " reached!\n";


    std::vector<std::shared_ptr<grpc::Channel> > channels;
    for(int i = 0; i < cluster.serverList.size(); ++i) {
      channels.emplace_back(grpc::CreateChannel(cluster.serverList[i], grpc::InsecureChannelCredentials()));
    }
    rpcClient = std::make_unique<Rpc::RaftRpcClient>(channels);
  }
  void RaftServer::shutdown() {
    rpcServer->shutdown();
    queueThread.interrupt();
    queueThread.join();
  }
}



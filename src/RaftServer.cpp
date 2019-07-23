#include <boost/thread/thread.hpp>
#include <chrono>
#include <thread>
#include "RaftServer.h"

namespace Raft {
  RaftServer::RaftServer(const std::string &fileName) : clusters(fileName) {}
  void RaftServer::start() {
    rpcServer = std::make_unique<Rpc::RaftRpcServer>();
    std::cout << "The local Address : " << clusters.localId << std::endl;
    rpcServer->start(clusters.localId);

    //boost::this_thread::sleep_for(boost::chrono::milliseconds(10000));
    std::cout <<"rpcServer has been built, rpcClient is going to be built..." << std::endl;  

    std::time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct std::tm *ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm,"%X") << '\n';
    std::cout << "Waiting for the next minute to begin...\n";
    ++ptm->tm_min; ptm->tm_sec=0;
    std::this_thread::sleep_until (std::chrono::system_clock::from_time_t (mktime(ptm)));
    std::cout << std::put_time(ptm,"%X") << " reached!\n";

    std::vector<std::shared_ptr<grpc::Channel> > channels;
    for(int i = 0; i < clusters.serverList.size(); ++i) {
      channels.emplace_back(grpc::CreateChannel(clusters.serverList[i], grpc::InsecureChannelCredentials()));
    }
    rpcClient = std::make_unique<Rpc::RaftRpcClient>(channels);
  }
  void RaftServer::RequestVotes() {
    Rpc::RpcRequestVoteRequest request;
    Term x = rand() % 65536;
    request.set_term(x);
    std::cout << "The local is going to send requests " << x << " to all servers..." << std::endl;
    rpcClient->RpcRequestVotes(request);
  }
}



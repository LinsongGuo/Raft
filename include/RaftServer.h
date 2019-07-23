#ifndef RAFT_RAFTSERVER_H
#define RAFT_RAFTSERVER_H

#include <cstdio>
#include <iostream>
#include "defines.h"
#include "RaftRpc.pb.h"
#include "RaftRpc.grpc.pb.h"
#include "RaftRpcServer.h"
#include "RaftRpcClient.h"

namespace Raft {
  class RaftServer {
  private:
    RaftServerInfo info;
    RaftServerClusters clusters;
    std::unique_ptr<Rpc::RaftRpcServer> rpcServer;
    std::unique_ptr<Rpc::RaftRpcClient> rpcClient;
  public:
    RaftServer(const std::string &fileName);
    void start();
    void RequestVotes();
  };
}
#endif

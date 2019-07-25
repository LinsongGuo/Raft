#ifndef RAFT_ROLE_ROLE_H
#define RAFT_ROLE_ROLE_H

#include <cstdio>
#include <iostream>
#include "defines.h"
#include "RaftRpcClient.h"
#include "RaftRpcServer.h"
#include "RaftRpc.pb.h"
#include "RaftRpc.grpc.pb.h"

namespace Raft {
  class Role {
  protected:
    std::shared_ptr<RaftServerInfo> info;
    std::shared_ptr<RaftServerCluster> cluster;
    std::shared_ptr<Rpc::RaftRpcClient> rpcClient;
  public:
    Role(std::shared_ptr<RaftServerInfo> _info, 
      std::shared_ptr<RaftServerCluster>_cluster, 
      std::shared_ptr<Rpc::RaftRpcClient> _rpcClient);
    virtual void init();
    virtual RequestVoteReply respondRequestVote(const RequestVoteRequest &request);
  };
} 

#endif

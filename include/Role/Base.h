#ifndef RAFT_ROLE_BASE_H
#define RAFT_ROLE_BASE_H

#include <cstdio>
#include <iostream>
#include "defines.h"
#include "RaftRpcClient.h"

namespace Raft {
  class Base {
  protected:
    std::shared_ptr<RaftServerInfo> info;
    std::shared_ptr<RaftServerCluster> cluster;
    std::shared_ptr<Rpc::RaftRpcClient> rpcClient;
  public:
    Base(const RaftServerInfo &_info, const RaftServerCluster &_cluster, const Rpc::RaftRpcClient &_rpcClient);
    virtual void init() = 0;
    virtual RequestVoteReply respondRequestVote(const RequestVoteRequest &request) = 0;
  };
} 

#endif

#ifndef RAFT_ROLE_ROLE_H
#define RAFT_ROLE_ROLE_H

#include <cstdio>
#include <iostream>
#include "defines.h"
#include "Transformer.h"
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
    std::shared_ptr<Transformer> transformer;
  public:
    Role(std::shared_ptr<RaftServerInfo> _info, 
      std::shared_ptr<RaftServerCluster>_cluster, 
      std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
      std::shared_ptr<Transformer> _transformer);
    virtual void init(Term currentTerm) = 0;
    virtual RequestVoteReply respondRequestVote(const RequestVoteRequest &request) = 0;
    virtual AppendEntriesReply respondAppendEntries(const AppendEntriesRequest &request) = 0;
  };
} 

#endif

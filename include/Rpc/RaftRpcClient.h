#ifndef RAFT_RPC_RAFTRPCCLIENT_H
#define RAFT_RPC_RAFTRPCCLIENT_H

#include <grpc++/grpc++.h>
#include "RaftRpc.grpc.pb.h"
#include "RaftRpc.pb.h"
#include "defines.h"

namespace Raft {
  namespace Rpc {
    class RaftRpcClient {
    private:
      std::vector<std::unique_ptr<RaftRpc::Stub> > stubs;
    public:
      RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels);
      bool RequestVote(size_t id, const RequestVoteRequest &request, RequestVoteReply &reply);
      void RequestVotes(const RequestVoteRequest &request);
    };
  }
}

#endif
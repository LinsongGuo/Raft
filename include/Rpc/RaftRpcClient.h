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
      std::ofstream fout;
      size_t size;
      std::vector<boost::future<std::pair<bool, RequestVoteReply> > > sendFuture;
      std::vector<std::unique_ptr<RaftRpc::Stub> > stubs;
    public:
      RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels);
      std::pair<bool, RequestVoteReply> sendRequestVote(size_t id, const RequestVoteRequest &request);
      std::pair<RaftServerRole, Term> sendRequestVotes(size_t localServer, const RequestVoteRequest &request);
    };
  }
}

#endif
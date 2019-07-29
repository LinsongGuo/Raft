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
      const Timer broadcastTimeout;
      std::vector<boost::future<std::pair<bool, RequestVoteReply> > > voteFuture;
      std::vector<boost::future<std::pair<bool, AppendEntriesReply> > > appendFuture;
      std::vector<std::unique_ptr<RaftRpc::Stub> > stubs;
    public:
      RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels, Timer timeout);
      std::pair<bool, RequestVoteReply> sendRequestVote(size_t id, const RequestVoteRequest &request);
      std::pair<bool, AppendEntriesReply> sendAppendEntries(size_t id, const AppendEntriesRequest &request);
      std::pair<RaftServerRole, Term> sendRequestVotes(size_t localServer, const RequestVoteRequest &request);
      std::pair<RaftServerRole, Term> sendHeartbeats(size_t localServer, const AppendEntriesRequest &request);
    };
  }
}

#endif
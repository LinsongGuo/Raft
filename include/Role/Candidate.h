#ifndef RAFT_ROLE_CANDIDATE_H
#define RAFT_ROLE_CANDIDATE_H

#include "Role.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>

namespace Raft {
  class Candidate: public Role {
  public:
    Candidate(std::shared_ptr<RaftServerInfo> _info, 
      std::shared_ptr<RaftServerCluster>_cluster, 
      std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
      std::shared_ptr<Transformer> _transformer);
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}
#endif

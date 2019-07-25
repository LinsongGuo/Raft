#ifndef RAFT_ROLE_CANDIDATE_H
#define RAFT_ROLE_CANDIDATE_H

#include "Role.h"
#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>

namespace Raft {
  class Candidate: public Role {
  public:
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}
#endif

#ifndef RAFT_ROLE_CANDIDATE_H
#define RAFT_ROLE_CANDIDATE_H

#include "Base.h"

namespace Raft {
  class Candidate: public Base {
  public:
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}
#endif

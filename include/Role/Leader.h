#ifndef RAFT_ROLE_LEADER_H
#define RAFT_ROLE_LEADER_H

#include "Base.h"

namespace Raft {    
  class Leader: public Base {
  public:
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}

#endif

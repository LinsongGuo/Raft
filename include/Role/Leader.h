#ifndef RAFT_ROLE_LEADER_H
#define RAFT_ROLE_LEADER_H

#include "Role.h"

namespace Raft {    
  class Leader: public Role {
  public:
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}

#endif

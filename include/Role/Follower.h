#ifndef RAFT_ROLE_FOLLOWER_H
#define RAFT_ROLE_FOLLOWER_H

#include "Base.h"

namespace Raft {
  class Follower: public Base {
  public:
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}

#endif

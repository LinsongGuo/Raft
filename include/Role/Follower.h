#ifndef RAFT_ROLE_FOLLOWER_H
#define RAFT_ROLE_FOLLOWER_H

#include "Role.h"

namespace Raft {
  class Follower: public Role {
  private:
    boost::thread sleepThread;
  public:
    void init() override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
  };
}

#endif

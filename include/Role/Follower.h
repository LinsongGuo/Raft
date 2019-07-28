#ifndef RAFT_ROLE_FOLLOWER_H
#define RAFT_ROLE_FOLLOWER_H

#include "Role.h"

namespace Raft {
  class Follower: public Role {
  private:
    boost::thread sleepThread;
  public:
    Follower(std::shared_ptr<RaftServerInfo> _info, 
      std::shared_ptr<RaftServerCluster>_cluster, 
      std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
      std::shared_ptr<Transformer> _transformer);
    void init() override;
    bool checkMajorityEntries(const RequestVoteRequest &request);
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
    AppendEntriesReply respondAppendEntries(const AppendEntriesRequest &request) override;  
  };
}

#endif

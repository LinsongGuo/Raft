#ifndef RAFT_ROLE_LEADER_H
#define RAFT_ROLE_LEADER_H

#include "Role.h"

namespace Raft {    
  class Leader: public Role {
  private:
    boost::thread heartbeatThread;
  public:
    Leader(std::shared_ptr<RaftServerInfo> _info, 
      std::shared_ptr<RaftServerCluster>_cluster, 
      std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
      std::shared_ptr<Transformer> _transformer);
    void init(Term currentTerm) override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
    AppendEntriesReply respondAppendEntries(const AppendEntriesRequest &request) override;
  };
}

#endif

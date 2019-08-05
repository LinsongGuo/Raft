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
      std::shared_ptr<Transformer> _transformer,
      std::ofstream &_writeToLog);
    void init(Term currentTerm) override;
    bool put(const std::string &key, const std::string &args) override;
    std::pair<bool, std::string> get(const std::string &key) override;
    RequestVoteReply respondRequestVote(const RequestVoteRequest &request) override;
    AppendEntriesReply respondHeartbeat(const AppendEntriesRequest &request) override;
    AppendEntriesReply respondAppendEntries(const Raft::Rpc::RpcAppendEntriesRequest *request) override;
  };
}

#endif

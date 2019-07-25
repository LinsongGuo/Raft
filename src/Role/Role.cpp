#include "Role.h"

namespace Raft {
  Role::Role(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient):
    info(_info), cluster(_cluster), rpcClient(_rpcClient){;} 
  void Role::init() {;}
  RequestVoteReply Role::respondRequestVote(const RequestVoteRequest &request) {;}
}
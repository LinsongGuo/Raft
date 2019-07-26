#include "Leader.h"

namespace Raft {
  Leader::Leader(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 
   RequestVoteReply Leader::respondRequestVote(const RequestVoteRequest &request) {
    ;
   }
   void Leader::init() {
    std::cout << cluster->localId << " becomes a leader!---------------------------- " << std::endl;
    exit(0);
   }
}
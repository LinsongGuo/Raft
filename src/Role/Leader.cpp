#include "Leader.h"

namespace Raft {
  Leader::Leader(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 
   
  RequestVoteReply Leader::respondRequestVote(const RequestVoteRequest &request) {
    if(request.term > info->currentTerm) {
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, request.term);
    }
    return RequestVoteReply(false, info->currentTerm);
  }

  AppendEntriesReply Leader::respondAppendEntries(const AppendEntriesRequest &request) {
    if(request.term > info->currentTerm) {
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, requst.term);
    }
    return AppendEntriesReply(false, info->currentTerm);
  }

   void Leader::init() {
    std::cout << cluster->localId << " becomes a leader!---------------------------- " << std::endl;
        
   }
}
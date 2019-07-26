#include "Candidate.h"

namespace Raft {
  Candidate::Candidate(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 
  RequestVoteReply Candidate::respondRequestVote(const RequestVoteRequest &request) {
   if(request.term > info->currentTerm) {
    transformer->Transform(RaftServerRole::candidate, RaftServerRole::follower, request.term);
   }
   return RequestVoteReply(false, info->currentTerm);
  }
  void Candidate::init() {
    std::cout << cluster->localId << " becomes a candidate!---------------------------- " << std::endl;
    RequestVoteRequest request(cluster->localId, info->currentTerm, 0, 0);
    size_t siz = cluster->serverList.size();
    boost::future<std::pair<bool, RequestVoteReply> > sendFuture[siz];
    for(size_t i = 0; i < siz; ++i) {
      if(cluster->localId == cluster->serverList[i]) continue;
      sendFuture[i] = boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendRequestVote, rpcClient, i, request);
    }
    size_t getVotes = 1;
    for(size_t i = 0; i < siz; ++i) {
      if(cluster->localId == cluster->serverList[i]) continue;
      std::pair<bool, RequestVoteReply> reply = sendFuture[i].get();
      if(reply.first) {
        if(reply.second.voteGranted) {
          getVotes++;
          if(reply.second.term > info->currentTerm) {
            transformer->Transform(RaftServerRole::candidate, RaftServerRole::follower, reply.second.term);
            return;
          }
        }
      }
    }
    std::cout <<"get Votes: " << getVotes << std::endl;
    if(getVotes * 2 > siz) {
      std::cout << cluster->localId << " get " << getVotes <<" votes and becomes a leader!" << std::endl; 
      transformer->Transform(RaftServerRole::candidate, RaftServerRole::leader, info->currentTerm); 
    }
  }
}
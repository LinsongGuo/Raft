#include "Candidate.h"

namespace Raft {
  RequestVoteReply Candidate::respondRequestVote(const RequestVoteRequest &request) {
   ;
  }
  void Candidate::init() {
    RequestVoteRequest request(cluster->localId, info->currentTerm, 0, 0);
    size_t siz = cluster->serverList.size();
    boost::future<std::pair<bool, RequestVoteReply> > sendFuture[siz];
    for(size_t i = 0; i < siz; ++i) {
      sendFuture[i] = boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendRequestVote, rpcClient, i, request);
    }
    size_t getVotes = 0;
    for(size_t i = 0; i < siz; ++i) {
      std::pair<bool, RequestVoteReply> reply = sendFuture[i].get();
      if(reply.first) {
        if(reply.second.voteGranted) {
          getVotes++;
          if(reply.second.term > info->currentTerm){
            ;
          }
        }
      }
    }
    if(getVotes * 2 > siz) {
      std::cout << cluster->localId << " get " << getVotes <<" votes and becomes a leader!" << std::endl;  
    }
  }
}
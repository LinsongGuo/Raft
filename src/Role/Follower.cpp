#include "Follower.h"

namespace Raft {
  RequestVoteReply Follower::respondRequestVote(const RequestVoteRequest &request) {
   sleepThread.interrupt();
   if(request.term < info->currentTerm) return RequestVoteReply(false, info->currentTerm);
   if(request.term > info->currentTerm) {
    info->currentTerm = request.term;
    info->votedFor = request.candidateId; 
    return RequestVoteReply(true, info->currentTerm); 
   }
   if(info->votedFor == invalidServerId) {
    info->votedFor = request.candidateId;
    return RequestVoteReply(true, info->currentTerm);
   }
   return RequestVoteReply(false, info->currentTerm);
  }
  void Follower::init() {
    Timer waitTime = randTimer(info->electionTimeout);
    sleepThread = boost::thread([this, waitTime] {
      while(true) {
        try{
          boost::this_thread::sleep_for(boost::chrono::milliseconds(waitTime));
        }
        catch(boost::thread_interrupted &e) {
          continue;
        }          
      }
    });
  }
}
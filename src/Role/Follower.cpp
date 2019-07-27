#include "Follower.h"

namespace Raft {
  Follower::Follower(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;}

  RequestVoteReply Follower::respondRequestVote(const RequestVoteRequest &request) {
   // std::cout<<"Follower respondRequestVote " <<' ' << request.candidateId <<' '<< request.term << ' ' <<info->currentTerm<<std::endl;
   sleepThread.interrupt();
   if(request.term < info->currentTerm) {
    return RequestVoteReply(false, info->currentTerm);
   }
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
    std::cout << cluster->localId << " becomes a follower! ---------------------------- " << std::endl;
    sleepThread = boost::thread([this] {
      std::ofstream fout;
      fout.open(cluster->localId + "-follower");
      while(true) {  
        Timer waitTime = randTimer(info->electionTimeout);
        fout <<"waitTime: " << waitTime << std::endl;
        fout <<"sleeping..." << std::endl;
        try{
          boost::this_thread::sleep_for(boost::chrono::milliseconds(waitTime));
        }
        catch(boost::thread_interrupted &e) {
          fout <<"catch interrupt " << std::endl;
          continue;
        }          
        fout << cluster->localId << " transform form follower to candidate." << std::endl;

        transformer->Transform(RaftServerRole::follower, RaftServerRole::candidate, info->currentTerm + 1);
        
        fout << cluster->localId << " transform end." << std::endl;
        break;
      }
      fout.close();
    });
  }
}
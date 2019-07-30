#include "Candidate.h"

namespace Raft {
  Candidate::Candidate(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 

  bool Candidate::put(const std::string &key, const std::string &args) {
    return false;
  }

  std::pair<bool, std::string> Candidate::get(const std::string &key) {
    return std::make_pair(false, invalidString);
  }
  
  RequestVoteReply Candidate::respondRequestVote(const RequestVoteRequest &request) {
   if(request.term > info->currentTerm) {
     voteThread.interrupt();
     transformer->Transform(RaftServerRole::candidate, RaftServerRole::follower, request.term);
   }
   return RequestVoteReply(false, info->currentTerm);
  }

  AppendEntriesReply Candidate::respondHeartbeat(const AppendEntriesRequest &request) {
    if(request.term >= info->currentTerm) {
      voteThread.interrupt();
      transformer->Transform(RaftServerRole::candidate, RaftServerRole::follower, request.term);
    }
    return AppendEntriesReply(false, info->currentTerm);    
  }

  AppendEntriesReply Candidate::respondAppendEntries(const Raft::Rpc::RpcAppendEntriesRequest *request) {
    if(request->term() >= info->currentTerm) {
      voteThread.interrupt();
      transformer->Transform(RaftServerRole::candidate, RaftServerRole::follower, request->term());
    }
    return AppendEntriesReply(false, info->currentTerm);    
  }

  void Candidate::init(Term currentTerm) {
    boost::unique_lock<boost::mutex> lk(info->infoMutex);
    info->currentTerm = currentTerm;
    info->votedFor = invalidServerId;
    RequestVoteRequest request(cluster->localId, info->currentTerm, info->lastLogTerm(), info->lastLogIndex());
    std::cout << getTime() <<' '<<cluster->localId << " becomes a candidate, currentTerm = " << info->currentTerm << std::endl;
    lk.unlock();
    
    std::cout<<getTime() << " build voteThread " << std::endl;
    
    Timer electionTimeout = cluster->electionTimeout;
    voteThread.interrupt();
    voteThread.join();
    voteThread = boost::thread([this, request, electionTimeout] {
      //std::ofstream fout(cluster->localId + "-candidate");
      //fout.close();
      std::pair<RaftServerRole, Term> result = rpcClient->sendRequestVotes(cluster->localServer, request);
      if(result.first == RaftServerRole::candidate) {
        Timer waitTime = randTimer(electionTimeout);          
        try{
          boost::this_thread::sleep_for(boost::chrono::milliseconds(waitTime));
        }
        catch(boost::thread_interrupted &e){
          return;
        }
      } 
      transformer->Transform(RaftServerRole::candidate, result.first, result.second);
    });
    //voteThread.detach();
    // /std::cout<<getTime() << " end candidate::init " << std::endl;
  }
}
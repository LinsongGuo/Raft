#include "Leader.h"

namespace Raft {
  Leader::Leader(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 
   
  RequestVoteReply Leader::respondRequestVote(const RequestVoteRequest &request) {
    if(request.term > info->currentTerm) {
      heartbeatThread.interrupt();
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, request.term);
    }
    return RequestVoteReply(false, info->currentTerm);
  }

  AppendEntriesReply Leader::respondAppendEntries(const AppendEntriesRequest &request) {
    if(request.term > info->currentTerm) {
      heartbeatThread.interrupt();
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, request.term);
    }
    return AppendEntriesReply(false, info->currentTerm);
  }

  //AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit);
  void Leader::init() {
    boost::unique_lock<boost::mutex> lk(info->infoMutex);
    std::cout << getTime() <<' '<<cluster->localId << " becomes a leader, currentTerm = " << info->currentTerm << std::endl;
    AppendEntriesRequest request(cluster->localId, info->currentTerm, invalidTerm, invalidIndex, info->commitIndex);
    lk.unlock();
    
    Timer heartbeatTimeout = cluster->heartbeatTimeout;
    heartbeatThread.interrupt();
    heartbeatThread.join();
    heartbeatThread = boost::thread([this, request, heartbeatTimeout]{
      std::ofstream fout(cluster->localId + "-leader"); 
      while(true) {
        try{
          fout << getTime() << " sleeping... " << std::endl;
          boost::this_thread::sleep_for(boost::chrono::milliseconds(heartbeatTimeout));
          fout << getTime() << " sending..." << std::endl;
          std::pair<RaftServerRole, Term> result = rpcClient->sendHeartbeats(cluster->localServer, request);
          fout << getTime() << " result " << result.first <<' '<< result.second << std::endl;
          if(result.first == RaftServerRole::follower) {
            transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, result.second);
            return;
          }
        }
        catch(boost::thread_interrupted &e) {
          return;
        }       
      }
      fout.close();
    });     
    //heartbeatThread.detach(); 
  }
}
#include "Follower.h"

namespace Raft {
  Follower::Follower(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;}

  bool Follower::checkMajorityEntries(const RequestVoteRequest &request) {
    return request.lastLogTerm > info->lastLogTerm() || 
    (request.lastLogTerm == info->lastLogTerm() && request.lastLogIndex >= info->lastLogIndex());
  }

  RequestVoteReply Follower::respondRequestVote(const RequestVoteRequest &request) {
    if(request.term < info->currentTerm) {
      return RequestVoteReply(false, info->currentTerm);
    }
    else if(request.term > info->currentTerm) {
      info->currentTerm = request.term;
      if(checkMajorityEntries(request)) {
        info->votedFor = request.candidateId; 
        sleepThread.interrupt();
        return RequestVoteReply(true, info->currentTerm);
      }
      else {
        return RequestVoteReply(false, info->currentTerm);
      } 
    }
    else {
      if(info->votedFor == invalidServerId) {
        info->votedFor = request.candidateId;
        sleepThread.interrupt();
        return RequestVoteReply(true, info->currentTerm);
      }
      else {
        return RequestVoteReply(false, info->currentTerm);
      } 
    }
  }
  
  AppendEntriesReply Follower::respondAppendEntries(const AppendEntriesRequest &request) {
    if(request.term < info->currentTerm) return AppendEntriesReply(false, info->currentTerm);
    if(request.term > info->currentTerm) {
      info->currentTerm = request.term;
    } 
    if(request.entries.size() > 0) {
      //not heartbeat
      if(request.prevLogIndex > info->replicatedEntries.size() - 1 || 
        (request.prevLogIndex != invalidIndex && info->replicatedEntries[request.prevLogIndex].term != request.prevLogTerm)) {
        return AppendEntriesReply(false, info->currentTerm);
      }
      while(info->replicatedEntries.size() - 1 > request.prevLogIndex) {
        info->replicatedEntries.pop_back();
      }  
      for(auto &p: request.entries) {
        info->replicatedEntries.push_back(p);
      }
    }
    if(request.leaderCommit > info->commitIndex) {
      info->commitIndex = std::min(request.leaderCommit, info->replicatedEntries.size() - 1);
    }
    if(info->lastApplied < info->commitIndex) {
      for(size_t i = info->lastApplied + 1; i <= info->commitIndex; ++i) {
        info->appliedEntries.push_back(info->replicatedEntries[i]);
      }
      info->lastApplied = info->commitIndex;
    }
    sleepThread.interrupt();
    return AppendEntriesReply(true, info->currentTerm);
  }
  
  void Follower::init() {
    std::cout << cluster->localId << " becomes a follower! ---------------------------- " << std::endl;
    boost::unique_lock<boost::mutex> lk(info->infoMutex);
    Timer electionTimeout = info->electionTimeout;
    Term currentTerm = info->currentTerm;
    lk.unlock();
    sleepThread = boost::thread([this, electionTimeout, currentTerm] {
      std::ofstream fout;
      fout.open(cluster->localId + "-follower");
      while(true) {  
        Timer waitTime = randTimer(electionTimeout);
        fout <<getTime() << " waitTime: " << waitTime << std::endl;
        fout <<getTime() << " sleeping..." << std::endl;
        try{
          boost::this_thread::sleep_for(boost::chrono::milliseconds(waitTime));
        }
        catch(boost::thread_interrupted &e) {
          fout << getTime() << " catch interrupt " << std::endl;
          continue;
        }          
        fout << getTime() <<' '<<cluster->localId << " transform form follower to candidate." << std::endl;
        transformer->Transform(RaftServerRole::follower, RaftServerRole::candidate, currentTerm + 1);
        break;
      }
      fout.close();
    });
  }
}
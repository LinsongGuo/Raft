#include "Follower.h"

namespace Raft {
  Follower::Follower(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;}


  bool Follower::put(const std::string &key, const std::string &args) {
    return false; 
  }

  std::pair<bool, std::string> Follower::get(const std::string &key) {
    return std::make_pair(false, invalidString);
  }

  bool Follower::checkMajorityEntries(const RequestVoteRequest &request) {
    return request.lastLogTerm > info->lastLogTerm() || 
    (request.lastLogTerm == info->lastLogTerm() && request.lastLogIndex >= info->lastLogIndex());
  }

  RequestVoteReply Follower::respondRequestVote(const RequestVoteRequest &request) {
     sleepThread.interrupt();
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
      return RequestVoteReply(false, info->currentTerm); 
    }
    else {
      if((info->votedFor == invalidServerId || info->votedFor == request.candidateId) && checkMajorityEntries(request)) {
        info->votedFor = request.candidateId;
        sleepThread.interrupt();
        return RequestVoteReply(true, info->currentTerm);
      }
      return RequestVoteReply(false, info->currentTerm); 
    }
  }
  
  AppendEntriesReply Follower::respondHeartbeat(const AppendEntriesRequest &request) {
    if(request.term < info->currentTerm) {
      return AppendEntriesReply(false, info->currentTerm);
    }
    if(request.term > info->currentTerm) {
      info->currentTerm = request.term;
    } 
    if(request.leaderCommit > info->commitIndex) {
      info->commitIndex = std::min(request.leaderCommit, info->replicatedEntries.size() - 1);
    }
    if(info->lastApplied < info->commitIndex) {
      for(size_t i = info->lastApplied + 1; i <= info->commitIndex; ++i) {
        info->appliedEntries[info->replicatedEntries[i].key] = info->replicatedEntries[i].args;
      }
      info->lastApplied = info->commitIndex;
    }
    sleepThread.interrupt();
    return AppendEntriesReply(true, info->currentTerm);
  }

  AppendEntriesReply Follower::respondAppendEntries(const Raft::Rpc::RpcAppendEntriesRequest *request) {
    Term term = request->term(), prevLogTerm = request->prevlogterm();
    Index prevLogIndex = request->prevlogindex(), leaderCommit = request->leadercommit();
    if(term < info->currentTerm) {
      return AppendEntriesReply(false, info->currentTerm);
    }
    if(term > info->currentTerm) {
      info->currentTerm = term;
    } 
    size_t siz = request->entries().size();
    if(siz > 0) { 
      if(prevLogIndex != invalidIndex && 
        (prevLogIndex > info->replicatedEntries.size() - 1 || 
        info->replicatedEntries[prevLogIndex].term != prevLogTerm) ) {
        return AppendEntriesReply(false, info->currentTerm);
      }
      while(info->replicatedEntries.size() - 1 > prevLogIndex) {
        info->replicatedEntries.pop_back();
      } 
      if(!request->entries().empty()) {
        for(size_t i = siz - 1; i >= 0; --i) {
          auto tmp = request->entries()[i];
          info->replicatedEntries.push_back(ReplicatedEntry(tmp.key(), tmp.args(), tmp.term()));
        }
      }
    }
    if(leaderCommit > info->commitIndex) {
      info->commitIndex = std::min(leaderCommit, info->replicatedEntries.size() - 1);
    }
    if(info->lastApplied < info->commitIndex) {
      for(size_t i = info->lastApplied + 1; i <= info->commitIndex; ++i) {
        info->appliedEntries[info->replicatedEntries[i].key] = info->replicatedEntries[i].args;
      }
      info->lastApplied = info->commitIndex;
    }
    sleepThread.interrupt();
    return AppendEntriesReply(true, info->currentTerm);
  }
  
  void Follower::init(Term currentTerm) {
    boost::unique_lock<boost::mutex> lk(info->infoMutex);
    info->currentTerm = currentTerm;
    info->votedFor = invalidServerId;
    std::cout << getTime() <<' '<<cluster->localId << " becomes a follower, currentTerm = " << info->currentTerm << std::endl;
    lk.unlock();
    
    Timer electionTimeout = cluster->electionTimeout;
    sleepThread.interrupt();
    sleepThread.join();
    sleepThread = boost::thread([this, electionTimeout, currentTerm] {
      std::ofstream fout;
      fout.open("follower-"+cluster->localId+"-"+std::to_string(currentTerm));
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
        fout.close();
        transformer->Transform(RaftServerRole::follower, RaftServerRole::candidate, currentTerm + 1);
        break;
      }
    });
  }
}
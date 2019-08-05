#include "Leader.h"

namespace Raft {
  Leader::Leader(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 
  
  bool Leader::put(const std::string &key, const std::string &args) {
    info->replicatedEntries.push_back(ReplicatedEntry(key, args, info->currentTerm));
    size_t siz = cluster->size;
    std::vector<boost::future<AppendEntriesReply> > appendFuture;
    for(size_t i = 0; i < siz; ++i) {
      if(i == cluster->localServer) continue;
      Raft::Rpc::RpcAppendEntriesRequest rpcRequest;
      rpcRequest.set_leaderid(cluster->localId);
      rpcRequest.set_term(info->currentTerm);
      rpcRequest.set_prevlogindex(info->nextIndex[i] - 1);
      rpcRequest.set_prevlogterm(info->replicatedEntries[info->nextIndex[i] - 1].term);
      rpcRequest.set_leadercommit(info->commitIndex);
      for(size_t j = info->nextIndex[i]; j < info->replicatedEntries.size(); ++j) {
        Raft::Rpc::Entry tmp;
        tmp.set_key(info->replicatedEntries[j].key);
        tmp.set_args(info->replicatedEntries[j].args);
        tmp.set_term(info->replicatedEntries[j].term);
        *rpcRequest.add_entries() = std::move(tmp);
      }

      appendFuture.push_back(boost::async(boost::launch::async, [this, i, rpcRequest]() mutable -> AppendEntriesReply {
        Timer startTime = getTime();
        do {
          std::pair<bool, AppendEntriesReply> result = rpcClient->sendAppendEntries(i, rpcRequest);
          if(result.first) {
            if(result.second.success) {
              boost::unique_lock<boost::mutex> lk2(info->infoMutex);
              info->nextIndex[i] = info->replicatedEntries.size();
              info->matchIndex[i] = info->nextIndex[i] - 1;
              return AppendEntriesReply(true, result.second.term);
            }
            else {
              if(result.second.term > rpcRequest.term()) {
                AppendEntriesReply(false, result.second.term);
              } 
              if(rpcRequest.prevlogindex() > 0) {
                Index prevLogIndex = rpcRequest.prevlogindex() - 1;
                info->nextIndex[i]--;
                if(prevLogIndex > 0) {
                  rpcRequest.set_prevlogindex(prevLogIndex);
                  rpcRequest.set_prevlogterm(info->replicatedEntries[prevLogIndex].term);
                  Raft::Rpc::Entry tmp;
                  tmp.set_key(info->replicatedEntries[prevLogIndex].key);
                  tmp.set_args(info->replicatedEntries[prevLogIndex].args);
                  tmp.set_term(info->replicatedEntries[prevLogIndex].term);
                  *rpcRequest.add_entries() = std::move(tmp);  
                }
              }
            }
          }
        } while(getTime() <= startTime + cluster->appendTimeout);
        return AppendEntriesReply(false, invalidTerm);
      }));
    }

    std::vector<Index> matchIndexes;
    size_t getAppends = 1, nowId = 0;
    for(size_t i = 0; i < siz; ++i) {
      if(i == cluster->localServer) continue;
      AppendEntriesReply result = appendFuture[nowId++].get();
      //fout <<"result " << i <<' ' << result.success <<' '<< result.term << ' ' 
      //<< info->matchIndex[i] << ' ' << info->nextIndex[i] << std::endl;
      if(result.success) {
        getAppends++;
        if(info->matchIndex[i] < info->replicatedEntries.size() 
          && info->replicatedEntries[info->matchIndex[i]].term == info->currentTerm) {
          matchIndexes.push_back(info->matchIndex[i]);
        }
      }
      else if(result.term > info->currentTerm) {
        heartbeatThread.interrupt();
        transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, result.term);
        return false;
      }
    }
    /*
    if(getAppends * 2 <= cluster->size) {
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, info->currentTerm);
      return false;
    }*/
    sort(matchIndexes.begin(), matchIndexes.end(), [](Index x, Index y)->bool{return x > y;});
    
    if(siz == 1) {
      info->commitIndex = info->replicatedEntries.size() - 1;
    }
    else if((siz >> 1) - 1 < matchIndexes.size()) {
      info->commitIndex = matchIndexes[(siz >> 1) - 1];
    } 
    while(info->lastApplied < info->commitIndex) {
      ++info->lastApplied;
      info->appliedEntries[info->replicatedEntries[info->lastApplied].key] = info->replicatedEntries[info->lastApplied].args;
    }
    return info->replicatedEntries.size() - 1 == info->commitIndex;
  }

  std::pair<bool, std::string> Leader::get(const std::string &key) {
    if(info->appliedEntries.count(key)) return std::make_pair(true, info->appliedEntries[key]);
    return std::make_pair(true, notFound);
  }

  RequestVoteReply Leader::respondRequestVote(const RequestVoteRequest &request) {
    if(request.term > info->currentTerm) {
      heartbeatThread.interrupt();
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, request.term);
    }
    return RequestVoteReply(false, info->currentTerm);
  }

  AppendEntriesReply Leader::respondHeartbeat(const AppendEntriesRequest &request) {
    if(request.term > info->currentTerm) {
      heartbeatThread.interrupt();
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, request.term);
    }
    return AppendEntriesReply(false, info->currentTerm);
  }

  AppendEntriesReply Leader::respondAppendEntries(const Raft::Rpc::RpcAppendEntriesRequest *request) {
    if(request->term() > info->currentTerm) {
      heartbeatThread.interrupt();
      transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, request->term());
    }
    return AppendEntriesReply(false, info->currentTerm);
  }

  void Leader::init(Term currentTerm) {
    info->currentTerm = currentTerm;
    info->votedFor = invalidServerId;
    for(size_t i = 0; i < cluster->size; ++i) {
      info->nextIndex[i] = info->replicatedEntries.size();
      info->matchIndex[i] = 0;
    }
    AppendEntriesRequest request(cluster->localId, info->currentTerm, invalidTerm, invalidIndex, info->commitIndex);
    
    std::cout << getTime() <<' '<<cluster->localId << " becomes a leader, currentTerm = " << info->currentTerm << std::endl;
    fout << getTime() <<' '<<cluster->localId << " becomes a leader, currentTerm = " << info->currentTerm << std::endl;

    Timer heartbeatTimeout = cluster->heartbeatTimeout;
    heartbeatThread.interrupt();
    heartbeatThread.join();
    heartbeatThread = boost::thread([this, request, heartbeatTimeout]{
      //std::ofstream //fout(cluster->localId + "-leader"); 
      while(true) {
        try{
          //fout << getTime() << " sleeping... " << std::endl;
          boost::this_thread::sleep_for(boost::chrono::milliseconds(heartbeatTimeout));
          //fout << getTime() << " sending..." << std::endl;
          std::pair<RaftServerRole, Term> result = rpcClient->sendHeartbeats(request);
          //fout << getTime() << " result " << result.first <<' '<< result.second << std::endl;
          if(result.first == RaftServerRole::follower) {
            transformer->Transform(RaftServerRole::leader, RaftServerRole::follower, result.second);
            return;
          }
        }
        catch(boost::thread_interrupted &e) {
          return;
        }       
      }
      //fout.close();
    });     
    //heartbeatThread.detach(); 
  }
}
#include "Leader.h"

namespace Raft {
  Leader::Leader(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer):
    Role(_info, _cluster, _rpcClient, _transformer) {;} 
  
  bool Leader::put(const std::string &key, const std::string &args) {
    boost::unique_lock<boost::mutex> lk(info->infoMutex);
    info->replicatedEntries.push_back(ReplicatedEntry(key, args, info->currentTerm));
    std::vector<boost::future<AppendEntriesReply> > appendFuture;
    for(size_t i = 0; i < cluster->size; ++i) {
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
      lk.unlock();
      appendFuture.push_back(boost::async(boost::launch::async, [this, i, rpcRequest]() -> AppendEntriesReply {
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
              if(result.second.term > request.term()) {
                AppendEntriesReply(false, result.second.term);
              } 
              Index prevLogIndex = rpcRequest.prevLogIndex - 1;
              if(prevLogIndex > 0) {
                rpcRequest.set_prevlogindex(prevLogIndex);
                rpcRequest.set_prevlogterm(info->replicatedEntries[prevLogIndex].term);
                Raft::Rpc::Entry tmp;
                tmp.set_key(info->replicatedEntries[prevLogIndex].key);
                tmp.set_args(info->replicatedEntries[prevLogIndex].args);
                tmp.set_term(info->replicatedEntries[prevLogIndex].term);
                *rpcRequest.add_entries() = std::move(tmp);  
              }
              rpcRequest.set_leadercommit(info->commitIndex);
            }
          }
        } while(startTime + cluster->appendTimeout <= getTime());
        return AppendEntriesReply(false, invalidTerm);
      }));
    }

    size_t nowId = 0, getAppends = 1;
    std::vector<AppendEntriesReply> appendReply;
    for(size_t i = 0; i < cluster->size; ++i) {
      if(i == cluster->localServer) continue;
      appendReply.push_back(appendFuture[nowId++].get());
      if(result.success) {
        getAppends++;
      }
      else if(result.term > )
    }
  }

  std::pair<bool, std::string> Leader::get(const std::string &key) {

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

  //AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit);
  void Leader::init(Term currentTerm) {
    boost::unique_lock<boost::mutex> lk(info->infoMutex);
    info->currentTerm = currentTerm;
    info->votedFor = invalidServerId;
    AppendEntriesRequest request(cluster->localId, info->currentTerm, invalidTerm, invalidIndex, info->commitIndex);
    std::cout << getTime() <<' '<<cluster->localId << " becomes a leader, currentTerm = " << info->currentTerm << std::endl;
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
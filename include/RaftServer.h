#ifndef RAFT_RAFTSERVER_H
#define RAFT_RAFTSERVER_H

#include <cstdio>
#include <iostream>
#include <functional>
#include <boost/thread/thread.hpp>
#include <boost/thread/lock_factories.hpp>
#include <boost/thread/future.hpp>
#include "defines.h"
#include "Task.h"
#include "Role.h"
#include "Follower.h"
#include "Candidate.h"
#include "Leader.h"
#include "RaftRpc.pb.h"
#include "RaftRpc.grpc.pb.h"
#include "RaftRpcServer.h"
#include "RaftRpcClient.h"
#include "RaftExternalServer.h"
#include "RaftExternalClient.h"

namespace Raft {
  class RaftServer {
  private:
    std::ofstream fout0, fout1, fout2, fout3, fout4, fout5, fout6;
    RaftServerRole currentRole;
    std::shared_ptr<RaftServerCluster> cluster;
    std::shared_ptr<RaftServerInfo> info;
    
    std::shared_ptr<Rpc::RaftRpcServer> rpcServer;
    std::shared_ptr<Rpc::RaftRpcClient> rpcClient;
    std::shared_ptr<External::RaftExternalServer> externalServer;
    
    std::shared_ptr<Transformer> transformer;
    
    std::queue<TaskType> taskQueue;
    std::queue<PutTask> putQueue;
    std::queue<GetTask> getQueue;
    std::queue<RespondRequestVoteTask> respondRequestVoteQueue;
    std::queue<RespondHeartbeatTask> respondHeartbeatQueue;
    std::queue<RespondAppendEntriesTask> respondAppendEntriesQueue;
    std::queue<TransformTask> transformQueue;
    boost::mutex queueMutex;
    boost::condition_variable queueCond;
    boost::thread queueThread;

    std::unique_ptr<Role> roles[4];

  public:
    RaftServer(const std::string &fileName);
    void start();
    void shutdown();    
    void executeTask();
    bool put(const std::string &key, const std::string &args);
    std::pair<bool, std::string> get(const std::string &key);
    RequestVoteReply respondRequestVote(const RequestVoteRequest &requset);
    AppendEntriesReply respondHeartbeat(const AppendEntriesRequest &request);
    AppendEntriesReply respondAppendEntries(const Raft::Rpc::RpcAppendEntriesRequest *request);
    void transform(const RaftServerRole &fromRole, const RaftServerRole &toRole, const Term &term);
  };
}
#endif

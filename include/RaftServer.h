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
#include "RaftRpc.pb.h"
#include "RaftRpc.grpc.pb.h"
#include "RaftRpcServer.h"
#include "RaftRpcClient.h"

namespace Raft {
  class RaftServer {
  private:
    RaftServerCluster cluster;
    RaftServerInfo info;
    
    std::shared_ptr<Rpc::RaftRpcServer> rpcServer;
    std::shared_ptr<Rpc::RaftRpcClient> rpcClient;
    
    std::queue<TaskType> taskQueue;
    std::queue<PutTask> putQueue;
    std::queue<GetTask> getQueue;
    std::queue<RespondRequestVoteTask> respondRequestVoteQueue;
    std::queue<RespondAppendEntriesTask> respondAppendEntiresQueue;
    std::queue<HeartbeatTask> heartbeatQueue;
    std::queue<TransformTask> transformQueue;
    boost::mutex queueMutex;
    boost::condition_variable queueCond;
    boost::thread queueThread;

    std::unique_ptr<Role> roles[4];
    RaftServerRole currentRole;

  public:
    RaftServer(const std::string &fileName);
    void start();
    void shutdown();    
    void executeTask();
    RequestVoteReply respondRequestVote(RequestVoteRequest requset);

  };
}
#endif

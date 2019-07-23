#ifndef RAFT_RAFTSERVER_H
#define RAFT_RAFTSERVER_H

#include <cstdio>
#include <iostream>
#include <boost/thread/thread.hpp>
#include <boost/thread/lock_factories.hpp>
#include <boost/fiber/future/future.hpp>
#include "defines.h"
#include "Task.h"
#include "RaftRpc.pb.h"
#include "RaftRpc.grpc.pb.h"
#include "RaftRpcServer.h"
#include "RaftRpcClient.h"

namespace Raft {
  class RaftServer {
  private:
    RaftServerRole currentRole;
    RaftServerCluster cluster;
    RaftServerInfo info;
    
    std::unique_ptr<Rpc::RaftRpcServer> rpcServer;
    std::unique_ptr<Rpc::RaftRpcClient> rpcClient;
    
    std::queue<Task::Type> taskQueue;
    std::queue<Task::Put> putQueue;
    std::queue<Task::Get> getQueue;
    std::queue<Task::RespondRequestVote> respondRequestVoteQueue;
    std::queue<Task::RespondAppendEntries> respondAppendEntiresQueue;
    std::queue<Task::Heartbeat> heartbeatQueue;
    std::queue<Task::Transform> transformQueue;
    
    boost::mutex queueMutex;
    boost::condition_variable queueCond;
   // unique_ptr<Role> roles[4];

  public:
    RaftServer(const std::string &fileName);
    void start();
    void RequestVotes();
    void executeTask();
    RequestVoteReply respondRequestVote(const RequestVoteRequest &requset);

  };
}
#endif

#ifndef RAFT_RPC_RAFTRPCSERVER_H
#define RAFT_RPC_RAFTRPCSERVER_H

#include <cstdio>
#include <iostream>
#include <functional>
#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include <boost/thread/thread.hpp>
#include <boost/chrono/chrono.hpp>
#include "RaftRpc.grpc.pb.h"
#include "RaftRpc.pb.h"
#include "defines.h"

namespace Raft {
  namespace Rpc {
    class RaftRpcServiceImpl final: public RaftRpc::Service {
    private:
      std::function<RequestVoteReply(const RequestVoteRequest&)> respondRequestVote;
      std::function<RequestVoteReply(const AppendEntriesRequest&)> respondAppendEntries;
    public:
      grpc::Status RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *request, RpcAppendEntriesReply *reply) override;
      grpc::Status RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *request, RpcRequestVoteReply *reply) override;
      template <class T> 
      void bindRespondRequestVote(T &&func);
      template<class T>
      void bindRespondAppendEntries(T &&func);
      ServerId localId;  
    };
    class RaftRpcServer {
    private:
       RaftRpcServiceImpl service;
       std::unique_ptr<grpc::Server> server;
       boost::thread raftRpcServerThread;
    public:
      void start(const std::string &str);
      void shutdown();
      template <class T> 
      void bindRespondRequestVote(T &&func);
      template<class T>
      void bindRespondAppendEntries(T &&func);
      ServerId localId;  
    };
  }
}

#endif

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
      std::function<Raft::RequestVoteReply(const RequestVoteRequest&)> RequestVote;
      std::function<Raft::RequestVoteReply(const AppendEntriesRequest&)> AppendEntries;
    public:
      grpc::Status RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *request, RpcAppendEntriesReply *reply) override;
      grpc::Status RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *request, RpcRequestVoteReply *reply) override;
      template <class func> 
      void bindRequestVote(func &&f) {
        RequestVote = std::forward<func>(f);
      }
      template<class func>
      void bindAppendEntries(func &&f) {
        AppendEntries = std::forward<func>(f);
      }
      ServerId localId;  
    };
    class RaftRpcServer {
    private:
       RaftRpcServiceImpl service;
       std::unique_ptr<grpc::Server> server;
       boost::thread serviceThread;
    public:
      void start(const std::string &str);
    };
  }
}

#endif

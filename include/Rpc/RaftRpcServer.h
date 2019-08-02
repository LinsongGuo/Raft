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
      std::ofstream fout1, fout2, fout3;
      std::function<RequestVoteReply(const RequestVoteRequest&)> respondRequestVote;
      std::function<AppendEntriesReply(const AppendEntriesRequest&)> respondHeartbeat;
      std::function<AppendEntriesReply(const RpcAppendEntriesRequest*)> respondAppendEntries;
    public:
      grpc::Status RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *request, RpcRequestVoteReply *reply) override;
      grpc::Status RpcHeartbeat(grpc::ServerContext *context, const RpcAppendEntriesRequest *request, RpcAppendEntriesReply *reply) override;
      grpc::Status RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *request, RpcAppendEntriesReply *reply) override;
      template <class T> 
      void bindRespondRequestVote(T &&func) {
        respondRequestVote = std::forward<T>(func);
      }
      template <class T>
      void bindRespondHeartbeat(T &&func) {
        respondHeartbeat = std::forward<T>(func);
      }
      template <class T>
      void bindRespondAppendEntries(T &&func) {
        respondAppendEntries = std::forward<T>(func);
      }  
      void openFile(const std::string &address);
      void closeFile();
    };
    class RaftRpcServer {
    private:
       RaftRpcServiceImpl service;
       std::unique_ptr<grpc::Server> server;
       boost::thread raftRpcServerThread;
    public:
      void start(const std::string &address);
      void shutdown(); 
      template <class T>
      void bindRespondRequestVote(T &&func) {
        service.bindRespondRequestVote(std::forward<T>(func));
      }
      template <class T>
      void bindRespondHeartbeat(T &&func) {
        service.bindRespondHeartbeat(std::forward<T>(func));
      }
      template <class T>
      void bindRespondAppendEntries(T &&func) {
        service.bindRespondAppendEntries(std::forward<T>(func));
      }
    };
  }
}

#endif

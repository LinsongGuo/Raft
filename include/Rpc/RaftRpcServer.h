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
    public:
      grpc::Status RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *request, RpcAppendEntriesReply *reply) override;
      grpc::Status RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *request, RpcRequestVoteReply *reply) override;
     // template <class func> 
      //void bind
      Address adr;  
      
    private:
      //std::function<Raft::RequestVoteReply(const RequestVoteRequest&)> RequestVote;
      //std::function<Raft::RequestVoteReply(const AppendEntriesRequest&)> AppendEntries;

    };
    class RaftRpcServer {
    private:
       Address address;
       RaftRpcServiceImpl service;
       std::unique_ptr<grpc::Server> server;
       boost::thread serviceThread;
    public:
      void start(const std::string &_address);
    };
  }
}

#endif

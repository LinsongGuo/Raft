#include "RaftRpcServer.h"

namespace Raft {
  namespace Rpc {
    grpc::Status RaftRpcServiceImpl::RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *rpcRequest, RpcAppendEntriesReply *rpcReply) {
      reply->set_term(233);
      reply->set_success(1);
      return grpc::Status::OK;
    }
    grpc::Status RaftRpcServiceImpl::RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *rpcRequest, RpcRequestVoteReply *rpcReply) {
      RequestVoteRequest request(rpcRequest->candidateid(), 
        rpcRequest->term(), 
        rpcRequest->lastlogterm(), 
        rpcRequest->lastlogindex()
      );
      RequestVoteReply reply = respondRequestVote(request);
      rpcRequest->set_success(reply.success);
      rpcRequest->set_term(reply.term);
      return grpc::Status::OK;
    }
    template <class T> 
    void RaftRpcServiceImpl::bindRespondRequestVote(T &&func) {
      respondRequestVote = std::forward<T>(func);
    }
    template<class T>
    void RaftRpcServiceImpl::bindRespondAppendEntries(T &&func) {
      respondAppendEntries = std::forward<T>(func);
    }
    template <class T> 
    void RaftRpcServer::bindRespondRequestVote(T &&func) {
      service.bindRespondRequestVote<T>(std::forward<T>(func));
    }
    template<class T>
    void RaftRpcServer::bindRespondAppendEntries(T &&func) {
      service.bindrespondAppendEntries<T>(std::forward<T>(func));
    }
    void RaftRpcServer::start(const std::string &str) {
      service.localId = str;
      grpc::ServerBuilder builder;
      builder.AddListeningPort(str, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      server = builder.BuildAndStart();
      raftRpcServerThread = boost::thread([this]{server->Wait();});
    }
    void RaftRpcServer::shutdown(){
      if (server) server->Shutdown();
      raftRpcServerThread.join();
    }
  }
}



#include "RaftRpcServer.h"

namespace Raft {
  namespace Rpc {
    grpc::Status RaftRpcServiceImpl::RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *rpcRequest, RpcAppendEntriesReply *rpcReply) {
      rpcReply->set_term(233);
      rpcReply->set_success(1);
      return grpc::Status::OK;
    }
    grpc::Status RaftRpcServiceImpl::RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *rpcRequest, RpcRequestVoteReply *rpcReply) {
      RequestVoteRequest request(rpcRequest->candidateid(), 
        rpcRequest->term(), 
        rpcRequest->lastlogterm(), 
        rpcRequest->lastlogindex()
      );
      std::cout <<"receive a requestvote from " << rpcRequest->candidateid() << std::endl;
      RequestVoteReply reply = respondRequestVote(request);
      rpcReply->set_votegranted(reply.voteGranted);
      rpcReply->set_term(reply.term);
      return grpc::Status::OK;
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



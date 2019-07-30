#include "RaftRpcServer.h"

namespace Raft {
  namespace Rpc {
    grpc::Status RaftRpcServiceImpl::RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *rpcRequest, RpcAppendEntriesReply *rpcReply) {
      AppendEntriesRequest request(rpcRequest->leaderid(),
        rpcRequest->term(),
        rpcRequest->prevlogterm(),
        rpcRequest->prevlogindex(),
        rpcRequest->leadercommit()
      );
      std::cout <<getTime() << " receive a AppendEntries from " << rpcRequest->leaderid() << std::endl;
      
      AppendEntriesReply reply = respondAppendEntries(request);
      rpcReply->set_success(reply.success);
      rpcReply->set_term(reply.term);
      std::cout<<getTime() << " appendresult " << reply.success <<' '<< reply.term << std::endl;
      return grpc::Status::OK;
    }
    grpc::Status RaftRpcServiceImpl::RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *rpcRequest, RpcRequestVoteReply *rpcReply) {
      //std::ofstream fout("vote-"+rpcRequest->candidateid() + "-" + std::to_string(rpcRequest->term()));
      //fout<<getTime() <<" enter requestvote" << std::endl;
      RequestVoteRequest request(rpcRequest->candidateid(), 
        rpcRequest->term(), 
        rpcRequest->lastlogterm(), 
        rpcRequest->lastlogindex()
      );
      std::cout <<getTime() << " receive a requestvote from " << rpcRequest->candidateid() << std::endl;
      RequestVoteReply reply = respondRequestVote(request);
      std::cout <<getTime() <<" voteresult " << reply.voteGranted << ' '<< reply.term << std::endl;
      //fout.close();
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



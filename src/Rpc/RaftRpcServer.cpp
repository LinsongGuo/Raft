#include "RaftRpcServer.h"

namespace Raft {
  namespace Rpc {
    grpc::Status RaftRpcServiceImpl::RpcRequestVote(grpc::ServerContext *context, const RpcRequestVoteRequest *rpcRequest, RpcRequestVoteReply *rpcReply) {
      fout1 << getTime() << " receive a requestvote from " << rpcRequest->candidateid() << ":" <<rpcRequest->term() << ' ' 
      << rpcRequest->lastlogterm() << ' ' << rpcRequest->lastlogindex() << std::endl;
      
      RequestVoteRequest request(rpcRequest->candidateid(), 
        rpcRequest->term(), 
        rpcRequest->lastlogterm(), 
        rpcRequest->lastlogindex()
      );
      RequestVoteReply reply = respondRequestVote(request);
      rpcReply->set_votegranted(reply.voteGranted);
      rpcReply->set_term(reply.term);
      
      fout1 << getTime() <<" result " << reply.voteGranted << ' '<< reply.term << std::endl;
      
      return grpc::Status::OK;
    }
    grpc::Status RaftRpcServiceImpl::RpcHeartbeat(grpc::ServerContext *context, const RpcAppendEntriesRequest *rpcRequest, RpcAppendEntriesReply *rpcReply) {
      fout2 << getTime() << " receive a heartbeat from " << rpcRequest->leaderid() << ' ' << rpcRequest->term() << ' ' 
      << ' ' << rpcRequest->prevlogterm() << ' ' << rpcRequest->prevlogindex() << ' ' << rpcRequest->leadercommit() << std::endl;
     
      AppendEntriesRequest request(rpcRequest->leaderid(),
        rpcRequest->term(),
        rpcRequest->prevlogterm(),
        rpcRequest->prevlogindex(),
        rpcRequest->leadercommit()
      );
      AppendEntriesReply reply = respondHeartbeat(request);
      rpcReply->set_success(reply.success);
      rpcReply->set_term(reply.term);
      
      fout2 << getTime() << " result " << reply.success <<' '<< reply.term << std::endl;
      
      return grpc::Status::OK;
    }
    grpc::Status RaftRpcServiceImpl::RpcAppendEntries(grpc::ServerContext *context, const RpcAppendEntriesRequest *rpcRequest, RpcAppendEntriesReply *rpcReply) {
      fout3 << getTime() << " receive a appendentries from " << rpcRequest->leaderid() << ' ' << rpcRequest->term() << ' ' 
      << ' ' << rpcRequest->prevlogterm() << ' ' << rpcRequest->prevlogindex() << ' ' << rpcRequest->leadercommit() << std::endl;
      
      AppendEntriesReply reply = respondAppendEntries(rpcRequest);
      rpcReply->set_success(reply.success);
      rpcReply->set_term(reply.term);
      
      fout3 << getTime() << " result " << reply.success <<' '<< reply.term << std::endl;
      
      return grpc::Status::OK;
    }
    void RaftRpcServiceImpl::openFile(const Address &address) {
      fout1.open(address + "/receive-requestvote");
      fout2.open(address + "/receive-heartbeat");
      fout3.open(address + "/receive-appendentries");
    }
    void RaftRpcServiceImpl::closeFile() {
      fout1.close();
      fout2.close();
      fout3.close();
    }  

    void RaftRpcServer::start(const Address &address, const ServerId &localId) {
      service.openFile(address);
      
      grpc::ServerBuilder builder;
      builder.AddListeningPort(localId, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      server = builder.BuildAndStart();
      raftRpcServerThread = boost::thread([this]{server->Wait();});
    }
    void RaftRpcServer::shutdown(){
      if (server) server->Shutdown();
      raftRpcServerThread.join();
      
      service.closeFile();
    }
  }
}



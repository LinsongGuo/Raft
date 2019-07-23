#include "RaftRpcServer.h"

namespace Raft {
  namespace Rpc {
    grpc::Status RaftRpcServiceImpl::AppendEntries(grpc::ServerContext *context, const AppendEntriesRequest *request, AppendEntriesReply *reply) {
      reply->set_term(233);
      reply->set_success(1);
      return grpc::Status::OK;
    }
    grpc::Status RaftRpcServiceImpl::RequestVote(grpc::ServerContext *context, const RequestVoteRequest *request, RequestVoteReply *reply) {
      Term x = rand() % 65536;
      reply->set_term(x);
      reply->set_votegranted(1);
      std::cout << adr << " receives the request " << request->term();
      std::cout << " and returns a reply " << x << std::endl;
      boost::this_thread::sleep_for(boost::chrono::milliseconds(2000));
      return grpc::Status::OK;
    }
    void RaftRpcServer::start(const std::string &_address) {
      address = _address;
      service.adr = _address;
      grpc::ServerBuilder builder;
      builder.AddListeningPort(address, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      server = builder.BuildAndStart();
      serviceThread = boost::thread([this]{server->Wait();});
    }
  }
}



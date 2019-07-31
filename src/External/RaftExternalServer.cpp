#include "RaftExternalServer.h"

namespace Raft {
  namespace External{
    grpc::Status RaftExternalServerImpl::Put(grpc::ServerContext *context,
                                         const PutRequest *request,
                                         PutReply *reply) {
      reply->set_status(put(request->key(), request->args()));
      return grpc::Status::OK;
    }

    grpc::Status RaftExternalServerImpl::Get(grpc::ServerContext *context,
                                         const GetRequest *request,
                                         GetReply *reply) {
      std::pair<bool, std::string> result = get(request->key());
      reply->set_status(result.first);
      reply->set_args(result.second);
      return grpc::Status::OK;
    }

    void RaftExternalServer::start(const std::string &addr) {
      grpc::ServerBuilder builder;
      builder.AddListeningPort(addr, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      Server = builder.BuildAndStart();
      runningThread = std::thread([this] { Server->Wait(); });
    }
    
    void RaftExternalServer::shutdown() {
      if (Server)
        Server->Shutdown();
      runningThread.join();
    }
  }
}
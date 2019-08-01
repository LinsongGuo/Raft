#include "RaftExternalServer.h"
#include "defines.h"

namespace Raft {
  namespace External{
    grpc::Status RaftExternalServerImpl::Put(grpc::ServerContext *context,
                                         const PutRequest *request,
                                         PutReply *reply) {
      std::ofstream fout("receiveput-" + request->key() + "-" + request->args());
      fout << getTime() << " receive a put " << request->key() <<' ' << request->args() << std::endl;
      std::cout << getTime() << " receive a put " << request->key() <<' ' << request->args() << std::endl;
      bool result = put(request->key(), request->args());
      reply->set_status(result);
      fout << getTime() << " putresult "<< result << std::endl; 
      fout.close();
      std::cout << getTime() << " putresult "<< result << std::endl; 
      return grpc::Status::OK;
    }

    grpc::Status RaftExternalServerImpl::Get(grpc::ServerContext *context,
                                         const GetRequest *request,
                                         GetReply *reply) {
      std::cout << getTime() << " receive a get " << request->key() << std::endl;
      std::pair<bool, std::string> result = get(request->key());
      reply->set_status(result.first);
      reply->set_args(result.second);
      std::cout << getTime() << " getresult " << result.first << ' ' << result.second << std::endl; 
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
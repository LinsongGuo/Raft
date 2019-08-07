#include "RaftExternalServer.h"
#include "defines.h"

namespace Raft {
  namespace External{
    grpc::Status RaftExternalServerImpl::Put(grpc::ServerContext *context,
                                         const PutRequest *request,
                                         PutReply *reply) {
      
      //fout1 << getTime() << " receive a put " << request->key() <<' ' << request->args() << std::endl;
      
      bool result = put(request->key(), request->args());
      reply->set_status(result);
      
      //fout1 << getTime() << " result " << result << std::endl; 
      
      return grpc::Status::OK;
    }

    grpc::Status RaftExternalServerImpl::Get(grpc::ServerContext *context,
                                         const GetRequest *request,
                                         GetReply *reply) {
      
      //fout2 << getTime() << " receive a get " << request->key() << std::endl;
      
      std::pair<bool, std::string> result = get(request->key());
      reply->set_status(result.first);
      reply->set_args(result.second);
      
      //fout2 << getTime() << " result " << result.first << ' ' << result.second << std::endl; 
      
      return grpc::Status::OK;
    }
    void RaftExternalServerImpl::openFile(const Address &address) {
      //fout1.open(address + "/receive-put");
      //fout2.open(address + "/receive-get");
    }
    void RaftExternalServerImpl::closeFile() {
      //fout1.close();
      //fout2.close();
    }

    void RaftExternalServer::start(const Address &address, const ServerId &externalId) {
      service.openFile(address);

      grpc::ServerBuilder builder;
      builder.AddListeningPort(externalId, grpc::InsecureServerCredentials());
      builder.RegisterService(&service);
      Server = builder.BuildAndStart();
      runningThread = std::thread([this] { Server->Wait(); });
    }
    
    void RaftExternalServer::shutdown() {
      if (Server)
        Server->Shutdown();
      runningThread.join();

      service.closeFile();
    }
  }
}
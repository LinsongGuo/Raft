#ifndef RAFT_EXTERNAL_RAFTEXTERNALSERVER_H
#define RAFT_EXTERNAL_RAFTEXTERNALSERVER_H

#include <functional>
#include <thread>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include "defines.h"
#include "RaftExternal.pb.h"
#include "RaftExternal.grpc.pb.h"

namespace Raft {
  namespace External {
    class RaftExternalServerImpl : public RaftExternal::Service {
    private:
      std::ofstream fout1, fout2;
      std::function<bool(const std::string &, const std::string &)> put;
      std::function<std::pair<bool, std::string>(const std::string &)> get;
    public:
      template <class Func> 
      void bindPut(Func &&f) { put = std::forward<Func>(f); }

      template <class Func> 
      void bindGet(Func &&f) { get = std::forward<Func>(f); }

      grpc::Status Put(grpc::ServerContext *context, const PutRequest *request,
                       PutReply *response) override;

      grpc::Status Get(grpc::ServerContext *context, const GetRequest *request,
                       GetReply *response) override;
      void openFile(const Address &address);
      void closeFile();
     };

    class RaftExternalServer {
    private:
      RaftExternalServerImpl service;
      std::unique_ptr<grpc::Server> Server;
      std::thread runningThread;
    public:
      template <class Func>
      void bindPut(Func && f) { service.bindPut(std::forward<Func>(f)); }
      
      template <class Func>
      void bindGet(Func && f) { service.bindGet(std::forward<Func>(f)); }

      void start(const Address &address, const ServerId &externalId);
      void shutdown();
    };
  }
}
#endif 

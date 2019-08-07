#include "RaftExternalClient.h"

namespace Raft {
  namespace External {
    
    struct RaftExternalClient::Impl {
      std::vector<std::unique_ptr<RaftExternal::Stub>> stubs;
      std::atomic<std::size_t> cur{0};
    }; 

    RaftExternalClient::RaftExternalClient(const std::string &filename) : pImpl(std::make_unique<Impl>()) {
      //fout.open("ExternalClient");
      namespace pt = boost::property_tree;
      pt::ptree tree;
      pt::read_json(filename, tree);

      std::vector<std::string> srvList;
      for (auto &&srv : tree.get_child("serverList"))
        srvList.emplace_back(srv.second.get_value<std::string>());

      for (const auto & srv : srvList) {
        pImpl->stubs.emplace_back(RaftExternal::NewStub(grpc::CreateChannel(
            srv, grpc::InsecureChannelCredentials())));
      }
    }

    RaftExternalClient::~RaftExternalClient() = default;

    template <class Tp>
    decltype(auto) timeFrom(const Tp & tp) {
      return std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now() - tp).count();
    }

    void RaftExternalClient::Put(std::string key, std::string args, uint64_t timeout) {
      auto startTimePoint = std::chrono::system_clock::now();
      //fout << "put " << key << ' ' << args << std::endl;
      while ((uint64_t)timeFrom(startTimePoint) <= timeout) {
        auto & stub = pImpl->stubs[pImpl->cur % pImpl->stubs.size()];
        grpc::ClientContext ctx;
        ctx.set_deadline(startTimePoint + std::chrono::milliseconds(timeout));
        ctx.set_idempotent(true);

        PutRequest request;
        request.set_key(key);
        request.set_args(args);
        PutReply reply;
        auto status = stub->Put(&ctx, request, &reply);
        if (status.ok() && reply.status())
          return;
        pImpl->cur++;
      }

      //throw RequestTimeout();
    }

    std::string RaftExternalClient::Get(std::string key, uint64_t timeout) {
      auto startTimePoint = std::chrono::system_clock::now();
      //fout << "get " << key << std::endl;
      while ((uint64_t)timeFrom(startTimePoint) <= timeout) {
        auto & stub = pImpl->stubs[pImpl->cur % pImpl->stubs.size()];
        grpc::ClientContext ctx;
        ctx.set_deadline(startTimePoint + std::chrono::milliseconds(timeout));
        ctx.set_idempotent(true);

        GetRequest request;
        request.set_key(key);
        GetReply reply;
        auto status = stub->Get(&ctx, request, &reply);

        if (status.ok() && reply.status())
          return reply.args();
        pImpl->cur++;
      }

      //throw RequestTimeout();
    }
  }
} 
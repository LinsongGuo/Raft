#include <atomic>
#include <vector>
#include <chrono>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <grpc++/create_channel.h>

#include "Client.h"
#include "External.grpc.pb.h"

namespace Raft {

struct Client::Impl {
  std::vector<std::unique_ptr<External::Stub>> stubs;
  std::atomic<std::size_t> cur{0};
}; /* struct Client::Impl */


} // namespace ppca

namespace Raft {

Client::Client(const std::string &filename) : pImpl(std::make_unique<Impl>()) {
  namespace pt = boost::property_tree;
  pt::ptree tree;
  pt::read_json(filename, tree);

  std::vector<std::string> srvList;
  for (auto &&srv : tree.get_child("serverList"))
    srvList.emplace_back(srv.second.get_value<std::string>());

  for (const auto & srv : srvList) {
    pImpl->stubs.emplace_back(External::NewStub(grpc::CreateChannel(
        srv, grpc::InsecureChannelCredentials())));
  }
}

Client::~Client() = default;

template <class Tp>
decltype(auto) timeFrom(const Tp & tp) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now() - tp).count();
}

void Client::Put(std::string key, std::string value, uint64_t timeout) {
  auto startTimePoint = std::chrono::system_clock::now();

  while ((uint64_t)timeFrom(startTimePoint) <= timeout) {
    auto & stub = pImpl->stubs[pImpl->cur % pImpl->stubs.size()];
    grpc::ClientContext ctx;
    ctx.set_deadline(startTimePoint + std::chrono::milliseconds(timeout));
    ctx.set_idempotent(true);

    PutRequest request;
    request.set_key(key);
    request.set_value(value);
    PutReply reply;
    auto status = stub->Put(&ctx, request, &reply);

    if (status.ok() && reply.status())
      return;
    pImpl->cur++;
  }

  throw RequestTimeout();
}

std::string Client::Get(std::string key, uint64_t timeout) {
  auto startTimePoint = std::chrono::system_clock::now();

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
      return reply.value();
    pImpl->cur++;
  }

  throw RequestTimeout();
}

} 
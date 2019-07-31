#ifndef RAFT_EXTERNAL_RAFTEXTERNALCLIENT_H
#define RAFT_EXTERNAL_RAFTEXTERNALCLIENT_H

#include <cstdint>
#include <memory>
#include <string>
#include <exception>
#include <atomic>
#include <vector>
#include <chrono>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <grpc++/create_channel.h>
#include "RaftExternal.pb.h"
#include "RaftExternal.grpc.pb.h"

namespace Raft {
  namespace External {
    class RequestTimeout : public std::exception {};

    class RaftExternalClient {
    public:
      explicit RaftExternalClient(const std::string & filename);
      RaftExternalClient(const RaftExternalClient &) = delete;
      RaftExternalClient(RaftExternalClient &&) = delete;
      RaftExternalClient& operator=(const RaftExternalClient &) = delete;
      RaftExternalClient& operator=(RaftExternalClient &&) = delete;
      ~RaftExternalClient();

      void Put(std::string key, std::string value, std::uint64_t timeout = 5000);

      std::string Get(std::string key, std::uint64_t timeout = 5000);

    private:
      struct Impl;
      std::unique_ptr<Impl> pImpl;

    };

  }
} 
#endif 

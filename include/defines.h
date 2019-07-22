#ifndef RAFT_DEFINES_H
#define RAFT_DEFINES_H

#include <iostream>

namespace Raft {
  using Term = uint64_t;
  using Index = uint64_t;
  using Address = std::string;
  using Port = uint16_t; 
  using Timer = uint64_t;
  const Term invalidTerm = 0;
  const Index invalidIndex = 0;
  enum ServerRole {
    follower, candidate, leader, deadServer
  };
  struct LogEntry {
    Term term = invalidTerm;
    std::string opt;
    std::string arg;
  };
  struct ServerID {
    Address address;
    Port port;
    ServerID(Address _address, Port _port) : address(_address), port(_port) {}
    std::string toString() {
      return address + ":" + std::to_string(port);
    }
    bool operator == (const ServerID &other) {
      return address == other.address && port == other.port;
    }
    bool operator != (const ServerID &other) {
      return address != other.address && port != other.port;
    }
  };
  const ServerID invalidServerID = {"", 0};
}
#endif

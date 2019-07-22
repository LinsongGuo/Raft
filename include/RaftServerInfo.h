#ifndef RAFT_RAFTSERVERINFO_H
#define RAFT_RAFTSERVERINFO_H

#include <vector>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "defines.h"

namespace Raft {
  struct RaftServerInfo {
    Address local;
    std::vector<Address> serverList;
    Term currentTerm;
    Address votedFor;
    Index commitIndex, lastApplied;
    RaftServerInfo();
    RaftServerInfo(const std::string &fileName);
    RaftServerInfo(Address _local, const std::vector<Address> &v);
  }; 
}
#endif

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "RaftServerInfo.h"

namespace Raft {
  RaftServerInfo::RaftServerInfo() {
    local = votedFor = "";
    currentTerm = invalidTerm;
    commitIndex = lastApplied = invalidIndex; 
  }
  RaftServerInfo::RaftServerInfo(const std::string &fileName) {
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(fileName, tree);
    local = tree.get<std::string>("local");
    for(auto &&adr : tree.get_child("serverList")) {
      serverList.emplace_back(adr.second.get_value<std::string>());
    }
    votedFor = "";
    currentTerm = invalidTerm;
    commitIndex = lastApplied = invalidIndex;
  }
  RaftServerInfo::RaftServerInfo(Address _local, const std::vector<Address> &v) {
    local = _local;
    for(size_t i = 0; i < v.size(); ++i) {
      serverList.push_back(v[i]);
    }
    votedFor = "";
    currentTerm = invalidTerm;
    commitIndex = lastApplied = invalidIndex;
  }
}

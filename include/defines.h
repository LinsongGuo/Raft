#ifndef RAFT_DEFINES_H
#define RAFT_DEFINES_H

#include <iostream>
#include <vector>
#include <boost/chrono/chrono.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace Raft {
  using Term = uint64_t;
  const Term invalidTerm = 0;
  

  using Index = uint64_t; 
  const Index invalidIndex = 0;


  using Timer = uint64_t;
  /*Timer getTime() {
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
  }*/


  using Address = std::string;  
  using Port = uint16_t; 
  using ServerId = std::string;
  const ServerId invalidServerId = " : ";
  /*ServerId getServerId(const Address &address, const Port &port) {
    return address + ":" + std::to_string(port);
  }*/
  

  enum ServerRole {
    follower, candidate, leader, deadServer
  };

  
  struct Entry {
    Term term = invalidTerm;
    std::string opt, args;
  };
  

  struct AppendEntriesRequest {
    ServerId leaderId;
    Term term, prevLogTerm;
    Index prevLogIndex, leaderCommit;
    std::vector<Entry> entries;
    AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit) :
      leaderId(_leaderId), term(_term), prevLogTerm(_prevLogTerm), prevLogIndex(_prevLogIndex), leaderCommit(_leaderCommit) {;}
  };
  struct AppendEntiresReply {
    Term term;
    bool success;
    AppendEntiresReply(Term _term, bool _success) : term(_term), success(_success) {;}
  };
  

  struct RequestVoteRequest {
    ServerId candidateId;
    Term term, lastLogTerm;
    Index lastLogIndex;
    RequestVoteRequest(ServerId _candidateId, Term _term, Term _lastLogTerm, Index _lastLogIndex) :
      candidateId(_candidateId), term(_term), lastLogTerm(_lastLogTerm), lastLogIndex(_lastLogIndex) {;}
  };
  struct RequestVoteReply {
    Term term;
    bool voteGranted;
    RequestVoteReply(Term _term, bool _voteGranted) : term(_term), voteGranted(_voteGranted) {;}
  };


  struct RaftServerClusters {
    ServerId localId;
    std::vector<ServerId> serverList;
    RaftServerClusters(): localId(invalidServerId) {;}
    RaftServerClusters(const std::string &fileName) {
      boost::property_tree::ptree tree;
      boost::property_tree::read_json(fileName, tree);
      localId = tree.get<std::string>("local");
      for(auto &&adr : tree.get_child("serverList")) {
        serverList.emplace_back(adr.second.get_value<std::string>());
      }
    }
    RaftServerClusters(ServerId _localId, const std::vector<ServerId> &v) {
      localId = _localId;
      for(size_t i = 0; i < v.size(); ++i) {
        serverList.push_back(v[i]);
      }
    }
  }; 


  struct RaftServerInfo {
    Term currentTerm;
    ServerId votedFor;
    Index commitIndex, lastApplied;
    RaftServerInfo(Term _currentTerm = invalidTerm, ServerId _votedFor = invalidServerId, Index _commitIndex = invalidIndex, Index _lastApplied = invalidIndex):
      currentTerm(_currentTerm), votedFor(_votedFor), commitIndex(_commitIndex), lastApplied(_lastApplied) {;}
  };
}
#endif

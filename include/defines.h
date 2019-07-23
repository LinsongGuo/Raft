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

  using Address = std::string;  
  using Port = uint16_t; 
  using ServerId = std::string;
  const ServerId invalidServerId = " : ";

  enum RaftServerRole {
    Follower, Candidate, Leader, DeadServer
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
    AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit);
  };

  struct AppendEntiresReply {
    Term term;
    bool success;
    AppendEntiresReply(Term _term, bool _success);
  };
  

  struct RequestVoteRequest {
    ServerId candidateId;
    Term term, lastLogTerm;
    Index lastLogIndex;
    RequestVoteRequest(ServerId _candidateId, Term _term, Term _lastLogTerm, Index _lastLogIndex);
  };

  struct RequestVoteReply {
    Term term;
    bool voteGranted;
    RequestVoteReply(Term _term, bool _voteGranted);
  };

  struct RaftServerCluster {
    ServerId localId;
    std::vector<ServerId> serverList;
    RaftServerCluster();
    RaftServerCluster(const std::string &fileName);
    RaftServerCluster(ServerId _localId, const std::vector<ServerId> &v);
  }; 

  struct RaftServerInfo {
    Term currentTerm;
    ServerId votedFor;
    Index commitIndex, lastApplied;
    RaftServerInfo(Term _currentTerm = invalidTerm, ServerId _votedFor = invalidServerId, Index _commitIndex = invalidIndex, Index _lastApplied = invalidIndex);
  };
}
#endif

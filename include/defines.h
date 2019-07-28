#ifndef RAFT_DEFINES_H
#define RAFT_DEFINES_H

#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <exception>
#include <boost/thread/thread.hpp>
#include <boost/thread/future.hpp>
#include <boost/chrono/chrono.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "exception.hpp"

namespace Raft {
  using Term = uint64_t;
  extern const Term invalidTerm;

  using Index = uint64_t; 
  extern const Index invalidIndex;

  using Timer = uint64_t;
  Timer getTime();
  Timer randTimer(Timer range);
  extern const Timer broadcastTimeout;
  
  using Address = std::string;  
  using Port = uint16_t; 
  using ServerId = std::string;
  extern const ServerId invalidServerId;


  enum RaftServerRole {
    follower, candidate, leader, deadServer
  };

  struct ReplicatedEntry {
    Term term;
    std::string opt, args;
    ReplicatedEntry(Term _term = invalidTerm, std::string _opt = "", std::string _args = "");
  };
  
  struct AppliedEntry {
    std::string opt, args;
    AppliedEntry(std::string _opt = "", std::string _args = "");
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
    AppendEntiresReply(bool _success, Term _term);
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
    RequestVoteReply(bool _voteGranted, Term _term);
  };

  struct RaftServerCluster {
    size_t size, localServer;
    ServerId localId;
    std::vector<ServerId> serverList;
    RaftServerCluster();
    RaftServerCluster(const std::string &fileName);
    RaftServerCluster(ServerId _localId, const std::vector<ServerId> &v);
  }; 

  struct RaftServerInfo {
    Term currentTerm;
    ServerId votedFor;
    std::vector<ReplicatedEntry> replicatedEntries;
    std::vector<AppliedEntry> appliedEntries;
    Index commitIndex, lastApplied;
    Timer electionTimeout;
    Term lastLogTerm();
    Index lastLogIndex();
    RaftServerInfo(Term _currentTerm);
  };

}
#endif

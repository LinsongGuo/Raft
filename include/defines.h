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
#include "exception.h"

namespace Raft {
  extern const std::string invalidString;

  using Term = uint64_t;
  extern const Term invalidTerm;

  using Index = uint64_t; 
  extern const Index invalidIndex;

  using Timer = uint64_t;
  Timer getTime();
  Timer randTimer(Timer range);
  
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
    ReplicatedEntry(Term _term = invalidTerm, std::string _opt = invalidString, std::string _args = invalidString);
  };
  
  struct AppliedEntry {
    std::string opt, args;
    AppliedEntry(std::string _opt = invalidString, std::string _args = invalidString);
    AppliedEntry(const ReplicatedEntry &replicatedEntry);
  };

  struct AppendEntriesRequest {
    ServerId leaderId;
    Term term, prevLogTerm;
    Index prevLogIndex, leaderCommit;
    std::vector<ReplicatedEntry> entries;
    AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit);
  };

  struct AppendEntriesReply {
    Term term;
    bool success;
    AppendEntriesReply(bool _success, Term _term);
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
    Timer electionTimeout, heartbeatTimeout, broadcastTimeout;
    size_t size, localServer;
    ServerId localId;
    std::vector<ServerId> serverList;
    RaftServerCluster();
    RaftServerCluster(const std::string &fileName);
    RaftServerCluster(ServerId _localId, const std::vector<ServerId> &v);
  }; 

  struct RaftServerInfo {
    boost::mutex infoMutex;
    RaftServerRole currentRole;
    Term currentTerm;
    ServerId votedFor;
    Index commitIndex, lastApplied;
    std::vector<ReplicatedEntry> replicatedEntries;
    std::vector<AppliedEntry> appliedEntries;
    std::vector<Index> nextIndex;
    std::vector<Index> matchIndex;

    RaftServerInfo();
    Term lastLogTerm();
    Index lastLogIndex();
  };

}
#endif

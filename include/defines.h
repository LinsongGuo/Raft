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
#include "RaftRpc.pb.h"
#include "RaftRpc.grpc.pb.h"


namespace Raft {
  extern const std::string invalidString;
  extern const std::string notFound;
  
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
    std::string key, args;
    ReplicatedEntry(std::string _key = invalidString, std::string _args = invalidString, Term _term = invalidTerm);
  };
  
  struct AppliedEntry {
    std::string key, args;
    AppliedEntry(std::string _key = invalidString, std::string _args = invalidString);
  };

  struct AppendEntriesRequest {
    ServerId leaderId;
    Term term, prevLogTerm;
    Index prevLogIndex, leaderCommit;
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
    Timer electionTimeout, heartbeatTimeout, broadcastTimeout, appendTimeout;
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
    std::vector<Index> nextIndex;
    std::vector<Index> matchIndex;
    std::vector<ReplicatedEntry> replicatedEntries;
    std::map<std::string, std::string> appliedEntries;
    RaftServerInfo(size_t size);
    Term lastLogTerm();
    Index lastLogIndex();
  };

}
#endif

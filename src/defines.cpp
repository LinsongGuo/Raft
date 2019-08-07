#include "defines.h"

namespace Raft {
  const std::string invalidString = "";
  const std::string notFound = "NOT FOUND";

  const Term invalidTerm = 0;

  const Index invalidIndex = 0;

  const ServerId invalidServerId = " : ";

  Timer getTime() {
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
  }
  Timer randTimer(Timer range) {
    return range + rand() % (range + 1);
  }

  ServerId getServerId(const Address &address, const Port &port) {
    return address + ":" + std::to_string(port);
  }
  
  ReplicatedEntry::ReplicatedEntry(std::string _key, std::string _args, Term _term): 
    key(_key), args(_args), term(_term) {;}

  AppliedEntry::AppliedEntry(std::string _key, std::string _args): 
    key(_key), args(_args) {;}

  AppendEntriesRequest::AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit):
    leaderId(_leaderId), term(_term), prevLogTerm(_prevLogTerm), prevLogIndex(_prevLogIndex), leaderCommit(_leaderCommit) {;}
 
  AppendEntriesReply::AppendEntriesReply(bool _success, Term _term): 
    success(_success), term(_term) {;}
  
  RequestVoteRequest::RequestVoteRequest(ServerId _candidateId, Term _term, Term _lastLogTerm, Index _lastLogIndex):
    candidateId(_candidateId), term(_term), lastLogTerm(_lastLogTerm), lastLogIndex(_lastLogIndex) {;}
  
  RequestVoteReply::RequestVoteReply(bool _voteGranted, Term _term): 
    voteGranted(_voteGranted), term(_term) {;}
  
  RaftServerCluster::RaftServerCluster(): 
    localId(invalidServerId) {;}

  RaftServerCluster::RaftServerCluster(const std::string &fileName) {
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(fileName, tree);
    address = tree.get<std::string>("address");
    localPort = tree.get<uint16_t>("localPort");
    externalPort = tree.get<uint16_t>("externalPort");
    localId = address + ":" + std::to_string(localPort);
    externalId = address + ":" + std::to_string(externalPort);
    electionTimeout = tree.get<uint64_t>("electionTimeout");
    heartbeatTimeout = tree.get<uint64_t>("heartbeatTimeout");
    broadcastTimeout = tree.get<uint64_t>("broadcastTimeout");
    appendTimeout = tree.get<uint64_t>("appendTimeout");
    for(auto &&adr : tree.get_child("serverList")) {
      serverList.emplace_back(adr.second.get_value<std::string>());
    }
    size = serverList.size();
    for(size_t i = 0; i < size; ++i) {
      if(localId == serverList[i]) {
        localServer = i;
        break;
      }
    }
    address = "server" + std::to_string(localServer);
  }

  RaftServerCluster::RaftServerCluster(ServerId _localId, const std::vector<ServerId> &v) {
    localId = _localId;
    for(size_t i = 0; i < v.size(); ++i) {
      serverList.push_back(v[i]);
    }
  }

  RaftServerInfo::RaftServerInfo(size_t size) {
    currentRole = RaftServerRole::follower;
    currentTerm = 1;
    votedFor = invalidServerId;
    commitIndex = lastApplied = invalidIndex;
    replicatedEntries.push_back(ReplicatedEntry());
    for(size_t i = 0; i < size; ++i) {
      nextIndex.push_back(1);
      matchIndex.push_back(0);
    }
  }

  Term RaftServerInfo::readLog(std::fstream &logScanner) {
    Term term = 1;
    std::string key, args;
    while(logScanner >> key) {
      logScanner >> args;
      logScanner >> term;
      replicatedEntries.push_back(ReplicatedEntry(key, args, term));   
    }
    lastApplied = commitIndex = replicatedEntries.size() - 1;
    return term;
  }
  
  Index RaftServerInfo::lastLogIndex() {
    return replicatedEntries.size() - 1;
  }

  Term RaftServerInfo::lastLogTerm() {
    return replicatedEntries.back().term;
  }

}


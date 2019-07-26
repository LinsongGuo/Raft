#include "defines.h"

namespace Raft {
  const Term invalidTerm = 0;

  const Index invalidIndex = 0;

  const ServerId invalidServerId = " : ";

  const Timer broadcastTimeout = 10;
  Timer getTime() {
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(boost::chrono::system_clock::now().time_since_epoch()).count();
  }
  Timer randTimer(Timer range) {
    return range + rand() % (range + 1);
  }

  ServerId getServerId(const Address &address, const Port &port) {
    return address + ":" + std::to_string(port);
  }
  
  AppendEntriesRequest::AppendEntriesRequest(ServerId _leaderId, Term _term, Term _prevLogTerm, Index _prevLogIndex, Index _leaderCommit):
    leaderId(_leaderId), term(_term), prevLogTerm(_prevLogTerm), prevLogIndex(_prevLogIndex), leaderCommit(_leaderCommit) {;}
 
  AppendEntiresReply::AppendEntiresReply(bool _success, Term _term): 
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
    localId = tree.get<std::string>("local");
    for(auto &&adr : tree.get_child("serverList")) {
      serverList.emplace_back(adr.second.get_value<std::string>());
    }
  }

  RaftServerCluster::RaftServerCluster(ServerId _localId, const std::vector<ServerId> &v) {
    localId = _localId;
    for(size_t i = 0; i < v.size(); ++i) {
      serverList.push_back(v[i]);
    }
  }

  RaftServerInfo::RaftServerInfo(Term _currentTerm):
    currentTerm(_currentTerm) , electionTimeout(1000) {;}
}


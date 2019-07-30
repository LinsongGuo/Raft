#ifndef RAFT_QUEUE_H
#define RAFT_QUEUE_H

#include <queue>
#include <boost/thread/future.hpp>
#include "defines.h"

namespace Raft {
  enum TaskType {
    put, get, respondRequestVote, respondAppendEntries, transform  
  };
  struct Task {
    TaskType opt;
    Task(TaskType _opt);
  };
  struct PutTask {
    std::string key, args;
    boost::promise<bool> &prm;
    PutTask(std::string _key, std::string _args, boost::promise<bool> &_prm);
  };
  struct GetTask {
    std::string key;
    boost::promise<std::pair<bool, std::string> > &prm;
    GetTask(std::string _key, boost::promise<std::pair<bool, std::string> > &_prm);
  };
  struct RespondRequestVoteTask {
    RequestVoteRequest request;
    boost::promise<RequestVoteReply> &prm;
    RespondRequestVoteTask(RequestVoteRequest _request, boost::promise<RequestVoteReply> &_prm);
  };
  struct RespondAppendEntriesTask {
    AppendEntriesRequest request;
    boost::promise<AppendEntriesReply> &prm;
    RespondAppendEntriesTask(AppendEntriesRequest _request, boost::promise<AppendEntriesReply> &_prm);
  };
  struct TransformTask {
    RaftServerRole fromRole, toRole;
    Term term;
    TransformTask(RaftServerRole _fromRole, RaftServerRole _toRole, Term _term);
  }; 
}

#endif

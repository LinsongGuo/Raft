#ifndef RAFT_QUEUE_H
#define RAFT_QUEUE_H

#include <queue>
#include <boost/thread/future.hpp>
#include "defines.h"

namespace Raft {
  enum TaskType {
    put, get, respondRequestVote, respondAppendEntires, transform  
  };
  struct Task {
    TaskType opt;
    Task(TaskType _opt);
  };
  struct PutTask {
    /*std::string key, args;
    boost::promise<bool> &prm;
    Put(std::string _key, std::string _args, boost::promise<bool> _prm);
  */
    PutTask();
  };
  struct GetTask {
   /* std::string key;
    boost::promise<std::pair<bool, std::string> > prm;
    Get(std::string _key, boost::promise<std::pair<bool, std::string> > _prm);
  */
    GetTask();
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

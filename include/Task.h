#ifndef RAFT_QUEUE_H
#define RAFT_QUEUE_H

#include <queue>
#include <boost/thread/future.hpp>
#include "defines.h"

namespace Raft {
  enum TaskType {
    put, get, respondRequestVote, respondHeartbeat, respondAppendEntries, transform  
  };
  struct Task {
    TaskType opt;
    Task(TaskType _opt);
  };
  struct PutTask {
    std::string key, args;
    boost::promise<bool> &prm;
    PutTask(const std::string &_key, const std::string &_args, boost::promise<bool> &_prm);
  };
  struct GetTask {
    std::string key;
    boost::promise<std::pair<bool, std::string> > &prm;
    GetTask(const std::string &_key, boost::promise<std::pair<bool, std::string> > &_prm);
  };
  struct RespondRequestVoteTask {
    RequestVoteRequest request;
    boost::promise<RequestVoteReply> &prm;
    RespondRequestVoteTask(const RequestVoteRequest &_request, boost::promise<RequestVoteReply> &_prm);
  };
  struct RespondHeartbeatTask {
    AppendEntriesRequest request;
    boost::promise<AppendEntriesReply> &prm;
    RespondHeartbeatTask(const AppendEntriesRequest &_request, boost::promise<AppendEntriesReply> &_prm);
  };
  struct RespondAppendEntriesTask {
    const Raft::Rpc::RpcAppendEntriesRequest *request;
    boost::promise<AppendEntriesReply> &prm;
    RespondAppendEntriesTask(const Raft::Rpc::RpcAppendEntriesRequest *_request, boost::promise<AppendEntriesReply> &_prm);
  };
  struct TransformTask {
    RaftServerRole fromRole, toRole;
    Term term;
    TransformTask(const RaftServerRole &_fromRole, const RaftServerRole &_toRole, const Term &_term);
  }; 
}

#endif

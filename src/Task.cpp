#include "Task.h"

namespace Raft {
  Task::Task(TaskType _opt): opt(_opt) {;}
  PutTask::PutTask(const std::string &_key, const std::string &_args, boost::promise<bool> &_prm):
    key(_key), args(_args), prm(_prm) {;}
  GetTask::GetTask(const std::string &_key, boost::promise<std::pair<bool, std::string> > &_prm):
    key(_key), prm(_prm) {;}
  RespondRequestVoteTask::RespondRequestVoteTask(const RequestVoteRequest &_request, boost::promise<RequestVoteReply> &_prm):
    request(_request), prm(_prm) {;}  
  RespondHeartbeatTask::RespondHeartbeatTask(const AppendEntriesRequest &_request, boost::promise<AppendEntriesReply> &_prm):
    request(_request), prm(_prm) {;}
  RespondAppendEntriesTask::RespondAppendEntriesTask(const Raft::Rpc::RpcAppendEntriesRequest *_request, boost::promise<AppendEntriesReply> &_prm):
    request(_request), prm(_prm) {;}
  TransformTask::TransformTask(const RaftServerRole &_fromRole, const RaftServerRole &_toRole, const Term &_term) :
    fromRole(_fromRole), toRole(_toRole), term(_term) {;}
}

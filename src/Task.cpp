#include "Task.h"

namespace Raft {
  Task::Task(TaskType _opt): opt(_opt) {;}
  PutTask::PutTask(std::string _key, std::string _args, boost::promise<bool> &_prm):
    key(_key), args(_args), prm(_prm) {;}
  GetTask::GetTask(std::string _key, boost::promise<std::pair<bool, std::string> > &_prm):
    key(_key), prm(_prm) {;}
  RespondRequestVoteTask::RespondRequestVoteTask(RequestVoteRequest _request, boost::promise<RequestVoteReply> &_prm):
    request(_request), prm(_prm) {;}  
  RespondAppendEntriesTask::RespondAppendEntriesTask(AppendEntriesRequest _request, boost::promise<AppendEntriesReply> &_prm):
    request(_request), prm(_prm) {;}
  TransformTask::TransformTask(RaftServerRole _fromRole, RaftServerRole _toRole, Term _term) :
    fromRole(_fromRole), toRole(_toRole), term(_term) {;}
}

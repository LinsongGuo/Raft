#include "Task.h"

namespace Raft {
  Task::Task(TaskType _opt): opt(_opt) {;}
  GetTask::GetTask() {;}
  PutTask::PutTask() {;}
  RespondRequestVoteTask::RespondRequestVoteTask(RequestVoteRequest _request, boost::promise<RequestVoteReply> &_prm):
    request(_request), prm(_prm) {;}  
  RespondAppendEntriesTask::RespondAppendEntriesTask() {;}
  HeartbeatTask::HeartbeatTask() {;}
  TransformTask::TransformTask() {;}
}

#include "RaftRpcClient.h"

namespace Raft {
  namespace Rpc {
    RaftRpcClient::RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels, Timer timeout, const std::string &address):
      broadcastTimeout(timeout)
    {
      fout1.open(address + "/send-requestvote");
      fout2.open(address + "/send-heartbeat");
      fout3.open(address + "/send-appendentries");
      size = channels.size();
      for(int i = 0; i < size; ++i) {
        stubs.emplace_back(RaftRpc::NewStub(channels[i]));
      }
    }

    std::pair<bool, RequestVoteReply> RaftRpcClient::sendRequestVote(size_t id, const RequestVoteRequest &request) {
      fout1 << getTime() << " send requestvote to " << id << ":" << request.term << ' ' << request.lastLogTerm << ' ' << request.lastLogIndex << std::endl;
      RpcRequestVoteRequest rpcRequest;
      rpcRequest.set_candidateid(request.candidateId);
      rpcRequest.set_term(request.term);
      rpcRequest.set_lastlogterm(request.lastLogTerm);
      rpcRequest.set_lastlogindex(request.lastLogIndex);
      RpcRequestVoteReply rpcReply;
      grpc::ClientContext context;
      fout1<<getTime() << " broadcastTimeout " << broadcastTimeout << std::endl;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      context.set_idempotent(true);
      grpc::Status status = stubs[id]->RpcRequestVote(&context, rpcRequest, &rpcReply);
      fout1 <<getTime() << " reply " << status.ok() <<' ' << rpcReply.votegranted() <<' ' << rpcReply.term() << std::endl;
      return std::make_pair(status.ok(), RequestVoteReply(rpcReply.votegranted(), rpcReply.term()));
    }
    
    std::pair<bool, AppendEntriesReply> RaftRpcClient::sendHeartbeat(size_t id, const AppendEntriesRequest &request) {
      fout2 << getTime() << " send heartbeat to " << id << ":" << request.leaderId << ' ' << request.term << ' ' 
      << request.prevLogTerm << ' ' << request.prevLogIndex << ' ' << request.leaderCommit << std::endl;
      RpcAppendEntriesRequest rpcRequest;
      rpcRequest.set_leaderid(request.leaderId);
      rpcRequest.set_term(request.term);
      rpcRequest.set_prevlogterm(request.prevLogTerm);
      rpcRequest.set_prevlogindex(request.prevLogIndex);
      rpcRequest.set_leadercommit(request.leaderCommit);
      RpcAppendEntriesReply rpcReply;
      grpc::ClientContext context;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      context.set_idempotent(true);
      grpc::Status status = stubs[id]->RpcHeartbeat(&context, rpcRequest, &rpcReply);
      fout2 << getTime() << " reply " << status.ok() << ' ' << rpcReply.success() << ' ' << rpcReply.term() << std::endl;
      return std::make_pair(status.ok(), AppendEntriesReply(rpcReply.success(), rpcReply.term()));
    }
    
    std::pair<bool, AppendEntriesReply> RaftRpcClient::sendAppendEntries(size_t id, RpcAppendEntriesRequest rpcRequest) {
      fout3 << getTime() << " send appendentries to " << id << ":" << rpcRequest.leaderid() << ' ' << rpcRequest.term() << ' ' 
      << rpcRequest.prevlogterm() << ' ' << rpcRequest.prevlogindex() << ' ' << rpcRequest.leadercommit() << std::endl;
      RpcAppendEntriesReply rpcReply;
      grpc::ClientContext context;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      context.set_idempotent(true);
      grpc::Status status = stubs[id]->RpcAppendEntries(&context, rpcRequest, &rpcReply);
      fout3 << getTime() << " reply " << status.ok() << ' ' << rpcReply.success() << ' ' << rpcReply.term() << std::endl;
      return std::make_pair(status.ok(), AppendEntriesReply(rpcReply.success(), rpcReply.term()));
    }

    std::pair<RaftServerRole, Term> RaftRpcClient::sendRequestVotes(const RequestVoteRequest &request) {
      size_t getVotes = 1;
      voteFuture.clear();
      for(size_t i = 0; i < size; ++i) {
        voteFuture.push_back(boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendRequestVote, this, i, request));
      }
      for(size_t i = 0; i < size; ++i) {
        std::pair<bool, RequestVoteReply> reply = voteFuture[i].get();
        if(reply.first) {
          if(reply.second.voteGranted) {
            getVotes++;
            if(reply.second.term > request.term) {
              return std::make_pair(RaftServerRole::follower, reply.second.term);
            }
          }
        }
      }
      if(getVotes * 2 > size + 1) return std::make_pair(RaftServerRole::leader, request.term);
      return std::make_pair(RaftServerRole::follower, request.term);
    }
    
    std::pair<RaftServerRole, Term> RaftRpcClient::sendHeartbeats(const AppendEntriesRequest &request) {
      size_t getAppends = 1;
      heartbeatFuture.clear();
      for(size_t i = 0; i < size; ++i) {
        heartbeatFuture.push_back(boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendHeartbeat, this, i, request));
      }
      for(size_t i = 0; i < size; ++i) {
        std::pair<bool, AppendEntriesReply> reply = heartbeatFuture[i].get();
        if(reply.first) {
          if(reply.second.success) {
            getAppends++;
            if(reply.second.term > request.term) {
              return std::make_pair(RaftServerRole::follower, reply.second.term);
            }
          }
        }
      }
      if(getAppends * 2 > size + 1) return std::make_pair(RaftServerRole::leader, request.term);
      return std::make_pair(RaftServerRole::follower, request.term);
    }
  }
}

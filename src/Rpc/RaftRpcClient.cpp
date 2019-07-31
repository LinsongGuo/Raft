#include "RaftRpcClient.h"

namespace Raft {
  namespace Rpc {
    RaftRpcClient::RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels, Timer timeout):
      broadcastTimeout(timeout)
    {
      size = channels.size();
      for(int i = 0; i < size; ++i) {
        stubs.emplace_back(RaftRpc::NewStub(channels[i]));
      }
    }

    std::pair<bool, RequestVoteReply> RaftRpcClient::sendRequestVote(size_t id, const RequestVoteRequest &request) {
      //fout << "enter " << id <<' ' <<getTime() << std::endl;
      RpcRequestVoteRequest rpcRequest;
      rpcRequest.set_candidateid(request.candidateId);
      rpcRequest.set_term(request.term);
      rpcRequest.set_lastlogterm(request.lastLogTerm);
      rpcRequest.set_lastlogindex(request.lastLogIndex);
      RpcRequestVoteReply rpcReply;
      grpc::ClientContext context;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      //fout <<getTime() <<' ' <<request.candidateId <<" is sending RequestVote to the server " << id << "..." << std::endl;
      grpc::Status status = stubs[id]->RpcRequestVote(&context, rpcRequest, &rpcReply);
      //fout <<getTime() << " get vote "<< status.ok() <<' ' << rpcReply.votegranted() <<' ' << rpcReply.term() << std::endl;
      return std::make_pair(status.ok(), RequestVoteReply(rpcReply.votegranted(), rpcReply.term()));
    }
    
    std::pair<bool, AppendEntriesReply> RaftRpcClient::sendHeartbeat(size_t id, const AppendEntriesRequest &request) {
      RpcAppendEntriesRequest rpcRequest;
      rpcRequest.set_leaderid(request.leaderId);
      rpcRequest.set_term(request.term);
      rpcRequest.set_prevlogterm(request.prevLogTerm);
      rpcRequest.set_prevlogindex(request.prevLogIndex);
      rpcRequest.set_leadercommit(request.leaderCommit);
      RpcAppendEntriesReply rpcReply;
      grpc::ClientContext context;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      grpc::Status status = stubs[id]->RpcHeartbeat(&context, rpcRequest, &rpcReply);
      return std::make_pair(status.ok(), AppendEntriesReply(rpcReply.success(), rpcReply.term()));
    }
    
    std::pair<bool, AppendEntriesReply> RaftRpcClient::sendAppendEntries(size_t id, RpcAppendEntriesRequest rpcRequest) {
      RpcAppendEntriesReply rpcReply;
      grpc::ClientContext context;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      grpc::Status status = stubs[id]->RpcAppendEntries(&context, rpcRequest, &rpcReply);
      return std::make_pair(status.ok(), AppendEntriesReply(rpcReply.success(), rpcReply.term()));
    }

    std::pair<RaftServerRole, Term> RaftRpcClient::sendRequestVotes(size_t localServer, const RequestVoteRequest &request) {
      size_t getVotes = 1;
      voteFuture.clear();
      fout.open("vote-" + std::to_string(localServer) + "-" + request.candidateId + "-" + std::to_string(request.term));
      fout << "opentime " << getTime() << std::endl;
      for(size_t i = 0; i < size; ++i) {
        if(i == localServer) continue;
        fout <<"build " << i <<' ' << getTime() << std::endl;
        voteFuture.push_back(boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendRequestVote, this, i, request));
        fout <<"build end " << i <<' ' << getTime() << std::endl;
      }
      size_t nowId = 0;
      for(size_t i = 0; i < size; ++i) {
        if(i == localServer) continue;
        std::pair<bool, RequestVoteReply> reply = voteFuture[nowId++].get();
        if(reply.first) {
          if(reply.second.voteGranted) {
            fout <<"get vote from " << i << std::endl;
            getVotes++;
            if(reply.second.term > request.term) {
              fout << "transform to follower " << std::endl;
              return std::make_pair(RaftServerRole::follower, reply.second.term);
            }
          }
        }
      }
      fout.close();
      if(getVotes * 2 > size) return std::make_pair(RaftServerRole::leader, request.term);
      return std::make_pair(RaftServerRole::follower, request.term);
    }
    
    std::pair<RaftServerRole, Term> RaftRpcClient::sendHeartbeats(size_t localServer, const AppendEntriesRequest &request) {
      size_t getAppends = 1;
      heartbeatFuture.clear();
      std::ofstream fout2("heartbeat-" + std::to_string(localServer) + "-" + request.leaderId + "-" + std::to_string(request.term));
      fout << "opentime " << getTime() << std::endl;
      for(size_t i = 0; i < size; ++i) {
        if(i == localServer) continue;
        fout2 << "build " << i << ' ' << getTime() << std::endl;
        heartbeatFuture.push_back(boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendHeartbeat, this, i, request));
        fout2 << "build end " << i <<' '<< getTime() << std::endl;
      }
      size_t nowId = 0;
      for(size_t i = 0; i < size; ++i) {
        if(i == localServer) continue;
        std::pair<bool, AppendEntriesReply> reply = heartbeatFuture[nowId++].get();
        fout2 <<"get " << i <<' ' << reply.first <<' ' << reply.second.success <<' ' << reply.second.term << std::endl;
        if(reply.first) {
          if(reply.second.success) {
            getAppends++;
            fout2 <<"get append from " << i << std::endl;
            if(reply.second.term > request.term) {
              fout2 << "transform to follower " << std::endl; 
              return std::make_pair(RaftServerRole::follower, reply.second.term);
            }
          }
        }
      }
      fout2.close();
      if(getAppends * 2 > size) return std::make_pair(RaftServerRole::leader, request.term);
      return std::make_pair(RaftServerRole::follower, request.term);
    }
  }
}

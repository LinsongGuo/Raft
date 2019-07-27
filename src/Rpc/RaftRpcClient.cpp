#include "RaftRpcClient.h"

namespace Raft {
  namespace Rpc {
    RaftRpcClient::RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels) {
      size = channels.size();
      for(int i = 0; i < size; ++i) {
        stubs.emplace_back(RaftRpc::NewStub(channels[i]));
      }
    }
    std::pair<bool, RequestVoteReply> RaftRpcClient::sendRequestVote(size_t id, const RequestVoteRequest &request) {
      fout << "enter " << id <<' ' <<getTime() << std::endl;
      RpcRequestVoteRequest rpcRequest;
      rpcRequest.set_candidateid(request.candidateId);
      rpcRequest.set_term(request.term);
      RpcRequestVoteReply rpcReply;
      grpc::ClientContext context;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      std::cout <<getTime() <<' ' <<request.candidateId <<" is sending RequestVote to the server " << id << "..." << std::endl;
      grpc::Status status = stubs[id]->RpcRequestVote(&context, rpcRequest, &rpcReply);
      return std::make_pair(status.ok(), RequestVoteReply(rpcReply.term(), rpcReply.votegranted()));
    }
    std::pair<RaftServerRole, Term> RaftRpcClient::sendRequestVotes(size_t localServer, const RequestVoteRequest &request) {
      size_t getVotes = 1;
      sendFuture.clear();
      fout.open("votes" + std::to_string(localServer));
      fout << "opentime " << getTime() << std::endl;
      for(int i = 0; i < size; ++i) {
        if(i == localServer) continue;
        fout <<"build " << i <<' ' << getTime() << std::endl;
        sendFuture.push_back(boost::async(boost::launch::async, &Rpc::RaftRpcClient::sendRequestVote, this, i, request));
        fout <<"build end " << i <<' ' << getTime() << std::endl;
      }
      int nowId = 0;
      for(int i = 0; i < size; ++i) {
        if(i == localServer) continue;
        std::pair<bool, RequestVoteReply> reply = sendFuture[nowId++].get();
        if(reply.first) {
          if(reply.second.voteGranted) {
            std::cout<<"get vote from " << i << std::endl;
            getVotes++;
            if(reply.second.term > request.term) {
              return std::make_pair(RaftServerRole::follower, reply.second.term);
            }
          }
        }
      }
      if(getVotes * 2 > size) return std::make_pair(RaftServerRole::leader, request.term);
      return std::make_pair(RaftServerRole::candidate, request.term + 1);
      fout.close();
    }
    
  }
}

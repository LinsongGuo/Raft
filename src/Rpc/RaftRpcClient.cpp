#include "RaftRpcClient.h"

namespace Raft {
  namespace Rpc {
    RaftRpcClient::RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels) {
      for(int i = 0; i < channels.size(); ++i) {
        stubs.emplace_back(RaftRpc::NewStub(channels[i]));
      }
    }
    std::pair<bool, RequestVoteReply> RaftRpcClient::sendRequestVote(size_t id, const RequestVoteRequest &request) {
      std::cout << "enter " << getTime() << std::endl;
      RpcRequestVoteRequest rpcRequest;
      rpcRequest.set_candidateid(request.candidateId);
      rpcRequest.set_term(request.term);
      RpcRequestVoteReply rpcReply;
      grpc::ClientContext context;
      std::cout<<"before deadline "<< getTime() << std::endl;
      context.set_deadline(std::chrono::system_clock::now() + std::chrono::milliseconds(broadcastTimeout));
      std::cout <<getTime() <<' ' <<request.candidateId <<" is sending RequestVote to the server " << id << "..." << std::endl;
      grpc::Status status = stubs[id]->RpcRequestVote(&context, rpcRequest, &rpcReply);
      return std::make_pair(status.ok(), RequestVoteReply(rpcReply.term(), rpcReply.votegranted()));
    }
    /*
    void RaftRpcClient::RpcRequestVotes(const RpcRequestVoteRequest &request) {
    	for(size_t i = 0; i < stubs.size(); ++i) {
		     RpcRequestVoteReply reply;
    		if(RpcRequestVote(i, request, reply)) {
    			std::cout << "The local receives the reply from the server " << i << " :" << reply.term() << std::endl; 
    		}
        else std::cout << "error..." << std::endl;
    	}
    }
    */
  }
}

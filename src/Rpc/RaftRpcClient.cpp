#include "RaftRpcClient.h"

namespace Raft {
  namespace Rpc {
    RaftRpcClient::RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels) {
      for(int i = 0; i < channels.size(); ++i) {
        stubs.emplace_back(RaftRpc::NewStub(channels[i]));
      }
    }
    bool RaftRpcClient::RpcRequestVote(size_t id, const RpcRequestVoteRequest &request, RpcRequestVoteReply &reply) {
      grpc::ClientContext context;
      grpc::Status status = stubs[id]->RpcRequestVote(&context, request, &reply);
      std::cout <<"sending..." << std::endl;
      return status.ok();
    }
    void RaftRpcClient::RpcRequestVotes(const RpcRequestVoteRequest &request) {
    	for(size_t i = 0; i < stubs.size(); ++i) {
		     RpcRequestVoteReply reply;
    		if(RpcRequestVote(i, request, reply)) {
    			std::cout << "The local receives the reply from the server " << i << " :" << reply.term() << std::endl; 
    		}
        else std::cout << "error..." << std::endl;
    	}
    }
  }
}

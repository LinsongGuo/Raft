#include "RaftRpcClient.h"

namespace Raft {
  namespace Rpc {
    RaftRpcClient::RaftRpcClient(std::vector<std::shared_ptr<grpc::Channel> > channels) {
      for(int i = 0; i < channels.size(); ++i) {
        stubs.emplace_back(RaftRpc::NewStub(channels[i]));
      }
    }
    bool RaftRpcClient::RequestVote(size_t id, const RequestVoteRequest &request, RequestVoteReply &reply) {
      grpc::ClientContext context;
      grpc::Status status = stubs[id]->RequestVote(&context, request, &reply);
      std::cout <<"sending..." << std::endl;
      return status.ok();
    }
    void RaftRpcClient::RequestVotes(const RequestVoteRequest &request) {
    	for(size_t i = 0; i < stubs.size(); ++i) {
		     RequestVoteReply reply;
    		if(RequestVote(i, request, reply)) {
    			std::cout << "The local receives the reply from the server " << i << " :" << reply.term() << std::endl; 
    		}
        else std::cout << "error..." << std::endl;
    	}
    }
  }
}

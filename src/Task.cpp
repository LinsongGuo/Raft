#include "Task.h"

namespace Raft {
	namespace Task {
    /*Put::Put(std::string _key, std::string _args, boost::promise<bool> _prm):
      key(_key), args(_args), prm(_prm) {;}
    
    Get::Get(std::string _key, boost::promise<std::pair<bool, std::string> > _prm):
      key(_key), prm(_prm){;}
    */
    RespondRequestVote::RespondRequestVote(RequestVoteRequest _request, const boost::promise<RequestVoteReply> &_prm):
      request(_request), prm(std::make_shared<boost::promise<RequestVoteReply> >(_prm)) {;}  
  } 
}

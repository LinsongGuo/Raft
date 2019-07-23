#ifndef RAFT_QUEUE_H
#define RAFT_QUEUE_H

#include <queue>
#include <boost/thread/future.hpp>
#include "defines.h"

namespace Raft {
  namespace Task {
    enum Type {
      put, get, respondRequestVote, respondAppendEntires, heartbeat, transform  
    };

    struct Put {
     /* std::string key, args;
      std::shared_ptr<boost::promise<bool> > prm;
      Put(std::string _key, std::string _args, boost::promise<bool> _prm);
    */
    };
    struct Get {
     /* std::string key;
      boost::promise<std::pair<bool, std::string> > prm;
      Get(std::string _key, boost::promise<std::pair<bool, std::string> > _prm);
    */};
    struct RespondRequestVote {
      RequestVoteRequest request;
      std::shared_ptr<boost::promise<RequestVoteReply> > prm;
      RespondRequestVote(RequestVoteRequest _request, const boost::promise<RequestVoteReply> &_prm);
    };
    struct RespondAppendEntries {

    };
    struct Heartbeat {

    };
    struct Transform {
      
    };
  }  
}

#endif

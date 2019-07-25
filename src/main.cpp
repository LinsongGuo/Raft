#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main() {
  srand(time(NULL));
  std::string fileName;
  std::cin >> fileName;
	std::unique_ptr<Raft::RaftServer>raftServer(new Raft::RaftServer(fileName));
  raftServer->start();
  //raftServer->RequestVotes();
  return 0;
}

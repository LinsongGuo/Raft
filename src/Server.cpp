#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));
  std::string id = argv[1];
  std::string fileName = "server" + id + ".json";
  std::unique_ptr<Raft::RaftServer>Server(new Raft::RaftServer(fileName));
  Server->start();
  while(1);
  Server->shutdown();
  std::cout<<"over!" << std::endl;
  return 0;
}

#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));
  std::string fileName = argv[1];
  std::unique_ptr<Raft::RaftServer>Server(new Raft::RaftServer(fileName));
  std::string opt = argv[2];
  if(opt == "restart") Server->restart();
  else Server->start();
  boost::this_thread::sleep_for(boost::chrono::milliseconds(1000000000));
  Server->shutdown();
  std::cout<<"over!" << std::endl;
  return 0;
}

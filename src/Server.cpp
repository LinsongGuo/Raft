#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main(int argc, char *argv[]) {
  //std::cout << getpid() << std::endl;
  //getchar();
  srand(time(NULL));
  std::string id = argv[1];
  std::string fileName = "server" + id + ".json";
  std::cout <<"get " << fileName << std::endl;
  std::unique_ptr<Raft::RaftServer>Server(new Raft::RaftServer(fileName));
  Server->start();
  boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
  Server->shutdown();
  std::cout<<"over!" << std::endl;
  return 0;
}

#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main() {
  srand(time(NULL));
  std::string fileName;
  std::cin >> fileName;
	std::unique_ptr<Raft::RaftServer>Server(new Raft::RaftServer(fileName));
  Server->start();
  std::cout<<"begin slepp" << std::endl;
  boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
  Server->shutdown();
  std::cout<<"over!" << std::endl;
  return 0;
}

#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main(int argc, char *argv[]) {
  
  srand(time(NULL));
  
  std::vector<std::unique_ptr<Raft::RaftServer> > Server;
  for(int i = 0; i < argc; ++i) {
    std::string fileName = argv[i + 1];
    Server.push_back(std::make_unique<Raft::RaftServer>(fileName));
  }
  
  std::vector<boost::thread> threads;
  for(int i = 0; i < argc; ++i) {
     threads.push_back(boost::thread([&]{
      Server[i]->start();
      boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
      Server[i]->shutdown();
     }));
  }
  
  for(int i = 0; i < argc; ++i) {
    threads[i].interrupt();
    threads[i].join();
  }
  
  std::cout<<"over!" << std::endl;
  
  return 0;
}

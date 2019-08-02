#include <cstdlib>
#include <ctime>
#include "RaftServer.h"

int main(int argc, char *argv[]) {
  
  srand(time(NULL));
  std::vector<std::shared_ptr<Raft::RaftServer> > Server;
  for(int i = 0; i < argc - 1; ++i) {
    std::string fileName = argv[i + 1];
    std::cout << i <<' ' << fileName << std::endl;
    Server.push_back(std::make_shared<Raft::RaftServer>(fileName));
  }
  //std::cout << "end server " << std::endl;
  /*std::array<boost::thread, 10> threads;
  for(int i = 0; i < argc - 1; ++i) {
     threads[i] = std::move(boost::thread([&]{
      Server[i]->start();
      boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
      Server[i]->shutdown();
     }));
  }
  std::cout <<"end thread " << std::endl;
  for(int i = 0; i < argc - 1; ++i) {
    threads[i].join();
  }*/
  
  boost::thread t1 =
  boost::thread([&]{
    Server[0]->start();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
    Server[0]->shutdown();
  }); 

  boost::thread t2 =
  boost::thread([&]{
    Server[1]->start();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
    Server[1]->shutdown();
  });
  
  boost::thread t3 =
  boost::thread([&]{
    Server[2]->start();
    boost::this_thread::sleep_for(boost::chrono::milliseconds(10000000));
    Server[2]->shutdown();
  }); 

  t1.join();
  t2.join();
  t3.join();

  std::cout<<"over!" << std::endl;
  
  return 0;
}

#include <iostream>
#include <cstdlib>
#include <ctime>
#include "RaftExternalClient.h"

int main() {
  srand(time(NULL));
  std::string fileName = "client.json";
  Raft::External::RaftExternalClient Client(fileName);
  int n;
  std::cin >> n;
  std::string opt, key, args;
  for(int i = 1; i <= n; ++i) {
    std::cin >> opt;
    if(opt == "put") {
      std::cin >> key >> args;
      std::cerr << opt << ' ' << key << ' ' << args << std::endl;
      Client.Put(key, args);
    }
    else {
      std::cin >> key;
      std::cerr << opt << ' ' << key << std::endl; 
      std::cout << Client.Get(key) << std::endl;
    }
  }
  return 0;
}

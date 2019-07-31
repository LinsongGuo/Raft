#include <iostream>
#include <cstdlib>
#include <ctime>
#include "RaftExternalClient.h"

int main() {
  srand(time(NULL));
  std::string fileName;
  std::cin >> fileName;
  Raft::External::RaftExternalClient Client(fileName);
  int n;
  std::cin >> n;
  std::string opt, key, args;
  for(int i = 1; i <= n; ++i) {
    std::cin >> opt;
    if(opt == "put") {
      std::cin >> key >> args;
      Client.Put(key, args);
    }
    else {
      std::cin >> key;
      std::cout << Client.Get(key) << std::endl;
    }
  }
  return 0;
}

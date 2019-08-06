#include <iostream>
#include <cstdlib>
#include <ctime>
#include "RaftExternalClient.h"

int main(int argc, char *argv[]) {
  srand(time(NULL));
  std::string fileName = "client.json";
  Raft::External::RaftExternalClient Client(fileName);
  std::string opt, key, args;

  std::string id = argv[1];
  std::ofstream fout("client" + id);

  while (std::cin.peek() != EOF) {
    std::string type;
    std::cin >> type;
    if (type == "put") {
      std::string k, v;
      std::cin >> k >> v;
      fout << type << ' ' << k << ' ' << v << std::endl;
      Client.Put(k, v);
    } else {
      std::string k;
      std::cin >> k;
      fout << type << ' ' << k << std::endl;
      std::cout << k << " " << Client.Get(k) << std::endl;
    }

    while (std::isspace(std::cin.peek()))
      std::cin.get();
  }
  return 0;
}

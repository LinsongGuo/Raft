#include <cassert>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

int main(int argc, char **argv) {
  std::string cmd;
  std::unordered_map<std::string, std::string> pseudo;

  while (std::cin.peek() != EOF) {
    std::string type;
    std::cin >> type;
    if (type == "put") {
      std::string k, v;
      std::cin >> k >> v;
      pseudo[k] = v;
    } else {
      std::string k;
      std::cin >> k;
      std::cout << k << " " << pseudo.at(k) << std::endl;
    }

    while (std::isspace(std::cin.peek()))
      std::cin.get();
  }
  return 0;
}

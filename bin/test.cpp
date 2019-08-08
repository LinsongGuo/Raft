#include <cassert>
#include <cctype>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

class RaftTest {
private:
  using AnswerMap =
      std::unordered_map<std::string, std::unordered_set<std::string>>;

public:
  RaftTest(std::string srvCmd, std::string clientCmd)
      : srvCmd(std::move(srvCmd)), clientCmd(std::move(clientCmd)) {}

  bool naive(std::size_t nServers, std::size_t nClients,
             std::size_t nKvPairs) const {
    using namespace std::chrono_literals;
    assert(nServers <= 5);
    // generate the test case
    std::cerr << "generating the test cases: nServers = " << nServers
              << ", nClients = " << nClients << ", nKvPairs = " << nKvPairs
              << std::endl;
    auto kv = generateRandomKvPairs(nKvPairs);
    std::ofstream fout("naive_test.in");
    dumpKvPairs(kv, fout);
    AnswerMap ans;
    simulate(ans, kv);

    // test
    std::cerr << "running...\n";
    auto servers = createServers(nServers);
    auto clients = createClients(nClients, "naive_test.in");
    for (auto pid : clients)
      waitpid(pid, nullptr, 0);
    for (auto pid : servers)
      kill(pid, SIGTERM);

    // check the result
    std::cerr << "checking...\n";
    for (std::size_t i = 0; i < nClients; ++i) {
      std::ifstream fin("client" + std::to_string(i) + ".out");
      auto res = readResult(fin);
      if (!checkResult(ans, res))
        return false;
    }
    return true;
  }

  bool comprehensive(std::size_t testCaseSize, double p,
                     std::size_t timeOut = 180) const {
    using namespace std::chrono_literals;

    // generate the test case
    std::cerr << "generating the test cases: testCaseSize = " << testCaseSize
              << ", p = " << p << std::endl;
    auto kvs = generateRandomKvPairs(testCaseSize);
    std::ofstream fout("comprehensive_test.in");
    dumpKvPairs(kvs, fout);
    fout.close();
    AnswerMap ans;
    simulate(ans, kvs);

    // test
    std::cerr << "running...\n";
    std::size_t nServers = 5, nClients = 5;
    auto servers = createServers(nServers, 0000);
    auto clients = createClients(nClients, "comprehensive_test.in");

    std::mt19937 eng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> serversDist(0, nServers - 1);
    auto randServer = std::bind(serversDist, eng);
    std::uniform_real_distribution<double> uniformDist(0, 1);
    auto rand = std::bind(uniformDist, eng);

    auto start = std::chrono::system_clock::now();
    while (std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now() - start)
               .count() < timeOut) {
      std::this_thread::sleep_for(2s);
      auto x = rand();
      auto srvIdx = randServer();
      auto srv = servers.at(srvIdx);
      if (x <= p) {
        kill(srv, SIGSTOP);
        std::cerr << "pause server " << srvIdx << std::endl;
      } else {
        kill(srv, SIGCONT);
        std::cerr << "continue server " << srvIdx << std::endl;
      }
    }

	for (auto pid : servers) 
	  kill(pid, SIGCONT);
	  
    for (auto pid : clients)
      kill(pid, SIGTERM);

    fout.open("comprehensive_test_get.in");
    for (auto &kv : kvs)
      fout << "get " << kv.first << std::endl;

    clients.clear();
    clients = createClients(nClients, "comprehensive_test_get.in", nClients);
    for (auto pid : clients)
      waitpid(pid, nullptr, 0);
    for (auto pid : servers)
      kill(pid, SIGTERM);

    // check the result
    std::cerr << "checking...\n";
    for (std::size_t i = 0; i < nClients; ++i) {
      std::ifstream fin("client" + std::to_string(i) + ".out");
      auto res = readResult(fin);
      if (!checkResult(ans, res))
        return false;
    }
    for (auto &kv : kvs) {
      ans[kv.first] = {kv.second};
    }
    for (std::size_t i = nClients; i < nClients * 2; ++i) {
      std::ifstream fin("client" + std::to_string(i) + ".out");
      auto res = readResult(fin);
      if (!checkResult(ans, res))
        return false;
    }
    return true;
  }

private:
  std::vector<std::pair<std::string, std::string>>
  generateRandomKvPairs(std::size_t nKvPairs) const {
    std::mt19937 eng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 100);

    std::vector<std::pair<std::string, std::string>> res;
    res.reserve(nKvPairs);
    for (std::size_t i = 0; i < nKvPairs; ++i) {
      res.emplace_back(std::to_string(dist(eng)), std::to_string(dist(eng)));
    }
    return res;
  }

  void dumpKvPairs(const std::vector<std::pair<std::string, std::string>> &kvs,
                   std::ostream &out, bool dumpGets = true) const {
    for (auto &kv : kvs) {
      out << "put " << kv.first << " " << kv.second << std::endl;
    }

    if (!dumpGets)
      return;

    for (auto &kv : kvs) {
      out << "get " << kv.first << std::endl;
    }
  }

  void
  simulate(AnswerMap &mp,
           const std::vector<std::pair<std::string, std::string>> &puts) const {
    for (auto &kv : puts) {
      mp[kv.first].emplace(kv.second);
    }
  }

  std::vector<std::pair<std::string, std::string>>
  readResult(std::istream &in) const {
    std::vector<std::pair<std::string, std::string>> res;
    std::string k, v;
    while (in.peek() != EOF) {
      in >> k >> v;
      res.emplace_back(k, v);
      while (std::isspace(in.peek()))
        in.get();
    }
    return res;
  }

  bool checkResult(
      const AnswerMap &mp,
      const std::vector<std::pair<std::string, std::string>> &output) const {
    if (output.empty())
      return false;
    for (auto &kv : output) {
      if (mp.find(kv.first) == mp.end())
        return false;
      auto &acceptable = mp.at(kv.first);
      if (acceptable.find(kv.second) == acceptable.end())
        return false;
    }
    return true;
  }

private:
  std::vector<pid_t> createServers(std::size_t nServers,
                                   std::uint32_t interval = 0,
                                   std::size_t baseId = 0) const {
    std::vector<pid_t> res;
    res.reserve(nServers);
    for (std::size_t i = 0; i < nServers; ++i) {
      res.emplace_back(createServer(baseId + i));
      std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
    return res;
  }

  pid_t createServer(std::size_t id) const {
    std::cerr << "creating server " << id << std::endl;
    auto pid = fork();
    if (pid == 0) { // child
      int devNull = open("/dev/null", O_WRONLY);
      assert(devNull >= 0);
      dup2(devNull, STDOUT_FILENO);
      close(devNull);

      auto res = execl(srvCmd.c_str(), "server", std::to_string(id).c_str());
      assert(false);
    }
    return pid;
  }

  std::vector<pid_t> createClients(std::size_t nClients,
                                   const std::string &inputFile,
                                   std::size_t baseId = 0) const {
    std::vector<pid_t> res;
    res.reserve(nClients);
    for (std::size_t i = 0; i < nClients; ++i) {
      res.emplace_back(createClient(baseId + i, inputFile));
    }
    return res;
  }

  pid_t createClient(std::size_t id, const std::string &inputFile) const {
    std::cerr << "creating client " << id << std::endl;
    auto pid = fork();
    if (pid == 0) { // child
      std::string file = "client" + std::to_string(id) + ".out";
      int fd =
          open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR);
      dup2(fd, STDOUT_FILENO);
      close(fd);
      fd = open(inputFile.c_str(), O_RDONLY, S_IRUSR);
      dup2(fd, STDIN_FILENO);
      close(fd);

      auto tmp = execl(clientCmd.c_str(), "client", std::to_string(id).c_str());
      assert(false);
    }
    return pid;
  }

  bool checkAlive(const std::vector<pid_t> &pids) const {
    for (auto pid : pids) {
      if (waitpid(pid, nullptr, WNOHANG) == pid)
        return true;
    }
    return false;
  }

private:
  std::string srvCmd, clientCmd;
};

// raft-test <server> <client>
int main(int argc, char **argv) {
  assert(argc == 3);
  RaftTest test(argv[1], argv[2]);
  
  unsigned long long t1 = clock();
  
  system("bash ./dir.sh");
  std::cerr << "running naive_test0:\n";
  std::cerr << (test.naive(3, 1, 10) ? "passed" : "failed");
  
  unsigned long long t2 = clock();
  std::cerr << std::endl << t2 - t1 << std::endl;
  
  system("bash ./dir.sh");
  std::cerr << "running naive_test1:\n";
  std::cerr << (test.naive(3, 3, 100) ? "passed" : "failed");
  
  unsigned long long t3 = clock();
  std::cerr << std::endl << t3 - t2 << std::endl;
  
  system("bash ./dir.sh");
  std::cerr << "running naive_test2:\n";
  std::cerr << (test.naive(5, 5, 5000) ? "passed" : "failed");
  
  unsigned long long t4 = clock();
  std::cerr << std::endl << t4 - t3 << std::endl;
  
  system("bash ./dir.sh");
  std::cerr << "running naive_test3:\n";
  std::cerr << (test.naive(3, 5, 5000) ? "passed" : "failed");
  
  unsigned long long t5 = clock();
  std::cerr << std::endl << t5 - t4 << std::endl;
  
  system("bash ./dir.sh");
  std::cerr << "running comprehensive_test:\n";
  std::cerr << (test.comprehensive(1000, 0.3, 300) ? "passed" : "failed");
  
  unsigned long long t6 = clock();
  std::cerr << std::endl << t6 - t5 << std::endl;

  return 0;
}

#include "Role.h"

namespace Raft {
  Role::Role(std::shared_ptr<RaftServerInfo> _info, 
    std::shared_ptr<RaftServerCluster> _cluster, 
    std::shared_ptr<Rpc::RaftRpcClient> _rpcClient,
    std::shared_ptr<Transformer> _transformer,
    std::fstream &_logScanner):
    info(_info), cluster(_cluster), rpcClient(_rpcClient), transformer(_transformer), logScanner(_logScanner) {;} 
}
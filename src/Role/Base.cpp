#include "Base.h"

namespace Raft {
  Base::Base(const RaftServerInfo &_info, const RaftServerCluster &_cluster, const Rpc::RaftRpcClient &_rpcClient) {
    info = std::make_shared<RaftServerInfo>(_info);
    cluster = std::make_shared<RaftServerCluster>(_cluster);
    rpcClient = std::make_shared<Raft::Rpc::RaftRpcClient>(_rpcClient);
  } 
}
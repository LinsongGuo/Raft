# Raft

The Raft consensus protocol is implemented by c++11, grpc, boost 1.68.0 and cmake 3.10. 

## The Raft Cluster


The are five servers in the raft cluster. The server's address is 127.0.0.1, 127.0.0.2, 127.0.0.3, 127.0.0.4 and 127.0.0.5, respectively. Every server consists of grpc client(127.0.0.x:50001), grpc server(127.0.0.x:50001) and external server(127.0.0.x:50002). In order to test the correctness, there's a client composed of five external clients(127.0.0.x:50002) in the raft cluster. 

The grpc client(127.0.0.x:50001): send Requestvote, Heartbeat and Appendentries to all the other grpc servers. 

The grpc server(127.0.0.x:50001): receive and reply Requestvote, Heartbeat and Appendentries from all the other grpc clients. 

The external client(127.0.0.x:50001): send Put and Get to all the externl servers.

The external server(127.0.0.x:50001): receive and reply Put and Get from all the external clients. 

## C++ Source Folder Structure

├── External

│   ├── RaftExternal.pb.cc

│   ├── RaftExternal.grpc.pb.cc

│   ├── RaftExternalClient.cpp

│   └── RaftExternalServer.cpp

├── Role

│   ├── Role.cpp

│   ├── Follower.cpp

│   ├── Candidate.cpp

│   └── Leader.cpp

├── Rpc

│   ├── RaftRpc.pb.cc

│   ├── RaftRpc.grpc.pb.cc

│   ├── RaftRpcClient.cpp

│   └── RaftRpcServer.cpp

├── defines.cpp

├── Task.cpp

├── Transformer.cpp

├── RaftServer.cpp

├── Client.cpp

└── Server.cpp


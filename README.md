# Raft

The Raft consensus protocol is implemented by c++11, grpc, boost 1.68.0 and cmake 3.10. 

## The Raft Cluster

There are five servers in the raft cluster. The server's address is 127.0.0.1, 127.0.0.2, 127.0.0.3, 127.0.0.4 and 127.0.0.5, respectively. Every server consists of grpc client(127.0.0.x:50001), grpc server(127.0.0.x:50001) and external server(127.0.0.x:50002). In order to test the correctness, there's also a client composed of five external clients(127.0.0.x:50002) in the raft cluster. 

The grpc client(127.0.0.x:50001): send Requestvote, Heartbeat and Appendentries to all the other grpc servers.
 
The grpc server(127.0.0.x:50001): receive and reply Requestvote, Heartbeat and Appendentries from all the other grpc clients. 

The external client(127.0.0.x:50002): send Put and Get to all the externl servers.

The external server(127.0.0.x:50002): receive and reply Put and Get from all the external clients. 

## C++ Source Folder Structure

![avatar](https://github.com/LinsongGuo/Raft/blob/master/structure.jpg)

## Test

[√] 10000 Put/Get entries test. 

[√] 10000 Put/Get entries test with only 3 servers built.

[√] 10000 Put/Get entries test with 2 followers deaded.

[√] 10000 Put/Get entries test with one old leader deaded and another new leader elected. 

[√] 10000 Put/Get entries test with a leader and a follower deaded. 

[√] 5000 Put/Get entries test with two leaders deaded. 

[√] 10000 Put/Get entries test with 5 clients. 

[√] 5000(start) + 5000(restart) Put/Get entries test. 

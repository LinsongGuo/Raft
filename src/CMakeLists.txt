add_executable(Server Server.cpp
					  defines.cpp
					  Task.cpp
					  RaftServer.cpp
					  Transformer.cpp
					  External/RaftExternal.grpc.pb.cc
					  External/RaftExternal.pb.cc
					  External/RaftExternalClient.cpp
					  External/RaftExternalServer.cpp
					  Role/Role.cpp
					  Role/Follower.cpp
					  Role/Candidate.cpp
					  Role/Leader.cpp
					  Rpc/RaftRpc.grpc.pb.cc
					  Rpc/RaftRpc.pb.cc
					  Rpc/RaftRpcClient.cpp
					  Rpc/RaftRpcServer.cpp)
					  
target_link_libraries(Server
	PRIVATE
	grpc++
	gpr
	${Boost_LIBRARIES} 
	${PROTOBUF_LIBRARIES} 
	${GRPC_LIBRARIES}
)

TARGET_INCLUDE_DIRECTORIES(Server PRIVATE ${Boost_INCLUDE_DIRS})



add_executable(Client Client.cpp
					  defines.cpp
					  Task.cpp
					  RaftServer.cpp
					  Transformer.cpp
					  External/RaftExternal.grpc.pb.cc
					  External/RaftExternal.pb.cc
					  External/RaftExternalClient.cpp
					  External/RaftExternalServer.cpp
					  Role/Role.cpp
					  Role/Follower.cpp
					  Role/Candidate.cpp
					  Role/Leader.cpp
					  Rpc/RaftRpc.grpc.pb.cc
					  Rpc/RaftRpc.pb.cc
					  Rpc/RaftRpcClient.cpp
					  Rpc/RaftRpcServer.cpp)
					  
target_link_libraries(Client
	PRIVATE
	grpc++
	gpr
	${Boost_LIBRARIES} 
	${PROTOBUF_LIBRARIES} 
	${GRPC_LIBRARIES}
)

TARGET_INCLUDE_DIRECTORIES(Client PRIVATE ${Boost_INCLUDE_DIRS})


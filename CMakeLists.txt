cmake_minimum_required(VERSION 3.10)

project(raft)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

#set(Boost_USE_STATIC_LIBS FALSE)
find_package(Boost REQUIRED COMPONENTS thread chrono)

find_package(Protobuf REQUIRED)

find_package(GRPC REQUIRED)

include_directories(
	include
	include/Rpc
	include/External	
	${Boost_INCLUDE_DIR}
	${PROTOBUF_INCLUDE_DIR}
	${GRPC_INCLUDE_DIR}
)

add_subdirectory(proto)
add_subdirectory(src)


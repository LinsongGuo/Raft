cmake_minimum_required(VERSION 3.10)

project(raft)

add_compile_options(-O2)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin) 

set(Boost_USE_STATIC_LIBS FALSE)
find_package(Boost REQUIRED COMPONENTS thread chrono)
add_definitions(
        -D BOOST_THREAD_VERSION=4
        -D BOOST_LOG_DYN_LINK
)

find_package(Protobuf REQUIRED)

find_package(GRPC REQUIRED)

include_directories(
	include
	include/Rpc
	include/External
	include/Role	
	${Boost_INCLUDE_DIR}
	${PROTOBUF_INCLUDE_DIR}
	${GRPC_INCLUDE_DIR}
)

add_subdirectory(protobuf)
add_subdirectory(src)


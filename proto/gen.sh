protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` RaftRpc.proto
protoc -I=. --cpp_out=. RaftRpc.proto
mv RaftRpc.pb.h ../include/Rpc/
mv RaftRpc.grpc.pb.h ../include/Rpc/
mv RaftRpc.pb.cc ../src/Rpc/
mv RaftRpc.grpc.pb.cc ../src/Rpc/

protoc -I=. --cpp_out=. RaftRpc.proto
protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` RaftRpc.proto

protoc -I=. --cpp_out=. External.proto
protoc -I=. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` External.proto

mv RaftRpc.pb.h ../include/Rpc/
mv RaftRpc.grpc.pb.h ../include/Rpc/
mv RaftRpc.pb.cc ../src/Rpc/
mv RaftRpc.grpc.pb.cc ../src/Rpc/

mv External.pb.h ../include/External/
mv External.grpc.pb.h ../include/External/
mv External.pb.cc ../src/External/
mv External.grpc.pb.cc ../src/External/



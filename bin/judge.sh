echo "executing std..."
./std <comprehensive_test.in >std.out

echo "comparing client5.out and std.out..."
diff client5.out std.out
echo "comparing client6.out and std.out..."
diff client6.out std.out
echo "comparing client7.out and std.out..."
diff client7.out std.out
echo "comparing client8.out and std.out..."
diff client8.out std.out
echo "comparing client9.out and std.out..."
diff client9.out std.out

echo "comparing log/server0 and log/server1..."
diff log/server0 log/server1
echo "comparing log/server0 and log/server2..."
diff log/server0 log/server2
echo "comparing log/server0 and log/server3..."
diff log/server0 log/server3
echo "comparing log/server0 and log/server4..."
diff log/server0 log/server4


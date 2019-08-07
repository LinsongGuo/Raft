./std <comprehensive_test.in >std.out

echo "comparing client0.out and std.out..."
diff client0.out std.out
echo "comparing client1.out and std.out..."
diff client1.out std.out
echo "comparing client2.out and std.out..."
diff client2.out std.out
echo "comparing client3.out and std.out..."
diff client3.out std.out
echo "comparing client4.out and std.out..."
diff client4.out std.out


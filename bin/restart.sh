echo "restart the raft cluster with 5 servers..."
gnome-terminal --title="Server1" --window -x bash -c "./Server server1.json restart"
gnome-terminal --title="Server2" --window -x bash -c "./Server server2.json restart"
gnome-terminal --title="Server3" --window -x bash -c "./Server server3.json restart"
gnome-terminal --title="Server4" --window -x bash -c "./Server server4.json restart"
gnome-terminal --title="Server5" --window -x bash -c "./Server server5.json restart"

echo "executing the client2..."
./Client <test2.in >raft2.out

<<COMMENT
cat raft1.out raft2.out > raft.out
echo "comparing raft.out and std.out..."
diff raft.out std.out
COMMENT

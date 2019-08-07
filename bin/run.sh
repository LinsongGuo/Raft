rm -rf server0
rm -rf server1
rm -rf server2
rm -rf server3
rm -rf server4

mkdir server0
mkdir server1
mkdir server2
mkdir server3
mkdir server4

rm -rf log
mkdir log
touch log/server0
touch log/server1
touch log/server2
touch log/server3
touch log/server4

echo "executing std..."
bash -c "./std <test.in >std.out"

sleep 1s

echo "building the raft cluster with 5 servers..."
gnome-terminal --title="Server0" --window -x bash -c "./Server 0"
gnome-terminal --title="Server1" --window -x bash -c "./Server 1"
gnome-terminal --title="Server2" --window -x bash -c "./Server 2"
gnome-terminal --title="Server3" --window -x bash -c "./Server 3"
gnome-terminal --title="Server4" --window -x bash -c "./Server 4"

sleep 3s

<<COMMENT

echo "executing Client..."
./Client 0 <test.in >raft.out

sleep 1s

echo "comparing raft.out and std.out..."
diff raft.out std.out
COMMENT

sleep 5s

echo "executing Client..."
gnome-terminal --title="Client0" --window -x bash -c "./Client 0 <test.in >raft.out"
gnome-terminal --title="Client1" --window -x bash -c "./Client 1 <test.in >raft1.out"
gnome-terminal --title="Client2" --window -x bash -c "./Client 2 <test.in >raft2.out"
gnome-terminal --title="Client3" --window -x bash -c "./Client 3 <test.in >raft3.out"
gnome-terminal --title="Client4" --window -x bash -c "./Client 4 <test.in >raft4.out"

<<COMMENT
sleep 60s
echo "comparing raft0.out and std.out..."
diff raft0.out std.out
echo "comparing raft1.out and std.out..."
diff raft1.out std.out
echo "comparing raft2.out and std.out..."
diff raft2.out std.out
echo "comparing raft3.out and std.out..."
diff raft3.out std.out
echo "comparing raft4.out and std.out..."
diff raft4.out std.out
COMMENT

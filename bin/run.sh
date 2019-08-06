rm -rf 127.0.0.1
rm -rf 127.0.0.2
rm -rf 127.0.0.3
rm -rf 127.0.0.4
rm -rf 127.0.0.5

mkdir 127.0.0.1
mkdir 127.0.0.2
mkdir 127.0.0.3
mkdir 127.0.0.4
mkdir 127.0.0.5

rm -rf log
mkdir log
touch log/127.0.0.1
touch log/127.0.0.2
touch log/127.0.0.3
touch log/127.0.0.4
touch log/127.0.0.5

echo "executing std..."
bash -c "./std <test.in >std.out"

sleep 1s

echo "building the raft cluster with 5 servers..."
gnome-terminal --title="Server1" --window -x bash -c "./Server server1.json"
gnome-terminal --title="Server2" --window -x bash -c "./Server server2.json"
gnome-terminal --title="Server3" --window -x bash -c "./Server server3.json"

sleep 3s

echo "executing Client..."
gnome-terminal --title="Client" --window -x bash -c "./Client <test.in >raft.out"

sleep 2s

gnome-terminal --title="Server4" --window -x bash -c "./Server server4.json"

<<COMMENT

gnome-terminal --title="Server5" --window -x bash -c "./Server server5.json"

sleep 5s

echo "executing Client..."
gnome-terminal --title="Client1" --window -x bash -c "./Client <test.in >raft1.out"
gnome-terminal --title="Client2" --window -x bash -c "./Client <test.in >raft2.out"
gnome-terminal --title="Client3" --window -x bash -c "./Client <test.in >raft3.out"
gnome-terminal --title="Client4" --window -x bash -c "./Client <test.in >raft4.out"
gnome-terminal --title="Client5" --window -x bash -c "./Client <test.in >raft5.out"


sleep 60s
echo "comparing raft1.out and std.out..."
diff raft1.out std.out
echo "comparing raft2.out and std.out..."
diff raft2.out std.out
echo "comparing raft3.out and std.out..."
diff raft3.out std.out
echo "comparing raft4.out and std.out..."
diff raft4.out std.out
echo "comparing raft5.out and std.out..."
diff raft5.out std.out
COMMENT


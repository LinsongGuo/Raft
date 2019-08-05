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

echo "start the raft cluster with 5 servers..."
gnome-terminal --title="Server1" --window -x bash -c "./Server server1.json start"
gnome-terminal --title="Server2" --window -x bash -c "./Server server2.json start"
gnome-terminal --title="Server3" --window -x bash -c "./Server server3.json start"
#gnome-terminal --title="Server4" --window -x bash -c "./Server server4.json start"
#gnome-terminal --title="Server5" --window -x bash -c "./Server server5.json start"

#echo "executing the client..."
#./Client <test.in >raft.out

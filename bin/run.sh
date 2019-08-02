rm -rf localhost:50001
rm -rf localhost:50002
rm -rf localhost:50003

mkdir localhost:50001
mkdir localhost:50002
mkdir localhost:50003

gnome-terminal --title="Client" --window -x bash -c "./Client client.json"

gnome-terminal --title="Server" --window -x bash -c "./Server server1.json server2.json server3.json"

#gnome-terminal --title="Server1" --window -x bash -c "./Server server1.json"
#gnome-terminal --title="Server2" --window -x bash -c "./Server server2.json"
#gnome-terminal --title="Server3" --window -x bash -c "./Server server3.json"

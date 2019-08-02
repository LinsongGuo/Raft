rm -rf 127.0.0.1:50001
rm -rf 127.0.0.2:50001
rm -rf 127.0.0.3:50001
rm -rf 127.0.0.4:50001
rm -rf 127.0.0.5:50001

mkdir 127.0.0.1:50001
mkdir 127.0.0.2:50001
mkdir 127.0.0.3:50001
mkdir 127.0.0.4:50001
mkdir 127.0.0.5:50001

gnome-terminal --title="Client" --window -x bash -c "./Client client.json"

gnome-terminal --title="Server" --window -x bash -c "./Server server1.json server2.json server3.json server4.json server5.json"

#gnome-terminal --title="Server1" --window -x bash -c "./Server server1.json"
#gnome-terminal --title="Server2" --window -x bash -c "./Server server2.json"
#gnome-terminal --title="Server3" --window -x bash -c "./Server server3.json"

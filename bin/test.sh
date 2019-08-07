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

echo "executing test..."
./test ./Server ./Client



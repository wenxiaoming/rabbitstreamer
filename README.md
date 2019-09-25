# rabbitstreamer
Rabbitstreamer is the P2P based media streaming suite, includes streaming server which can stream media through P2P protocol, streaming player which can play the streaming data from streaming server and the peers.

Here is the architecture of the system

Super Peer and Tracker Server are implemented in this project.

RabbitPlayer is implemented in [rabbitplayer](https://github.com/wenxiaoming/rabbitplayer).

P2PCapturer is implemented in [p2pcapturer](https://github.com/wenxiaoming/p2pcapturer).

![system architecture](https://github.com/wenxiaoming/rabbitstreamer/blob/master/doc/system_architecture.png)

Here is the architecture of the rabbitstreamer

![rabbitstreamer architecture](https://github.com/wenxiaoming/rabbitstreamer/blob/master/doc/rabbitstreamer_architecture.png)

## How to sync and build

git clone https://github.com/wenxiaoming/rabbitstreamer.git

cd rabbitstreamer/

git submodule init

git submodule update

cd third_party/googletest/

mkdir build

cd build/

cmake ../ -DCMAKE_BUILD_TYPE=Debug

make

cd ../../../

./build_all.sh 


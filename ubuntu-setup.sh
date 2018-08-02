#!/bin/bash
sudo apt-get install build-essential pkg-config libssl-dev libevent-dev
sudo apt-get install libboost-all-dev
sudo add-apt-repository ppa:bitcoin/bitcoin
sudo apt-get update
sudo apt-get install libdb4.8-dev libdb4.8++-dev
sudo apt-get install libminiupnpc-dev
apt-get install qt5-qmake libqt5gui5 libqt5core5 libqt5dbus5 qttools5-dev-tools
sudo apt-get install libqrencode-dev

mkdir /root/CampusCoin
cd /root/CampusCoin
echo "****************     Copying...  *****************"
wget https://github.com/campuscoindev/CampusCoin-Source/files/2252535/Ubuntu-Qt-Update.tar.gz
echo "**************** Uncompressing...*****************"
tar -xvzf Ubuntu-Qt-Update.tar.gz
sleep 2
rm Ubuntu-Qt-Update.tar.gz
chmod +x CampusCoind
chmod +x CampusCoin-qt
echo "****************Job completed successfully, please run /root/CampusCoin/CampusCoin-qt"

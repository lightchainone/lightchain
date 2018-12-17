#!/bin/bash

rm ~/.bitcoin/regtest/debug.log
killall  bitcoind
sleep 2
#./bitcoind -addnode=192.168.10.113:18444  -server=1 -rpcuser=regtest -rpcport=9399 -rpcpassword=regtest  -regtest  -rpcallowip=127.0.0.1 -daemon 
#./bitcoind -addnode=114.67.231.223:18445  -server=1 -rpcuser=regtest -rpcport=9399 -rpcpassword=regtest  -regtest  -rpcallowip=127.0.0.1 -daemon -disablebridgeowner=false -debug=net -txindex=1 
#./bitcoind -addnode=114.67.231.223:18445  -server=1 -rpcuser=regtest -rpcport=9399 -rpcpassword=regtest  -regtest  -rpcallowip=127.0.0.1 -daemon -disablebridgeowner=false -debug=net -txindex=1 -reindex=0 -reindex-chainstate=0 
./bitcoind  -server=1 -rpcuser=regtest -rpcport=9399 -rpcpassword=regtest  -regtest  -rpcallowip=127.0.0.1 -daemon -disablebridgeowner=false -debug=net -txindex=1 -reindex=0 -reindex-chainstate=0
#./bitcoind   -server=1 -rpcuser=regtest -rpcport=9399 -rpcpassword=regtest  -regtest  -rpcallowip=127.0.0.1 -daemon -disablebridgeowner=false -debug=net 

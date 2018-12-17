#!/bin/bash

function gettopnode()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  gettopnode 
}

function newaddr()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  getnewaddress
}

function createsubchaininfo()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  createsubchaininfo "signdata"   34 
}

function getbalance()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  getbalance
}

function getbalance_platform()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  getbalance mpJwvWhVpqy4wKFforJ7pbYewTXX9npgST
}

function generate()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  generate  $1 
}

function send()
{
./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  sendtoaddress  "mpJwvWhVpqy4wKFforJ7pbYewTXX9npgST" 0.1 
}

function run()
{
    generate  100
    generate 10
    getbalance
    createsubchaininfo
    getbalance
}

#./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  getnewaddress 
#./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  listaccounts 
#./bitcoin-cli -regtest -rpcuser=regtest  -rpcport=9399 -rpcpassword=regtest  dumpbridgeowner savefile 

#gettopnode
#newaddr
#createsubchaininfo


#run
#generate 100
#
#    getbalance
#
#    generate 10
#for((i=0;i<200;++i)){
#    generate 10
#}
#    getbalance
#generate 10
#getbalance

#generate 100
#generate 10
getbalance
#send
#createsubchaininfo
#generate 20
#getbalance
#getbalance_platform
#generate  100
#generate 10
#getbalance
#./testapi
#generate 1
#getbalance

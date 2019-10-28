// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CLIAPI_H
#define BITCOIN_CLIAPI_H

struct rpcinfo{
   std::string rpc_conf_file; 
   std::string datadir; 
};


struct createsubchainRequest{
    rpcinfo info;
    std::string subchainowner;
    std::string subchainid;
    std::string subchainname;
    std::string subcoinname;
    std::string subchainseeds;
};


struct commonResponse{
    int retErrCode;
    std::string retErrMsg;
    std::string retDebugInfo;
    std::string retResult;
    commonResponse():retErrCode(0),retErrMsg(""),retDebugInfo(""),retResult(""){
    }
};

struct createsubchainResponse: public commonResponse
{
    std::string extdatahash;
    createsubchainResponse(): extdatahash(""){

    }
};

struct gensubchainblockRequest
{
    rpcinfo info;
    std::string subchainid;
    std::string subblockheight;
    std::string subblockdata;
};


struct gensubchainblockResponse: public commonResponse{
    std::string extdatahash;
    std::string subblockhash;
    gensubchainblockResponse(): extdatahash(""),subblockhash(""){

    }
};

struct commitsubchainRequest{
    rpcinfo info;
    std::string pubkey;
    std::string extdatahashsignature;
    std::string subblockhashsignature;
    std::string wtxdata;
    std::string subblockdata;
};

int createsubchain(const createsubchainRequest& req, createsubchainResponse& res);
int gensubchainblock(const gensubchainblockRequest& req, gensubchainblockResponse& res);
int commitsubchain(const commitsubchainRequest& req, commonResponse& res);
#endif  //BITCOIN_CLIAPI_H

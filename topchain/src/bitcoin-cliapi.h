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


struct gensubchainblockRequest
{
    rpcinfo info;
    std::string subchainid;
    std::string subblockheight;
    std::string subblockdata;
    gensubchainblockRequest(): subchainid(""),subblockheight(""),  subblockdata(""){

    }
};


struct commitsubchainRequest{
    rpcinfo info;
    std::string wtxdata;
    std::string subblockdata;
};

struct genbridgeownerinfoRequest{
    rpcinfo info;
    std::string reset;
    genbridgeownerinfoRequest():reset("0"){}
};

int createsubchain(const createsubchainRequest& req, commonResponse& res);
int gensubchainblock(const gensubchainblockRequest& req, commonResponse& res);
int commitsubchain(const commitsubchainRequest& req, commonResponse& res);
int genbridgeownerinfo(const genbridgeownerinfoRequest& req, commonResponse& res);

extern void RandomInit();
extern std::string SHA256AutoDetect();
extern void ECC_Start(void);
extern void ECC_Stop(void);

#endif  //BITCOIN_CLIAPI_H

// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CLI_H
#define BITCOIN_CLI_H

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "chainparamsbase.h"
#include "clientversion.h"
#include "fs.h"
#include "rpc/client.h"
#include "rpc/protocol.h"
#include "util.h"
#include "utilstrencodings.h"

#include <stdio.h>

#include <event2/buffer.h>
#include <event2/keyvalq_struct.h>
#include "support/events.h"

#include <univalue.h>
#include "random.h"
#include "crypto/sha256.h"

static const char DEFAULT_RPCCONNECT[] = "127.0.0.1";
static const int DEFAULT_HTTP_CLIENT_TIMEOUT=900;
static const bool DEFAULT_NAMED=false;
static const int CONTINUE_EXECUTION=-1;

std::string HelpMessageCli();

//////////////////////////////////////////////////////////////////////////////
//
// Start
//

//
// Exception thrown on connection error.  This error is used to determine
// when to wait if -rpcwait is given.
//
class CConnectionFailed : public std::runtime_error
{
public:

    explicit inline CConnectionFailed(const std::string& msg) :
        std::runtime_error(msg)
    {}

};


/** Reply structure for request_done to fill in */
struct HTTPReply
{
    HTTPReply(): status(0), error(-1) {}

    int status;
    int error;
    std::string body;
};


const char *http_errorstring(int code);

#if LIBEVENT_VERSION_NUMBER >= 0x02010300
static void http_error_cb(enum evhttp_request_error err, void *ctx);
#endif

UniValue CallRPC(const std::string& strMethod, const UniValue& params);

int CommandRPC(const std::string& strMethod, const std::vector<std::string>& args, const int iMaxTryTime, UniValue& errCode, UniValue& errMsg, UniValue& result, std::string& strPrint);


int runCommand(const std::string& conf_file, const std::string& datadir, const std::string& strMethod, const std::vector<std::string>& args, const int maxTryTime, UniValue& errCode, UniValue& errMsg, UniValue& result, std::string& strPrint);
#endif  //BITCOIN_CLI_H

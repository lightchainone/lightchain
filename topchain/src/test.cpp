#include <iostream>
#include "bitcoin-cliapi.h"




using namespace std;

std::string createsubchainAction()
{
   createsubchainRequest req;
   req.info.rpc_conf_file = "bitcoin.conf";
   req.info.datadir = "";

   req.subchainowner = "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4e";
   req.subchainid = "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4e";
   req.subchainname = "subchainname";
   req.subcoinname = "subcoinname";
   req.subchainseeds = "127.0.0.1:80|127.0.0.1:81";

   commonResponse res;

   int ret = createsubchain(req, res);
   cout<<"ret="<<ret<<endl;
   cout<<"retErrorCode="<<res.retErrCode<<endl;
   cout<<"retErrMsg="<<res.retErrMsg<<endl;
   cout<<"retResult="<<res.retResult<<endl;
   return res.retResult;
}

string gensubchainblockAction(const string& subblockdata)
{
   gensubchainblockRequest req;
   req.info.rpc_conf_file = "bitcoin.conf";
   req.info.datadir = "";

   req.subchainid = "cf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4e";
   req.subblockdata = subblockdata; 
   req.subblockheight = "4294967295";

   commonResponse res;

   int ret = gensubchainblock(req, res);
   cout<<"ret="<<ret<<endl;
   cout<<"retErrorCode="<<res.retErrCode<<endl;
   cout<<"retErrMsg="<<res.retErrMsg<<endl;
   cout<<"retDebugInfo="<<res.retDebugInfo<<endl;
   cout<<"result="<<res.retResult<<endl;
   return  res.retResult;
}

void commitsubchainAction(const string& wtxdata,  const string& subblockdata)
{
   commitsubchainRequest req;
   req.info.rpc_conf_file = "bitcoin.conf";
   req.info.datadir = "";

   req.wtxdata = wtxdata;
   req.subblockdata =  subblockdata;

   commonResponse  res;

   int ret = commitsubchain(req, res);
   cout<<"ret="<<ret<<endl;
   cout<<"retErrorCode="<<res.retErrCode<<endl;
   cout<<"retErrMsg="<<res.retErrMsg<<endl;
   cout<<"retResult="<<res.retResult<<endl;
   cout<<"retDebugInfo="<<res.retDebugInfo<<endl;
}

void genbridgeownerinfoAction()
{
   genbridgeownerinfoRequest req;
   req.info.rpc_conf_file = "bitcoin.conf";
   req.info.datadir = "";
   req.reset = "1";
   commonResponse  res;

   int ret = genbridgeownerinfo(req, res);
   cout<<"ret="<<ret<<endl;
   cout<<"retErrorCode="<<res.retErrCode<<endl;
   cout<<"retErrMsg="<<res.retErrMsg<<endl;
   cout<<"retResult="<<res.retResult<<endl;
   cout<<"retDebugInfo="<<res.retDebugInfo<<endl;
}

void dumpbridgeownerinfoAction()
{
   rpcinfo req;
   req.rpc_conf_file = "bitcoin.conf";
   req.datadir = "";
   commonResponse  res;

   //int ret = dumpbridgeownerinfo(req, res);
   //cout<<"ret="<<ret<<endl;
   //cout<<"retErrorCode="<<res.retErrCode<<endl;
   //cout<<"retErrMsg="<<res.retErrMsg<<endl;
   //cout<<"retResult="<<res.retResult<<endl;
   //cout<<"retDebugInfo="<<res.retDebugInfo<<endl;
}

int main(int argc, char** argv){

string t = "af057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4fd1d2";
string subblockdata = "bf057bbfb72640471fd910bcb67639c22df9f92470936cddc1ade0e2f2e7dc4fd1d2";
for(int i=0;i<8;++i){
   subblockdata += subblockdata;
}
std::string result = createsubchainAction();
commitsubchainAction(result, "");
std::string result2 = gensubchainblockAction(subblockdata);
commitsubchainAction(result2, subblockdata);

//genbridgeownerinfoAction();

return 0;
}

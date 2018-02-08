
#ifndef PROTO_H_
#define PROTO_H_

#ifdef __cplusplus
extern "C" {
#endif

static const int NOTIFY_OP=0;
static const int CREATE_OP=1;
static const int DELETE_OP=2;
static const int EXISTS_OP=3;
static const int GETDATA_OP=4;
static const int SETDATA_OP=5;
static const int GETACL_OP=6;
static const int SETACL_OP=7;
static const int GETCHILDREN_OP=8;
static const int SYNC_OP=9;
static const int PING_OP=11;
static const int CLOSE_OP=-11;
static const int SETAUTH_OP=100;
static const int SETWATCHES_OP=101;

#ifdef __cplusplus
}
#endif

#endif 

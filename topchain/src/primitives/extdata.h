#ifndef BITCOIN_PRIMITIVES_EXTDATA_H
#define BITCOIN_PRIMITIVES_EXTDATA_H
#include "serialize.h"
#include "util.h"
#include "utilstrencodings.h"
#include "amount.h"
#include "uint256.h"
#include "univalue.h"
#include "script/script.h"
#include "boost/any.hpp"

enum ExtDataType {
    EXTDATA_NO,
    EXTDATA_GENESISCOINBASE,
    EXTDATA_CREATESUBCHAIN,
    EXTDATA_BACKUPSUBBLOCK
};

class CGenesisCoinBaseExt
{
public:
    CAmount       nInitPlatformFee = 10 * COIN;//光链初始平台费
    std::string     platformAddress = "mygQdvfE4nZTRwBqp7WSMggWmvn44kmEjy";

    ADD_SERIALIZE_METHODS;
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(nInitPlatformFee);
        READWRITE(platformAddress);
    }

    std::string ToString() const;
};

class CCreateSubChainExt
{
public:
    uint256                           subChainId;//子链id
    uint256                           subChainOwner;//子链拥有者
    std::string                      subChainName;//子链名称
    std::string                      subCoinName;//子链币名称
    std::vector<std::string> subChainSeeds;//子链种子节点
    CScript                            signature;//签名

    ADD_SERIALIZE_METHODS;
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(subChainId);
        READWRITE(subChainOwner);
        READWRITE(subChainName);
        READWRITE(subCoinName);
        READWRITE(subChainSeeds);
        READWRITE(signature);
    }

    bool Verify() const;
};

class CBackupSubBlockExt
{
public:
    uint256           subChainId;//子链id
    uint256           subBlockHash;//当前交易对应区块哈希
    uint32_t          subBlockHeight;//当前交易对应区块高度
    CScript            signature;//子链owner签名,这个owner在子链超级节点间共享

    ADD_SERIALIZE_METHODS;
    
    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(subChainId);
        READWRITE(subBlockHash);
        READWRITE(subBlockHeight);
        READWRITE(signature);
    }

    bool Verify() const;
};

class CExtData
{
public:
    uint8_t nExtType;
    boost::any data;

public:
    CExtData() : nExtType(0), data() {}

    template <typename Stream>
    inline void Serialize(Stream& s) const {
        SerializeExtData(*this, s);
    }

    template <typename Stream>
    inline void Unserialize(Stream& s) {
        UnserializeExtData(*this, s);
    }

    bool NeedsToSaveMeta() const
    {
        if(nExtType == EXTDATA_CREATESUBCHAIN)
        {
            return true;
        }
        return false;
    }

    uint256 GetSubChainId() const
    {
        uint256 ret;
        if(nExtType == EXTDATA_CREATESUBCHAIN)
        {
            CCreateSubChainExt ext =  boost::any_cast<CCreateSubChainExt>(data);
            ret = ext.subChainId;
        }
        else if(nExtType == EXTDATA_BACKUPSUBBLOCK)
        {
            CBackupSubBlockExt ext =  boost::any_cast<CBackupSubBlockExt>(data);
            ret = ext.subChainId;
        }
        return ret;
    }

    uint256 GetPreTxId() const
    {
        uint256 ret;
        return ret;
    }

};

template<typename Stream>
inline void SerializeExtData(const CExtData& extdata, Stream& s) 
{
    uint8_t nExtType = extdata.nExtType;
    boost::any data = extdata.data;
    s << nExtType;
    if(nExtType == EXTDATA_NO)
    {
        return ;
    }
    else if(nExtType == EXTDATA_GENESISCOINBASE)
    {
        CGenesisCoinBaseExt ext =  boost::any_cast<CGenesisCoinBaseExt>(data);
        s << ext;
    }
    else if(nExtType == EXTDATA_CREATESUBCHAIN)
    {
        CCreateSubChainExt ext =  boost::any_cast<CCreateSubChainExt>(data);
        s << ext;
    }
    else if(nExtType == EXTDATA_BACKUPSUBBLOCK)
    {
        CBackupSubBlockExt ext =  boost::any_cast<CBackupSubBlockExt>(data);
        s << ext;
    }

    LogPrintf("SerializeExtData%d", nExtType);
}

template<typename Stream>
inline void  UnserializeExtData(CExtData& extdata, Stream& s) 
{
    s >> extdata.nExtType;
    uint8_t nExtType = extdata.nExtType;
    if(nExtType == EXTDATA_NO)
    {
        return ;
    }
    else if(nExtType == EXTDATA_GENESISCOINBASE)
    {
        CGenesisCoinBaseExt ext;
        s >> ext;
        extdata.data = ext;
    }
    else if(nExtType == EXTDATA_CREATESUBCHAIN)
    {
        CCreateSubChainExt ext;
        s >> ext;
        extdata.data = ext;
    }
    else if(nExtType == EXTDATA_BACKUPSUBBLOCK)
    {
        CBackupSubBlockExt ext;
        s >> ext;
        extdata.data = ext;
    }

    LogPrintf("UnserializeExtData%d", nExtType);
}

inline UniValue GetValueFromAmount(const CAmount& amount)
{
    bool sign = amount < 0;
    int64_t n_abs = (sign ? -amount : amount);
    int64_t quotient = n_abs / COIN;
    int64_t remainder = n_abs % COIN;
    return UniValue(UniValue::VNUM, strprintf("%s%d.%08d", sign ? "-" : "", quotient, remainder));
}

inline void ExtDataToValue(const CExtData& extdata, UniValue& extentry)
{
    UniValue entry(UniValue::VOBJ);
    uint8_t nExtType = extdata.nExtType;
    boost::any data = extdata.data;
    extentry.pushKV("type", (uint8_t)nExtType);
    if(nExtType == EXTDATA_NO)
    {
        return ;
    }
    else if(nExtType == EXTDATA_GENESISCOINBASE)
    {
        CGenesisCoinBaseExt ext =  boost::any_cast<CGenesisCoinBaseExt>(data);
        entry.pushKV("platformaddress", ext.platformAddress);
        entry.pushKV("initialplatformfee", ext.nInitPlatformFee);
    }
    else if(nExtType == EXTDATA_CREATESUBCHAIN)
    {
        CCreateSubChainExt ext =  boost::any_cast<CCreateSubChainExt>(data);
        entry.pushKV("subchain-id", ext.subChainId.ToString());
        entry.pushKV("subchain-name", ext.subChainName);
        entry.pushKV("subcoin-name", ext.subCoinName);
        entry.pushKV("subchain-owner", ext.subChainOwner.ToString());
        UniValue seeds(UniValue::VARR);
        for (std::string str : ext.subChainSeeds)
        {
            seeds.push_back(str);
        }
        entry.pushKV("subchain-seeds", seeds);
        entry.pushKV("subchain-signature", HexStr(ext.signature.begin(), ext.signature.end()));
    }
    else if(nExtType == EXTDATA_BACKUPSUBBLOCK)
    {
        CBackupSubBlockExt ext =  boost::any_cast<CBackupSubBlockExt>(data);
        entry.pushKV("subchain-id", ext.subChainId.ToString());
        entry.pushKV("subblock-hash", ext.subBlockHash.ToString());
        entry.pushKV("subblock-height",std::to_string(ext.subBlockHeight));
        entry.pushKV("subchain-signature", HexStr(ext.signature.begin(), ext.signature.end()));
    }
    extentry.pushKV("data", entry);
}

inline bool  VerifyExtData(const CExtData& extdata)
{
    uint8_t nExtType = extdata.nExtType;
    boost::any data = extdata.data;
    bool ret = false;
    if(nExtType == EXTDATA_NO)
    {
        ret = true ;
    }
    else if(nExtType == EXTDATA_GENESISCOINBASE)
    {
        ret = true ;
    }
    else if(nExtType == EXTDATA_CREATESUBCHAIN)
    {
        CCreateSubChainExt ext =  boost::any_cast<CCreateSubChainExt>(data);
        ret = ext.Verify();
    }
    else if(nExtType == EXTDATA_BACKUPSUBBLOCK)
    {
        CBackupSubBlockExt ext =  boost::any_cast<CBackupSubBlockExt>(data);
        ret = ext.Verify();
    }

    return ret ;
}

#endif // BITCOIN_PRIMITIVES_EXTDATA_H

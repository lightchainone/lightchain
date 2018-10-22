#include "primitives/extdata.h"

std::string CGenesisCoinBaseExt::ToString() const
{
    std::string str;
    str += strprintf("CGenesisCoinBaseExt(platformfee=%s, platformaddress=%s)\n",
        GetValueFromAmount(nInitPlatformFee).getValStr(),
        platformAddress);

    return str;
}

bool CCreateSubChainExt::Verify() const
{
    return false;
}

bool CBackupSubBlockExt::Verify() const
{
    return false;
}
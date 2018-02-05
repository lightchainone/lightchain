
#include "lcclient_rangestrategy.h"

#include <lc_log.h>

#include "lcclient_connectserver.h"

namespace lc
{

    int LcClientRangeStrategy :: filterServer(ConnectionRequestex * req, 
                        					bsl::var::IVar & , 
    										ConnectionRes *serverSelectedInfo)
    {
        LC_LOG_DEBUG("%s", "__func__");
		if (NULL == req || NULL == serverSelectedInfo) {
			return -1;
		}

        int serverNum = _mgr->getServerSize();
        if (serverNum <= 0) {
            return -1;
        }

        int minRange;
        int maxRange;
        int mod;
        int ret = parseRange(req->serverArg.range, minRange, maxRange, mod);
        if (0 != ret) {
            return -1;
        }

		
		if (0 == mod) {
        	req->key = -1;
		} else {
			req->key = mod;
		}   

        LC_LOG_DEBUG("[%s] [%s] [%d] [%d] [%d]", req->serverArg.range, req->serverArg.version,
                    minRange, maxRange, mod);

        int i;
        LcClientServer *server = NULL;
       	const char *range;
        const char *version;
        int min;
        int max;
        for (i=0; i<serverNum; i++) {
            server = dynamic_cast<LcClientServer *>(_mgr->getServerInfo(i));
            if(NULL == server || (false == server->isEnable()) || (false == server->isHealthy())) {
                LC_LOG_DEBUG("server[%d] is error", i);
                continue;
            }
            LC_LOG_DEBUG("in server[%d] ip[%s] port[%d]", i, server->getIP(), server->getPort());
            range = server->getRange();
            version = server->getVersion();
            LC_LOG_DEBUG("[%s] [%s] ***", range, version);
            ret = parseRange1(range, min, max);
            
            LC_LOG_DEBUG("[%s] [%s] [%d] [%d]", range,
						 version, min, max);

            if (0 != ret) {
				LC_LOG_WARNING("server range [%s] is error", server->getRange());
                continue;
            }
            if (min>minRange || max<maxRange) {
                continue;
            }
            if (strcmp(version, req->serverArg.version) != 0) {
                continue;
            }

			int j = 0;
			for (j=0; j<serverSelectedInfo->selectedServerID.size(); j++) {
					if (serverSelectedInfo->selectedServerID[j] == i) {
							break;
					}
			}
			if (j < serverSelectedInfo->selectedServerID.size()) {
				continue;
			}
			LC_LOG_DEBUG("push [%d]", i);
			if (serverSelectedInfo->resultServerID.push_back(i) != 0 ) {
					LC_LOG_WARNING("live server num [%d] big than allowed num [%d]", i,
									LCCLIENT_SELECTEDSERVER_SIZE);
					return -1;
			}
            
        }


        return 0;
    }

	int LcClientRangeStrategy :: parseRange1(const char *range, int &min, int &max)
	{
		if (NULL == range) {
			return -1;
		}
		
		int rangeLen = strlen(range);
		if (rangeLen < 3) {
			return -1;
		}
		
		min = max = -1;
		const char *strRange = range;
        const int tempStrLen = 12;
        char tempStr[tempStrLen];
        int k = 0;
		int startTag = 0;
		int i;
		for (i=0; i<rangeLen; i++) {
			if (range[i] == '-') {
				if (k == 0) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
				if (1 == startTag) {
					LC_LOG_WARNING("range[%s] error too many -", range);
					return -1;
				}
				startTag = 1;
				tempStr[k] = '\0';
				min = atoi(tempStr);
				k = 0;
				tempStr[0] = '\0';
				continue;	
			}
			
			if (strRange[i] >= '0' && strRange[i] <= '9') {
            	tempStr[k] = strRange[i];
            	k ++;
				if (k >= tempStrLen) {
					LC_LOG_WARNING("input Range [%s] is error", range);
					return -1;
				}
			} else {
				LC_LOG_WARNING("range[%s] error", range);
				return -1;
			}
		}
        if (i > 0) {
            if (strRange[i-1] != '\0') {
				if (k >= tempStrLen) {
					LC_LOG_WARNING("input Range [%s] is error", range);
					return -1;
				}
				if (k <= 0) {
					LC_LOG_WARNING("input Range [%s] is error", range);
					return -1;
				}
                tempStr[k] = '\0';
				if (startTag != 1 || min == -1) {
					return -1;
				}
                max = atoi(tempStr);
            }       
        }
		
		return 0;
	}

    int LcClientRangeStrategy :: parseRange(const char *range, int &min, int &max, int &mod)
    {
        if (NULL == range) {
			return -1;
		}

		if (strlen(range) < 7) {
			return -1;
		}

        min = max = mod = -1;
        const char *strRange = range;
        int i;
        const int tempStrLen = 12;
        char tempStr[tempStrLen];
        int k = 0;
		int rangeLen = strlen(range);
		int startTag = 0;
		int secondTag = 0;
		int thirdTag = 0;
		int endTag = 0;
		if ('(' != strRange[0]) {
			LC_LOG_WARNING("range[%s] error", range);
			return -1;
		} else {
			startTag = 1;
		}
        for (i=1; i<rangeLen; i++) {
            if (strRange[i] == '%') {
		    	if (startTag != 1 || secondTag != 1 || thirdTag != 1 || endTag == 1) {
					LC_LOG_WARNING("range[%s] error", range);
			    	return -1;
		    	} else {
			    	endTag = 1;
			    	continue;
		    	}
            }
            if (strRange[i] == '-') {
				if (startTag != 1 || secondTag == 1) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
				if (k == 0) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
				secondTag = 1;
                
            	tempStr[k] = '\0';
                min = atoi(tempStr);
                k = 0;
                tempStr[0] = '\0';
                continue;
            }
            if (strRange[i] == ')') {
				if (startTag != 1 || secondTag != 1 || min == -1 || thirdTag == 1) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
				if (k == 0) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
				thirdTag = 1;
                tempStr[k] = '\0';
                max = atoi(tempStr);
                k = 0;
                tempStr[0] = '\0';
                continue;
            }
			if (strRange[i] >= '0' && strRange[i] <= '9') {
            	tempStr[k] = strRange[i];
            	k ++;
				if (k >= tempStrLen) {
					LC_LOG_WARNING("input Range [%s] is error", range);
					return -1;
				}
			} else {
				LC_LOG_WARNING("range[%s] error", range);
				return -1;
			}
        }
        if (i > 0) {
            if (strRange[i-1] != '\0') {
				if (k >= tempStrLen) {
					LC_LOG_WARNING("input Range [%s] is error", range);
					return -1;
				}
				if (k == 0) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
                tempStr[k] = '\0';
				if (startTag != 1 || secondTag != 1 || thirdTag != 1 || endTag != 1 ||
					min == -1 || max == -1) {
					LC_LOG_WARNING("range[%s] error", range);
					return -1;
				}
                mod = atoi(tempStr);
            }       
        }

        return 0;
        
    }

	int LcClientRangeStrategy :: isInputRangeOK(const char *range)
	{
        int minRange;
        int maxRange;
        int mod;
        int ret = parseRange(range, minRange, maxRange, mod);
        if (0 != ret) {
            return -1;
        }
		
		return 0;
	}

	int LcClientRangeStrategy :: isServerRangeOK(const char *range)
	{
		int min;
		int max;
		int ret = parseRange1(range, min, max);
		if (0 != ret) {
			return -1;
		}

		return 0;
	}
}


















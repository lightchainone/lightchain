


#ifndef  __LCCLIENT_SELFSTRATEGY_H_
#define  __LCCLIENT_SELFSTRATEGY_H_

#include "lcclient_strategy.h"

#include <map>

namespace lc
{
    class LcClientSelfStrategy : plclic LcClientStrategy
    {
        plclic:
            typedef std::multimap<bsl::string, int> str_int_multimap;

            LcClientSelfStrategy():_init_tag(0), _id_map_tag(0){}

            virtual ~LcClientSelfStrategy(){}

            
            virtual int load(const comcfg::ConfigUnit & conf);

            
            virtual int reload(const comcfg::ConfigUnit & conf);
            
            virtual int filterServer(
                                ConnectionRequestex *req,
                                bsl::var::IVar & confVar, 
                                ConnectionRes *serverSelectedInfo
                                );

            virtual int balanceServer(
                                ConnectionRequestex *req,
                                bsl::var::IVar & confVar, 
                                const ConnectionRes &serverSelectedInfo
                                );

        protected:
            virtual int parseIdConf();

            virtual int parseStr(const char *str, str_int_multimap &id_map, int idx);

            int clearMap() {
                if (_id_map_tag % 2 == 0) {
                    _id_map.clear();
                } else {
                    _id_map_back.clear();
                }
                return 0;
            }
        protected:
            int _init_tag;
            str_int_multimap _id_map;
            str_int_multimap _id_map_back;
            long long _id_map_tag;
    };
}















#endif  



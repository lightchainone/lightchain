g++ -std=c++11 -DHAVE_CONFIG_H \
               -D_FORTIFY_SOURCE=2 \
               -DBOOST_SP_USE_STD_ATOMIC -DBOOST_AC_USE_STD_ATOMIC \
               -DBOOST_NO_SCOPED_ENUMS \
               -DBOOST_NO_CXX11_SCOPED_ENUMS \
               -DHAVE_BUILD_INFO \
               -D__STDC_FORMAT_MACROS \
               -I. \
               -Isecp256k1 \
              -Wstack-protector \
              -fstack-protector-all  \
              -fPIE -g -O2 -Wall -Wextra -Wformat -Wvla -Wformat-security -Wno-unused-parameter \
              test.cpp \
              -Wstack-protector -fstack-protector-all -fPIE -g -O2 -Wall -Wextra -Wformat -Wvla -Wformat-security -Wno-unused-parameter \
              -pthread -Wl,-z -Wl,relro -Wl,-z -Wl,now -pie -o testapi  libbitcoin_cliapi.a   univalue/.libs/libunivalue.a  -L/usr/lib64 -lboost_system -lboost_filesystem -lboost_program_options-mt -lboost_thread -lboost_chrono-mt -lssl -lcrypto -levent -pthread 

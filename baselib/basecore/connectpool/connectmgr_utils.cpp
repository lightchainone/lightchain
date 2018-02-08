
#include "connectmgr_utils.h"

namespace comcm{
	unsigned int ip_to_int(const char * ip){
		unsigned int ip_int;
		if( inet_pton(AF_INET, ip, &ip_int) <= 0){
			return 0;
		}
		return ip_int;
	}
	void ms_sleep(int ms) 
	{
		struct timespec t;
		struct timespec s;
		t.tv_sec = ms/1000;
		t.tv_nsec = (ms%1000) * 1000000;

		nanosleep(&t, &s);
	}

	mstime_t ms_time(){
		struct timeval tt;
		gettimeofday(&tt, NULL);
		mstime_t ret = tt.tv_sec * 1000 + tt.tv_usec / 1000;
		return ret;
	}
};


















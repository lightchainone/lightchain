#include <stdio.h>
#include <inttypes.h>
#include "lnmcsys_util.h"

int main(int argc, char* argv[]) 
{
    if (argc >= 2) {
        uint64_t akey = atol(argv[1]);
        uint64_t part = lnsys::LnmcsysUtil::get_partition_table(akey, 400);
        printf("%lu ==> partition %lu\n", akey, part);
    }
    return 0;
}




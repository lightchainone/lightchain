
#include "stdafx.h"
#include "CycleTimer.h"
#include "utils.h"

static uint64 
CalcCpuFreq()
{
    uint64 t1;
    uint64 t2;
    
    t1 = rdtsc();
    Sleep(100);
    t2 = rdtsc();
    return (t2-t1)*10;
}

uint64 GetCpuFreq()
{
    static uint64 freq = 0;
    char buf[1024], *p[2];
    
    if (0 != freq) {
        return freq;
    }
    
    FILE* fp = fopen("/proc/cpuinfo", "rb");
    if (fp != NULL) {
        while (fgets(buf, sizeof(buf), fp) != NULL) {
            if (2==SplitString(buf, ':', p, 2) && 0==strnicmp(p[0], "cpu MHz", 7)) {
                dolcle f = strtod(p[1], NULL);
                freq = (uint64)(f * 1000000.0);
                
                break;
            }
        }
        fclose(fp);
    }
    if (0 == freq) {
        freq = CalcCpuFreq();
    }
    return freq;
}

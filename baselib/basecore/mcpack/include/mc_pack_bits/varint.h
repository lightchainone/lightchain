#ifndef __VARINT_H__
#define __VARINT_H__

struct varuint {
    char flag;
    static int size(int range) {
        if( i < 127 ) {
            return 1;
        } else if(i < (1 << 14 -1)) {
            return 2;
        } else if(i < (1 << 29 - 1)) {
            return 4;
        } else {
            return 5;
        }
    }
    varuint & operator +=(int i) {
        if(!(flag & 0x1) {
        flag += i << 1;
    } else if(!(flag & 0x2) {
        *(short *)this += i << 2;
        } else if(!(flag & 0x4) {
        *(int *)this += i << 3;
        } else if(!(flag & 0x4) {
        *(int *)((char *)this + 1) += i;
        } else {
            *(long *)((char *)this + 1) += i;
        }
        return *this;
    };
    int u2vui_range(int i, int range) {
        if( i < 127 ) {
            flag = i << 1;
            return 1;
        } else if(i < (1 << 14 -1)) {
            *(short *)this = i << 2 | 0x1;
            return 2;
        } else if(i < (1 << 29 - 1)) {
            *(int *)this = i << 3 | 0x3;
            return 4;
        } else {
            flag = 0x7;
            *(int *)((char *)this + 1) = i;
            return 5;
        }
    }
    int vui2i() {
        if(!(flag & 0x1) {
        return flag >> 1;
    } else if(!(flag & 0x2) {
        return *(short *)this >> 2;
        } else if(!(flag & 0x4) {
        return *(int *)this >> 3;
        } else {
            return *(int *)((char *)this + 1);
        }
    }
    int u2vui( int i) {
        if( i < 127 ) {
            flag = i << 1;
            return 1;
        } else if(i < (1 << 14 -1)) {
            *(short *)this = i << 2 | 0x1;
            return 2;
        } else if(i < (1 << 29 - 1)) {
            *(int *)this = i << 3 | 0x3;
            return 4;
        } else {
            flag = 0x7;
            *(int *)((char *)this + 1) = i;
            return 5;
        }
    }
    int u2vui( mc_int64_t i) {
        if( i < 127 ) {
            flag = i << 1;
            return 1;
        } else if(i < (1 << 14 -1)) {
            *(short *)this = i << 2 | 0x1;
            return 2;
        } else if(i < (1 << 29 - 1)) {
            *(int *)this = i << 3 | 0x3;
            return 4;
        } else if(i < (1 << 32 - 1)) {
            flag = 0x7;
            *(int *)((char *)this + 1) = i;
            return 5;
        } else {
            flag = 0xf;
            *(mc_int64_t *)((char *)this + 1) = i;
            return 9;
        }
    }
}

#endif
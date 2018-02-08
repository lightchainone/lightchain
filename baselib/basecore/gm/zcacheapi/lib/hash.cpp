#include "hash.h"

#define ENDIAN_LITTLE 1

#if ENDIAN_BIG == 1
    #define HASH_LITTLE_ENDIAN 0
    #define HASH_BIG_ENDIAN 1
#else
    #if ENDIAN_LITTLE == 1
        #define HASH_LITTLE_ENDIAN 1
        #define HASH_BIG_ENDIAN 0
    #else
        #define HASH_LITTLE_ENDIAN 0
        #define HASH_BIG_ENDIAN 0
    #endif
#endif

#define rot(x,k) (((x)<<(k)) ^ ((x)>>(32-(k))))



#define mix(a,b,c) \
{ \
    a -= c;  a ^= rot(c, 4);  c += b; \
    b -= a;  b ^= rot(a, 6);  a += c; \
    c -= b;  c ^= rot(b, 8);  b += a; \
    a -= c;  a ^= rot(c,16);  c += b; \
    b -= a;  b ^= rot(a,19);  a += c; \
    c -= b;  c ^= rot(b, 4);  b += a; \
}


#define final(a,b,c) \
{ \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
}

#if HASH_LITTLE_ENDIAN == 1
unsigned int hash(
                 const void *key,                                       
                 size_t      length,                                    
                 const unsigned int    initval)                         
{
    unsigned int a,b,c;                                                 
    union
    {
        const void *ptr; size_t i;
    } u;                                                                

    
    a = b = c = 0xdeadbeef + ((unsigned int)length) + initval;

    u.ptr = key;
    if (HASH_LITTLE_ENDIAN && ((u.i & 0x3) == 0))
    {
        const unsigned int *k = (unsigned int*)key;                     
    #ifdef VALGRIND
        const unsigned char  *k8;
    #endif 

        
        while (length > 12)
        {
            a += k[0];
            b += k[1];
            c += k[2];
            mix(a,b,c);
            length -= 12;
            k += 3;
        }

        
        
    #ifndef VALGRIND

        switch (length)
        {
        case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
        case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
        case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
        case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
        case 8 : b+=k[1]; a+=k[0]; break;
        case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
        case 6 : b+=k[1]&0xffff; a+=k[0]; break;
        case 5 : b+=k[1]&0xff; a+=k[0]; break;
        case 4 : a+=k[0]; break;
        case 3 : a+=k[0]&0xffffff; break;
        case 2 : a+=k[0]&0xffff; break;
        case 1 : a+=k[0]&0xff; break;
        case 0 : return c;                                              
        }

    #else 

        k8 = (const unsigned char *)k;
        switch (length)
        {
        case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
        case 11: c+=((unsigned int)k8[10])<<16;                         
        case 10: c+=((unsigned int)k8[9])<<8;                           
        case 9 : c+=k8[8];                                              
        case 8 : b+=k[1]; a+=k[0]; break;
        case 7 : b+=((unsigned int)k8[6])<<16;                          
        case 6 : b+=((unsigned int)k8[5])<<8;                           
        case 5 : b+=k8[4];                                              
        case 4 : a+=k[0]; break;
        case 3 : a+=((unsigned int)k8[2])<<16;                          
        case 2 : a+=((unsigned int)k8[1])<<8;                           
        case 1 : a+=k8[0]; break;
        case 0 : return c;                                              
        }

    #endif 

    }
    else if (HASH_LITTLE_ENDIAN && ((u.i & 0x1) == 0))
    {
        const unsigned short *k = (unsigned short*)key;                 
        const unsigned char  *k8;

        
        while (length > 12)
        {
            a += k[0] + (((unsigned int)k[1])<<16);
            b += k[2] + (((unsigned int)k[3])<<16);
            c += k[4] + (((unsigned int)k[5])<<16);
            mix(a,b,c);
            length -= 12;
            k += 6;
        }

        
        k8 = (const unsigned char *)k;
        switch (length)
        {
        case 12: c+=k[4]+(((unsigned int)k[5])<<16);
            b+=k[2]+(((unsigned int)k[3])<<16);
            a+=k[0]+(((unsigned int)k[1])<<16);
            break;
        case 11: c+=((unsigned int)k8[10])<<16;                         
        case 10: c+=k[4];                                               
            b+=k[2]+(((unsigned int)k[3])<<16);
            a+=k[0]+(((unsigned int)k[1])<<16);
            break;
        case 9 : c+=k8[8];                                              
        case 8 : b+=k[2]+(((unsigned int)k[3])<<16);
            a+=k[0]+(((unsigned int)k[1])<<16);
            break;
        case 7 : b+=((unsigned int)k8[6])<<16;                          
        case 6 : b+=k[2];
            a+=k[0]+(((unsigned int)k[1])<<16);
            break;
        case 5 : b+=k8[4];                                              
        case 4 : a+=k[0]+(((unsigned int)k[1])<<16);
            break;
        case 3 : a+=((unsigned int)k8[2])<<16;                          
        case 2 : a+=k[0];
            break;
        case 1 : a+=k8[0];
            break;
        case 0 : return c;                                              
        }

    }
    else                                                                
    {
        const unsigned char *k = (unsigned char*)key;

        
        while (length > 12)
        {
            a += k[0];
            a += ((unsigned int)k[1])<<8;
            a += ((unsigned int)k[2])<<16;
            a += ((unsigned int)k[3])<<24;
            b += k[4];
            b += ((unsigned int)k[5])<<8;
            b += ((unsigned int)k[6])<<16;
            b += ((unsigned int)k[7])<<24;
            c += k[8];
            c += ((unsigned int)k[9])<<8;
            c += ((unsigned int)k[10])<<16;
            c += ((unsigned int)k[11])<<24;
            mix(a,b,c);
            length -= 12;
            k += 12;
        }

        
        switch (length)                                                 
        {
        case 12: c+=((unsigned int)k[11])<<24;
        case 11: c+=((unsigned int)k[10])<<16;
        case 10: c+=((unsigned int)k[9])<<8;
        case 9 : c+=k[8];
        case 8 : b+=((unsigned int)k[7])<<24;
        case 7 : b+=((unsigned int)k[6])<<16;
        case 6 : b+=((unsigned int)k[5])<<8;
        case 5 : b+=k[4];
        case 4 : a+=((unsigned int)k[3])<<24;
        case 3 : a+=((unsigned int)k[2])<<16;
        case 2 : a+=((unsigned int)k[1])<<8;
        case 1 : a+=k[0];
            break;
        case 0 : return c;                                              
        }
    }

    final(a,b,c);
    return c;                                                           
}

#elif HASH_BIG_ENDIAN == 1


unsigned int hash( const void *key, size_t length, const unsigned int initval)
{
    unsigned int a,b,c;
    union
    {
        const void *ptr; size_t i;
    } u;                                                                

    
    a = b = c = 0xdeadbeef + ((unsigned int)length) + initval;

    u.ptr = key;
    if (HASH_BIG_ENDIAN && ((u.i & 0x3) == 0))
    {
        const unsigned int *k = key;                                    
    #ifdef VALGRIND
        const unsigned char  *k8;
    #endif 

        
        while (length > 12)
        {
            a += k[0];
            b += k[1];
            c += k[2];
            mix(a,b,c);
            length -= 12;
            k += 3;
        }

        
        
    #ifndef VALGRIND

        switch (length)
        {
        case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
        case 11: c+=k[2]&0xffffff00; b+=k[1]; a+=k[0]; break;
        case 10: c+=k[2]&0xffff0000; b+=k[1]; a+=k[0]; break;
        case 9 : c+=k[2]&0xff000000; b+=k[1]; a+=k[0]; break;
        case 8 : b+=k[1]; a+=k[0]; break;
        case 7 : b+=k[1]&0xffffff00; a+=k[0]; break;
        case 6 : b+=k[1]&0xffff0000; a+=k[0]; break;
        case 5 : b+=k[1]&0xff000000; a+=k[0]; break;
        case 4 : a+=k[0]; break;
        case 3 : a+=k[0]&0xffffff00; break;
        case 2 : a+=k[0]&0xffff0000; break;
        case 1 : a+=k[0]&0xff000000; break;
        case 0 : return c;                                              
        }

    #else  

        k8 = (const unsigned char *)k;
        switch (length)                                                 
        {
        case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
        case 11: c+=((unsigned int)k8[10])<<8;                          
        case 10: c+=((unsigned int)k8[9])<<16;                          
        case 9 : c+=((unsigned int)k8[8])<<24;                          
        case 8 : b+=k[1]; a+=k[0]; break;
        case 7 : b+=((unsigned int)k8[6])<<8;                           
        case 6 : b+=((unsigned int)k8[5])<<16;                          
        case 5 : b+=((unsigned int)k8[4])<<24;                          
        case 4 : a+=k[0]; break;
        case 3 : a+=((unsigned int)k8[2])<<8;                           
        case 2 : a+=((unsigned int)k8[1])<<16;                          
        case 1 : a+=((unsigned int)k8[0])<<24; break;
        case 0 : return c;
        }

    #endif 

    }
    else                                                                
    {
        const unsigned char *k = key;

        
        while (length > 12)
        {
            a += ((unsigned int)k[0])<<24;
            a += ((unsigned int)k[1])<<16;
            a += ((unsigned int)k[2])<<8;
            a += ((unsigned int)k[3]);
            b += ((unsigned int)k[4])<<24;
            b += ((unsigned int)k[5])<<16;
            b += ((unsigned int)k[6])<<8;
            b += ((unsigned int)k[7]);
            c += ((unsigned int)k[8])<<24;
            c += ((unsigned int)k[9])<<16;
            c += ((unsigned int)k[10])<<8;
            c += ((unsigned int)k[11]);
            mix(a,b,c);
            length -= 12;
            k += 12;
        }

        
        switch (length)                                                 
        {
        case 12: c+=k[11];
        case 11: c+=((unsigned int)k[10])<<8;
        case 10: c+=((unsigned int)k[9])<<16;
        case 9 : c+=((unsigned int)k[8])<<24;
        case 8 : b+=k[7];
        case 7 : b+=((unsigned int)k[6])<<8;
        case 6 : b+=((unsigned int)k[5])<<16;
        case 5 : b+=((unsigned int)k[4])<<24;
        case 4 : a+=k[3];
        case 3 : a+=((unsigned int)k[2])<<8;
        case 2 : a+=((unsigned int)k[1])<<16;
        case 1 : a+=((unsigned int)k[0])<<24;
            break;
        case 0 : return c;
        }
    }

    final(a,b,c);
    return c;
}

#else 
    #error Must define HASH_BIG_ENDIAN or HASH_LITTLE_ENDIAN
#endif 

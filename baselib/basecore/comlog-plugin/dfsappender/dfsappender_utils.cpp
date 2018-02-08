
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <unistd.h>
#include <sys/stat.h>

#include "dfsappender_utils.h"

namespace comspace 
{
namespace comlogplugin 
{
namespace dfsappender
{
int normalize_file(char * filename)
{
    char *p = filename;
    char *q = filename;
    int status = 0;
    if (strncmp(filename, "../", 3) == 0) { 
        return -1;
    }
    for (;*p != '\0'; p++) {
        switch (status) {
        
            case 0: 
                if (*p == '/') status = 1;
                *q++ = *p;
                break;  
            case 1: 
                if (*p == '/') break;
                status = ((*p == '.')?2:0);
                *q++ = *p;
                break;  
            case 2: 
                status = ((*p == '.')?3:0); 

                if (*p == '/') {
                    q--;    
                    status = 1;
                } else {
                    *q++ = *p;
                }       
                break;  
            case 3: 
                if (*p == '/') {
                    q -= 4; 
                    if (q < filename) return -1;
                    status = 1;
                    while (*q != '/' && q > filename) q--;
                    if (*q == '/') q++;
                } else {
                    *q++ = *p;
                    status = 0;
                }
               break;
            default:
                return -1;
                break;
        }
    }
    *q = '\0';
    return 0;
}

int check_slcdir(char *path)
{
    struct stat sbuff;
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s", path);
    char *p = fullpath; 
    while (1) {
        ++p;
        if (*p == '/' || *p == '\0') {
            char ch = *p;
            *p = '\0';
            if (stat(fullpath, &sbuff) < 0) {
                if (mkdir(fullpath, 0755) < 0) {
                    return -1;
                }
            }
            if (!S_ISDIR(sbuff.st_mode)) {
                return -1;
            }
            *p = ch;
        }
        if (*p == '\0') {
            break;
        }
    }
    return 0;
}


static char s_proc_name[1024];        
static char s_path_name[1024];        

bool s_proc_name_is_set = false;          

static void set_proc_path_name()
{
    
    pid_t pid = getpid();
    
    char exec_file[1024];
    char proc_name[1024];
    snprintf(exec_file, sizeof(exec_file), "/proc/%d/exe", pid);
    int ret = readlink(exec_file, proc_name, sizeof(proc_name));
    proc_name[ret] = '\0';
    
    char *p = strrchr(proc_name, '/');
    snprintf(s_proc_name, sizeof(s_proc_name), "%s", p + 1);
    
    *p = '\0';
    snprintf(s_path_name, sizeof(s_path_name), "%s", proc_name);
    if (s_path_name[0] == '\0') {
        s_path_name[0] = '/';
        s_path_name[1] = '\0';
    }
    s_proc_name_is_set = true;
}

const char* get_proc_name()
{
    
    
    if (s_proc_name_is_set) {
        return s_proc_name;
    }
    set_proc_path_name();
    return s_proc_name;
}

const char* get_proc_path()
{
    
    
    if (s_proc_name_is_set) {
        return s_path_name;
    }
    return s_path_name;
}


int get_dev_confnstr(com_device_t *dev, const char* key, char *value, size_t value_size)
{
    char name[WORD_SIZE];
    snprintf(name, sizeof(name), "COMLOG_DFS_%s", key);
    if (ul_getconfnstr(&(dev->reservedconf), name, value, value_size)) {
        return 0;
    }

    snprintf(name, sizeof(name), "%s_%s", dev->name, key);
    if (ul_getconfnstr(&(dev->reservedconf), name, value, value_size)) {
        return 0;
    }
    return -1;
}

int get_dev_confuint32(com_device_t *dev, const char* key, uint32_t *value)
{
    char name[WORD_SIZE];
    snprintf(name, sizeof(name), "COMLOG_DFS_%s", key);
    if (ul_getconfuint(&(dev->reservedconf), name, value)) {
        return 0;
    }
    snprintf(name, sizeof(name), "%s_%s", dev->name, key);
    if (ul_getconfuint(&(dev->reservedconf), name, value)) {
        return 0;
    }
    return -1;
}



static void md5_process_block (const void *buffer, size_t len, struct md5_ctx *ctx);
    
static void md5_process_bytes (const void * buffer, size_t len, struct md5_ctx *ctx);
    
static void *md5_buffer (const char *buffer, size_t len, void *resblock);




int handle_sign(char *filename, comlogproxy_handle_t * sign)
{
    size_t len = strlen(filename);
    md5_buffer(filename, len, sign);
    return 0;
}








#ifdef _LIBC
# include <endian.h>
# if __BYTE_ORDER == __BIG_ENDIAN
#  define WORDS_BIGENDIAN 1
# endif
# endif

#ifdef WORDS_BIGENDIAN
# define SWAP(n)							\
    (((n) << 24) | (((n) & 0xff00) << 8) | (((n) >> 8) & 0xff00) | ((n) >> 24))
#else
# define SWAP(n) (n)
#endif


typedef uint32_t md5_uint32;
typedef uintptr_t md5_uintptr;

struct md5_ctx 
{
    md5_uint32 A;
    md5_uint32 B;
    md5_uint32 C;
    md5_uint32 D;

    md5_uint32 total[2];
    md5_uint32 buflen; 
    char buffer[128] __attribute__ ((__aligned__ (__alignof__ (md5_uint32))));
};



static const unsigned char fillbuf[64] = { 0x80, 0  };



static void
md5_init_ctx (struct md5_ctx *ctx)
{
  ctx->A = 0x67452301;
  ctx->B = 0xefcdab89;
  ctx->C = 0x98badcfe;
  ctx->D = 0x10325476;

  ctx->total[0] = ctx->total[1] = 0;
  ctx->buflen = 0;
}


static void *
md5_read_ctx (const struct md5_ctx *ctx, void *resbuf)
{
  ((md5_uint32 *) resbuf)[0] = SWAP (ctx->A);
  ((md5_uint32 *) resbuf)[1] = SWAP (ctx->B);
  ((md5_uint32 *) resbuf)[2] = SWAP (ctx->C);
  ((md5_uint32 *) resbuf)[3] = SWAP (ctx->D);

  return resbuf;
}


static void *
md5_finish_ctx (struct md5_ctx *ctx, void *resbuf)
{
  
  md5_uint32 bytes = ctx->buflen;
  size_t pad;

  
  ctx->total[0] += bytes;
  if (ctx->total[0] < bytes)
    ++ctx->total[1];

  pad = bytes >= 56 ? 64 + 56 - bytes : 56 - bytes;
  memcpy (&ctx->buffer[bytes], fillbuf, pad);

  
  *(md5_uint32 *) &ctx->buffer[bytes + pad] = SWAP (ctx->total[0] << 3);
  *(md5_uint32 *) &ctx->buffer[bytes + pad + 4] = SWAP ((ctx->total[1] << 3) |
							(ctx->total[0] >> 29));

  
  md5_process_block (ctx->buffer, bytes + pad + 8, ctx);

  return md5_read_ctx (ctx, resbuf);
}


#if 0
static int
md5_stream (FILE *stream, void *resblock)
{
  
#define BLOCKSIZE 4096
  struct md5_ctx ctx;
  char buffer[BLOCKSIZE + 72];
  size_t sum;

  
  md5_init_ctx (&ctx);

  
  while (1)
    {
      
      size_t n;
      sum = 0;

      
      do
	{
	  n = fread (buffer + sum, 1, BLOCKSIZE - sum, stream);

	  sum += n;
	}
      while (sum < BLOCKSIZE && n != 0);
      if (n == 0 && ferror (stream))
        return 1;

      
      if (n == 0)
	break;

      
      md5_process_block (buffer, BLOCKSIZE, &ctx);
    }

  
  if (sum > 0)
    md5_process_bytes (buffer, sum, &ctx);

  
  md5_finish_ctx (&ctx, resblock);
  return 0;
}
#endif


static void *
md5_buffer (const char *buffer, size_t len, void *resblock)
{
  struct md5_ctx ctx;

  
  md5_init_ctx (&ctx);

  
  md5_process_bytes (buffer, len, &ctx);

  
  return md5_finish_ctx (&ctx, resblock);
}


static void
md5_process_bytes (const void *buffer, size_t len, struct md5_ctx *ctx)
{
  
  if (ctx->buflen != 0)
    {
      size_t left_over = ctx->buflen;
      size_t add = 128 - left_over > len ? len : 128 - left_over;

      memcpy (&ctx->buffer[left_over], buffer, add);
      ctx->buflen += add;

      if (ctx->buflen > 64)
	{
	  md5_process_block (ctx->buffer, ctx->buflen & ~63, ctx);

	  ctx->buflen &= 63;
	  
	  memcpy (ctx->buffer, &ctx->buffer[(left_over + add) & ~63],
		  ctx->buflen);
	}

      buffer = (const char *) buffer + add;
      len -= add;
    }

  
  if (len >= 64)
    {
#if !_STRING_ARCH_unaligned

# if __GNUC__ >= 2
#  define UNALIGNED_P(p) (((md5_uintptr) p) % __alignof__ (md5_uint32) != 0)
# else
#  define UNALIGNED_P(p) (((md5_uintptr) p) % sizeof (md5_uint32) != 0)
# endif
      if (UNALIGNED_P (buffer))
	while (len > 64)
	  {
	    md5_process_block (memcpy (ctx->buffer, buffer, 64), 64, ctx);
	    buffer = (const char *) buffer + 64;
	    len -= 64;
	  }
      else
#endif
	{
	  md5_process_block (buffer, len & ~63, ctx);
	  buffer = (const char *) buffer + (len & ~63);
	  len &= 63;
	}
    }

  
  if (len > 0)
    {
      size_t left_over = ctx->buflen;

      memcpy (&ctx->buffer[left_over], buffer, len);
      left_over += len;
      if (left_over >= 64)
	{
	  md5_process_block (ctx->buffer, 64, ctx);
	  left_over -= 64;
	  memcpy (ctx->buffer, &ctx->buffer[64], left_over);
	}
      ctx->buflen = left_over;
    }
}




#define FF(b, c, d) (d ^ (b & (c ^ d)))
#define FG(b, c, d) FF (d, b, c)
#define FH(b, c, d) (b ^ c ^ d)
#define FI(b, c, d) (c ^ (b | ~d))



static void
md5_process_block (const void *buffer, size_t len, struct md5_ctx *ctx)
{
  md5_uint32 correct_words[16];
  const md5_uint32 *words = (const md5_uint32*) buffer;
  size_t nwords = len / sizeof (md5_uint32);
  const md5_uint32 *endp = words + nwords;
  md5_uint32 A = ctx->A;
  md5_uint32 B = ctx->B;
  md5_uint32 C = ctx->C;
  md5_uint32 D = ctx->D;

  
  ctx->total[0] += len;
  if (ctx->total[0] < len)
    ++ctx->total[1];

  
  while (words < endp)
    {
      md5_uint32 *cwp = correct_words;
      md5_uint32 A_save = A;
      md5_uint32 B_save = B;
      md5_uint32 C_save = C;
      md5_uint32 D_save = D;

      

#define OP(a, b, c, d, s, T)						\
      do								\
        {								\
	  a += FF (b, c, d) + (*cwp++ = SWAP (*words)) + T;		\
	  ++words;							\
	  CYCLIC (a, s);						\
	  a += b;							\
        }								\
      while (0)

      
#define CYCLIC(w, s) (w = (w << s) | (w >> (32 - s)))

      

      
      OP (A, B, C, D,  7, 0xd76aa478);
      OP (D, A, B, C, 12, 0xe8c7b756);
      OP (C, D, A, B, 17, 0x242070db);
      OP (B, C, D, A, 22, 0xc1bdceee);
      OP (A, B, C, D,  7, 0xf57c0faf);
      OP (D, A, B, C, 12, 0x4787c62a);
      OP (C, D, A, B, 17, 0xa8304613);
      OP (B, C, D, A, 22, 0xfd469501);
      OP (A, B, C, D,  7, 0x698098d8);
      OP (D, A, B, C, 12, 0x8b44f7af);
      OP (C, D, A, B, 17, 0xffff5bb1);
      OP (B, C, D, A, 22, 0x895cd7be);
      OP (A, B, C, D,  7, 0x6b901122);
      OP (D, A, B, C, 12, 0xfd987193);
      OP (C, D, A, B, 17, 0xa679438e);
      OP (B, C, D, A, 22, 0x49b40821);

      
#undef OP
#define OP(f, a, b, c, d, k, s, T)					\
      do 								\
	{								\
	  a += f (b, c, d) + correct_words[k] + T;			\
	  CYCLIC (a, s);						\
	  a += b;							\
	}								\
      while (0)

      
      OP (FG, A, B, C, D,  1,  5, 0xf61e2562);
      OP (FG, D, A, B, C,  6,  9, 0xc040b340);
      OP (FG, C, D, A, B, 11, 14, 0x265e5a51);
      OP (FG, B, C, D, A,  0, 20, 0xe9b6c7aa);
      OP (FG, A, B, C, D,  5,  5, 0xd62f105d);
      OP (FG, D, A, B, C, 10,  9, 0x02441453);
      OP (FG, C, D, A, B, 15, 14, 0xd8a1e681);
      OP (FG, B, C, D, A,  4, 20, 0xe7d3fbc8);
      OP (FG, A, B, C, D,  9,  5, 0x21e1cde6);
      OP (FG, D, A, B, C, 14,  9, 0xc33707d6);
      OP (FG, C, D, A, B,  3, 14, 0xf4d50d87);
      OP (FG, B, C, D, A,  8, 20, 0x455a14ed);
      OP (FG, A, B, C, D, 13,  5, 0xa9e3e905);
      OP (FG, D, A, B, C,  2,  9, 0xfcefa3f8);
      OP (FG, C, D, A, B,  7, 14, 0x676f02d9);
      OP (FG, B, C, D, A, 12, 20, 0x8d2a4c8a);

      
      OP (FH, A, B, C, D,  5,  4, 0xfffa3942);
      OP (FH, D, A, B, C,  8, 11, 0x8771f681);
      OP (FH, C, D, A, B, 11, 16, 0x6d9d6122);
      OP (FH, B, C, D, A, 14, 23, 0xfde5380c);
      OP (FH, A, B, C, D,  1,  4, 0xa4beea44);
      OP (FH, D, A, B, C,  4, 11, 0x4bdecfa9);
      OP (FH, C, D, A, B,  7, 16, 0xf6bb4b60);
      OP (FH, B, C, D, A, 10, 23, 0xbebfbc70);
      OP (FH, A, B, C, D, 13,  4, 0x289b7ec6);
      OP (FH, D, A, B, C,  0, 11, 0xeaa127fa);
      OP (FH, C, D, A, B,  3, 16, 0xd4ef3085);
      OP (FH, B, C, D, A,  6, 23, 0x04881d05);
      OP (FH, A, B, C, D,  9,  4, 0xd9d4d039);
      OP (FH, D, A, B, C, 12, 11, 0xe6db99e5);
      OP (FH, C, D, A, B, 15, 16, 0x1fa27cf8);
      OP (FH, B, C, D, A,  2, 23, 0xc4ac5665);

      
      OP (FI, A, B, C, D,  0,  6, 0xf4292244);
      OP (FI, D, A, B, C,  7, 10, 0x432aff97);
      OP (FI, C, D, A, B, 14, 15, 0xab9423a7);
      OP (FI, B, C, D, A,  5, 21, 0xfc93a039);
      OP (FI, A, B, C, D, 12,  6, 0x655b59c3);
      OP (FI, D, A, B, C,  3, 10, 0x8f0ccc92);
      OP (FI, C, D, A, B, 10, 15, 0xffeff47d);
      OP (FI, B, C, D, A,  1, 21, 0x85845dd1);
      OP (FI, A, B, C, D,  8,  6, 0x6fa87e4f);
      OP (FI, D, A, B, C, 15, 10, 0xfe2ce6e0);
      OP (FI, C, D, A, B,  6, 15, 0xa3014314);
      OP (FI, B, C, D, A, 13, 21, 0x4e0811a1);
      OP (FI, A, B, C, D,  4,  6, 0xf7537e82);
      OP (FI, D, A, B, C, 11, 10, 0xbd3af235);
      OP (FI, C, D, A, B,  2, 15, 0x2ad7d2bb);
      OP (FI, B, C, D, A,  9, 21, 0xeb86d391);

      
      A += A_save;
      B += B_save;
      C += C_save;
      D += D_save;
    }

  
  ctx->A = A;
  ctx->B = B;
  ctx->C = C;
  ctx->D = D;
}

} 
} 
} 






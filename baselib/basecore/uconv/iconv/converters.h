





typedef unsigned int ucs4_t;


typedef unsigned int state_t;


typedef struct conv_struct * conv_t;


struct mbtowc_funcs {
  int (*xxx_mbtowc) (conv_t conv, ucs4_t *pwc, unsigned char const *s, int n);
  
  int (*xxx_flushwc) (conv_t conv, ucs4_t *pwc);
  
};


#define RET_SHIFT_ILSEQ(n)  (-1-2*(n))

#define RET_ILSEQ           RET_SHIFT_ILSEQ(0)

#define RET_TOOFEW(n)       (-2-2*(n))

#define DECODE_SHIFT_ILSEQ(r)  ((unsigned int)(RET_SHIFT_ILSEQ(0) - (r)) / 2)
#define DECODE_TOOFEW(r)       ((unsigned int)(RET_TOOFEW(0) - (r)) / 2)


struct wctomb_funcs {
  int (*xxx_wctomb) (conv_t conv, unsigned char *r, ucs4_t wc, int n);
  
  int (*xxx_reset) (conv_t conv, unsigned char *r, int n);
  
};

struct loop_funcs {};


#define RET_ILUNI      -1

#define RET_TOOSMALL   -2


struct conv_struct {
  struct loop_funcs lfuncs;
  
  int iindex;
  struct mbtowc_funcs ifuncs;
  state_t istate;
  
  int oindex;
  struct wctomb_funcs ofuncs;
  int oflags;
  state_t ostate;
  
  int transliterate;
  int discard_ilseq;
#if 0
  #ifndef LIBICONV_PLUG
  struct iconv_fallbacks fallbacks;
  struct iconv_hooks hooks;
  #endif
#endif
};



typedef struct {
  unsigned short indx; 
  unsigned short used; 
} Summary16;

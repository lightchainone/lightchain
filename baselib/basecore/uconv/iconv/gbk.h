





#include "gbkext1.h"
#include "gbkext2.h"
#include "gbkext_inv.h"
#include "cp936ext.h"

static int
gbk_mbtowc (conv_t conv, ucs4_t *pwc, const unsigned char *s, int n)
{
  unsigned char c = *s;

  if (c >= 0x81 && c < 0xff) {
    if (n < 2)
      return RET_TOOFEW(0);
    if (c >= 0xa1 && c <= 0xf7) {
      unsigned char c2 = s[1];
      if (c == 0xa1) {
        if (c2 == 0xa4) {
          *pwc = 0x00b7;
          return 2;
        }
        if (c2 == 0xaa) {
          *pwc = 0x2014;
          return 2;
        }
      }
      if (c2 >= 0xa1 && c2 < 0xff) {
        unsigned char buf[2];
        int ret;
        buf[0] = c-0x80; buf[1] = c2-0x80;
        ret = gb2312_mbtowc(conv,pwc,buf,2);
        if (ret != RET_ILSEQ)
          return ret;
        buf[0] = c; buf[1] = c2;
        ret = cp936ext_mbtowc(conv,pwc,buf,2);
        if (ret != RET_ILSEQ)
          return ret;
      }
    }
    if (c >= 0x81 && c <= 0xa0)
      return gbkext1_mbtowc(conv,pwc,s,2);
    if (c >= 0xa8 && c <= 0xfe)
      return gbkext2_mbtowc(conv,pwc,s,2);
    if (c == 0xa2) {
      unsigned char c2 = s[1];
      if (c2 >= 0xa1 && c2 <= 0xaa) {
        *pwc = 0x2170+(c2-0xa1);
        return 2;
      }
    }
  }
  return RET_ILSEQ;
}

static int
gbk_wctomb (conv_t conv, unsigned char *r, ucs4_t wc, int n)
{
  unsigned char buf[2];
  int ret;

  if (wc != 0x30fb && wc != 0x2015) {
    ret = gb2312_wctomb(conv,buf,wc,2);
    if (ret != RET_ILUNI) {
      if (ret != 2) abort();
      if (n < 2)
        return RET_TOOSMALL;
      r[0] = buf[0]+0x80;
      r[1] = buf[1]+0x80;
      return 2;
    }
  }
  ret = gbkext_inv_wctomb(conv,buf,wc,2);
  if (ret != RET_ILUNI) {
    if (ret != 2) abort();
    if (n < 2)
      return RET_TOOSMALL;
    r[0] = buf[0];
    r[1] = buf[1];
    return 2;
  }
  if (wc >= 0x2170 && wc <= 0x2179) {
    if (n < 2)
      return RET_TOOSMALL;
    r[0] = 0xa2;
    r[1] = 0xa1 + (wc-0x2170);
    return 2;
  }
  ret = cp936ext_wctomb(conv,buf,wc,2);
  if (ret != RET_ILUNI) {
    if (ret != 2) abort();
    if (n < 2)
      return RET_TOOSMALL;
    r[0] = buf[0];
    r[1] = buf[1];
    return 2;
  }
  if (wc == 0x00b7) {
    if (n < 2)
      return RET_TOOSMALL;
    r[0] = 0xa1;
    r[1] = 0xa4;
    return 2;
  }
  if (wc == 0x2014) {
    if (n < 2)
      return RET_TOOSMALL;
    r[0] = 0xa1;
    r[1] = 0xaa;
    return 2;
  }

  return RET_ILUNI;
}

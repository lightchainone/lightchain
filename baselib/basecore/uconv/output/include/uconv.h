#ifndef __UCONV_H__
#define __UCONV_H__

#ifndef uint16_t
#define uint16_t unsigned short
#endif 
#ifndef uint8_t
#define uint8_t unsigned char
#endif 

#ifdef __cplusplus
extern "C" {
#endif

#define UCS2_NO_MAPPING (( unsigned short ) 0xfffd)
#define IS_ASCII(a) (0 == (0xff80 & (a)))
    
#define GBK_EURO  ((char)0x80)
#define IS_GBK_EURO(c) ((char)0x80 == (c)) 
#define UCS2_EURO  (( unsigned short ) 0x20ac)
#define IS_UCS2_EURO(c) ((uint16_t)0x20ac == (c))

#define CAST_CHAR_TO_UNICHAR(a) (( unsigned short )((unsigned char)(a)))
#define CAST_UNICHAR_TO_CHAR(a) ((char)a)

#define UINT8_IN_RANGE(min, ch, max) \
    (((uint8_t)(ch) >= (uint8_t)(min)) && ((uint8_t)(ch) <= (uint8_t)(max)))


#define LEGAL_GBK_MULTIBYTE_FIRST_BYTE(c)  \
          (UINT8_IN_RANGE(0x81, (c), 0xFE))
#define FIRST_BYTE_IS_SURROGATE(c)  \
          (UINT8_IN_RANGE(0x90, (c), 0xFE))
#define LEGAL_GBK_2BYTE_SECOND_BYTE(c) \
          (UINT8_IN_RANGE(0x40, (c), 0x7E)|| UINT8_IN_RANGE(0x80, (c), 0xFE))
#define LEGAL_GBK_4BYTE_SECOND_BYTE(c) \
          (UINT8_IN_RANGE(0x30, (c), 0x39))
#define LEGAL_GBK_4BYTE_THIRD_BYTE(c)  \
          (UINT8_IN_RANGE(0x81, (c), 0xFE))
#define LEGAL_GBK_4BYTE_FORTH_BYTE(c) \
          (UINT8_IN_RANGE(0x30, (c), 0x39))

#define LEGAL_GBK_FIRST_BYTE(c)  \
          (UINT8_IN_RANGE(0x81, (c), 0xFE))
#define LEGAL_GBK_SECOND_BYTE(c) \
          (UINT8_IN_RANGE(0x40, (c), 0x7E)|| UINT8_IN_RANGE(0x80, (c), 0xFE))

    enum uconv_flags_t {
        UCONV_INVCHAR_IGNORE = 0x0,  
        UCONV_INVCHAR_REPLACE = 0x1, 
        UCONV_INVCHAR_ERROR = 0x2,    
        UCONV_INVCHAR_ENTITES = 0x4, 
        UCONV_INVCHAR_GBK_EURO = 0x8, 
        UCONV_INVCHAR_DEC_ENTITES = 0x10, 
    };

    
#define ONE_OCTET_BASE          0x00    
#define ONE_OCTET_MASK          0x7F    
#define CONTINUING_OCTET_BASE   0x80    
#define CONTINUING_OCTET_MASK   0x3F    
#define TWO_OCTET_BASE          0xC0    
#define TWO_OCTET_MASK          0x1F    
#define THREE_OCTET_BASE        0xE0    
#define THREE_OCTET_MASK        0x0F    
#define FOUR_OCTET_BASE         0xF0    
#define FOUR_OCTET_MASK         0x07    
#define FIVE_OCTET_BASE         0xF8    
#define FIVE_OCTET_MASK         0x03    
#define SIX_OCTET_BASE          0xFC    
#define SIX_OCTET_MASK          0x01    

#define IS_UTF8_1ST_OF_1(x) (( (x)&~ONE_OCTET_MASK  ) == ONE_OCTET_BASE)
#define IS_UTF8_1ST_OF_2(x) \
((( (x)&~TWO_OCTET_MASK  ) == TWO_OCTET_BASE) && ( (x) != 0xC0 ) && ( (x) != 0xC1))
#define IS_UTF8_1ST_OF_3(x) (( (x)&~THREE_OCTET_MASK) == THREE_OCTET_BASE)
#define IS_UTF8_1ST_OF_4(x) (( (x)&~FOUR_OCTET_MASK ) == FOUR_OCTET_BASE)
#define IS_UTF8_1ST_OF_5(x) (( (x)&~FIVE_OCTET_MASK ) == FIVE_OCTET_BASE)
#define IS_UTF8_1ST_OF_6(x) (( (x)&~SIX_OCTET_MASK  ) == SIX_OCTET_BASE)
#define IS_UTF8_2ND_THRU_6TH(x) \
        (( (x)&~CONTINUING_OCTET_MASK  ) == CONTINUING_OCTET_BASE)
#define IS_UTF8_1ST_OF_UCS2(x) \
        IS_UTF8_1ST_OF_1(x) \
|| IS_UTF8_1ST_OF_2(x) \
|| IS_UTF8_1ST_OF_3(x)

    


    
    int gbk_to_unicode(const char *src, unsigned int src_len,  unsigned short  *dst, unsigned int dst_size,
                       int flags);

    


    
    int unicode_to_gbk(const  unsigned short  * src, unsigned int src_len, char * dest, unsigned int size,
                       int flags,  unsigned short  replace_char);

    

    int gbk_to_utf8(const char *src, unsigned int src_len, char *dst, unsigned int dst_size,
                    int flags);


    
    int utf8_to_gbk(const char *src, unsigned int src_len, char *dst, unsigned int dst_size,
                    int flags);


    
    int is_utf8(const char *str, unsigned int len, int check_last_char);

 
  	int is_utf8_ex(const char *src, unsigned int len);
    
    int is_utf8_strict(const char *src, unsigned int len, bool is_ascii_utf8);

    
    int uconv_is_gbk(const char * src);

    
    int uconv_is_gbk_n(const char * src,
                       int length);


#ifndef uint32_t
#define uint32_t unsigned int
#endif
#define UCS4_NO_MAPPING ((uint32_t) 0xfffdffff)
#define CAST_UCS4_TO_UCHAR(a) ((unsigned char)a)

    
    int gb18030_to_ucs4(const unsigned char *src,
                        unsigned int src_len,
                        uint32_t *dst,
                        unsigned int dst_size,
                        int flags);

    
    int ucs4_to_gb18030(const uint32_t * src,
                        unsigned int src_len,
                        unsigned char * dst,
                        unsigned int dst_size,
                        int flags,
                        uint32_t replace_char);

    

    int gb18030_to_utf8(const unsigned char *src,
                        unsigned int src_len,
                        unsigned char *dst,
                        unsigned int dst_size,
                        int flags);


    
    int utf8_to_gb18030(const unsigned char *src,
                        unsigned int src_len,
                        unsigned char *dst,
                        unsigned int dst_size,
                        int flags);

#ifdef __cplusplus
}
#endif

#endif 

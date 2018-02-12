/**
 */
#ifndef _UL_MODULE_STRING_
#define _UL_MODULE_STRING_

extern int UL_CHAR_SPACE[];
extern char legal_char_set[];
extern char url_eng_set[];
extern char legal_word_set[];

/**
 *  判断给定的字符是否为空格\n
 *  字符串存放到dststr中，srcstr保持不变
 *  
 *  - 0 表示不是空格
 *  - 1 表示是空格
 */
#define ul_isspace(ch) UL_CHAR_SPACE[(unsigned char)(ch)]

/**
 *  将字符串中的大写字母转换为小写字母
 *  
 *  - 0 表示转换失败
 *  - 1 表示转换成功
 */
int ul_tolowerstr_singlebuf(unsigned char *pstr);

/**
 *  将大写字母转换为小写字母，非大写字母则返回字母本身
 *  
 */
int ul_tolower(unsigned char srcchar);

/**
 *  将字符串中的大写字母转换为小写，并将转换后的\n
 *  字符串存放到dststr中，srcstr保持不变
 *  
 *  - 0 表示转换失败
 *  - 1 表示转换成功
 */
int ul_tolowerstr(unsigned char *dststr, unsigned char *srcstr);

/**
 *  从字符串中取出第一个单词，并返回下一个单词的位置
 *  
 */
char *ul_sgetw(char *ps_src, char *ps_result, char *charset);

/**
 *  从字符串中取出第一个单词，并返回下一个单词的位置，支持双字节编码
 *  
 */
char *ul_sgetw_ana(char *ps_src, char *ps_result, char *charset);


/**
 *  不区分大小写字符串比较(利用map实现)
 *  
 * - 1 相等
 * - 0 不相等
 */
int ul_strncasecmp(const char *s1, const char *s2, size_t n);

/**
 * 			from the NUL-terminated string src to dst, NULL-terminating the result.
 *
**/
size_t ul_strlcpy(char *dst, const char *src, size_t siz);

/**
 * 			It will append at most size - strlen(dst) - 1 bytes, NUL-terminating the result.
 *
**/
size_t ul_strlcat(char *dst, const char *src, size_t siz);

#endif

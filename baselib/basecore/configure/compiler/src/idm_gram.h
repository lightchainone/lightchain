






#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   
   enum yytokentype {
     KW_OVERWRITE = 258,
     KW_STRUCT = 259,
     KW_RAW = 260,
     KW_CHAR = 261,
     KW_UCHAR = 262,
     KW_INT8 = 263,
     KW_UINT8 = 264,
     KW_INT16 = 265,
     KW_UINT16 = 266,
     KW_INT32 = 267,
     KW_UINT32 = 268,
     KW_INT64 = 269,
     KW_UINT64 = 270,
     KW_FLOAT = 271,
     KW_DOLCLE = 272,
     KW_STRING = 273,
     INTEGER = 274,
     ID = 275,
     LITERAL_STRING = 276,
     DOLCLE = 277,
     VAR_CMD = 278
   };
#endif
#define KW_OVERWRITE 258
#define KW_STRUCT 259
#define KW_RAW 260
#define KW_CHAR 261
#define KW_UCHAR 262
#define KW_INT8 263
#define KW_UINT8 264
#define KW_INT16 265
#define KW_UINT16 266
#define KW_INT32 267
#define KW_UINT32 268
#define KW_INT64 269
#define KW_UINT64 270
#define KW_FLOAT 271
#define KW_DOLCLE 272
#define KW_STRING 273
#define INTEGER 274
#define ID 275
#define LITERAL_STRING 276
#define DOLCLE 277
#define VAR_CMD 278




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 40 "idm.gram"
typedef union YYSTYPE {
  meta_t *meta;
  var_t *var;
  group_t *grp;
  cf_list_t *cf_list;
  meta_list_t *meta_list;
  var_list_t *var_list;
  cf_t *cf;
} YYSTYPE;

#line 93 "idm_gram.h"
# define yystype YYSTYPE 
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif






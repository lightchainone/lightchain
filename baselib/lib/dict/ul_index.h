/**
 */
//**********************************************************************
//
//			Indexing Utilities (IU) 1.0
//
//	These utilities are used to create signature, create index
//	merge index, and manage index.
//**********************************************************************

#ifndef __INDEX_H__
#define __INDEX_H__

#include "ul_dict.h"

#define WORKPATH		"PARSEPATH"
#define	MAX_URL_PER_TERM	100000
//#define	MAX_URL_PER_TERM	100000
#define	SURL_NUM_PER_FILE	1000000
#define	MERGE_FILE_NUM          (300)   // must define
#define MERGE_INDEX_SLOT	(1000)
#define	BASE_PER100		90
#define MAX_LIMIT_PAGE_NUM	(1000*1000*20)

#define	URL_DICT_SRC	1	/* only deal with the url.src */
#define	URL_DICT_DEL	2	/* deal with the file url.src and url.del */


/* added by Xiao Wang */
#define BFURL_CACHE_FLAG(info)   (((unsigned short)(info).code_type)>>15)
#define BFURL_CODETYPE(info)   ((info).code_type&0x7fff)
/* end add */

#define Dweight( index2 )	((unsigned int) ((index2)>>25))
#define Durlno( index2 )	(index2 & 0x1ffffff)
#define Dindex2( w, uno )	((unsigned int) ((w<<25)|(uno&0x1ffffff)))

//*******************************
// Structure definition
//*******************************

//typedef struct {
//	unsigned 7;	//weight
//	unsigned 25;	//term_no
//} Sterm;

typedef struct {
	int off; 	// offset in index2
	int num;	// number of this index
} Sindex1;

 typedef struct {
 	unsigned weight : 7;	// weight
	unsigned url_no : 25;	// url_no
} Sindex2;

typedef struct {
	u_int site_sign1;	// signature of the site
	u_int site_sign2;	// signature of the site
	u_int con_sign1;	// signature of the content of the url
	u_int con_sign2;	// signature of the content of the url
	u_int lastmod_time;	// lastmodify time 
	short code_type;	// code type of content 
	short ip_region;	// ip region
} Surlbrief;

/*
typedef struct {
	u_int term_sign1;
	u_int term_sign2;
	u_int url_sign1;
	u_int url_sign2;
	int   weight;
} Slinksign;
*/

typedef struct {
	u_int term_sign1;
	u_int term_sign2;
	u_int url_sign1;
	u_int url_sign2;
	u_int weight;
} Slinksign;

typedef struct {
	u_int term_code;
	unsigned url_code : 25;
	unsigned weight : 7;
} Slinkcode;

//**********************************
// functions : manage index
//**********************************

int build_term_dict(char *path, char *sfterm, char *dfdict, char *dfterm);
int rebuild_dictn(char *path, int cur_times);
int rebuild_dict1(char *path, int cur_ind);

int reweight_term(char *path, char *fterm, char *fbrief);

int build_brief_info(char *path, char *sfn_site, char *dfn_brief);
int build_part_index(char *path, char* fn_dict, char *fn_url,
                int cur_parno);
int merge_part_index( char *path, char *sf_name, char *df_name,
		char *lweight, char *fn_idf );
int merge_one_info( char *path, char *sfile, int sstart, int send,
				char *dfile, int dsize);
int merge_part_info( char *path, char *sf_disp, char *df_disp,
		char *sf_brf, char *df_brf, char *sf_cache, char *df_cache);
int assign_index_work(char *path, char *fnsrc, char *fndes);

int rebuild_url_dict(char *path, char *preurl, char *nowurl, char *fninv,
		int init_num, int build_mode);

int rebuild_url_inv(char *path, char *preurl, char *nowurl, char *fninv);

int rebuild_dict_filter(char *path, char *dbase, char *dpre, char *dnew,
                int lowest);

int coding_term2(char *path, char *fndict, char *fnterm2, char *fnterm3);

int url_num_in_term(u_int *pind2, int num);

int coding_link(Sdict_search *pterm, Sdict_search *purl, char *path,
		char *fnsrc, char *fndes);

int trunc_index2(u_int *pindex2, int *pind_num);

int merge_link_index(char *path, char *fnlink, char *fnindex, char *fndes,
		char *dict, int per100);

int merge_all_index( char *path, char *sf_name, char *lf_name, char *df_name,
		char *fn_lweight, char *fn_idf);

int limit_index(char *path);
#endif

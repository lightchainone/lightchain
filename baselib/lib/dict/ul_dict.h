/**
 */
//***********************************************************************
//			Dictionary Utilities (DU) 1.0
//
//	These utilities are used to manage the dictionary. Its main funcion
//	is to give a code to a string. 
//***********************************************************************


#ifndef __DICT_H__
#define __DICT_H__

#include <ul_def.h>

/*
#define IS_GBK(ps)	((((u_char)ps[0])>0x7f) && (((u_char)ps[0])<0xff))\
		      && (((u_char)ps[1])>0x39) && (((u_char)ps[1])<0xff)))
*/

#define Dlweight( index2 )	((int) (((unsigned int)(index2))>>25))
#define Dfreq( index2 )		(index2 & 0x1ffffff)
#define Dterm( w, uno )		((unsigned int) ((w<<25)|(uno&0x1ffffff)))

#define Dcpoff(i1, i2, i3)	((((i1*CHAR_LONUM)+i2)*CHAR_HINUM)+i3)

//********************************
// structs for building dictionary
//********************************

#define PHRASE_IND1_NUM	100001
#define	SUR_IND1_NUM	65536
#define	NODE_BLOCK_NUM	(256*1024)
#define MID_NODE_NUM	(4*1024)
#define BASE_SIGN	65547
#define MAX_SQRT_NUM	400000

#define	BASE_DICT_NUM	1000000
#define	BIG_DICT_HNUM	10000001

#define	IND2_NUM_PER_FILE	100000
#define WORDDELIMITER	'|'

/* this enum defined for dict operation db_op & ds_op */
typedef enum{
ADD,		//for dict building	: have been in dict(f++) not(add)
CLUSTER,	//for searcher cluster	: same sign -> keep the high weight
MOD,		//for dict building	: modify the struct of dict
SEEK,		//for dict building	: seek by sign
SEEKP,		//for parser identify	: found -> dict freq ++
DEL,		//for dict building	: del by sign
MODF,		//for dict searching	: frequence
MODW,		//for dict searching	: lowest weight
MODC		//for dict searching	: term code
} Eop_method;

/* for node in mem, using link */
struct ss_dict_build_node{
	u_int 	sign1;	//signature 1
	u_int 	sign2;	//signature 2
	int 	code;	//term code
	u_int 	other;	//lowest_weight(7) + frequence(25)
	struct 	ss_dict_build_node *next;	//next node
};
typedef struct ss_dict_build_node Sdict_bnode;

/* for hash in mem */
typedef struct{
	Sdict_bnode	*pnode;	//hash table
} Sdict_bhash;

/* for block of nodes in mem */
struct ss_dict_build_node_block{
	Sdict_bnode nodes[NODE_BLOCK_NUM];	//block of nodes, for build
	struct ss_dict_build_node_block	*next;	//next block
};
typedef struct ss_dict_build_node_block	Sdict_bnode_block;

/* for dict in mem */
typedef struct{
	int	hash_num;	// number of hash
	int	node_num;	// number of nodes
	int	cur_code;	// current minimum unused term code

	Sdict_bhash	*hash;	// hash

	Sdict_bnode_block	*nblock;	//first block
	Sdict_bnode_block	*cur_nblock;	//current block
	Sdict_bnode		*cur_node;	//current node
	int			node_avail;	//avail node_num in cur_b
} Sdict_build;

/* for creating key(sign-weight) list from dict -> for parser */
typedef struct{
	u_int sign1;
	u_int sign2;
	int weight;
} Skeyweight;

typedef struct{
	int 		num;
	Skeyweight 	*key;
} Skeyweightlist;

typedef struct{
	unsigned int	weight;
	unsigned int	urlno;
} Slongindex2;

typedef struct{
	int		num;
	void		*index2;
	int		*rept;
} Spagecluster;


//********************************
// structs for dictionary search
//********************************

/* node */
typedef struct{
	u_int 	sign1;		//signature 1
	u_int 	sign2;		//signature 2
	int 	code;		//term code
	u_int 	other;	//(7:lowest_w+25:freq)
} Sdict_snode;

/* hash, implemented by unsinged int */
//typedef struct{
//	unsigned : 25 	//off
//	unsigned : 7	//num
//} Sdict_shash;

/* dict */
typedef struct{
	int	hash_num;
	int	node_num;
	int 	cur_code;	// current minimum code

	unsigned int	*hash;	// hash
	unsigned int 	*num;	// number of nodes in hash entries
	Sdict_snode	*node;	// nodes
} Sdict_search;

typedef struct{
	int hash_num;
	int node_num;
	int cur_code;

	int find1;
	int find2;
}Sdict_search_hd;


//*********************************
// structs for phrase dictionary
//*********************************

typedef struct {
	int off;
	int num;
} Shash_cphrase;

typedef struct {
	Shash_cphrase *hash;
	unsigned char *phrase;
	char	      *surname;
} Sdict_cphrase;

typedef struct{
	int off;
	int num;
} Shash_phrase;

typedef struct{
	Shash_phrase	*hash;
	char		*phrase;
} Sdict_phrase;


//*********************************
// structs for synonym dictionary
//*********************************

#define WORDBUFSIZE (512*1024)

struct ss_dict_build_syn_node{
	u_int 	sign1;	//signatures of word 1
	u_int 	sign2;
	u_int	sign3;	//signatures of word 2
	u_int	sign4;
	struct 	ss_dict_build_syn_node *next;	//next node
};
typedef struct ss_dict_build_syn_node Sdict_syn_bnode;

typedef struct{
	Sdict_syn_bnode	*pnode;	//hash table
} Sdict_syn_bhash;

struct ss_dict_syn_build_node_block{
	Sdict_syn_bnode nodes[NODE_BLOCK_NUM];	//block of nodes, for build
	struct ss_dict_syn_build_node_block	*next;	//next block
};
typedef struct ss_dict_syn_build_node_block	Sdict_syn_bnode_block;

struct ss_dict_syn_buffer_block{
	char	buf[WORDBUFSIZE];	//buffer block for words
	struct ss_dict_syn_buffer_block	*next;	//next block
};
typedef struct ss_dict_syn_buffer_block	Sdict_syn_buf_block;

typedef struct{
	int	hash_num;	// number of hash
	int	node_num;	// number of nodes

	Sdict_syn_bhash	*hash;	// hash

	Sdict_build		*pdword;	// word dictionary
	int			words;

	Sdict_syn_bnode_block	*nblock;	// first node block
	Sdict_syn_bnode_block	*cur_nblock;	// current node block
	Sdict_syn_bnode		*cur_node;	// current node
	int			node_avail;	// available node_num in cur_nblock

	Sdict_syn_buf_block	*bufblock;	// first buffer block
	Sdict_syn_buf_block	*cur_bufblock;	// current buffer block
	int			nrbufblock;	// number of buffer blocks
	int			cur_buf;	// available buffer
} Sdict_build_syn;

typedef struct{
	u_int 	sign1;	//signatures of word 1
	u_int 	sign2;
	u_int	sign3;	//signatures of word 2
	u_int	sign4;
} Sdict_syn_snode;

typedef struct{
	int	hash_num;
	int	node_num;
	Sdict_search	*pdword;// word dictionary

	unsigned int	*hash;	// hash
	unsigned int 	*num;	// number of nodes in hash entries
	Sdict_syn_snode	*node;	// nodes
	char		*wordbuffer;
} Sdict_search_syn;

//********************************
// functions : synonym dictionary
//********************************
 

/**
 *  构建同义词词典，并按文件中的相互关系创建联系
 *  
 * - 非NULL   成功
 * - NULL     失败
 * - 0 每两个词之间建立双向联系,相互可以查到
 * - 1 每行第一个词和后面所有的词建立双向联系
 */
Sdict_build_syn *db_syn_construct( char *fullpath, int hash_num, int dict_type );


/**
 *  创建同义词字典，并按文件中的单向关系创建联系
 *  
 * - 非NULL   成功
 * - NULL     失败
 * - 0 每个词与后面一个词之间的关系（单向）
 * - 1 每个词与第一个词之间的关系（单向）
 */
Sdict_build_syn *db_syn_construct_unidirect( char *fullpath, int hash_num, int dict_type );


/**
 *  同义词dict创建
 *  
 * - 非NULL   成功
 * - NULL     失败
 */
Sdict_build_syn *db_syn_creat( int hash_num );



/**
 *  释放同义词词典
 *  
 * - 1        成功
 * - other    失败
 */
int db_syn_del( Sdict_build_syn *pdb );




/**
 *  调整词典中的hash结构
 *  
 * - 0     成功
 * - -2    失败
 */
int db_syn_adjust( Sdict_build_syn *pdb );
//int db_syn_op1( Sdict_build_syn *pdb, Sdict_syn_snode *pnode, Eop_method op_method);

/**
 *  向同义词词典中添加一对同义词
 *  
 * - 0     成功
 * - -1    失败
 * - 1     同义词已经存在
 */
int db_syn_add( Sdict_build_syn *pdb, char *word1, char *word2 );



/**
 *  取出一个词的所有同义词
 *  
 * - >0    同义词的个数
 * - -1    失败
 */
int db_syn_seek( Sdict_build_syn *pdb, char *word, char *outbuf, int len );



/**
 *  将词典存储到文件中
 *  
 * - 1    存储成功
 * - -1   失败
 * - -3   文件打开失败
 */
int db_syn_save( Sdict_build_syn *pdb, char *path, char *fname );




/**
 *  从文件中load词典(build型)
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_build_syn *db_syn_load( char *path, char *fname, int hash_n);



/**
 *  从文件中load词典(search型)
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_search_syn  *ds_syn_load( char *path, char *fname );






/**
 *  释放一个只读的同义词词典(search型)
 *  
 * - 0      成功
 * - other  失败
 */
int ds_syn_del( Sdict_search_syn *pds );






/**
 *  在词典中查找(search型)
 *  
 */
int ds_syn_seek( Sdict_search_syn *pds, char *word, char *outbuf, int len );
//int ds_syn_op1( Sdict_search_syn *pds, Sdict_syn_snode *pnode, Eop_method op_method);
















//**********************
// functions : signature
//**********************


/**
 *  创建一个签名，31bit和32bit是标记位
 *  
 *  - 31th=1 32th=1 : 中文,两个字
 *  - 31th=1 32th=0 : 中文,短语
 *  - 31th=0 32th=? : 英文单词
 */
u_int creat_term_sign(char *str_src);




/**
 *  创建一个32bit签名
 *  
 */
u_int creat_sign32(char *str_src);



/**
 *  为字符串创建一个64bit签名
 *  
 */
int creat_sign64(char *str_src, u_int *psign1, u_int *psign2);



/**
 *  为buf创建一个64bit签名
 *  
 */
int creat_sign_t64(void *t_src, int t_size, char *pstr, 
		u_int *psign1, u_int *psign2);
		
		
		
/**
 *  为buf创建一个64bit签名
 *  
 */
int creat_sign_nt64(void *t_src, int t_size, char *pstr, 
		u_int *psign1, u_int *psign2);




int creat_sign_t256(char *psrc, int slen, u_int* psign1, u_int* psign2);


//******************************
// functions : create dictionary
//******************************



/**
 *  创建字典
 *  
 * - 非NULL   成功
 * - NULL     失败
 */ 
Sdict_build *db_creat( int hash_num, int now_code );


/**
 *  清空字典
 *  
 * - 1   成功
 * - other     失败
 */ 
int db_renew( Sdict_build *pdb);


/**
 *  清空字典
 *  
 * - 1   成功
 * - other     失败
 */ 
int db_renew_fast(Sdict_build *pdb);



/**
 *  释放字典资源
 *  
 * - 1         成功
 * - other     失败
 */
int db_del( Sdict_build *pdb);



/**
 *  各种字典操作
 *  
 * - 1         成功
 * - 0         没找到
 * - other     失败
 * - SEEK 查找，如果找到，用pnode返回找到的节点信息，函数返回1；如果没找到，函数返回0,同时Pnode->code = -1
 * - MOD  修改，如果项目在字典中，修改节点，函数返回1；否则，返回0
 * - DEL  删除操作，从字典中删除改节点,但不回收,只是修改字典内节点的code值为-1.如果是频繁的删除并插入操作,需要在上层定义自己的逻辑,合理使用MOD选项进行删除.
 * - MODW 修改权重
 * - MODF 修改词频
 * - ADD  如果查找项在字典中，修改词频，并且返回结点信息。如果不在字典中，添加新节点，并初始化节点（code=cur_code, freq=1,low_weight=0）返回新节点的信息。
 *   这个操作修改会带来字典中数据的变动,谨慎使用,如果不是做分词等工作,建议不使用此dict.
 *   另外,可采用先插入,后修改的方式来真正确保插入的数据不会改变:具体如下:
 *            node.code = -1;
 *            node.other = 0;
 *            db_op1(dict,&node,ADD);
 *            node.code = v1;
 *            node.other = v2;
 *            db_op1(dict,&node,MOD); 
 */
int db_op1( Sdict_build *pdb, Sdict_snode *pnode, Eop_method op_method);


/**
 *  批量对字典操作
 *  
 * - 1         成功
 * - 0         没找到
 * - other     失败
 * - SEEK 查找，如果找到，用pnode返回找到的节点信息，函数返回1；如果没找到，函数返回0,同时Pnode->code = -1
 * - MOD  修改，如果项目在字典中，修改节点，函数返回1；否则，返回0
 * - DEL  删除操作，从字典中删除改节点,但不回收,只是修改字典内节点的code值为-1.
 * - MODW 修改权重
 * - MODF 修改词频
 * - ADD  如果查找项在字典中，修改词频，并且返回结点信息。如果不在字典中，添加新节点，并初始化节点（code=cur_code, freq=1,low_weight=0）返回新节点的信息。
 *   这个操作修改会带来字典中数据的变动,谨慎使用,如果不是做分词等工作,建议不使用此dict.
 */
int db_opn( Sdict_build *pdb, Sdict_snode *plnode, int lnum, 
		Eop_method op_method);
		
		
		
/**
 *  保存字典中的key-other对
 *  
 */
int db_keylist (Sdict_build *pdb, Skeyweight *pkey, int pkey_num);









/**
 *  按hash顺序保存code,权重和词频到聚簇数组中
 *  
 *  - >=0 表示返回节点的数量
 *  - <0  表示操作失败
 */
int db_cluster(Sdict_build *pdb, Spagecluster *pcluster);



/**
 *  调整词典中的hash结构
 *  
 * - 0     成功
 * - -2    失败
 */
int db_adjust (Sdict_build *pdb);



/**
 *  将字典存储到文件中
 *  
 * - 1    存储成功
 * - -1   失败
 */
int db_save( Sdict_build *pdb, char *path, char *fname );



/**
 *  将词频存储到文件中,清空词典中的词频
 *  
 * - 2    内存分配失败
 * - -3   打开文件失败
 */
int db_save_freq( Sdict_build *pdb, char *path, char *fname );


/**
 *  从文件中load词典(build型)
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_build *db_load( char *path, char *fname, int h_num );

/**
 * 使用mmap方式打开字典，以在多个进程中共享内存
 * 使用该方式打开的字典只能使用ds_del_mmap方式删除
 * - 非NULL    load成功
 * - NULL      失败
**/
Sdict_search *ds_load_mmap( char *path,  char *fname );

/**
 *  从文件中load词典(build型)
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_build *db_load_0( char *path, char *fname, int h_num );



/**
 *  创建一个字典，输入node数组做聚簇，然后得到聚簇的分析结果
 *  
 */
int page_cluster(Sdict_snode *pnode, int node_num, Spagecluster *pcluster);

// for compatibility only


/**
 *  将node插入到字典中.hash链表按code从小到大的排列，插入也按这个规则插入，如果code相同，那么词频++;for compatibility only
 *  
 * - 1    成功
 * - -1   失败
 */
int db_add_link( Sdict_build *pdb, Sdict_snode *pnode);



/**
 *  重构dict,按某种方式重新计算全重和词频.for compatibility only
 *  
 */
int db_stand_link( Sdict_build *pdb );

/**
 *  在文件中保存hash链表,仅仅保存index code和other的组合换算.for compatibility only
 *  
 * - 1    成功
 * - -2   失败
 */
int db_save_link( Sdict_build *pdb, char *path, char *fname );



/**
 *  在index1中存储的是每个hash位置在文件中起始的node的偏移和node个数;分成多个文件存储index2,在index2中存储的是code和other的一种合成计算
 *  
 * - 1    成功
 * - -2   失败
 */
int db_save_link_search( Sdict_build *pdb, char *path, char *fname );



//******************************
// functions : search dictionary
//******************************



/**
 *  从文件中load字典(search型)
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_search *ds_load( char *path,  char *fname );




/**
 *  从文件中load字典(search型),并把词频和权重清零
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_search *ds_load_parse( char *path,  char *fname );


/**
 *  从文件中load字典(search型),与::ds_load相同
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_search *ds_load_name( char *path,  char *fname );



int ds_reload_test(Sdict_search *pds, char *path, char *fname);



/**
 *  将词频保存在文件中,重新load字典
 *
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_search *ds_reload(Sdict_search *pds, char *path, char *fname, 
		char *cpath);
		
		
/**
 *  释放字典资源
 *
 */
int ds_del(Sdict_search *pds);

/**
 * 删除使用mmap方式打开的字典,需要注意该函数仅能删除用ds_load_mmap建立的字典
**/
int ds_del_mmap(Sdict_search *pds);


/**
 *  将字典保存在文件中
 *
 * - 1    成功
 * - -1   失败
 */
int ds_save(Sdict_search *pds, char *path, char *fname);


/**
 *  将词频保存在文件中,并清空字典中的词频
 *
 * - >0    成功
 * - -2    内存分配失败
 * - -3    文件操作失败
 */
int ds_save_freq(Sdict_search *pds, char *path, char *fname);




/**
 *  自动生成词频文件名,将词频保存在文件中,并清空字典中的词频
 *
 */
int ds_save_parser(Sdict_search *pds, char *path, char *cpath);


/**
 *  各种字典操作
 *  
 * - 1         成功
 * - 0         没找到
 * - other     失败
 * - SEEKP 查找,如果找到,修改词频信息,用pnode并返回找到的结果,函数返回1;如果没找到，函数返回0,同时Pnode->code = -1
 * - SEEK 查找，如果找到，不修改任何信息,用pnode返回找到的节点信息，函数返回1；如果没找到，函数返回0,同时Pnode->code = -1
 * - MODW 修改权重
 * - MODF 修改词频
 * - MODC 修改code,注意,此时并不是利用pnode传递的code值修改,而是利用字典本身的流水号code修改.
 *   这个操作修改会带来字典中数据的变动,谨慎使用,如果不是做分词等工作,建议不使用此dict.
 */
int ds_op1( Sdict_search *pds, Sdict_snode *pnode, Eop_method op_method);

/**
 *  批量字典操作
 *  
 * - 1         成功
 * - 0         没找到
 * - other     失败
 * - SEEKP 查找,如果找到,修改词频信息,用pnode并返回找到的结果,函数返回1;如果没找到，函数返回0,同时Pnode->code = -1
 * - SEEK 查找，如果找到，不修改任何信息,用pnode返回找到的节点信息，函数返回1；如果没找到，函数返回0,同时Pnode->code = -1
 * - MODW 修改权重
 * - MODF 修改词频
 * - MODC 修改code,注意,此时并不是利用pnode传递的code值修改,而是利用字典本身的流水号code修改.
 *   这个操作修改会带来字典中数据的变动,谨慎使用,如果不是做分词等工作,建议不使用此dict.
 */
int ds_opn( Sdict_search *pds, Sdict_snode *plnode, int lnum, 
		Eop_method op_method);
		
		


int pre_creat_search_dict(char *path, char *fnold_dict, char *fnnew_dict,
		char *fn_dbfreq, char *fn_dsfreq);
int creat_search_dict(char *path, char *fnold_dict, char *fnnew_dict,
		char *fn_dbfreq);
int creat_vice_dict(char *path, char *fnbase_dict, char *fnold_dict, 
		char *fnnew_dict, char *fn_dbfreq);
int creat_parser_dict(char *path, char *fnold_dict, char *fnnew_dict,
		char *fn_lw);
int creat_stand_dict(char *path, char *fnsrc, char *fndes, int num );



/**
 *  利用一个字典中词频较大的数据(为保证创建速度,数据并不完全精确),重新创建一个字典
 *  
 * - 非NULL    成功
 * - NULL      失败
 */
Sdict_build *ds_filter(Sdict_search *pds, int number);

//*****************************
// functions : HD version dict
//*****************************



/**
 *  从文件中load字典(search型),仅load索引文件,不load节点数据
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_search_hd *ds_load_hd( char *path,  char *fname );

/**
 *  释放字典资源
 *  
 * - 1         成功
 * - other     失败
 */
int ds_del_hd(Sdict_search_hd *pds);



/**
 *  从硬盘字典中查找
 *  
 * - >=0    节点的code
 * - -1     查找失败(未找到)
 * - -2     内存分配错误
 */
int ds_seek_hd(Sdict_search_hd *pds, Sdict_snode *pnode);

//******************************
// functions : phrase dictionary
//******************************


/**
 *  从文件中load一个静态字符串的字典
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_cphrase *dcp_load(char *path, char *fname, char *fsurname);


/**
 *  在原字符串中搜索所有的中文姓名，以及字典中可匹配的子字符串
 *  
 */
int dcp_seek(Sdict_cphrase *dcp, char *src_ph, char *des_ph);

/**
 *  在原字符串中搜索所有的与字典中可匹配的子字符串，具体应用不祥
 *  
 */
int dcp_search(Sdict_cphrase *dcp, char *src_ph, char *des_ph);

/**
 *  释放字典资源
 *
 */
int dcp_del(Sdict_cphrase *dcp);



/**
 *  根据字符串文件创建一个静态的字符串字典
 *  
 * - 0     build成功
 * - -2    失败
 */
int dp_build(char *path, char *fnsrc, char *fndes);




/**
 *  将字符串字典load到内存中
 *  
 * - 非NULL    load成功
 * - NULL      失败
 */
Sdict_phrase *dp_load(char *path, char *fname);



/**
 *  找到字符串中所有能与字典中的字符串匹配上的子字符串
 *  
 */
int dp_seek(Sdict_phrase *dp, char *ssrc, char *sdes);



/**
 *  找到字典中所有能和原字符串开始部分一致的子字符串
 *  
 * - 1      找到
 * - 0      没找到
 */
int dp_search(Sdict_phrase *dp, char *ssrc, char *sdes);



/**
 *  释放字典资源
 *
 */
int dp_del(Sdict_phrase *pd);


//******************************
// functions : surname dictionary
//******************************
/**
 *  创建姓氏字典
 *  
 * - 非NULL     load成功
 * - NULL       失败
 */
char *dsur_load( char *path, char *fname );


/**
 *  在字符串中过滤出人名，输出到目的字符串中
 *  
 */
int dsur_seek( char *dsur, char *spstr, char *dpstr);


/**
 *  检查字符串是否以中文的姓名开头，如果是，返回1，并把姓名copy到目的字符串
 *  
 */
int dsur_search( char *dsur, char *spstr, char *dpstr);

/**
 *  释放字典资源
 *
 */
int dsur_del( char *dsur );

/*
============
signature

	this group of functions is used to create one or two integer
	to represent a string

======
int creat_term_sign(char *str_src);

*	creat term signature. the 31th bit and the 23th bit is the token
	31th	23th
	1	1	: chinese 2-gram
	1	0	: chinese phrase
	0	?	: english words

*	return value :
		the signature of the term
*	arguments(in) :
		char* str_src : input string

======
int creat_sign32(char *str_src);

*	creat string signature (32 bits) (url, site, content)
*	return value :
		the signature of the input string
*	arguments(in) :
		char* str_src : input string


======
int creat_sign64(char *str_src, int *psign1, int *psign2);

*	creat string signature (64 bits) (url, site, content)
*	return value :
		no
*	arguments(in) :
		char* str_src : input string
	arguments(out):
		int *psign1 : first 32 bits signature
		int *psign2 : other 32 bits signature


============
Create Dictionary

	This group of functions are used to creat codes for a term.
	The dictionary is located in memory. We can add, modify, delete,
	seek a signature in this dictionary. This dictionary's
	performance is worse than the dictionary for searching, but
	the dictionary for searching can not add any new term after
	it is initiated in memory.

======
Sdict_build *db_creat( int hash_num, int now_code );

*	create a dictionary for building in mem. it only allocate the
	space for the dictionary and do not add any new terms.

*	return value
		pointer of dict

*	arguments(in)
		int hash_num	: the initially hash number (good if it
			is equal to node number)
		int now_code	: current minimum term code

======
int db_del( Sdict_build *pdb);

*	delete this dictionary

* 	return value :
		no

*	arguments(in):
		pdb : pointer of the dictionary

======
int db_op1( Sdict_build *pdb, Sdict_snode *pnode, Eop_method op_method);

*	dictionary operations

*	return value :
		1 : success
		0 : have not this term
		other : error

*	arguments(in):
		pdb : pointer of dictionary
		pnode : the infomation for operation ( refer to op_method )
		op_method : operation method
*	arguments(out):
		pnode : the result of the operation

*	notice( about the op_method ) : 
		ADD  : 	if the term have been in dict, freq++ and get the
			info of this node, output by pnode; not in dict,
			add it to dict, code=cur_code, freq=1,
			low_weight=0;
		CLUSTER : for searcher. sign1&sign2 is the data should
			be clustered. code is the url_no. other is the
			weight.
		MOD  :	if term not in dict, MOD failed; in dict, change
			its info by input pnode;
		DEL  :	set code of this term -1 and keep its space.
			when dict is saved, this term will be ignored.
		SEEK :	found in dict, return info by pnode,
			else return 0

======
int db_opn( Sdict_build *pdb, Sdict_snode *plnode, int lnum,
                Eop_method op_method);

*	like op1, but deal with many nodes in one time.

*	arguments(in) :
		lnum	: number of plnode


======
int db_keylist (Sdict_build *pdb, Skeyweightlist *plkey);

*	create the keyword list for parser. This func trans the dict format
	to key-weight format. the key-weight was create by db_op

*	return value :
		the number of the list

*	arguments(in) :
		pdb	: dict
		plkey	: list of key-weight, space was given by caller

*	arguments(out) :
		plkey	: list of key-weight

* 	notice 	:
		this func malloc space for plkey
		plkey->key
		must be freed by caller

======
int db_cluster(Sdict_build *pdb, Spagecluster *pcluster);

*	save the clustered result. / refer to db_op (CLUSTER)

*	return value :
		the number of node after cluster

*	arguments(in) :
		pdb	: dict
		pcluster: clustered result

*	arguments(out) :
		pcluster: clustered result

*	notice :
		pcluster have malloced two block of space
		pcluster->rept & pcluster->index2
		must be freed by the caller.

======
int db_save( Sdict_build *pdb, char *path, char *fname );

*	save the created dictionary to file

*	return value :
		no

*	arguments(in) :
		pdb : dict
		path : working path
		fname : the file name where dict will be saved

======
int db_save_freq(Sdict_search *pds, char *path, char *fname);

*	save the freqence of every term to a file (by indexer)

*	return value :
		no

*	arguments(in) :
		pds	: dict
		path	: path
		fname	: the file name for saving

*	arguments(out) :
		path	: path
		fname	: the file name for saving

======
Sdict_build *db_load( char *path, char *fname, int h_num );

*	load dict from file

*	return value:
		a pointer of the initiated dict

*	arguments(in):
		path : working path
		fname : file name of the dict
		h_num : the hash number, 
			<=0: will according to the number saved in file
			>0 : ingored the number saved in file

============
Search Dictionary

	this dictionary is builled for searching. it save the memory space
	and enchance the performance of seeking. it can deal with the
	concurrency. the shortcoming of it is that it can not add new
	term.

======
Sdict_search *ds_load( char *path,  char *fname );

*	load dict from file

*	return value :
		pointer of dict

*	arguments(in):
		path : working path
		fname : file name of dictionary

======
int ds_del(Sdict_search *pds);

*	free the memory space taken by dict

*	return value:
		no

*	arguments(in):
		pds : pointer of the dict.

======
int ds_save(Sdict_search *pds, char *path, char *fname);

*	save the dict to file

*	return value :
		no

*	arguments(in):
		pds : pointer of dict
		path : working path
		fname : file name of dict

======
int ds_save_freq(Sdict_search *pds, char *path, char *fname);

*	save the freqence of every term to a file (by parser)

*	return value :
		no

*	arguments(in) :
		pds	: dict
		path	: path
		fname	: the file name for saving

*	arguments(out) :
		path	: path
		fname	: the file name for saving

======
int ds_op1( Sdict_search *pds, Sdict_snode *pnode, Eop_method op_method);

* 	operations of dictionary for searching

* 	return value :
		1 : success
		0 : not found
		other : failed

* 	arguments(in):
		pds : pointer of dict
		pnode : term info for operation
		op_method : operation method

*	arguments(out):
		pnode : the result of operation


*	notice (about op_method) :
		SEEK :	seek the term in the dict
		SEEKP:	seek the term in the dict & add the freq of it
		MODF :	freq++
		MODW :	change the lowest_weight to new value
		MODC :	give this term a new code (=cur_code)

======
int ds_opn( Sdict_search *pds, Sdict_snode *plnode, int lnum, 
		Eop_method op_method);

*	like op1, but deal with many nodes in one time.

*	arguments(in) :
		lnum	: number of plnode


============
*/

#endif

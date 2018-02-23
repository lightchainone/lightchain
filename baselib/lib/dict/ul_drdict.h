/**
 */
#ifndef UL_DRDICT_H
#define UL_DRDICT_H


typedef struct{
	int	hash_num;	// number of hash
	int	node_num;	// number of nodes
	int	cur_code;	// current minimum unused term code

	Sdict_bhash	*hash;	// hash
	
	Sdict_bnode_block	*nblock;	//first block
	Sdict_bnode_block	*cur_nblock;	//current block
	Sdict_bnode		*cur_node;	//current node
	Sdict_bnode *reclaim_node;
	int			node_avail;	//avail node_num in cur_b
} Sdict_reclaim;


 
/**
 *  创建字典
 *  
 * - 非NULL   成功
 * - NULL     失败
 */ 
Sdict_reclaim *dr_creat( int hash_num, int now_code );

 
/**
 *  释放字典资源
 *  
 * - 1         成功
 * - other     失败
 */ 
int dr_del( Sdict_reclaim *pdb);



/**
 *  各种字典操作
 *  
 * - 1         成功
 * - 0         失败(无数据)
 * - other     错误
 * - SEEK 查找，如果找到，用pnode返回找到的节点信息，函数返回1；如果没找到，函数返回0
 * - MOD  修改，如果项目在字典中，修改节点，函数返回1；否则，返回0
 * - DEL  删除操作，从字典中删除改节点
 * - ADD  如果查找项在字典中，freq++，并且返回结点信息。如果不在字典中，添加新节点，并初始化节点（code=cur_code, freq=1,low_weight=0）返回新节点的信息。
 */
int dr_op1( Sdict_reclaim *pdb, Sdict_snode *pnode, Eop_method op_method);

#endif

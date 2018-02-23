#include <stdio.h>
#include <fcntl.h>
#include <wctype.h>
#include <assert.h>

#include "ul_dict.h"
#include "ul_sign.h" 
#include "ul_index.h"
#include "ul_charset.h"

#include <ul_func.h>
#include <ul_log.h>
#include <ul_def.h>
#include <ul_conf.h>
#include <ul_file.h>
#include <ul_string.h>

#include "ul_drdict.h"

Sdict_reclaim *dr_creat( int hash_num, int now_code )
{
    Sdict_reclaim *pdb=NULL;
    Sdict_bhash* phash=NULL;
    Sdict_bnode_block* pblock=NULL;

    /* creat dict structure */
    if((pdb = (Sdict_reclaim*) ul_malloc(sizeof(Sdict_build)))==NULL)
        return NULL;
    
    if((phash =(Sdict_bhash*) ul_calloc(1,hash_num*sizeof(Sdict_bhash)))==NULL)
    {
        ul_free(pdb);
        return NULL;
    }
    
    if((pblock = (Sdict_bnode_block*) ul_calloc(1,sizeof(Sdict_bnode_block)))==NULL)
    {
        ul_free(pdb);
        ul_free(phash);
        return NULL;
    }   
    
    pdb->hash=phash;
    pdb->nblock=pblock;
    
    /* init the dict */
    pdb->hash_num = hash_num;
    pdb->cur_code = now_code;
    pdb->node_num = 0;
    pdb->cur_nblock = pdb->nblock;

    pdb->cur_node = pdb->nblock->nodes;
    pdb->node_avail = NODE_BLOCK_NUM;
    pdb->reclaim_node=NULL;
    
    /* return */
    return pdb;
}

int dr_renew(Sdict_reclaim *pdb)
{
    Sdict_bnode_block   *pnb, *next_pnb;
    int     bnode_size;

    bnode_size = sizeof(Sdict_bnode);

    pdb->node_num = 0;
    pdb->cur_code = 0;

    memset(pdb->hash, 0, pdb->hash_num*sizeof(Sdict_bhash));
    pdb->cur_node = pdb->nblock->nodes;
    if (pdb->nblock->next != NULL){
        memset(pdb->cur_node, 0, NODE_BLOCK_NUM*bnode_size );
    }
    else{
        memset(pdb->cur_node, 0, (NODE_BLOCK_NUM - pdb->node_avail)*bnode_size);
    }

    pdb->node_avail = NODE_BLOCK_NUM;

    pnb = pdb->nblock->next;
    pdb->nblock->next = NULL;

    while(pnb!=NULL){
        next_pnb = pnb->next;
        ul_free(pnb);
        pnb = next_pnb;
    }
    pdb->cur_nblock = pdb->nblock;
    pdb->reclaim_node=NULL;
    return 1;

}

int dr_renew_fast(Sdict_reclaim *pdb)
{
    Sdict_bnode_block   *pnb, *next_pnb;
    int     bnode_size;
    int i;


    if(pdb->cur_nblock != pdb->nblock)
    {
        // the other block has been used
        return dr_renew(pdb);
    }
    
    bnode_size = sizeof(Sdict_bnode);

    pdb->node_num = 0;
    pdb->cur_code = 0;

//  memset(pdb->hash, 0, pdb->hash_num*sizeof(Sdict_bhash));
    pdb->cur_node = pdb->nblock->nodes;
    if (pdb->nblock->next != NULL){
        for(i=0;i<NODE_BLOCK_NUM;i++)
        {
            pdb->hash[(pdb->nblock->nodes[i].sign1 + pdb->nblock->nodes[i].sign2)
                      %pdb->hash_num].pnode=NULL;
        }
        memset(pdb->cur_node, 0, NODE_BLOCK_NUM*bnode_size );
    }
    else{
        for(i=0;i<NODE_BLOCK_NUM - pdb->node_avail;i++)
        {
            pdb->hash[(pdb->nblock->nodes[i].sign1 + pdb->nblock->nodes[i].sign2)
                      %pdb->hash_num].pnode=NULL;
        }
        memset(pdb->cur_node, 0, (NODE_BLOCK_NUM - pdb->node_avail)*bnode_size);
    }

    pdb->node_avail = NODE_BLOCK_NUM;

    pnb = pdb->nblock->next;
    pdb->nblock->next = NULL;

    while(pnb!=NULL){
        next_pnb = pnb->next;
        ul_free(pnb);
        pnb = next_pnb;
    }
    pdb->cur_nblock = pdb->nblock;
    pdb->reclaim_node=NULL;
    return 1;

}

int dr_del( Sdict_reclaim *pdb)
{
    Sdict_bnode_block   *ptmp_nblock, *ptt_nblock;

    /* del node blocks */
    ptmp_nblock = pdb->nblock;

    while(ptmp_nblock!=NULL)
    {
        ptt_nblock = ptmp_nblock->next;
        ul_free(ptmp_nblock);
        ptmp_nblock = ptt_nblock;
    }

    /* del hash */
    ul_free(pdb->hash);

    /* del dict */
    ul_free(pdb);

    /* return */
    return 1;
}


int dr_op1( Sdict_reclaim *pdb, Sdict_snode *pnode, Eop_method op_method)
{
    int hkey;
    Sdict_bnode *pt_node;
    Sdict_bnode ** ppnode;
    Sdict_bnode* pnewnode;
    /* get hash key */
    hkey = (pnode->sign1+pnode->sign2)%pdb->hash_num;

    /* search this sign in dict */
    pt_node = pdb->hash[hkey].pnode;
    ppnode=&(pdb->hash[hkey].pnode);
    while(pt_node!=NULL){
        if ((pt_node->sign1==pnode->sign1) &&
            (pt_node->sign2==pnode->sign2) )
            break;
        ppnode=&(pt_node->next);
        pt_node=pt_node->next;
    }

    /* deal with op */

    switch (op_method){

        /* following is a group : the do not add new items */
    case SEEK :
    case MOD  : 
    case DEL  :
        if (pt_node==NULL){
            pnode->code = -1;
            return 0;
        }

        if (op_method==SEEK){   
            pnode->code = pt_node->code;
            pnode->other = pt_node->other;
            return 1;
        }

        if (op_method == DEL){
            *ppnode=pt_node->next;
            pt_node->next=pdb->reclaim_node;
            pdb->reclaim_node=pt_node;
            pnode->code = pt_node->code;
            pnode->other=pt_node->other;
            pdb->node_num--;
            return 1;
        }

        if (op_method == MOD){
            pt_node->code = pnode->code;
            pt_node->other = pnode->other;
            return 1;
        }

    /* add new items to dict : allow the items have been in dict. */
    case ADD :
        if (pt_node!=NULL){
            
            /* do other work */
            if ( (pnode->other &0x1ffffff)<1 ){
                pt_node->other++;
            }
            else{
                pt_node->other+=
                    (pnode->other &0x1ffffff);
            }
            pnode->code  = pt_node->code ;
            pnode->other = pt_node->other;
            
            return 1;
        }
        
        /* new item, add it to the dict */
        /* give code */
        if (pnode->code<0){
            pnode->code = pdb->cur_code;
            if(((pnode->other) & 0x1ffffff)<1){
                pnode->other = ((pnode->other) 
                                & (0xfe000000))+1;
            }
            
            pdb->cur_code++;
        }
        
        /* copy this node to node_list */
        if(pdb->reclaim_node != NULL)
        {
            pnewnode=pdb->reclaim_node;
            pdb->reclaim_node=pnewnode->next;
        }
        else
        {
            pnewnode=pdb->cur_node;
            pdb->cur_node++;
            pdb->node_avail--;
        }
        
        pnewnode->code = pnode->code;
        pnewnode->sign1 = pnode->sign1;
        pnewnode->sign2 = pnode->sign2;
        pnewnode->other = pnode->other;
        pnewnode->next = NULL;

        /* save it to dict */
        if (pdb->hash[hkey].pnode==NULL){
            pdb->hash[hkey].pnode = pnewnode;
        }
        else{
            pt_node = pdb->hash[hkey].pnode;
            pdb->hash[hkey].pnode = pnewnode;
            pnewnode->next = pt_node;
        }
        
        /* the all node block have been used */
        if (pdb->node_avail==0){
            if((pdb->cur_nblock->next = (Sdict_bnode_block *)
                ul_calloc (1, sizeof(Sdict_bnode_block)))==NULL)
                return -2;
            pdb->cur_nblock = pdb->cur_nblock->next;
            pdb->cur_node = pdb->cur_nblock->nodes;
            pdb->node_avail = NODE_BLOCK_NUM;
        }

        pdb->node_num++;
        
        /* return */
        return 1;
        
    default :
        ul_writelog(UL_LOG_WARNING, "[dr_op] op_method error");
        return -1;
    }
}



#ifdef TEST_FUNC
int main(int argc,char** argv)
{
    Sdict_reclaim * pdr;
    Sdict_snode snode;
    int op;
    
    if((pdr=dr_creat(1,0))==NULL)
    {
        fprintf(stderr,"Create drdict failed.\n");
        return -1;
    }
    while(fscanf(stdin,"%d %d %d %d %d",&op,&snode.sign1,&snode.sign2,
                 &snode.code,&snode.other)==5)
    {
        switch(op)
        {
        case 0:
            fprintf(stdout,"%d\n",dr_op1(pdr,&snode,ADD));
            fprintf(stdout,"%d %d %d %d\n",
                    snode.sign1,snode.sign2,snode.code,snode.other);
            break;
        case 1:
            fprintf(stdout,"%d\n",dr_op1(pdr,&snode,SEEK));
            fprintf(stdout,"%d %d %d %d\n",
                    snode.sign1,snode.sign2,snode.code,snode.other);
            break;
        case 2:
            fprintf(stdout,"%d\n",dr_op1(pdr,&snode,MOD));
            fprintf(stdout,"%d %d %d %d\n",
                    snode.sign1,snode.sign2,snode.code,snode.other);
            break;
        case 3:
            fprintf(stdout,"%d\n",dr_op1(pdr,&snode,DEL));
            fprintf(stdout,"%d %d %d %d\n",
                    snode.sign1,snode.sign2,snode.code,snode.other);
            break;
            
        }
    }
    return 0;
}

#endif

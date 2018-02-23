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


#define MMM_WEIGHT  10000
#define LINK_MIN_REQ    36

int db_syn_add_word( Sdict_build_syn *pdb, char *word );

/************************************************************************
    function    :   compose a path and file name to a full-path filename  
    arguments   :   buffer  :   the buffer of the result composed 
        path    :   the file's location 
        fname   :   the file's name 
        bufsize :   the size of the output buffer 
    result      :   if success, the fullpath in buffer and return 0
                if failure, the buffer is empty and return -1;
*************************************************************************/

int CmpsPath(char* buffer,const char* path,const char* fname,int bufsize)
{
    int len1,len2 ;


    assert(buffer);
    
    buffer[0]='\0';
    len1=strlen(path);
    len2=strlen(fname);

    if( ( len1+len2+2 ) > bufsize)
        return -1;
    
    //  buffer is enough 
    strcpy(buffer,path);
    strcat(buffer,"/");
    strcat(buffer,fname);
    return 0;
}

// for compatibility only
u_int creat_sign32(char *str_src)
{
    int times, i;
    u_int sign, tint;

/* for debugging */
    assert(str_src);
    
/* get times */
    times = strlen(str_src)/4;

/* get the sign */
    sign=0;
    for (i=0; i<times; i++){
        sign += *( (u_int *) (str_src + i*4) );
    }
    times = times%32;
    sign = (sign<<times)|(sign>>(32-times));

/* the tail of the string */
    tint=0;
    strcpy( (char*)(&tint), str_src+i*4);
    sign += tint;

/* return */
    return sign;
}

const size_t DB_IO_BUFFSIZE = 128*1024;       ///< 128KµÄbuff

static int db_read(int fno, void *data, size_t len) 
{
    ssize_t rsize = 0; 
    ssize_t lsize = 0; 
    if (data == NULL || fno < 0) { 
        return -1;
    }    
    char *ptr = (char *)data;

    while(len > 0) { 
        rsize = (len<DB_IO_BUFFSIZE)?len:DB_IO_BUFFSIZE;
        lsize = ul_read(fno, ptr, rsize);
        if (lsize < 0 || lsize != rsize) {
            return -1;
        }    
        ptr += lsize;
        len -= lsize;

    }    
    return 0;
}




// for compatibility only
int creat_sign64(char *str_src, u_int *psign1, u_int *psign2)
{
    int times, i, slen;
    char  tstr[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    u_int sign1, sign2, tint;

/* get slen */
    slen = strlen(str_src);

/* if string length <=8, do not change it */
    if (slen <= 8){
        strcpy(tstr, str_src);
        *psign1 = *((u_int *)tstr);
        *psign2 = *((u_int *)(tstr+4));
        return 1;
    }

/* get times */
    times = slen/4;

/* get the sign */
    sign1=sign2=0;
    for (i=0; i<times; i++){
        if (i & 1)
            sign1 += *( (u_int *) (str_src + i*4) );
        else
            sign2 += *( (u_int *) (str_src + i*4) );
    }

    times%=32;
    sign1 = (sign1<<times)|(sign1>>(32-times));
    sign2 = (sign2<<times)|(sign2>>(32-times));

/* the tail of the string */
    tint=0;
    strcpy( (char*)(&tint), str_src+i*4);
    sign1 += tint;
    sign2 += tint;

/* return */
    *psign1 = sign1;
    *psign2 = sign2;

    return 1;
}

// for compatibility only
int creat_sign_t64(void *t_src, int t_size, char *pstr, 
        u_int *psign1, u_int *psign2)
{
    u_int   *pstruct, tint;
    int times, i;
    unsigned short  psign[4];
    unsigned int    str_sign1, str_sign2;
    int     slen;

/* init vars */
    pstruct = (u_int *)t_src;
    psign[0] = 0;
    psign[1] = 0;
    psign[2] = 0;
    psign[3] = 0;

/* check correction */
/* old code : turn to ul lib 
   if (t_size%4 != 0){
   writelog(pf_log, "[creat_sign_t64] size must = 4*n", IPROG);
   }
*/
/*  new code */
    if(t_size % 4 !=0)
    {
        ul_writelog(UL_LOG_WARNING,"[creat_sign_t64] size must = 4*n ");    
    }
/* do */
    times=t_size/4;

    for (i=0; i<times; i++){
        tint = pstruct[i];
        tint = (tint>>8)|(tint<<24);
        psign[i%4] += tint%(BASE_SIGN + i*10);
    }

/* creat string sign */
    creat_sign64(pstr, &str_sign1, &str_sign2);

/* get string len */
    slen = strlen(pstr);

/* return */
    *psign1 = *( (u_int *) (psign)) + str_sign1;
    *psign2 = *( (u_int *) (psign + 2) ) + str_sign2;
//  *psign2 = *( (u_int *) (psign + 2) ) + str_sign2 + slen;

    return 1;
}


// for compatibility only
int creat_sign_nt64(void *t_src, int t_size, char *pstr, 
        u_int *psign1, u_int *psign2)
{
    u_int   *pstruct, tint;
    int times, i;
    unsigned short  psign[4];
    unsigned int    str_sign1, str_sign2;
    int     slen;

/* init vars */
    pstruct = (u_int *)t_src;
    psign[0] = 0;
    psign[1] = 0;
    psign[2] = 0;
    psign[3] = 0;

/* check correction */

    if(t_size % 4 !=0)
    {
        ul_writelog(UL_LOG_WARNING,"[creat_sign_nt64] size must = 4*n ");   
    }

/* do */
    times=t_size/4;

    for (i=0; i<times; i++){
        tint = pstruct[i];
        tint = (tint>>8)|(tint<<24);
        psign[i%4] += tint%(BASE_SIGN + i*10);
    }

/* creat string sign */
    creat_sign64(pstr, &str_sign1, &str_sign2);

/* get string len */
    slen = strlen(pstr);

/* return */
    *psign1 = *( (u_int *) (psign)) + str_sign1;
    *psign2 = *( (u_int *) (psign + 2) ) + str_sign2 + slen;

    return 1;
}





// for compatibility only
int a_creat_sign_t64(void *t_src, int t_size, u_int *psign1, u_int *psign2)
{
    char    *str_src;
    int     times, i;
    u_int   sign1, sign2, tint;
    //, ts1, ts2;

/* init vars */
    str_src = (char *)t_src;

/* get times */
    times = t_size/4;

/* get the sign */
    sign1=sign2=0;
    for (i=0; i<times; i++){
        if (i & 1)
            sign1 += *( (u_int *) (str_src + i*4) );
        else
            sign2 += *( (u_int *) (str_src + i*4) );
    }

    times%=32;
    sign1 = (sign1<<times)|(sign1>>(32-times));
    sign2 = (sign2<<times)|(sign2>>(32-times));

/* the tail of the string */
    tint=0;
    strncpy( (char*)(&tint), str_src+i*4, t_size-i*4);
    sign1 += tint;
    sign2 += tint;

/* return */
    *psign1 = sign1;
    *psign2 = sign2;

    return 1;
}


// for compatibility only
u_int creat_term_sign(char *str_src)
{
    int slen, i, times;
    u_int sign, tint;

/* get string length */
    slen = strlen(str_src);

/* deal with 2-gram chinese */
    if (slen==4){
        return *( (u_int*) (str_src) );
    }

/* deal with english term and chinese phrase */
    times = slen/4;

    sign=0;
    for (i=0; i<times; i++){
        sign +=  *( (u_int*) (str_src + i*4) );
    }
    times = times%32;
    sign = (sign<<times)|(sign>>(32-times));

/* the tail of the string */
    tint=0;
    strcpy( (char*)(&tint), str_src+i*4);
    sign += tint;

/* make the english and the chinese different */
    if (str_src[0] & 0x80){
        sign = (sign | 0x80000000) & 0xff7fffff;
    }
    else{
        sign = sign & 0x7fffffff;
    }

/* return */
    return sign;
}




Sdict_build *db_creat( int hash_num, int now_code )
{
    Sdict_build *pdb=NULL;
    Sdict_bhash* phash=NULL;
    Sdict_bnode_block* pblock=NULL;

    /* creat dict structure */
    if((pdb = (Sdict_build*) ul_malloc(sizeof(Sdict_build)))==NULL)
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

    /* return */
    return pdb;
}



int db_renew(Sdict_build *pdb)
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
    return 1;

}

int db_renew_fast(Sdict_build *pdb)
{
    Sdict_bnode_block   *pnb, *next_pnb;
    int     bnode_size;
    int i;


    if(pdb->cur_nblock != pdb->nblock)
    {
        // the other block has been used
        return db_renew(pdb);
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
    return 1;

}





int db_del( Sdict_build *pdb)
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





int db_op1( Sdict_build *pdb, Sdict_snode *pnode, Eop_method op_method)
{
    int hkey;
    Sdict_bnode *pt_node;
    int rep_num, high_weight;

    /* get hash key */
    hkey = (pnode->sign1+pnode->sign2)%pdb->hash_num;

    /* search this sign in dict */
    pt_node = pdb->hash[hkey].pnode;
    while(pt_node!=NULL){
        if ((pt_node->sign1==pnode->sign1) &&
            (pt_node->sign2==pnode->sign2) &&
            (pt_node->code>=0) )
            break;
        pt_node=pt_node->next;
    }

    /* deal with op */

    switch (op_method){

        /* following is a group : the do not add new items */
    case SEEK :
    case MOD  : 
    case DEL  :
    case MODF :
    case MODW :
    case MODC :
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
            if (pt_node->code!=-1){
                                pnode->code = pt_node->code;
                pt_node->code = -1;
                pdb->node_num--;
            }
            return 1;
        }

        if (op_method == MOD){
            pt_node->code = pnode->code;
            pt_node->other = pnode->other;
            return 1;
        }

        if (op_method == MODW){
            pt_node->other = ( ( (pnode->other)&0xfe000000 ) |
                      ( (pt_node->other)&0x1ffffff ) );
            return 1;
        }

        if (op_method == MODF){
            pt_node->other = ( ( (pnode->other)&0x1ffffff ) |
                      ( (pt_node->other)&0xfe000000 ) );
            return 1;
        }

        if (op_method == MODC){
            pt_node->code = pnode->code;
            return 1;
        }


    /* add new items to dict : allow the items have been in dict. */
    case ADD :
    case CLUSTER :

            /* this item have been in dict, add the freq */
        if (pt_node!=NULL){
            if (op_method==ADD){

                    /* is a deleted node */
                if (pt_node->code<0){
                    pt_node->code = pdb->cur_code;
                    pt_node->other = 0;
                    pdb->cur_code ++;
                    pdb->node_num ++;
                }

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
            }
            else{
                rep_num =  Dlweight (pt_node->other);
                if (rep_num<127){
                    rep_num++;
                }

                if (pnode->other > Dfreq(pt_node->other)){
                    high_weight = pnode->other;
                    pt_node->code = pnode->code;
                }
                else{
                    high_weight= Dfreq(pt_node->other);
                }

                pt_node->other=Dterm(rep_num, high_weight);
            }

            return 1;
        }

            /* new item, add it to the dict */
            /* give code */
        if (op_method == ADD){
            if (pnode->code<0){
                pnode->code = pdb->cur_code;
                if(((pnode->other) & 0x1ffffff)<1){
                    pnode->other = ((pnode->other) 
                            & (0xfe000000))+1;
                }

                pdb->cur_code++;
            }
        }
        else{
            rep_num = 1;
            pnode->other = Dterm(rep_num, pnode->other);
        }

            /* copy this node to node_list */
        pdb->cur_node->code = pnode->code;
        pdb->cur_node->sign1 = pnode->sign1;
        pdb->cur_node->sign2 = pnode->sign2;
        pdb->cur_node->other = pnode->other;
        pdb->cur_node->next = NULL;
        pdb->node_avail--;

            /* save it to dict */
        if (pdb->hash[hkey].pnode==NULL){
            pdb->hash[hkey].pnode = pdb->cur_node;
        }
        else{
            pt_node = pdb->hash[hkey].pnode;
            pdb->hash[hkey].pnode = pdb->cur_node;
            pdb->cur_node->next = pt_node;
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
        else{
            pdb->cur_node++;
        }
        pdb->node_num++;

            /* return */
        return 1;

    default :
        ul_writelog(UL_LOG_WARNING, "[db_op] op_method error");
        return -1;

    }
}



int db_opn( Sdict_build *pdb, Sdict_snode *plnode, int lnum, 
        Eop_method op_method)
{
    int         hkey;
    Sdict_bnode     *pt_node;
    Sdict_snode     *pnode;
    int         rep_num, high_weight;
    int         k;

    /* check input argument */
    for (k=0; k<lnum; k++){ // for begin 
        pnode = plnode+k;
    
        /* get hash key */
        hkey = (pnode->sign1+pnode->sign2)%pdb->hash_num;

        /* search this sign in dict */
        pt_node = pdb->hash[hkey].pnode;
        while(pt_node!=NULL){
            if ((pt_node->sign1==pnode->sign1) &&
                (pt_node->sign2==pnode->sign2) &&
                (pt_node->code>= 0)              )
                break;
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
                continue;
            }
            
            if (op_method==SEEK){   
                pnode->code = pt_node->code;
                pnode->other = pt_node->other;
                continue;
            }

            if (op_method == DEL){
                if (pt_node->code!=-1){
                    pt_node->code = -1;
                    pdb->node_num--;
                }
                continue;
            }

            if (op_method == MOD){
                pt_node->code = pnode->code;
                pt_node->other = pnode->other;
                continue;
            }

            /* add new items to dict : allow the items have been in dict. */
        case ADD :
        case CLUSTER :

            /* this item have been in dict, add the freq */
            if (pt_node!=NULL){
                if (op_method==ADD){
                    if ( (pnode->other &0x1ffffff)<1 ){
                        pt_node->other++;
                    }
                    else{
                        pt_node->other+=
                            (pnode->other &0x1ffffff);
                    }
                    pnode->code  = pt_node->code ;
                    pnode->other = pt_node->other;
                }
                else{
                    rep_num =  Dlweight (pt_node->other);
                    if (rep_num<127){
                        rep_num++;
                    }

                    if (pnode->other > Dfreq(pt_node->other)){
                        high_weight = pnode->other;
                        pt_node->code = pnode->code;
                    }
                    else{
                        high_weight= Dfreq(pt_node->other);
                    }

                    pt_node->other=Dterm(rep_num, high_weight);
                }

                continue;
            }

            /* new item, add it to the dict */
            /* give code */
            if (op_method == ADD){
                if (pnode->code<0){
                    pnode->code = pdb->cur_code;
                    if(((pnode->other) & 0x1ffffff)<1){
                        pnode->other = ((pnode->other) 
                                        & (0xfe000000))+1;
                    }

                    pdb->cur_code++;
                }
            }
            else{
                rep_num = 1;
                pnode->other = Dterm(rep_num, pnode->other);
            }

            /* copy this node to node_list */
            pdb->cur_node->code = pnode->code;
            pdb->cur_node->sign1 = pnode->sign1;
            pdb->cur_node->sign2 = pnode->sign2;
            pdb->cur_node->other = pnode->other;
            pdb->node_avail--;

            /* save it to dict */
            if (pdb->hash[hkey].pnode==NULL){
                pdb->hash[hkey].pnode = pdb->cur_node;
            }
            else{
                pt_node = pdb->hash[hkey].pnode;
                pdb->hash[hkey].pnode = pdb->cur_node;
                pdb->cur_node->next = pt_node;
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
            else{
                pdb->cur_node++;
            }
            pdb->node_num++;

            /* return */
            continue;

        default :
            ul_writelog(UL_LOG_WARNING, "[db_op] op_method error");
            return -1;

        }
    }//for end 

    return lnum;
}


// for compatibility only
int db_add_link( Sdict_build *pdb, Sdict_snode *pnode)
{
    int hkey;
    Sdict_bnode *pt_node;
    Sdict_bnode **pup = NULL, *pdown = NULL;

    /* get hash key */
    hkey = pnode->sign2;

    /* search this sign in dict */
    if (hkey >= pdb->hash_num){
        ul_writelog(UL_LOG_WARNING,"[db_add_link] term_code(%d ) > max_code (%d)",
                    hkey, pdb->hash_num);
        return -1;          
    }   

/* +debug 
    if ((unsigned long)pup < 1000000){
        printf("pup < 1000000 at place 1.\n");
    }
 -debug */

    pt_node = pdb->hash[hkey].pnode;
    if (pt_node==NULL){
        pup = &(pdb->hash[hkey].pnode);
        pdown = NULL;
    }

    else {
    if (pt_node->code == pnode->code){
        pt_node->other++;
        return 1;
    }

    if (pt_node->code > pnode->code){
        pup = &(pdb->hash[hkey].pnode);
        pdown = pdb->hash[hkey].pnode;
    }

    if (pt_node->code < pnode->code){
        pup = 0;
        while (pt_node->next != NULL){
            if (pt_node->next->code == pnode->code){
                pt_node->next->other++;
                return 1;
            }
            else if(pt_node->next->code > pnode->code){
                pup = &(pt_node->next);
                pdown = pt_node->next;
                break;
            }

            pt_node=pt_node->next;
        }

        if (pup == 0){
            pup = &(pt_node->next);
            pdown = NULL;
        }
    }

    }

/* +debug 
    if ((unsigned long)pup < 1000000){
        printf("pup < 1000000 at place 2.\n");
    }
 -debug */

/* copy this node to node_list */
    pdb->cur_node->code = pnode->code;
    pdb->cur_node->sign1 = pnode->sign1;
    pdb->cur_node->sign2 = pnode->sign2;
    pdb->cur_node->other = pnode->other;
    pdb->node_avail--;

/* +debug
    if ((unsigned long)pup < 1000000){
        printf("pup (%u) < 1000000 at place 3.\n", (unsigned long)pup);
    }
-debug */

/* save it to dict */
    *pup = pdb->cur_node;
    pdb->cur_node->next = pdown;

/* 
    if ((unsigned long)pup < 1000000){
        printf("pup < 1000000 at place 4.\n");
    }
 -debug */

/* the all node block have been used */
    if (pdb->node_avail==0){
        if((pdb->cur_nblock->next = (Sdict_bnode_block *)
            ul_calloc (1, sizeof(Sdict_bnode_block)))==NULL)
            return -2;
        pdb->cur_nblock = pdb->cur_nblock->next;
        pdb->cur_node = pdb->cur_nblock->nodes;
        pdb->node_avail = NODE_BLOCK_NUM;
    }
    else{
        pdb->cur_node++;
    }
    pdb->node_num++;

/* +debug 
    if ((unsigned long)pup < 1000000){
        printf("pup < 1000000 at place 5.\n");
    }
 -debug */

/* return */
    return 1;

}

// for compatibility only
int db_stand_link( Sdict_build *pdb )
{
    int         i, curn, curv, sqrt_max;
    int     h_num, wmax; //n_num;
    Sdict_bhash     *phash;
    Sdict_bnode *pnode;
    int     a_sqrt[MAX_SQRT_NUM];

/* init vars */
    phash = pdb->hash;
    h_num = pdb->hash_num;

    curn = 0;
    curv = 0;
    for (i=0; i<MAX_SQRT_NUM; i++){
        if (i*25*25>curv){
            curn++;
            curv = curn*curn*curn;
        }
        a_sqrt[i]=curn;
    }
    sqrt_max = a_sqrt[LINK_MIN_REQ];

/* adjsust */
    for (i=0; i<h_num; i++){
        pnode = phash[i].pnode;
        if (pnode == NULL){
            continue;
        }

            /* get max link pop */
        wmax = sqrt_max;
        while(pnode!=NULL){
            pnode->other = pnode->other/10 + 1;
            if (pnode->other >= MAX_SQRT_NUM){
                pnode->other = MAX_SQRT_NUM-1;
                ul_writelog(UL_LOG_NOTICE,"Enlarge MAX_SQRT_MAX (%d)",MAX_SQRT_NUM);
            }

            pnode->other = a_sqrt[pnode->other];
            if ((u_int)wmax<pnode->other){
                wmax = pnode->other;
            }
            pnode = pnode->next;
        }

            /* standard link pop */
        pnode = phash[i].pnode;
        while(pnode!=NULL){
            pnode->other = pnode->other * 126 / wmax;
            pnode = pnode->next;
        }
    }

    return 1;
}




// for compatibility only
int db_save_link( Sdict_build *pdb, char *path, char *fname )
{
    int find1, find2;
    int h_num, n_num;
    int     sind1, sind2;
    char    tname[256];
    
    Sdict_bhash *pbh;
    Sdict_bnode *pnode;

    Sindex1     pi1;
    u_int       pi2[MID_NODE_NUM];

    int     i, j;
    
    int ioff;
    //  used by new code 
    char fullpath[PATH_SIZE];

/* init vars */
    h_num = pdb->hash_num;
    n_num = pdb->node_num;
    pbh   = pdb->hash;
    sind1 = sizeof(Sindex1);
    sind2 = sizeof(Sindex2);

/* save the attribute of this dict */
    ul_writenum_init(path, fname);
    ul_writenum_oneint(path, fname, "term_num", pdb->hash_num);

/* open findex */
    sprintf(tname, "%s.ind1", fname);
    
    /* construct the full path name of index 1 */   
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((find1=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
        return -2;
    fchmod(find1,0644);
    
    sprintf(tname, "%s.ind2", fname);

    /* construct the full path name of indexfile 2 */
    
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    
    if((find2=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
        return -2;
    fchmod(find2,0644);
    
/* save the index */
    ioff=0;

    for (i=0; i<pdb->hash_num; i++){
        pi1.off = ioff;
        pnode = pbh[i].pnode;
        j=0;

        if (pnode==NULL){
            pi1.num = 0;
            ul_write(find1, &pi1, sind1);
            continue;
        }

        while (pnode!=NULL){
            pi2[j]=Dindex2(pnode->other, pnode->code);
            j++;
            ioff++;
            pnode = pnode->next;
            if (j>=MID_NODE_NUM){
                break;
            }
        }
        pi1.num = j;

        ul_write(find1, &pi1, sind1);
        ul_write(find2, pi2, pi1.num*sind2);
    }

/* close findex 1 */
    ul_close(find1);
    ul_close(find2);

/* return */
    return 1;

}


// for compatibility only
int db_save_link_search( Sdict_build *pdb, char *path, char *fname )
{
    int find1, find2;
    int h_num, n_num;
    int run_times, rstart, rend, rbase;
    int     sind2;
    char    tname[256];
    char    fullpath[PATH_SIZE];
    Sdict_bhash *pbh;
    Sdict_bnode *pnode;

    Sindex1     pi1[IND2_NUM_PER_FILE];
    u_int       pi2[MID_NODE_NUM];

    int     i, j, k, times;

//  u_int   *phi;   /* the hash of dict_searcher */
    int ioff;   //, inum;

/* init vars */
    h_num = pdb->hash_num;
    n_num = pdb->node_num;
    pbh   = pdb->hash;
    sind2 = sizeof(Sindex2);

/* calc times */
    run_times = (pdb->hash_num-1)/IND2_NUM_PER_FILE + 1;

/* save the attribute of this dict */
    ul_writenum_init(path, fname);
    ul_writenum_oneint(path, fname, "term_num", pdb->hash_num);
    ul_writenum_oneint(path, fname, "file_num", run_times);

/* open findex 1 */
    sprintf(tname, "%s.ind1", fname);

    /* construct the fullpath name and open it */
    
    CmpsPath(fullpath,path,tname,PATH_SIZE);

    if((find1=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
        return -2;
    fchmod(find1,0644);

/* save the index 2 */
    for (k=0; k<run_times; k++){
        rstart = 0;
        rbase  = k*IND2_NUM_PER_FILE;
        if (k==run_times-1){
            rend = pdb->hash_num - k*IND2_NUM_PER_FILE;
        }
        else{
            rend = IND2_NUM_PER_FILE;
        }

        sprintf(tname, "%s.ind2.%d", fname, k);
    
    /*  construct the fullpath name of the indexfile 2 */
    
        CmpsPath(fullpath,path,tname,PATH_SIZE);
        if((find2=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
            return -2;
        fchmod(find2,0644);
        
        times=0;
        j=0;
        ioff=0;
        bzero(pi1, IND2_NUM_PER_FILE*sind2);

        for (i=rstart; i<rend; i++){

            pi1[i].off = ioff;

            pnode = pbh[i+rbase].pnode;

            if (pnode == NULL){
                pi1[i].off = ioff;
                pi1[i].num = 0;
                continue;
            }

            pi1[i].num = 0;
            while (pnode!=NULL){
                pi2[j]=Dindex2(pnode->other, pnode->code);
                j++;
                pi1[i].num++;
                ioff++;

                if (j>=MID_NODE_NUM){
                    ul_write(find2, pi2, 
                            MID_NODE_NUM*sind2);
                    j=0;
                    times++;
                }

                pnode = pnode->next;
            }
        }

        if (j!=0){
            ul_write(find2,pi2 , j*sind2);
        }

        ul_close(find2);

            /* save index1 */
        ul_write(find1, pi1, rend*sizeof(Sindex1));
    }

/* close findex 1 */
    ul_close(find1);

/* return */
    return 1;

}




int db_cluster(Sdict_build *pdb, Spagecluster *pcluster)
{
    Slongindex2 *pindex2;
    int     *prept;
    Sdict_bhash *phash;
    Sdict_bnode *pnode;
    void    * ptmp1;
    int     *ptmp2;
    
    int i, j, node_size, hash_num, node_num;

/* int vars */
    node_size = sizeof(Sdict_snode);

/* malloc mem */
    hash_num = pdb->hash_num;
    node_num = pdb->node_num;

    pcluster->num = node_num;
    if((ptmp1= (void *) ul_malloc ( node_num* sizeof(Slongindex2)))==NULL)
        return -2;
    if((ptmp2= (int  *) ul_malloc ( node_num * sizeof(int) ))==NULL)
    {
        ul_free(ptmp1);
        return -2;
    }
    /* memory allocation success  */
    pcluster->index2 = ptmp1;
    pcluster->rept  = ptmp2;    

/* save the list */
    pindex2 = (Slongindex2 *)pcluster->index2;
    prept = pcluster->rept;
    phash= pdb->hash;

    j=0;
    for (i=0; i<hash_num; i++){
        if ( (pnode = phash[i].pnode)==NULL){
            continue;
        }

        do{
            pindex2[j].urlno = pnode->code;
            pindex2[j].weight = Dfreq(pnode->other);
            prept[j] = Dlweight(pnode->other);
            pnode = pnode->next;
            j++;
        }while (pnode!=NULL);
    }

/* check error */
    if (j!=pcluster->num){
        ul_writelog(UL_LOG_WARNING, "[db_cluster] creat key list num error");
        return -1;
    }

/* return */
    return pcluster->num;
}



int db_keylist(Sdict_build *pdb, Skeyweight *pkey, int pkey_num)
{
    //  Sdict_bhash *phash;
    Sdict_bnode *pnode;

    int i, node_size, hash_num, node_num, rnum;

/* int vars */
    node_size = sizeof(Sdict_snode);
    hash_num = pdb->hash_num;
    node_num = pdb->node_num;
    pnode = pdb->nblock->nodes;
    rnum = Min(pkey_num, node_num);
    rnum = Min(rnum, NODE_BLOCK_NUM);

/* save the list */
    for (i=0; i<rnum; i++){
        pkey[i].sign1 = pnode[i].sign1;
        pkey[i].sign2 = pnode[i].sign2;
        pkey[i].weight= pnode[i].other;
    }

/* return */
    return rnum;
}



int db_adjust (Sdict_build *pdb)
{
    int i, times, hkey;
    int h_num;
    int n_num;

    Sdict_bhash     *ph;
    Sdict_bnode *pn, *tp_node;
    Sdict_bnode_block   *pnb;

/* init vars */
    n_num = pdb->node_num;
    h_num = n_num;

    h_num /= 2; // slight performance penalty occurred

/* define new hash number */
    if ( (h_num % 2) == 0 ) {
        h_num++;
    }
    pdb->hash_num = h_num;

/* resize the hash */
    ul_free(pdb->hash);
    if((pdb->hash = (Sdict_bhash * )ul_calloc(1,h_num*sizeof(Sdict_bhash)))==NULL)
        return -2;
        
    ph=pdb->hash;

/* adjust the link of nodes */
    pnb = pdb->nblock;

    while(pnb!=NULL){
        pn = pnb->nodes;
        times = NODE_BLOCK_NUM;
        if (pnb->next==NULL){
            times-= pdb->node_avail;
        }

        for (i=0; i<times; i++){
            pn[i].next=NULL;
            hkey = (pn[i].sign1+pn[i].sign2)%h_num;
            if (ph[hkey].pnode==NULL){
                ph[hkey].pnode=pn+i;
            }
            else{
                tp_node = ph[hkey].pnode;
                ph[hkey].pnode = pn+i;
                pn[i].next = tp_node;
            }
        }

        pnb = pnb->next;
    }
    return 0;
}




int db_save_freq(Sdict_build *pdb, char *path, char *fname)
{
    int     fno;
    int     *pfreq;
    int     i, max_code, nnum;
    
    
    Sdict_bnode_block   *pbnode;
    Sdict_bnode     *pnode, *pend_node;

//  add by using new code for using new lib 
    char    fullpath[PATH_SIZE];
    
/* init vars */
    max_code = pdb->cur_code;
    pend_node = pdb->cur_node;
    pbnode = pdb->nblock;

/* write to .n file */
    ul_writenum_init(path, fname);
    ul_writenum_oneint(path, fname, "max_code", max_code);

/* get space */
    if((pfreq = (int *) ul_calloc(1,max_code* sizeof(int)))==NULL)
        return -2;

/* creat the freq array */
    nnum=0;
    while (pbnode!=NULL){
        pnode = pbnode->nodes;
        for (i=0; i<NODE_BLOCK_NUM; i++){
            if (pnode+i == pend_node){
                break;
            }
            pfreq[pnode[i].code]= pnode[i].other & 0x1ffffff;
            pnode[i].other = pnode[i].other & 0xfe000000;
            nnum++;
        }
        if (pnode+i == pend_node){
            break;
        }
        pbnode = pbnode->next;
    }

/* check error */
    if (nnum!=max_code){
        ul_writelog(UL_LOG_WARNING,"[db_save_freq] node num error");
    }

/* save it to file */
    // construct full path name 
    
    CmpsPath(fullpath,path,fname,PATH_SIZE);
    
    // open ,write and close it  
    
    if((fno=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644)) == -1 )
    {   
        ul_free(pfreq);
        return -3;
    }   
    fchmod(fno,0644);   
    ul_write(fno,pfreq,max_code * sizeof(int) );
    ul_close(fno);
    
/* free space */
    ul_free(pfreq);

/* return */
    return max_code;
}



int db_save_filter( Sdict_build *pdb, char *path, char *fname, int low)
{
    int fno;
    int h_num, n_num;
    int     snode;
    char    tname[256];

    Sdict_bhash *pbh;
    Sdict_bnode *pnode;

    Sdict_snode ds[MID_NODE_NUM];

    int     i, j, times;

    u_int   *phi;   /* the hash of dict_searcher, offset */
    u_int   *pnum;  /* number of nodes in hash entries */
    int ioff, inum;

    // used in new code 
    char    fullpath[PATH_SIZE];
    
/* init vars */
    h_num = pdb->hash_num;
    n_num = pdb->node_num;
    pbh   = pdb->hash;
    snode = sizeof(Sdict_snode);
    if((phi = (u_int*) ul_calloc(1, h_num * sizeof(int)))==NULL)
        return -2;
    if((pnum = (u_int*) ul_calloc(1, h_num * sizeof(int)))==NULL) {
        ul_free(phi);
        return -2;
    }   

/* save the attribute of this dict */
    ul_writenum_init(path, fname);
    ul_writenum_oneint(path, fname, "hash_num", pdb->hash_num);
    ul_writenum_oneint(path, fname, "node_num", pdb->node_num);
    ul_writenum_oneint(path, fname, "cur_code", pdb->cur_code);

/* save the index 2 */
    sprintf(tname, "%s.ind2", fname);

    // construct full path name 
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    
    // open ,write and close it  
    
    if((fno=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644)) == -1 )
    {
        ul_free(phi);
        ul_free(pnum);
        return -3;
    }
    fchmod(fno,0644);
    
    times=0;
    j=0;
    ioff=inum=0;
    for (i=0; i<h_num; i++){

        phi[i] = ioff;  // remove the limit
        //phi[i] = ioff<<7;

        pnode = pbh[i].pnode;

        while (pnode!=NULL){
            if (pnode->code<0){
                pnode = pnode->next;
                continue;
            }
            if ((int) Dfreq(pnode->other) < low){
                pnode = pnode->next;
                continue;
            }

            ds[j].sign1=pnode->sign1;
            ds[j].sign2=pnode->sign2;
            ds[j].code=pnode->code;
            ds[j].other=pnode->other;
            j++;
            pnum[i]++;  // count of nodes in one hash entry
            ioff++;

            if (j>=MID_NODE_NUM){
                ul_write(fno, ds, MID_NODE_NUM*snode);
                j=0;
                times++;
            }

            pnode = pnode->next;
        }
    }

    if (j!=0){
        ul_write(fno, ds, j*snode);
    }

    ul_close(fno);

/* save index1 */
    sprintf(tname, "%s.ind1", fname);

    // construct full path name 
    
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    
    // open it  
    
    if((fno=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644)) == -1 )
    {
        ul_free(phi);
        ul_free(pnum);
        return -3;
    }   
    fchmod(fno,0644);

    ul_write(fno, phi, h_num*sizeof(int));
    ul_write(fno, pnum, h_num*sizeof(int));

    ul_close(fno);

/* check correction ???? filtered! */
    if ( (times*MID_NODE_NUM+j != pdb->node_num) 
      || (ioff != pdb->node_num) ){
        ul_writelog(UL_LOG_WARNING, "[db_save_filter] node_num error");
        ul_free(phi);
        ul_free(pnum);
        return -1;
    }

/* free */
    ul_free(phi);
    ul_free(pnum);

/* return */
    return 1;

}


/*  checked in 20:33, Oct 30, 2000 */


int db_save( Sdict_build *pdb, char *path, char *fname )
{
    int fno=-1;
    int h_num, n_num;
    int     snode;
    char    tname[256];

    Sdict_bhash *pbh=NULL;
    Sdict_bnode *pnode=NULL;

    Sdict_snode ds[MID_NODE_NUM];

    int     i, j, times;

    u_int   *phi=NULL;   /* the hash of dict_searcher */
    u_int   *pnum=NULL;  /* number of nodes in hash entries */
    int ioff, inum;
    
    //  used by new code 
    char    fullpath[PATH_SIZE];
    

    /* init vars */
    h_num = pdb->hash_num;
    n_num = pdb->node_num;
    pbh   = pdb->hash;
    snode = sizeof(Sdict_snode);

    if((phi=(u_int *)ul_calloc(1,h_num * sizeof(int)))==NULL)
    {
        goto failed;
    }
    
    if((pnum = (u_int*) ul_calloc(1, h_num * sizeof(int)))==NULL)
    {
        goto failed;
    }   
        
    /* save the attribute of this dict */
    if(strlen(path)==0)
    {
        tname[0]=0;
    }
    else if ( path[strlen(path)-1] != '/')
    {
        snprintf(tname,sizeof(tname),"%s/",path);
    }
    else
    {
        snprintf(tname,sizeof(tname),"%s",path);
    }
    
    if(ul_writenum_init(tname, fname) < 0)
    {
            ul_writelog(UL_LOG_WARNING, "[db_save] ul_writenum_init(%s %s) error", tname, fname);
	    goto failed;
    }
    if(ul_writenum_oneint(tname, fname, "hash_num", pdb->hash_num) < 0)
    {
            ul_writelog(UL_LOG_WARNING, "[db_save] ul_writenum_oneint(hash_num=%d) error", pdb->hash_num);
	    goto failed;
    }
    if(ul_writenum_oneint(tname, fname, "node_num", pdb->node_num) < 0)
    {
            ul_writelog(UL_LOG_WARNING, "[db_save] ul_writenum_oneint(node_num=%d) error", pdb->node_num);
	    goto failed;
    }
    if(ul_writenum_oneint(tname, fname, "cur_code", pdb->cur_code) < 0)
    {
            ul_writelog(UL_LOG_WARNING, "[db_save] ul_writenum_oneint(cur_code=%d) error", pdb->cur_code);
	    goto failed;
    }

    /* save the index 2 */
    sprintf(tname, "%s.ind2", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((fno=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
    {
        goto failed;
    }

    fchmod(fno,0644);
    
    times=0;
    j=0;
    ioff=inum=0;
    for (i=0; i<h_num; i++){

        phi[i] = ioff;  // remove the limit
        //phi[i] = ioff<<7;

        pnode = pbh[i].pnode;

        while (pnode!=NULL){
            if (pnode->code<0){
                pnode = pnode->next;
                continue;
            }

            ds[j].sign1=pnode->sign1;
            ds[j].sign2=pnode->sign2;
            ds[j].code=pnode->code;
            ds[j].other=pnode->other;
            j++;
            pnum[i]++;

            ioff++;

            if (j >= MID_NODE_NUM) {
                if (ul_write(fno, ds, MID_NODE_NUM * snode) == -1)
                {
                    goto failed;
                }
                j=0;
                times++;
            }

            pnode = pnode->next;
        }
        if (pnum[i] >= 126) {
            ul_writelog(UL_LOG_WARNING,
                        "[db_save] too many items in one hash entry: %d",
                        pnum[i]);
        }
    }
    
    if (j != 0) {
        if (ul_write(fno, ds, j * snode) == -1)
        {
            goto failed;
        }
    }

    ul_close(fno);
    fno=-1;

    /* save index1 */
    sprintf(tname, "%s.ind1", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((fno=ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
    {
        goto failed;
    }
    fchmod(fno,0644);
    
    if (ul_write(fno, phi, h_num * sizeof(int)) == -1)
    {
        goto failed;
    }
    if (ul_write(fno, pnum, h_num * sizeof(int)) == -1)
    {
        goto failed;
    }

    ul_close(fno);
    fno=-1;
    
    /* check correction */
    if ( (times*MID_NODE_NUM+j != pdb->node_num) 
         || (ioff != pdb->node_num) ){
        ul_writelog(UL_LOG_WARNING,"[db_save] node_num error");
        goto failed;
    }

    /* free */
    ul_free(phi);
    ul_free(pnum);

    /* return */
    return 1;
 failed:
    if(phi == NULL)
    {
        ul_free(phi);
    }
    if(pnum != NULL)
    {
        ul_free(pnum);
    }
    if(fno != -1)
    {
        ul_close(fno);
    }
    return -1;
}


Sdict_build *db_load_0( char *path, char *fname, int h_num )
{
    int on_num;

    int n_num, cur_code;
    int snode, rnum, rsize, full_size;
    int i;
    int fno;
    int cur_h;  //, new_h;

    char tname[256];

    Sdict_snode sn[MID_NODE_NUM];
    Sdict_build *pdb;

    //  used by new code 
    char    fullpath[PATH_SIZE];
    
/* init vars*/
    snode = sizeof(Sdict_snode);

/* read num from file */
    if (h_num<=0){
        ul_readnum_oneint(path, fname, "hash_num", &h_num);
    }
    ul_readnum_oneint(path, fname, "node_num", &on_num);
    ul_readnum_oneint(path, fname, "cur_code", &cur_code);

/* creat dict */
    pdb = db_creat(h_num, cur_code);

/* read items from file */
    sprintf(tname, "%s.ind2", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((fno=ul_open(fullpath,O_RDONLY,0644))==-1)
        return NULL; 
    
    n_num=0;
    cur_h=0;
    full_size = MID_NODE_NUM*snode;
    while( (rsize = ul_read(fno, sn, full_size)) >0 ){
        rnum = rsize/snode;

        for (i=0; i<rnum; i++){
            sn[i].other=0;
            db_op1(pdb, sn+i, ADD);
        }

        n_num+=rnum;
    }

    if (n_num!=on_num){
        ul_writelog(UL_LOG_WARNING, "[db_load] node_num error");
    }

/* close file */
    ul_close(fno);

/* return */
    return pdb;

}



Sdict_build *db_load( char *path, char *fname, int h_num )
{
    int on_num;

    int n_num, cur_code;
    int snode, rnum, rsize, full_size;
    //  int i;
    int fno=-1;
    int cur_h;  //, new_h;

    char tname[256];

    Sdict_snode sn[MID_NODE_NUM];
    Sdict_build *pdb=NULL;

    //  used by new code 
    char    fullpath[PATH_SIZE];

    // check the file is exist or not 
    snprintf(tname,sizeof(tname),"%s.n",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    snprintf(tname,sizeof(tname),"%s.ind1",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    snprintf(tname,sizeof(tname),"%s.ind2",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    
/* init vars*/
    snode = sizeof(Sdict_snode);

/* read num from file */
    if (h_num<=0){
        if(ul_readnum_oneint(path, fname, "hash_num", &h_num)!= 1)
        {
            ul_writelog(UL_LOG_WARNING,"Read hashnum from %s/%s.n failed.",
                        path,fname);
            return NULL;
        };
    }
    if(ul_readnum_oneint(path, fname, "node_num", &on_num) != 1)
    {
        ul_writelog(UL_LOG_WARNING,"Read node_num from %s/%s.n failed.",
                path,fname);
        return NULL;
    }
    if(ul_readnum_oneint(path, fname, "cur_code", &cur_code) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,"Read cur_code from %s/%s.n failed.",
                path,fname);
        return NULL;
    }

/* creat dict */
    if((pdb = db_creat(h_num, cur_code))==NULL)
    {
        goto failed;
    }

/* read items from file */
    sprintf(tname, "%s.ind2", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((fno= ul_open(fullpath,O_RDONLY,0644))==-1)
    {
        goto failed;
    }
        
    n_num=0;
    cur_h=0;
    full_size = MID_NODE_NUM*snode;
    while( (rsize = ul_read(fno, sn, full_size)) >0 ){
        rnum = rsize/snode;

/*
        for (i=0; i<rnum; i++){
            db_op1(pdb, sn+i, ADD);
        }
*/
        db_opn(pdb, sn, rnum, ADD);

        n_num+=rnum;
    }

    if (n_num!=on_num){
        ul_writelog(UL_LOG_WARNING, "[db_load] node_num error");
    }

/* close file */
    ul_close(fno);

/* return */
    return pdb;
failed:
    if(pdb!=NULL)
    {
        db_del(pdb);
    }
    if(fno!= -1 )
    {
        close(fno);
    }
    return NULL;
}


int page_cluster(Sdict_snode *pnode, int node_num, Spagecluster *pcluster)
{
    Sdict_build *pdb;

    pdb = db_creat(node_num, 0);
    db_opn(pdb, pnode, node_num, CLUSTER);
    db_cluster(pdb, pcluster);
    db_del(pdb);

    return pcluster->num;
}


//********************************************************
//      Dictionary for Searcher (static)
//********************************************************



Sdict_search *ds_load( char *path,  char *fname )
{
    int snode;
    int h_num;
    int n_num;
    int c_code;
    int fno=-1;

    char    tname[256];

    Sdict_search    *pds=NULL;

    //  used by new code 
    char fullpath[PATH_SIZE];
    
    // check the file is exist or not 
    snprintf(tname,sizeof(tname),"%s.n",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    snprintf(tname,sizeof(tname),"%s.ind1",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    snprintf(tname,sizeof(tname),"%s.ind2",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    
    /* init vars */
    snode = sizeof(Sdict_snode);

    /* read num from file */
    if(ul_readnum_oneint(path, fname, "hash_num", &h_num) != 1)
    {
        ul_writelog(UL_LOG_WARNING,"Read hash_num failed.");
        return NULL;
    }
    if(ul_readnum_oneint(path, fname, "node_num", &n_num) != 1)
    {
        ul_writelog(UL_LOG_WARNING,"Read node_num failed.");
        return NULL;
    }
    if(ul_readnum_oneint(path, fname, "cur_code", &c_code) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,"Read cur_code failed.");
        return NULL;
    }

    /* create the dictionary for searcher */
    if((pds=(Sdict_search* ) ul_calloc(1, sizeof(Sdict_search)))==NULL)
        return NULL;
        
    pds->hash_num = h_num;
    pds->node_num = n_num;
    pds->cur_code = c_code;

    if((pds->hash = (u_int* )ul_malloc(h_num*sizeof(int)))==NULL)
    {
        ul_free(pds);
        return NULL;
    }
    if((pds->num = (u_int* )ul_malloc(h_num*sizeof(int)))==NULL)
    {
        ul_free(pds->hash);
        ul_free(pds);
        return NULL;
    }

    if((pds->node = (Sdict_snode* )ul_malloc(n_num*sizeof(Sdict_snode)))==NULL)
    {
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        return NULL;
    }
/* read data from file */
    snprintf(tname,sizeof(tname), "%s.ind1", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if( (fno= ul_open(fullpath,O_RDONLY,0644))==-1 )
    {
        ul_free(pds->node);
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        return NULL;
    }
    
    if (db_read(fno, pds->hash, h_num*sizeof(int)) != 0) 
    {
        ul_free(pds->node);
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        ul_close(fno);
        return NULL;
    }

    // read the number of nodes
    // in hash entries
    if (db_read(fno, pds->num, h_num*sizeof(int)) != 0)  
    {
        ul_free(pds->node);
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        ul_close(fno);
        return NULL;
    }

    ul_close(fno);

    snprintf(tname,sizeof(tname), "%s.ind2", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if( (fno= ul_open(fullpath,O_RDONLY,0644))==-1 )
    {
        ul_free(pds->node);
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        return NULL;
    }
    if (db_read(fno, pds->node, n_num*sizeof(Sdict_snode)) != 0)
    {
        ul_free(pds->node);
        ul_free(pds->hash);
        ul_free(pds->num);
        ul_free(pds);
        ul_close(fno);
        return NULL;
    }
    ul_close(fno);

/* return */
    return pds;

}


Sdict_search *ds_load_mmap( char *path,  char *fname )
{
    int snode;
    int h_num;
    int n_num;
    int c_code;
    int fno=-1;

    char    tname[256];

    Sdict_search    *pds=NULL;

    //  used by new code 
    char fullpath[PATH_SIZE];
    
    // check the file is exist or not 
    snprintf(tname,sizeof(tname),"%s.n",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    snprintf(tname,sizeof(tname),"%s.ind1",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    snprintf(tname,sizeof(tname),"%s.ind2",fname);
    if(ul_fexist(path,tname) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,
                "File %s/%s does not exist or cannot open to read.",
                path,tname);
        return NULL;
    }
    
    /* init vars */
    snode = sizeof(Sdict_snode);

    /* read num from file */
    if(ul_readnum_oneint(path, fname, "hash_num", &h_num) != 1)
    {
        ul_writelog(UL_LOG_WARNING,"Read hash_num failed.");
        return NULL;
    }
    if(ul_readnum_oneint(path, fname, "node_num", &n_num) != 1)
    {
        ul_writelog(UL_LOG_WARNING,"Read node_num failed.");
        return NULL;
    }
    if(ul_readnum_oneint(path, fname, "cur_code", &c_code) != 1 )
    {
        ul_writelog(UL_LOG_WARNING,"Read cur_code failed.");
        return NULL;
    }

    /* create the dictionary for searcher */
    if((pds=(Sdict_search* ) ul_calloc(1, sizeof(Sdict_search)))==NULL)
        return NULL;
        
    pds->hash_num = h_num;
    pds->node_num = n_num;



/* read data from file */
    snprintf(tname,sizeof(tname), "%s.ind1", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if( (fno= ul_open(fullpath,O_RDONLY,0644))==-1 )
    {
		return NULL;
    }
    pds->hash = (unsigned int*)mmap(NULL, 2*(size_t)h_num*sizeof(int), 
		PROT_READ, MAP_PRIVATE, fno, 0);
	ul_close(fno);
	fno = -1;
	if(pds->hash == MAP_FAILED)
	{
		ul_writelog(UL_LOG_WARNING,
			"[ODICT ERROR]mmap failed, %m, size %d",(int)( h_num * sizeof(int)));
		return NULL;
	}
	pds->num = pds->hash + h_num;
    snprintf(tname,sizeof(tname), "%s.ind2", fname);
	if(n_num > 0)
	{
		CmpsPath(fullpath,path,tname,PATH_SIZE);
		fno = ul_open(fullpath,O_RDONLY,0644);
		if(-1 == fno){
			munmap(pds->hash, 2*(size_t)pds->hash_num*sizeof(int));
			return NULL;
		}
		pds->node = (Sdict_snode *) mmap(NULL, (size_t)n_num * sizeof(Sdict_snode), 
				PROT_READ, MAP_PRIVATE, fno, 0);
		ul_close(fno);
		if(pds->node == MAP_FAILED)
		{	
			ul_writelog(UL_LOG_WARNING, "[ODICT ERROR]mmap failed, %m, size %d",
			(int)(n_num*sizeof(Sdict_snode)));
			munmap(pds->hash, 2*(size_t)pds->hash_num*sizeof(int));
			return NULL;
		}
	}
/* return */
    return pds;

}

Sdict_search_hd *ds_load_hd( char *path,  char *fname )
{
    int snode;
    int h_num;
    int n_num;
    int c_code;

    char    tname[256];

    Sdict_search_hd *pds;
    
    //  used by new code 
    char fullpath[PATH_SIZE];
    
/* init vars */
    snode = sizeof(Sdict_snode);

/* read num from file */
    ul_readnum_oneint(path, fname, "hash_num", &h_num);
    ul_readnum_oneint(path, fname, "node_num", &n_num);
    ul_readnum_oneint(path, fname, "cur_code", &c_code);

/* malloc space for dict_hd */
    if(( pds = (Sdict_search_hd *) ul_malloc(sizeof(Sdict_search_hd)))==NULL)
        return NULL;
/* create the dictionary for searcher */
    pds->hash_num = h_num;
    pds->node_num = n_num;
    pds->cur_code = c_code;

/* open index file */
    sprintf(tname, "%s.ind1", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((pds->find1 = ul_open(fullpath,O_RDONLY,0644))==-1)
    {
        ul_free(pds);
        return NULL;
    }   
    
    sprintf(tname, "%s.ind2", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((pds->find2 = ul_open(fullpath,O_RDONLY,0644))==-1)
    {
        ul_close(pds->find1);
        ul_free(pds);
        return NULL;
    }   
/* return */
    return pds;

}

//  checked at 01:27 ,Oct 31 ,2000


Sdict_search *ds_load_parse(char *path, char *fname)
{
    Sdict_search    *pds;
    Sdict_snode *pnode;
    int     node_num, i;

    pds = ds_load(path, fname);

    node_num = pds->node_num;
    pnode = pds->node;

    for (i=0; i<node_num; i++){
        pnode[i].other = 0;
    }

    return pds;
}

Sdict_search *ds_load_name(char *path, char *fname)
{
    return ds_load(path, fname);
}


int ds_reload_test(Sdict_search *pds, char *path, char *fname)
{
    int n_num;
    int c_code;

// just for test 

    return 1;


/* read num from file */
    ul_kreadnum_oneint(path, fname, "node_num", &n_num);
    ul_kreadnum_oneint(path, fname, "cur_code", &c_code);

/* check whether changed */
    if ( (n_num==pds->node_num)
            &&(c_code==pds->cur_code) ){
        return 0;
    }

    return 1;
}

int ds_save_parser(Sdict_search *pds, char *path, char *cpath)
{
    int     curt;
    char    tname[256];

    ul_readnum_oneint(path, "status/dsfreq.cur", "cur_dsfreq", &curt);
    sprintf(tname, "%s/dsfreq.%d", cpath, curt);
    ds_save_freq(pds, path, tname);
    ul_writenum_init(path, "status/dsfreq.cur");
    ul_writenum_oneint(path, "status/dsfreq.cur", "cur_dsfreq", curt+1);
    return 0;
}


Sdict_search *ds_reload(Sdict_search *pds, char *path, char *fname, 
            char *cpath)
{
    Sdict_search    *pds_new;

/* save the freq data */
    ds_save_parser(pds, path, cpath);

/* free the old dict */
    ds_del(pds);

/* load the new dict */
    pds_new = ds_load(path, fname);

/* return */
    return pds_new;

}





int ds_load_freq(Sdict_search *pds, char *path, char *fname)
{
    int     *pfreq;
    int max_code;
    int i, fno;

    Sdict_snode *pnode;

    //  used by new code 
    char    fullpath[PATH_SIZE];
    
/* init vars */
    pnode = pds->node;

/* read num */
    ul_readnum_oneint(path, fname, "max_code", &max_code);

/* check if the freq file match the dict */
    if (max_code != pds->cur_code){
        ul_writelog(UL_LOG_WARNING, "[ds_load_freq] freq do not match dict");
        max_code = pds->cur_code;
    }

/* malloc space to freq array */
    if((pfreq = (int *) ul_calloc(1, max_code*sizeof(int)))==NULL)
        return -2;
        
/* read freq array from file */
    CmpsPath(fullpath,path,fname,PATH_SIZE);
    if((fno = ul_open(fullpath,O_RDONLY,0644))==-1)
    {
        free(pfreq);
        return -2;
    }
    db_read (fno, pfreq, max_code*sizeof(int));
    ul_close(fno);

/* change the dict */
    for (i=0; i<pds->node_num; i++){
        pnode[i].other = Dterm ( Dlweight(pnode[i].other),
                pfreq[pnode[i].code]);
    }

/* free the space */
    ul_free(pfreq);

/* return */
    return 1;
}


int ds_del(Sdict_search *pds)
{
/* free the hash & node */
    ul_free(pds->hash);
    ul_free(pds->num);
    ul_free(pds->node);

/* free the dict */
    ul_free(pds);
    return 0;
}

int ds_del_mmap(Sdict_search *pds)
{
	if(NULL == pds)
	{
		return 0;
	}
	if(pds->node != MAP_FAILED && pds->node != NULL)
	{
		munmap(pds->node, (size_t)pds->node_num*sizeof(Sdict_snode));
		pds->node = NULL;
	}
	if(pds->hash != MAP_FAILED && pds->hash != NULL)
	{
		munmap(pds->hash, 2*(size_t)pds->hash_num*sizeof(int));
		pds->hash = NULL;
		pds->num = NULL;
	}
	/** free the dict */
	ul_free(pds);   
	pds = NULL;
	return 0;
}

int ds_del_hd(Sdict_search_hd *pds)
{
    ul_close(pds->find1);
    ul_close(pds->find2);
    ul_free(pds);
    return 0;
}

//  checked at 01:38,Oct 31,2000

int ds_save_freq(Sdict_search *pds, char *path, char *fname)
{
    int     fno;
    int     *pfreq;
    int     i, max_code;

    Sdict_snode *pnode;

    //  used by new code 
    char    fullpath[PATH_SIZE];
    
/* init vars */
    max_code = pds->cur_code;
    pnode = pds->node;

/* write to .n file */
    ul_writenum_init(path, fname);
    ul_writenum_oneint(path, fname, "max_code", max_code);

/* creat the freq array */
    if((pfreq = (int *) ul_calloc(1,max_code*sizeof(int)))==NULL)
        return -2;
        
    for (i=0; i<pds->node_num; i++){
        pfreq[pnode[i].code]= pnode[i].other & 0x1ffffff;
        pnode[i].other = pnode[i].other & 0xfe000000;
    }

/* save it to file */
    CmpsPath(fullpath,path,fname,PATH_SIZE);
    if((fno = ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))== -1 )
    {
        free(pfreq);
        return -3;
    }   
    fchmod(fno,0644);
    ul_write(fno, pfreq, max_code*sizeof(int));
    ul_close(fno);
    
/* free space */
    ul_free(pfreq);

/* return */
    return max_code;
}


//  checked at 1:52 ,Oct 31 ,2000

int ds_save(Sdict_search *pds, char *path, char *fname)
{
    int     fno=-1;
    char    tname[256];
    //  used by new code 
    char    fullpath[PATH_SIZE];
    
    /* write to .n file */
    if(strlen(path)==0)
    {
        tname[0]=0;
    }
    else if ( path[strlen(path)-1] != '/')
    {
        snprintf(tname,sizeof(tname),"%s/",path);
    }
    else
    {
        snprintf(tname,sizeof(tname),"%s",path);
    }
    
    ul_writenum_init(tname, fname);
    ul_writenum_oneint(tname, fname, "hash_num", pds->hash_num);
    ul_writenum_oneint(tname, fname, "node_num", pds->node_num);
    ul_writenum_oneint(tname, fname, "cur_code", pds->cur_code);

    /* write index 1 */
    sprintf(tname, "%s.ind1", fname);
    CmpsPath(fullpath,path,tname,PATH_SIZE);

    if((fno = ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
    {
        goto failed;
    }

    fchmod(fno,0644);
    
    if (ul_write(fno, pds->hash, pds->hash_num * sizeof(int)) == -1)
    {
        goto failed;
    }
    
    // save number of nodes in hash entries
    if (ul_write(fno, pds->num, pds->hash_num * sizeof(int)) == -1)
    {
        goto failed;
    }
    ul_close(fno);
    fno=-1;
    /* write index 2 */

    sprintf(tname, "%s.ind2", fname);

    CmpsPath(fullpath,path,tname,PATH_SIZE);
    if((fno = ul_open(fullpath,O_WRONLY|O_CREAT|O_TRUNC,0644))==-1)
    {
        goto failed;
    }
    
    fchmod(fno,0644);
    if (ul_write(fno, pds->node, pds->node_num * sizeof(Sdict_snode)) == -1)
    {
        goto failed;
    }
    
    ul_close(fno);
    /* return */
    return 1;
 failed:
    if(fno != -1 )
    {
        ul_close(fno);
    }
    return -1;
}





int ds_op1( Sdict_search *pds, Sdict_snode *pnode, Eop_method op_method)
{
    int hkey;
//  int tint, ioff, inum, i;
    int ioff, inum, i;

    Sdict_snode *pind2;

/* check input argument */


/* get hash key */
    hkey = (pnode->sign1+pnode->sign2)%pds->hash_num;

/* get the offest and number of index2 */
//  tint = pds->hash[hkey];
//  ioff = ((u_int)tint)>>7;
//  inum = tint&0x7f;
    ioff = pds->hash[hkey];     // remove the limit
    inum = pds->num[hkey];

/* searching in the index 2*/
    pind2 = pds->node+ioff;

    for (i=0; i<inum; i++){
        if ((pind2[i].sign1 == pnode->sign1) &&
            (pind2[i].sign2 == pnode->sign2) ){
            break;
        }
    }

    if (i==inum){
        pnode->code = -1;
        return 0;
    }

/* deal with op_method */
    switch (op_method){
    case SEEKP :
        pind2[i].other++;
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        return 1;

    case SEEK :
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        return 1;

    case MODF :
        pind2[i].other++;
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        return 1;
    case MODW :
        pind2[i].other = ( ( (pnode->other)&0xfe000000 ) 
                 | ( (pind2[i].other)&0x01ffffff ) );
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        return 1;
    case MODC :
        pind2[i].code = pds->cur_code;
        pds->cur_code++;
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        return 1;
    default :
        ul_writelog(UL_LOG_WARNING, "[ds_op1] unkown op");
        return -1;

    }
}


int ds_seek_hd(Sdict_search_hd *pds, Sdict_snode *pnode)
{
    int         hkey;
//  int         tint, ioff, inum, i;
    int         ioff, inum, i;

    Sdict_snode     *pnodes;

/* init */
    pnode->other = 0;
    pnode->code  = -1;

/* get hash key */
    hkey = (pnode->sign1+pnode->sign2)%pds->hash_num;

/* get the offest and number of index2 */
    
    ul_lseek(pds->find1, hkey*sizeof(int), SEEK_SET);
    db_read(pds->find1, &ioff, sizeof(int));
    ul_lseek(pds->find1, (pds->hash_num+hkey) * sizeof(int), SEEK_SET);
    db_read(pds->find1, &inum, sizeof(int));
//  ul_lseek(pds->find1, hkey*sizeof(int), SEEK_SET);
//  db_read(pds->find1, &tint, sizeof(int));
//  ioff = ((u_int)tint)>>7;
//  inum = tint&0x7f;

/* get the index2 and find the term */

    if((pnodes = (Sdict_snode *)ul_malloc(inum*sizeof(Sdict_snode)))==NULL)
        return -2;
        
    ul_lseek(pds->find2, ioff*sizeof(Sdict_snode), SEEK_SET);
    db_read(pds->find2, pnodes, inum*sizeof(Sdict_snode));

/* seek it */
    for (i=0; i<inum; i++){
        if( (pnodes[i].sign1==pnode->sign1)
                &&(pnodes[i].sign2==pnode->sign2) ){
            pnode->code = pnodes[i].code;
            pnode->other= pnodes[i].other;
            break;
        }
    }

/* free space */
    ul_free(pnodes);

/* return */
    if (pnode->code == -1){
        return -1;
    }
    else{
        return pnode->code;
    }

}



int ds_opn( Sdict_search *pds, Sdict_snode *plnode, int lnum,
        Eop_method op_method)
{
    int         hkey;
//  int         tint, ioff, inum, i, k;
    int         ioff, inum, i, k;

    Sdict_snode     *pind2;
    Sdict_snode     *pnode;

/* check input argument */

    for (k=0; k<lnum; k++){ //for begin
    pnode = plnode+k;

/* get hash key */
    hkey = (pnode->sign1+pnode->sign2)%pds->hash_num;

/* get the offest and number of index2 */
    ioff = pds->hash[hkey];
    inum = pds->num[hkey];
//  tint = pds->hash[hkey];
//  ioff = ((u_int)tint)>>7;
//  inum = tint&0x7f;

/* searching in the index 2*/
    pind2 = pds->node+ioff;

    for (i=0; i<inum; i++){
        if ((pind2[i].sign1 == pnode->sign1) &&
            (pind2[i].sign2 == pnode->sign2) ){
            break;
        }
    }

    if (i==inum){
        pnode->code = -1;
        continue;
    }

/* deal with op_method */
    switch (op_method){
    case SEEKP :
        pind2[i].other++;
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        continue;

    case SEEK :
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        continue;

    case MODF :
        pind2[i].other++;
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        continue;   

    case MODW :
        pind2[i].other = ( ( (pnode->other)&0xfe000000 ) 
                 | ( (pind2[i].other)&0x01ffffff ) );
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        continue;

    case MODC :
        pind2[i].code = pds->cur_code;
        pds->cur_code++;
        pnode->code = pind2[i].code;
        pnode->other= pind2[i].other;
        continue;

    default :
        ul_writelog(UL_LOG_WARNING, "[ds_op1] unkown op");
        return -1;

    }

    } // end for

    return lnum;
}



int get_log8(u_int sint){
    int reti=0;

    while(sint){
        reti++;
        sint = sint>>3;
    }

    return reti;
}


//  checked at 2:19, Oct 31 ,2000


int pre_creat_search_dict(char *path, char *fnold_dict, char *fnnew_dict,
        char *fn_dbfreq, char *fn_dsfreq)
{
    Sdict_search    *pds;
    Sdict_snode *pnode;

    int     *pfreq, *pfile;
    int     freq_num;
    int     term_num, term_size;
    int     tnum, this_num, this_log8, url_num, url_log8;

    int         i, j, fno;
    char        fullname[PATH_SIZE];

    //used by new code
    char    fpath[PATH_SIZE];
    

/* load old dict */
    ul_writelog(UL_LOG_NOTICE, "loading dict...");
    pds = ds_load(path, fnold_dict);

/* malloc space for dict freq */
    term_num = pds->cur_code;
    term_size= term_num*sizeof(int);

    if((pfreq = (int*) ul_calloc(1,term_size))==NULL)
        return -2;
    if((pfile = (int*) ul_calloc(1,term_size))==NULL)
    {
        ul_free(pfreq);
        return -2;
    }
    
/* get the number of dbfreq files */
    ul_readnum_oneint(path, "status/dbfreq.cur", "cur_dbfreq", &freq_num);

/* do dbfreq */
    for (i=0; i<freq_num; i++){
        ul_showgocmp("dbfreq", freq_num, i);
        sprintf(fullname, "%s.%d", fn_dbfreq, i);
        CmpsPath(fpath,path,fullname,PATH_SIZE);
        fno=ul_open(fpath,O_RDONLY,0644);
        if (fno != 0)
        {
            memset(pfile, 0, term_size);
            ul_read(fno, pfile, term_size);
            for (j=0; j<term_num; j++)
                pfreq[j]+=pfile[j];
            ul_close(fno);
        }
    }
    ul_showgoend("dbfreq", freq_num, i);

/* get ds_freq from num file */
    ul_readnum_oneint(path, "status/dsfreq.cur", "cur_dsfreq", &freq_num);

/* do ds_freq */
    for (i=0; i<freq_num; i++){
        ul_showgocmp("dsfreq", freq_num, i);
        sprintf(fullname, "%s.%d", fn_dsfreq, i);
        CmpsPath(fpath,path,fullname,PATH_SIZE);
        fno=ul_open(fpath,O_RDONLY,0644);
        if (fno>0){ 
            memset(pfile, 0, term_size);
            ul_read(fno, pfile, term_size);

            for (j=0; j<term_num; j++){
                pfreq[j]+=pfile[j];
            }
        }
        ul_close(fno);
    }
    ul_showgoend("dsfreq", freq_num, i);

/* get url_num and url_log8 */
    ul_readnum_oneint(path, "status/url", "url_num", &url_num);
    url_log8 = get_log8( (u_int) url_num );

/* modify the old dict to new dict */
    pnode = pds->node;

    for (i=0; i<pds->node_num; i++){
        this_log8 = 0;
        this_num  = pfreq[pnode[i].code];
        tnum      = this_num;

        while(tnum){
            this_log8++;
            tnum = tnum>>3;
        }

        this_log8 = url_log8 - this_log8 + 1;
        if ((this_log8<0) || (this_log8 > 10)){
            ul_writelog(UL_LOG_WARNING,"[creat_search_dict] itf < 0");
        }
        pnode[i].other = Dterm(this_log8, this_num);
    }

/* save the new dict */
    ul_writelog(UL_LOG_NOTICE, "save dict for searching...");
    ds_save(pds, path, fnnew_dict);

/* free the space */
    ul_free(pfile);
    ul_free(pfreq);
    ds_del(pds);

/* return */
    return 1;
}

//  checked at 2:53 ,Oct 31 ,2000


int creat_search_dict(char *path, char *fnold_dict, char *fnnew_dict,
        char *fn_dbfreq)
{
    Sdict_search    *pds;
    Sdict_snode *pnode;

    int     *pfreq;
    //  int     freq_num;
    int     term_num, term_size;
    int     tnum, this_num, this_log8, url_num, url_log8;

    int         i;  //, j, fno;

/* load old dict */
    ul_writelog(UL_LOG_NOTICE, "loading dict...");
    pds = ds_load(path, fnold_dict);

/* malloc space for dict freq */
    term_num = pds->cur_code;
    term_size= term_num*sizeof(int);

    if((pfreq = (int*) ul_calloc(1,term_size))==NULL)
        return -2;
        
    ul_readfile(path, fn_dbfreq, pfreq, term_size);

/* get url_num and url_log8 */
    ul_readnum_oneint(path, "status/url", "url_num", &url_num);
    url_log8 = get_log8( (u_int) url_num );

/* modify the old dict to new dict */
    pnode = pds->node;

    for (i=0; i<pds->node_num; i++){
        this_log8 = 0;
        this_num  = pfreq[pnode[i].code];
        tnum      = this_num;

        while(tnum){
            this_log8++;
            tnum = tnum>>3;
        }

        this_log8 = url_log8 - this_log8 + 1;
        if (this_log8 > 10){
            ul_writelog(UL_LOG_WARNING, "[creat_search_dict] itf <= 0");
            this_log8 = 10;
        }
        if (this_log8 <= 0){
            ul_writelog(UL_LOG_WARNING, "[creat_search_dict] itf <= 0");
            this_log8 = 1;
        }
        pnode[i].other = Dterm(this_log8, this_num);
    }

/* save the new dict */
    ul_writelog(UL_LOG_NOTICE, "save dict for searching...");
    ds_save(pds, path, fnnew_dict);

/* free the space */
    ul_free(pfreq);
    ds_del(pds);

/* return */
    return 1;
}





int creat_vice_dict(char *path, char *fnbase_dict, char *fnold_dict, 
        char *fnnew_dict, char *fn_dbfreq)
{
    Sdict_search    *pds, *pdb;
    Sdict_snode *pnode, tpnode;

    int     *pfreq;
    int     term_num, term_size;
    int         tnum, this_num, this_log8, url_num, url_log8;

    int         i;  //  , j, fno;

/* check the base dict */
    if (!ul_fexist(path, "raw/term.n")){
        ul_writelog(UL_LOG_WARNING, "have no the dict of gi");
    }
    if (!ul_fexist(path, "raw/url.n")){
        ul_writelog(UL_LOG_WARNING, "have no the url.n of gi");
    }

/* load base dict */
    ul_writelog(UL_LOG_NOTICE, "loading dict...");
    pdb = ds_load(path, fnbase_dict);

/* load old dict */
    pds = ds_load(path, fnold_dict);

/* malloc space for dict freq */
    term_num = pds->cur_code;
    term_size= term_num*sizeof(int);

    if((pfreq = (int*) ul_calloc (1, term_size))==NULL)
        return 2;
    ul_readfile(path, fn_dbfreq, pfreq, term_size);

/* get url_num and url_log8 */
    ul_readnum_oneint(path, "raw/url", "cur_code", &url_num);
    printf("[creat_vice_dict] gi url_num (%d)\n", url_num);
    url_log8 = get_log8( (u_int) url_num );

/* modify the old dict to new dict */
    pnode = pds->node;

    for (i=0; i<pds->node_num; i++){
        memcpy(&tpnode, pnode+i, sizeof(Sdict_snode));
        if (ds_op1(pdb, &tpnode, SEEK)){
            tnum = pfreq[pnode[i].code];
            pnode[i].other = Dterm(Dlweight(tpnode.other), tnum);
            continue;
        }

        this_log8 = 0;
        this_num  = pfreq[pnode[i].code];
        tnum      = this_num;

        while(tnum){
            this_log8++;
            tnum = tnum>>3;
        }

        this_log8 = url_log8 - this_log8 + 1;
        if (this_log8<=0){
            ul_writelog(UL_LOG_WARNING,"[creat_vice_dict] idf(%d)<=0, num(%d)",
                        this_log8,this_num);
                        
            this_log8 = 1;
        }

        if (this_log8>10){
            ul_writelog(UL_LOG_WARNING,"[creat_vice_dict] idf(%d)>10, num(%d)",
                        this_log8, this_num);
            this_log8 = 10;
        }

        pnode[i].other = Dterm(this_log8, this_num);
    }

/* save the new dict */
    ul_writelog(UL_LOG_NOTICE, "save dict for searching...");
    ds_save(pds, path, fnnew_dict);

/* free the space */
    ul_free(pfreq);
    ds_del(pds);
    ds_del(pdb);

/* return */
    return 1;
}



//  checked at 3:15 ,Oct 31 ,2000

//  waiting for check 


int creat_parser_dict(char *path, char *fnold_dict, char *fnnew_dict,
        char *fn_lw)
{
    Sdict_search    *pds;
    Sdict_snode *pnode;

    char    *plw;
    int     term_num, term_size, this_num;

    int     i, fno, zero;

    //  used by new code 
    char    fpath[PATH_SIZE];
    
/* load old dict */
    pds = ds_load(path, fnold_dict);

/* malloc space for dict freq */
    zero = 0;
    term_num = pds->cur_code;
    term_size= term_num*sizeof(int);

    if((plw =(char*) ul_calloc(1,term_num*sizeof(char)))==NULL)
        return -2;
        
/* open lowest weight file */
    CmpsPath(fpath,path,fn_lw,PATH_SIZE);
    if((fno= ul_open(fpath,O_RDONLY,0644))==-1)
    {
        ul_free(plw);
        return -3;
    }   
    
/* read data */
    ul_read(fno, plw, term_size);

/* modify the old dict to new dict */
    pnode = pds->node;

    for (i=0; i<pds->node_num; i++){
        this_num  = plw[pnode[i].code];
        pnode[i].other = Dterm(this_num, zero);
    }

/* save the new dict */
    ul_writelog(UL_LOG_NOTICE, "save dict for parsing...");
    ds_save(pds, path, fnnew_dict);

/* free the space */
    ul_free(plw);
    ds_del(pds);

/* return */
    return 1;
}

//  checked at 11:08 ,Oct 31,2000

int creat_stand_dict(char *path, char *fnsrc, char *fndes, int num)
{
    Sdict_search    *pds;
    Sdict_snode     *psnode;
    Sdict_snode     *pdnode;
    Sdict_build     *pdb;
    int     af[MMM_WEIGHT];
    int     cur_num, i, j, off, freq, reti;

/* malloc space */
    if((pdnode=(Sdict_snode *)ul_malloc(sizeof(Sdict_snode)*num))==NULL)
        return -2;
        
/* mem set */
    bzero(af, MMM_WEIGHT*sizeof(int));

/* load dict */
    pds = ds_load(path, fnsrc);
    psnode = pds->node;

/* calc number */
    for (i=0; i<pds->node_num; i++){
        freq = Dfreq(psnode[i].other);
        if (freq >= MMM_WEIGHT){
            af[MMM_WEIGHT-1]++;
        }
        else{
            af[freq]++;
        }
    }

/* get lowest */
    cur_num = 0;
    for (i=MMM_WEIGHT-1; i>0; i--){
        if ( (cur_num + af[i])<num ){
            cur_num += af[i];
        }
        else{
            af[i] -= (cur_num+af[i]-num);
            cur_num = num;
            break;
        }
    }

    reti = i;

    if (i==MMM_WEIGHT-1){
        ul_writelog(UL_LOG_WARNING, "[creat_stand_dict] MMM_WEIGHT too small");
    }

    for (j=i-1; j>=0; j--){
        af[j]=0;
    }

/* check */
    cur_num  = 0;
    for (i=0; i<MMM_WEIGHT; i++){
        cur_num += af[i];
    }

    if ( (cur_num>num) || (cur_num<num)&&(cur_num!=pds->node_num)){
        ul_writelog(UL_LOG_WARNING, "[creat_stand_dict] num error");
    }

/* get new item */
    j=0;
    for (i=0; i<pds->node_num; i++){
        freq = Dfreq(psnode[i].other);
        if (freq>=MMM_WEIGHT){
            off = MMM_WEIGHT-1;
        }
        else{
            off = freq; 
        }

        if (af[off]!=0){
            pdnode[j].sign1 = psnode[i].sign1;
            pdnode[j].sign2 = psnode[i].sign2;
            pdnode[j].code  = -1;
            pdnode[j].other = freq;
            j++;
            af[off]--;
/*
            if ((j+af[off])!=30000){
                off = j;
            }
*/
        }
    }

/* check */
    if (j!=cur_num){
        ul_writelog(UL_LOG_WARNING, "[creat_stand_dict] num 2 error");
    }

/* report */
    ul_writelog(UL_LOG_NOTICE,"[crear_stad_dict] (%d)->(%d) lw (%d)",
                pds->cur_code, cur_num, num);
/* del ds */
    ds_del(pds);

/* build new dict */
    pdb = db_creat(num+1, 0);
    db_opn(pdb, pdnode, cur_num, ADD);
    db_adjust(pdb);
    db_save(pdb, path, fndes);
    db_del(pdb);

/* del the space */
    ul_free(pdnode);

/* return */
    return reti;
}

//  checked at 11:15 ,Oct 31 ,2000


int dp_build(char *path, char *fnsrc, char *fndes)
{
    Shash_phrase    hash[PHRASE_IND1_NUM];
    char        *pph, *ph_src, *ph, *pstr;
    char        word[LINE_SIZE], istr[4];
    char        fullname[PATH_SIZE];
    u_int       wint;
    int     i, woff, wlen, plen, ph_size;
    int     phnum[PHRASE_IND1_NUM], phoff[PHRASE_IND1_NUM];
    int     phcoff[PHRASE_IND1_NUM];

/* get the phrase from file */
    ph_size = ul_fsize(path, fnsrc);
    if (ph_size <= 0){
        ul_writelog(UL_LOG_WARNING, "[dp_build] have no phrase");
    }

    if((ph_src =(char*) ul_malloc(ph_size+1))==NULL)
        return -2;
    if((ph=(char*) ul_malloc(ph_size+1))==NULL)
    {
        ul_free(ph_src);
        return -2;
    }
    ph_src[ph_size]=0;
    ph[ph_size]=0;

    ul_readfile(path, fnsrc, (void *)ph_src, ph_size);

/* calc number if hash item */
    plen = 0;
    pph = ph_src;
    while(1){
        pph = ul_sgetw(pph, word, legal_char_set);
        if (word[0] == 0){
            break;
        }

        wlen = strlen(word);
        if (wlen<3){
            ul_writelog(UL_LOG_WARNING,"[dp_build] phrase(%s) length <3",
                        word);
            continue;
        }

        if (wlen>WORD_SIZE-1){
            ul_writelog(UL_LOG_WARNING,"[dp_build] phrase (%s) length > %d",
                        word,WORD_SIZE-1);
            exit(-1);
        }

        strncpy(istr, word, 3);
        istr[3]=0;
        wint = *((unsigned int *) istr);
        woff = wint%PHRASE_IND1_NUM;

        phcoff[woff] += (wlen+1);
        phnum[woff]++;
        plen += (wlen+1);
    }

/* calc the off of ph */
    for (i=0; i<PHRASE_IND1_NUM-1; i++){
        phoff[i+1] = phoff[i] + phcoff[i];
        phcoff[i] = phoff[i];
    }

/* check */
    if ( (phoff[i] + phcoff[i]) != plen){
        ul_writelog(UL_LOG_WARNING, "[dp_build] number error");
    }

/* the end item */
    phcoff[i] = phoff[i];

/* build the dict */
    pph = ph_src;
    while (1){
        pph = ul_sgetw(pph, word, legal_char_set);
        if (word[0] == 0){
            break;
        }

        wlen = strlen(word);
        if (wlen<3){
            continue;
        }

        strncpy(istr, word, 3);
        istr[3]=0;
        wint = *((unsigned int *) istr);
        woff = wint%PHRASE_IND1_NUM;

        pstr = ph + phcoff[woff];
        strcpy(pstr, word);
        pstr[wlen]='\n';
        phcoff[woff] += (wlen+1);
    }

/* check */
    for (i=0; i<PHRASE_IND1_NUM-1; i++){
        if (phcoff[i] != phoff[i+1]){
            ul_writelog(UL_LOG_WARNING, "[dp_build] build dict error");
        }
    }
    if (phcoff[i] != plen){
        ul_writelog(UL_LOG_WARNING, "[dp_build] build dict error");
    }

/* check the max & avg hit */

/* write to file */
    for (i=0; i<PHRASE_IND1_NUM; i++){
        hash[i].off = phoff[i];
        hash[i].num = phnum[i];
    }
    sprintf(fullname, "%s.ind1", fndes);
    ul_writefile (path, fullname, hash, PHRASE_IND1_NUM*sizeof(Shash_phrase));
    sprintf(fullname, "%s.ind2", fndes);
    ul_writefile (path, fullname, ph, plen);
    return 0;
}

//checked at 11:40 ,Oct 31 ,2000


Sdict_phrase *dp_load(char *path, char *fname)
{
    Sdict_phrase    *dp;
    char        fullname[PATH_SIZE];
    int     hsize, i;

/* load dp */

    if((dp=(Sdict_phrase*)ul_malloc(sizeof(Sdict_phrase)))==NULL)
        return NULL;
        
/* load hash */
    sprintf(fullname, "%s.ind1", fname);
    hsize = ul_fsize(path, fullname);
    if (hsize == -1){
        ul_writelog(UL_LOG_WARNING, "[dp_load] open hash file error");
    }
    if (hsize != PHRASE_IND1_NUM*sizeof(Shash_phrase)){
        ul_writelog(UL_LOG_WARNING, "[dp_load] hash file num error");
    }

    if((dp->hash = (Shash_phrase *)ul_malloc( hsize))==NULL)
    {
        ul_free(dp);
        return NULL;
    }   
    ul_readfile(path, fullname, (void *)dp->hash, hsize);

/* load phrase */
    sprintf(fullname, "%s.ind2", fname);
    hsize = ul_fsize(path, fullname);
    if (hsize == -1){
        ul_writelog(UL_LOG_WARNING, "[dp_load] open phrase file error");
    }

    if((dp->phrase = (char*) ul_malloc(hsize))==NULL)
    {
        ul_free(dp->hash);
        ul_free(dp);
        return  NULL;
    }
    ul_readfile(path, fullname, (void *)dp->phrase, hsize);

/* change \n to 0 */
    for (i=0; i<hsize; i++){
        if (dp->phrase[i] == '\n'){
            dp->phrase[i] = 0;
        }
    }

/* return */
    return dp;

}

//  checked at 11:41 ,Oct 31 ,2000 

int dp_seek(Sdict_phrase *dp, char *ssrc, char *sdes)
{
    Shash_phrase    *hash;
    char        *ph;
    u_int       sign;
    char        *psrc, *pstr, istr[4];
    int     i, ph_num, slen, off;

/* init vars */
    psrc = ssrc;
    hash = dp->hash;
    ph   = dp->phrase;
    ph_num = 0;
    sdes[0] = 0;

/* do */
    while ( (psrc[0]!=0) && (psrc[1]!=0) && (psrc[2]!=0) ){
        strncpy(istr, psrc, 3);
        istr[3]=0;
        sign = *((u_int *)istr);
        off  = sign%PHRASE_IND1_NUM;
        pstr = ph + hash[off].off;
        for (i=0; i<hash[off].num; i++){
            slen = strlen(pstr);
            if (strncmp(psrc, pstr, slen) == 0){
                if ((u_int)(psrc[slen-1]) >127){
                    ph_num++;
                    strcat(sdes, pstr);
                    strcat(sdes, " ");
                }
                else if ((psrc[slen]==0)
                        ||((u_int)psrc[slen]>127)
                        ||(iswspace((wint_t)psrc[slen]))){
                    ph_num++;
                    strcat(sdes, pstr);
                    strcat(sdes, " ");
                }
            }
            pstr += (slen+1);
        }
        if ( ((u_int)(*psrc)) < 128 ){
            psrc++;
        }
        else{
            psrc+=2;
        }
    }

/* return */
    return ph_num;
}

//checked at 11:47, Oct 31, 2000


int dp_search(Sdict_phrase *dp, char *ssrc, char *sdes)
{
    Shash_phrase    *hash;
    char        *ph;
    u_int       sign;
    char        *psrc, *pstr, istr[4], tstr[WORD_SIZE];
    int     i, ph_num, slen, off;

/* init vars */
    psrc = ssrc;
    hash = dp->hash;
    ph   = dp->phrase;
    ph_num = 0;
    sdes[0] = 0;

/* do */
    strncpy(istr, psrc, 3);
    istr[3]=0;
    sign = *((u_int *)istr);
    tstr[0]=0;
    off  = sign%PHRASE_IND1_NUM;
    pstr = ph + hash[off].off;
    for (i=0; i<hash[off].num; i++){
        slen = strlen(pstr);
        if (strncmp(psrc, pstr, slen) == 0){
            if ((u_int)(psrc[slen-1]) >127){
                if (slen>(int)strlen(tstr)){
                    strcpy(tstr, pstr);
                }
            }
            else if ((psrc[slen]==0)
                    ||((u_int)psrc[slen]>127)
                    ||(iswspace((wint_t)psrc[slen]))){
                if (slen>(int)strlen(tstr)){
                    strcpy(tstr, pstr);
                }
            }
        }
        pstr+=(slen+1);
    }
    if (tstr[0]==0){
        return 0;
    }
    else{
        strcpy(sdes, tstr);
        return 1;
    }
}

//  checked at 11:48, Oct 31, 2000


int dp_del(Sdict_phrase *dp)
{
    ul_free(dp->hash);
    ul_free(dp->phrase);
    ul_free(dp);
    return 0;
}

//  checked at 11:52, Oct 31, 2000

Sdict_cphrase *dcp_load(char *path, char *fname, char *fsurname)
{
    Sdict_cphrase   *dcp;
    Shash_cphrase   *phash;
    unsigned char   *pp;

    char        fullname[PATH_SIZE];
    struct stat     st;
    int     fno;

    int         ind1, ind2, ind3;
    int         phrase_size, hash_size;
    int     i, icur;
    
    //  used by new code 
    char fpath[PATH_SIZE];
    
/* get file size */
    sprintf(fullname, "%s/%s", path, fname);
    if (ul_stat(fullname, &st)){
        ul_writelog(UL_LOG_WARNING, "[dcp_load] stat error");
        return NULL;
    }

    phrase_size = st.st_size;

/* malloc space to dict */
    if((dcp =(Sdict_cphrase*) ul_calloc(1,sizeof(Sdict_cphrase)))==NULL)
        return NULL;
        
    hash_size = sizeof(Shash_cphrase)*CHAR_HINUM*CHAR_LONUM*CHAR_HINUM;
    if((dcp->hash =(Shash_cphrase*)ul_calloc(1,hash_size))==NULL)
    {
        ul_free(dcp);
        return NULL;
    }
    if((dcp->phrase = (unsigned char *) ul_malloc(phrase_size+1))==NULL)
    {
        ul_free(dcp->hash);
        ul_free(dcp);
        return NULL;
    }
    dcp->phrase[phrase_size]=0;

/* load surname file */
    dcp->surname = dsur_load(path, fsurname);

/* read phrase from file */
    CmpsPath(fpath,path,fname,PATH_SIZE);
    fno=ul_open(fpath,O_RDONLY,0644);
    if (read(fno, dcp->phrase, phrase_size)!=phrase_size)
    {
        ul_writelog(UL_LOG_WARNING, "[dcp_load] read phrase error");
        ul_close(fno);
        ul_free(dcp->phrase);
        ul_free(dcp->hash);
        ul_free(dcp);
        return NULL;
    }
    ul_close(fno);

/* chage the '\n' to '\0' */
    pp = dcp->phrase;
    for (i=0; i<phrase_size; i++){
        if (pp[i]=='\n'){
            pp[i]=0;
        }
    }

/* manage phrase */
    icur=0;
    while (icur<phrase_size){
        ind1=pp[icur++]-CHAR_HIMIN;
        ind2=pp[icur++]-CHAR_LOMIN;
        ind3=pp[icur++]-CHAR_HIMIN;
        icur-=3;
        phash = dcp->hash + Dcpoff(ind1, ind2, ind3);
        if (phash->off==0)
            phash->off=icur;
        phash->num++;

        icur+=(strlen((char*)(pp+icur))+1);
    }

/* return */
    return dcp;
}

//  checked at 12:03 ,Oct 31 ,2000



int dcp_seek(Sdict_cphrase *dcp, char *src_ph, char *des_ph)
{
    unsigned char *ps_src, *ps_des;
    unsigned char *pp;
    Shash_cphrase   *phash;
    char        *psur;
    char        sword[WORD_SIZE];

    int ind1, ind2, ind3;
    int cmp_len, reti, i, phrase_num;
    int ph_off, ph_num;

/* init vars */
    ps_src = (unsigned char *)src_ph;
    ps_des = (unsigned char *)des_ph;
    pp = (unsigned char *)dcp->phrase;
    psur = dcp->surname;
    ps_des[0]=0;
    phrase_num=0;

/* search all phrase int the string */
    while(*ps_src!=0){

            /* check if end */
        if ( ps_src[1]==0 || ps_src[2]==0 || ps_src[3]==0 )
            break;

            /* deal with surname */
        sword[0] = 0;
        if (psur[ps_src[0]*256 + ps_src[1]] == 1){
            if (strlen((char *)ps_src) >= 6){
                strncpy((char *)sword, (char *)ps_src, 6);
                sword[6] = 0;
                strncat((char *)des_ph, (char *)ps_src, 6);
                strcat((char *) des_ph, " ");
                phrase_num++;
            }
        }

            /* get offset */
        ind1 = ps_src[0] - CHAR_HIMIN;
        ind2 = ps_src[1] - CHAR_LOMIN;
        ind3 = ps_src[2] - CHAR_HIMIN;

        ps_src+=3;

        phash = dcp->hash + Dcpoff(ind1, ind2, ind3);

        ph_off = phash->off;
        ph_num = phash->num;


            /* have no phrase in this hash item */
        if (ph_num==0){
            ps_src--;
            continue;
        }

            /* have */
        for (i=0; i<ph_num; i++){
            ph_off+=3;
            cmp_len = strlen((char *)(pp+ph_off));
            reti = strncmp((char *)ps_src, 
                (char *)(pp+ph_off), cmp_len);

                /* maybe have */
            if (reti>0){
                ph_off+=(cmp_len+1);
                continue;
            }

                /* found it */
            if (reti==0){
                if ((strlen( (char*)(pp+ph_off) ) != 3)
                    || (sword[0]==0) ){
                    strcat((char*)des_ph,(char*)(pp+ph_off-3));
                    strcat((char*)des_ph, " ");
                    phrase_num++;
                }
                ph_off+=(cmp_len+1);
                continue;
            }

                /* have not */
            break;

        }
        ps_src--;
    }

/* return */
    return phrase_num;
}


//  checked at 12:18, Oct 31, 2000


int dcp_search(Sdict_cphrase *dcp, char *src_ph, char *des_ph)
{
    unsigned char   *ps_src, *ps_des;
    unsigned char   *pp, tstr[LINE_SIZE];
    char        *psur, sword[WORD_SIZE];
    Shash_cphrase   *phash;

    int ind1, ind2, ind3;
    int cmp_len, reti, i, phrase_num;
    int ph_off, ph_num;

/* check if <=2 chinese char */
    if(strlen(src_ph)<=4){
        strcpy(des_ph, src_ph);
        strcat(des_ph, " ");
        return 1;
    }

/* init vars */
    ps_src = (unsigned char *)src_ph;
    ps_des = (unsigned char *)des_ph;
    pp = (unsigned char *)dcp->phrase;
    psur = dcp->surname;
    ps_des[0]=0;
    phrase_num=0;
    tstr[0]=0;

/* search all phrase int the string */
    while(*ps_src!=0){
            /* check if end */
        if ( ps_src[1]==0 || ps_src[2]==0 || ps_src[3]==0 )
            break;

            /* deal with surname */
        sword[0] = 0;
        if (psur[ps_src[0]*256 + ps_src[1]] == 1){
            if (strlen((char *)ps_src) >= 6){
                strncpy((char *)sword, (char *)ps_src, 6);
                sword[6] = 0;
            }
        }

            /* get offset */
        ind1 = ps_src[0] - CHAR_HIMIN;
        ind2 = ps_src[1] - CHAR_LOMIN;
        ind3 = ps_src[2] - CHAR_HIMIN;

        ps_src+=3;

        phash = dcp->hash + Dcpoff(ind1, ind2, ind3);

        ph_off = phash->off;
        ph_num = phash->num;

            /* init tstr */
        tstr[0] = 0;

            /* have no phrase in this hash item */
/*
        if (ph_num==0){
            ps_src--;
            strncat((char *)ps_des, (char *)(ps_src-2), 4);
            strcat((char *)ps_des, " ");
            phrase_num++;
            continue;
        }
*/

            /* have */
        for (i=0; i<ph_num; i++){
            ph_off+=3;
            cmp_len = strlen((char *)(pp+ph_off));
            reti = strncmp((char *)ps_src, 
                (char *)(pp+ph_off), cmp_len);

                /* maybe have */
            if (reti>0){
                ph_off+=(cmp_len+1);
                continue;
            }

                /* found it */
            if (reti==0){
                strcpy((char*)tstr, (char*)(pp+ph_off-3));
                ph_off+=(cmp_len+1);
                continue;
            }

                /* have not */
            break;

        }

        phrase_num++;

        if ( (tstr[0]==0) && (sword[0]==0) ){
            ps_src--;
            strncat((char *)ps_des, (char *)(ps_src-2), 4);
            strcat((char *)ps_des, " ");
        }
        else if ( (tstr[0]==0) && (sword[0]!=0) ){
            ps_src = ps_src + strlen((char *)sword) - 5;
            strcat((char *)ps_des, (char *)sword);
            strcat((char *)ps_des, " ");
        }
        else{
            ps_src = ps_src + strlen((char *)tstr) - 5;
            strcat((char *)ps_des, (char *)tstr);
            strcat((char *)ps_des, " ");
        }
    }

/* return */
    return phrase_num;
}

//  checked at 02:18 ,Nov 1 ,2000

int dcp_del(Sdict_cphrase *dcp)
{
/* free all */
    ul_free(dcp->surname);
    ul_free(dcp->phrase);
    ul_free(dcp->hash);
    ul_free(dcp);

/* return */
    return 1;
}


//  checked at 02:19, Nov 1, 2000

/* deal with the name of human */

char *dsur_load( char *path, char *fname )
{
    FILE    *fsrc;
    u_char  usur_str[WORD_SIZE];
    char    *psur_str, *dsur;
    //  used by new code 
    char    fpath[PATH_SIZE];
    
/* init to no surname */
    if((dsur = (char*) ul_calloc(1,SUR_IND1_NUM))==NULL)
        return NULL;
    psur_str = (char *)usur_str;

/* open file */
    CmpsPath(fpath,path,fname,PATH_SIZE);
    if((fsrc = ul_fopen(fpath,"r"))==NULL)
    {
        ul_free(dsur);
        return NULL;
    }
    
/* read surname from file */
    while (fscanf(fsrc, "%s", psur_str) > 0){

            /* check */
/*
        if(usur_str[2]!=0)
        {
            sprintf(inf_buf, "[dsur_load] surname(%s) in(%s)error", 
                    psur_str, fname);
            writelog(pf_log, inf_buf, IPROG);
        }
*/
            /* check */
        if ( !IS_GB_CODE(usur_str) )
        {
            ul_writelog(UL_LOG_WARNING, "[dsur_load] surname(%s) in(%s)error",
                    psur_str, fname);
            
        }

        dsur[usur_str[0]*256 + usur_str[1]] = 1;
    }

    return dsur;
}

//  checked at 02:25, Nov 1, 2000

int dsur_del( char *dsur )
{
    ul_free (dsur);
    return 1;
}

int dsur_seek( char *dsur, char *spstr, char *dpstr )
{
    int     slen;
    int i, count;
    u_char  *sstr, *dstr;

/* init vars */
    sstr = (u_char *)spstr;
    dstr = (u_char *)dpstr;
    slen = strlen((char *)sstr);
    dstr[0]=0;
    count = 0;

/* check the surname */
    for (i=0; i<slen; i++){
        if (IS_GB_CODE(((char*)sstr+i))){
            if(dsur[sstr[i]*256 + sstr[i+1]] == 1){
                if (IS_GB_CODE(((char*)sstr+i+2))
                &&IS_GB_CODE(((char*)sstr+i+4))){
                    strncat((char *)dstr, (char *)(sstr+i),
                             6);
                    strcat((char *)dstr, " ");
                    count++;
                }
            }
        }

        if (sstr[i]>127){
            i++;
        }
    }

/* return */
    return count;
}




int dsur_search( char *dsur, char *spstr, char *dpstr )
{
    int     slen;
    int i, count;
    u_char  *sstr, *dstr;

/* init vars */
    sstr = (u_char *)spstr;
    dstr = (u_char *)dpstr;
    slen = strlen((char *)sstr);
    dstr[0]=0;
    count = 0;

/* check the surname */
    i=0;
    if (IS_GB_CODE(((char*)sstr+i))){
        if(dsur[sstr[i]*256 + sstr[i+1]] == 1){
            if (IS_GB_CODE(((char*)sstr+i+2))
            &&IS_GB_CODE(((char*)sstr+i+4))){
                strncat((char *)dstr, (char *)(sstr+i),
                         6);
                strcat((char *)dstr, " ");
                count++;
            }
        }
    }

/* return */
    return count;
}


/***********************************************************
 * Function:    db_syn_creat
 * Description: create a empty synonym dictionary
 * Input:
 *  hash_num: size of the hash table
 * Output:
 *  pointer to the created dictionary, NULL if failed
 **********************************************************/

Sdict_build_syn *db_syn_creat( int hash_num )
{
    Sdict_build_syn *pdb = NULL;
    Sdict_syn_bhash *phash = NULL;
        Sdict_syn_bnode_block *pblock = NULL;
    Sdict_syn_buf_block *pbuf = NULL;
    Sdict_build *pdworddict = NULL;

    if ((pdb = (Sdict_build_syn*) ul_malloc(sizeof(Sdict_build_syn))) == NULL)
        return NULL;
    if ((phash = (Sdict_syn_bhash*) ul_calloc(1,hash_num*sizeof(Sdict_syn_bhash))) == NULL) {
        ul_free(pdb);
        return NULL;
    }
    if ((pblock = (Sdict_syn_bnode_block*) ul_calloc(1,sizeof(Sdict_syn_bnode_block))) == NULL) {
        ul_free(pdb);
        ul_free(phash);
        return NULL;
    }
    if ((pbuf = (Sdict_syn_buf_block*) ul_calloc(1,sizeof(Sdict_syn_buf_block))) == NULL) {
        ul_free(pdb);
        ul_free(phash);
        ul_free(pblock);
        return NULL;
    }
    if ((pdworddict = db_creat(hash_num, 0)) == NULL) {
        ul_free(pdb);
        ul_free(phash);
        ul_free(pblock);
        ul_free(pbuf);
        return NULL;
    }

    pdb->pdword = pdworddict;
    pdb->words = 0;

        pdb->hash = phash;
        pdb->hash_num = hash_num;
        pdb->node_num = 0;

        pdb->nblock = pblock;
    pdb->cur_nblock = pdb->nblock;
    pdb->cur_node = pdb->nblock->nodes;
        pdb->node_avail = NODE_BLOCK_NUM;

    pdb->bufblock = pbuf;
    pdb->nrbufblock = 1;
    pdb->cur_bufblock = pdb->bufblock;
    pdb->cur_buf = 0;

        return pdb;
}

/***********************************************************
 * Function:    db_syn_construct
 * Description: add synonyms in a configuration file to a 
 *  dictionary.
 * Input:
 *  fullpath: where to find the configuration file
 *  hash_num: size of the hash table
 *  conf_type: 0 - synonym dictionary
 *         1 - association dictionary
 * Output:
 *  pointer to the created dictionary, NULL if failed
 **********************************************************/

Sdict_build_syn *db_syn_construct( char* fullpath, int hash_num, int conf_type)
{
    Sdict_build_syn *pdb = NULL;
    char buf[4098];
    char words[1024][256];
    char *pbuf, *pword;
    int nrwords, i, j;
    FILE *pfconf;

    if (NULL == (pdb = db_syn_creat(hash_num))) {
        ul_writelog(UL_LOG_FATAL, "create synonym dict failed");
        return NULL;
    }   
    if (NULL == (pfconf = ul_fopen(fullpath, "r"))) {
        db_syn_del(pdb);
        ul_writelog(UL_LOG_FATAL, "open %s failed", fullpath);
    }

    pbuf = buf;
    nrwords = 0;
    pword = words[0];
    while (NULL != fgets(buf, 4098, pfconf)) {
        while (*pbuf != '\n' && *pbuf != '\0' && *pbuf != '\r') {
            if (*pbuf != WORDDELIMITER) {
                *pword++ = *pbuf++;
            } else {
                nrwords++;
                if (nrwords > 1024) {
                    ul_writelog(UL_LOG_WARNING, "number of synonyms > 1024");
                    break;
                }
                *pword = '\0';
                pword = words[nrwords];
                pbuf++;
            }
        }
        if (pbuf != buf && *--pbuf != WORDDELIMITER) {
            nrwords++;
            *pword = '\0';
        }
        switch (conf_type) {
        case 0:
            #ifdef _DEBUG_
            ul_writelog(UL_LOG_TRACE, "synonym nrwords: %d", nrwords);
            #endif
            for (i=0; i<nrwords; i++) {
                for (j=i+1; j<nrwords; j++) {
                    #ifdef _DEBUG_
                    ul_writelog(UL_LOG_TRACE, "%s\t%s", words[i], words[j]);
                    #endif
                    db_syn_add(pdb, words[i], words[j]);
                    db_syn_add(pdb, words[j], words[i]);
                }
            }
            break;
        case 1:
            #ifdef _DEBUG_
            ul_writelog(UL_LOG_TRACE, "association nrwords: %d", nrwords);
            #endif
            for (i=1; i<nrwords; i++) {
                #ifdef _DEBUG_
                ul_writelog(UL_LOG_TRACE, "%s\t%s", words[0], words[i]);
                #endif
                db_syn_add(pdb, words[0], words[i]);
                db_syn_add(pdb, words[i], words[0]);
            }
            break;
        default:
            break;
        }
        nrwords = 0;
        pbuf = buf;
        pword = words[0];
    }
    ul_fclose(pfconf);
    return pdb;
}

Sdict_build_syn *db_syn_construct_unidirect( char* fullpath, int hash_num, int conf_type)
{
    Sdict_build_syn *pdb = NULL;
    char buf[4098];
    char words[1024][256];
    char *pbuf, *pword;
    int nrwords, i;
    FILE *pfconf;

    if (NULL == (pdb = db_syn_creat(hash_num))) {
        ul_writelog(UL_LOG_FATAL, "create synonym dict failed");
        return NULL;
    }   
    if (NULL == (pfconf = ul_fopen(fullpath, "r"))) {
        db_syn_del(pdb);
        ul_writelog(UL_LOG_FATAL, "open %s failed", fullpath);
    }

    pbuf = buf;
    nrwords = 0;
    pword = words[0];
    while (NULL != fgets(buf, 4098, pfconf)) {
        while (*pbuf != '\n' && *pbuf != '\0' && *pbuf != '\r') {
            if (*pbuf != WORDDELIMITER) {
                *pword++ = *pbuf++;
            } else {
                nrwords++;
                if (nrwords > 1024) {
                    ul_writelog(UL_LOG_WARNING, "number of synonyms > 1024");
                    break;
                }
                *pword = '\0';
                pword = words[nrwords];
                pbuf++;
            }
        }
        if (pbuf != buf && *--pbuf != WORDDELIMITER) {
            nrwords++;
            *pword = '\0';
        }
        switch (conf_type) {
        case 0:
            #ifdef _DEBUG_
            ul_writelog(UL_LOG_TRACE, "synonym nrwords: %d", nrwords);
            #endif
            for (i=0; i<nrwords - 1; i++) {
                #ifdef _DEBUG_
                ul_writelog(UL_LOG_TRACE, "%s\t%s", words[i], words[j]);
                #endif
                db_syn_add(pdb, words[i], words[i+1]);
            }
            break;
        case 1:
            #ifdef _DEBUG_
            ul_writelog(UL_LOG_TRACE, "association nrwords: %d", nrwords);
            #endif
            for (i=1; i<nrwords; i++) {
                #ifdef _DEBUG_
                ul_writelog(UL_LOG_TRACE, "%s\t%s", words[0], words[i]);
                #endif
                db_syn_add(pdb, words[0], words[i]);
            }
            break;
        default:
            break;
        }
        nrwords = 0;
        pbuf = buf;
        pword = words[0];
    }
    ul_fclose(pfconf);
    return pdb;
}

/*******************************************************
 * Function:    db_syn_del
 * Description:
 *  free the dictionary
 * Input:
 *      pdb: the search dictionary
 *******************************************************/

int db_syn_del( Sdict_build_syn *pdb )
{
    Sdict_syn_bnode_block   *ptmp_nblock, *ptt_nblock;
    Sdict_syn_buf_block *ptmp_nbuf, *ptt_nbuf;
                    
    // free the node blocks
    ptmp_nblock = pdb->nblock;
    while (ptmp_nblock != NULL) {
        ptt_nblock = ptmp_nblock->next;
        ul_free(ptmp_nblock);
        ptmp_nblock = ptt_nblock;
    }       
    
    // free the word dictionary
    db_del( pdb->pdword );
    
    // free the word buffer blocks
    ptmp_nbuf = pdb->bufblock;
    while (ptmp_nbuf != NULL) {
        ptt_nbuf = ptmp_nbuf->next;
        ul_free(ptmp_nbuf);
        ptmp_nbuf = ptt_nbuf;
    }       
    
    ul_free(pdb->hash);
    ul_free(pdb);

    return 1;
}

/***********************************************************
 * Function:    db_syn_adjust
 * Description: adjust a synonym relationship to optimize
 * Input:
 *  pdb: dictionary to manupilate
 * Output:
 *  <0: failed
 *  0: adjusted
 **********************************************************/

int db_syn_adjust( Sdict_build_syn *pdb )
{
        int i, times, hkey;
        int h_num;
        int n_num;

        Sdict_syn_bhash     *ph;
        Sdict_syn_bnode     *pn, *tp_node;
        Sdict_syn_bnode_block       *pnb;

        n_num = pdb->node_num;
        h_num = n_num;

        h_num /= 2;     // slight performance penalty occurred

    /* define new hash number */
        if ( (h_num % 2) == 0 ) {
                h_num++;
        }
        pdb->hash_num = h_num;

    /* resize the hash */
        ul_free(pdb->hash);
        if ((pdb->hash = (Sdict_syn_bhash * )ul_calloc(1, h_num*sizeof(Sdict_bhash))) == NULL)
                return -2;

        ph = pdb->hash;

    /* adjust the link of nodes */
        pnb = pdb->nblock;

        while (pnb != NULL) {
                pn = pnb->nodes;
                times = NODE_BLOCK_NUM;
                if (pnb->next == NULL) {
                        times -= pdb->node_avail;
                }

                for (i=0; i<times; i++) {
                        pn[i].next = NULL;
                        hkey = (pn[i].sign1+pn[i].sign2) % h_num;
                        if (ph[hkey].pnode == NULL) {
                                ph[hkey].pnode = pn + i;
                        } else {
                                tp_node = ph[hkey].pnode;
                                ph[hkey].pnode = pn + i;
                                pn[i].next = tp_node;
                        }
                }
                pnb = pnb->next;
        }
        return 0;
}


/***********************************************************
 * Function:    db_syn_add
 * Description: add a synonym relationship to the dictionary 
 * Input:
 *  pdb: dictionary to manupilate
 *  word1, word2: word2 is the synonym of word2
 * Output:
 *  -1: failed
 *  0: added
 *  1: the relationship is already there
 **********************************************************/

int db_syn_add( Sdict_build_syn *pdb, char *word1, char *word2 )
{
    u_int sign1, sign2;
    u_int sign3, sign4;

    int hkey;
    Sdict_syn_bnode *pt_node;

    creat_sign_f64(word1, strlen(word1), &sign1, &sign2);
    creat_sign_f64(word2, strlen(word2), &sign3, &sign4);

    db_syn_add_word(pdb, word1);
    db_syn_add_word(pdb, word2);

    /* get hash key */
    hkey = (sign1 + sign2) % pdb->hash_num;

    /* search this sign in dict */
    pt_node = pdb->hash[hkey].pnode;
    while (pt_node != NULL) {
        if ( (pt_node->sign1==sign1) &&
             (pt_node->sign2==sign2) &&
             (pt_node->sign3==sign3) &&
             (pt_node->sign4==sign4) ) {    // the synonym relationship is already there, return 1
            return 1;
        }
        pt_node = pt_node->next;
    }

    /* the relationship is not there, add a new node to the dictionary. */
    pdb->cur_node->sign1 = sign1;
    pdb->cur_node->sign2 = sign2;
    pdb->cur_node->sign3 = sign3;
    pdb->cur_node->sign4 = sign4;
    pdb->node_avail--;

            /* save it to dict */
        if (pdb->hash[hkey].pnode == NULL) {
            pdb->hash[hkey].pnode = pdb->cur_node;
        } else {
            pt_node = pdb->hash[hkey].pnode;
            pdb->hash[hkey].pnode = pdb->cur_node;
            pdb->cur_node->next = pt_node;
        }

            /* the all node block have been used */
        if (pdb->node_avail == 0) {
            if((pdb->cur_nblock->next = (Sdict_syn_bnode_block *)
                ul_calloc(1, sizeof(Sdict_syn_bnode_block))) == NULL)
                return -2;
            pdb->cur_nblock = pdb->cur_nblock->next;
            pdb->cur_node = pdb->cur_nblock->nodes;
            pdb->node_avail = NODE_BLOCK_NUM;
        } else {
            pdb->cur_node++;
        }
        pdb->node_num++;

    return 0;
}

/***********************************************************
 * Function:    db_syn_add_word
 * Description: add a word to the word dictionary in the
 *  synonym dictionary
 * Input:
 *  pdb: dictionary to manupilate
 *  word: word to add
 * Output:
 *  -1: failed
 *  0: added
 *  1: the word is already there
 **********************************************************/

int db_syn_add_word( Sdict_build_syn *pdb, char *word )
{
    Sdict_build *pworddict;
    u_int sign1, sign2;
    Sdict_snode pdwnode;
    int wordlen;

    pworddict = pdb->pdword;

    wordlen = strlen(word);
    creat_sign_f64(word, wordlen, &sign1, &sign2);
    pdwnode.sign1 = sign1;
    pdwnode.sign2 = sign2;
    if (1 == db_op1(pworddict, &pdwnode, SEEK)) {
        // the word is found, return
        return 1;
    } else {
        // the word not found, add it
        #ifdef _DEBUG_
        ul_writelog(UL_LOG_TRACE, "the word to add: %s", word);
        #endif
        if ( (pdb->cur_buf+wordlen+1) >= WORDBUFSIZE ) {
            // allocate new buffer block
            if ((pdb->cur_bufblock->next = (Sdict_syn_buf_block *)
                ul_calloc (1, sizeof(Sdict_syn_buf_block))) == NULL)
                return -2;
            pdb->cur_bufblock = pdb->cur_bufblock->next;
            pdb->nrbufblock++;
            pdb->cur_buf = 0;
        }
        // copy the word to the buffer
        pdwnode.other = (pdb->nrbufblock-1) * WORDBUFSIZE + pdb->cur_buf;
        pdwnode.code = wordlen;
        strncpy(pdb->cur_bufblock->buf + pdb->cur_buf, word, wordlen+1);
        pdb->cur_buf += wordlen+1;  
        
        // add the word to the word dictionary
        db_op1(pworddict, &pdwnode, ADD);
        pdb->words++;
    }
    return 0;
}

/***********************************************************
 * Function:    db_syn_add_snode
 * Description: add a relationship to the dictionary, used
 *  by db_syn_load only, make sure that the words are
 *  already in the word dictionary!
 * Input:
 *  pdb: dictionary to manupilate
 *  psnode: search node to add 
 * Output:
 *  -1: failed
 *  0: added
 *  1: already there
 **********************************************************/

int db_syn_add_snode( Sdict_build_syn *pdb, Sdict_syn_snode *psnode )
{
    int hkey;

    Sdict_syn_bnode *pt_node;

    /* get hash key */
    hkey = (psnode->sign1 + psnode->sign2) % pdb->hash_num;

    /* search this sign in dict */
    pt_node = pdb->hash[hkey].pnode;
    while (pt_node != NULL) {
        if ( (pt_node->sign1==psnode->sign1) &&
             (pt_node->sign2==psnode->sign2) &&
             (pt_node->sign3==psnode->sign3) &&
             (pt_node->sign4==psnode->sign4) ) {    // already there, return 1
            return 1;
        }
        pt_node = pt_node->next;
    }

    /* the relationship is not there, add a new node to the dictionary. */
    pdb->cur_node->sign1 = psnode->sign1;
    pdb->cur_node->sign2 = psnode->sign2;
    pdb->cur_node->sign3 = psnode->sign3;
    pdb->cur_node->sign4 = psnode->sign4;
    pdb->node_avail--;

        /* save it to dict */
        if (pdb->hash[hkey].pnode == NULL) {
            pdb->hash[hkey].pnode = pdb->cur_node;
        } else {
            pt_node = pdb->hash[hkey].pnode;
            pdb->hash[hkey].pnode = pdb->cur_node;
            pdb->cur_node->next = pt_node;
        }

        /* the all node block have been used */
        if (pdb->node_avail == 0) {
            if((pdb->cur_nblock->next = (Sdict_syn_bnode_block *)
                ul_calloc(1, sizeof(Sdict_syn_bnode_block))) == NULL)
                return -2;
            pdb->cur_nblock = pdb->cur_nblock->next;
            pdb->cur_node = pdb->cur_nblock->nodes;
            pdb->node_avail = NODE_BLOCK_NUM;
        } else {
            pdb->cur_node++;
        }
        pdb->node_num++;

    return 0;
}

/***********************************************************
 * Function:    db_syn_seek
 * Description: seek the synonyms of a word in a dictionary 
 * Input:
 *  pdb: dictionary to seek
 *  word: word to seek
 *  outbuf: where to put the found synonyms
 *  len: the size of outbuf
 * Output:
 *  the number of found synonyms
 **********************************************************/


int db_syn_seek( Sdict_build_syn *pdb, char *word, char *outbuf, int len )
{
    u_int sign1, sign2;
    u_int sign3, sign4;

    int hkey;
    int i;
    int offset, blocks, offinblock, wordlen, totallen = 0;
    Sdict_syn_buf_block *pbufblock;
    int nrsyn = 0;  // number of synonyms found

    Sdict_syn_bnode *pt_node;
    Sdict_snode pwnode;

    creat_sign_f64(word, strlen(word), &sign1, &sign2);

    /* get hash key */
    hkey = (sign1 + sign2) % pdb->hash_num;

    /* search this sign in dict */
    pt_node = pdb->hash[hkey].pnode;
    while (pt_node != NULL) {
        if ( (pt_node->sign1==sign1) &&
             (pt_node->sign2==sign2) ) {    // one of the synonyms found
            sign3 = pt_node->sign3;
            sign4 = pt_node->sign4;
            pwnode.sign1 = sign3;
            pwnode.sign2 = sign4;
            if (-1 == db_op1(pdb->pdword, &pwnode, SEEK))
                continue;
            offset = pwnode.other;
            #ifdef _DEBUG_
            ul_writelog(UL_LOG_TRACE, "offset in the word dictionary buffer: %d", offset);
            #endif
            wordlen = pwnode.code;
            if ((totallen + wordlen + 1) > len) {   // no room in outbuf any more
                break;
            } else {
                blocks = offset / WORDBUFSIZE;
                offinblock = offset - WORDBUFSIZE * blocks;
                pbufblock = pdb->bufblock;
                for (i=0; i<blocks; i++) {
                    if (pbufblock->next != NULL) {
                        pbufblock = pbufblock->next;
                    } else {
                        return -1;
                    }
                }
                strncpy(outbuf+totallen, pbufblock->buf+offinblock, wordlen+1);
                totallen += wordlen+1;
                nrsyn++;
            }
        }
        pt_node = pt_node->next;
    }
    return nrsyn;
}



/***********************************************************
 * Function:    db_syn_save
 * Description: save a dictionary
 * Input:
 *  pdb:  dictionary to save
 *  path: path to save the files
 *  fname: dictionary file name
 * Output:
 *  <0 error
 *  1  saved
 **********************************************************/

int db_syn_save( Sdict_build_syn *pdb, char *path, char *fname )
{
        int     fno;
        int     h_num, n_num;
        int     snode;
        char    tname[256];

        Sdict_syn_bhash     *pbh;
        Sdict_syn_bnode     *pnode;
    Sdict_syn_buf_block *pbuf;
        Sdict_syn_snode     ds[MID_NODE_NUM];

        int     i, j, times;

        u_int   *phi;   /* the hash of dict_searcher */
        u_int   *pnum;  /* number of nodes in hash entries */
        int     ioff, inum;

        char    fullpath[PATH_SIZE];

        h_num = pdb->hash_num;
        n_num = pdb->node_num;
        pbh   = pdb->hash;
        snode = sizeof(Sdict_syn_snode);

        if ((phi = (u_int *)ul_calloc(1, h_num * sizeof(int))) == NULL)
                return -1;
        if ((pnum = (u_int*) ul_calloc(1, h_num * sizeof(int))) == NULL) {
                ul_free(phi);
                return -1;
        }

    /* save the attribute of this dict */
        ul_writenum_init(path, fname);
        ul_writenum_oneint(path, fname, "hash_num", pdb->hash_num);
        ul_writenum_oneint(path, fname, "node_num", pdb->node_num);
        ul_writenum_oneint(path, fname, "word_buf", pdb->nrbufblock);
    ul_writenum_oneint(path, fname, "cur_buf", pdb->cur_buf);

    /* save the word dictionary */
    db_adjust(pdb->pdword);
        sprintf(tname, "%s.word", fname);
    db_save(pdb->pdword, path, tname);

    /* save the word buffer */
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ((fno = ul_open(fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
                ul_free(phi);
                ul_free(pnum);
                return -3;
        }
        fchmod(fno,0644);
    pbuf = pdb->bufblock;
    while (pbuf != NULL) {
        ul_write(fno, pbuf->buf, WORDBUFSIZE);
        pbuf = pbuf->next;
    }
        ul_close(fno);

    /* save the index 2 */
        sprintf(tname, "%s.ind2", fname);
        CmpsPath(fullpath,path,tname,PATH_SIZE);
        if ((fno = ul_open(fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
                ul_free(phi);
                ul_free(pnum);
                return -3;
        }
        fchmod(fno,0644);

        times = 0;
        j = 0;
        ioff = inum = 0;
        for (i=0; i<h_num; i++) {

                phi[i] = ioff;  // remove the limit
                //phi[i] = ioff<<7;

                pnode = pbh[i].pnode;

                while (pnode != NULL) {

                        ds[j].sign1 = pnode->sign1;
                        ds[j].sign2 = pnode->sign2;
                        ds[j].sign3 = pnode->sign3;
                        ds[j].sign4 = pnode->sign4;
                        j++;
                        pnum[i]++;
                        if ( pnum[i] >= 126) {
                                ul_writelog(UL_LOG_WARNING,
                                        "[db_save] too many items in one hash entry");
                        }

                        ioff++;

                        if (j >= MID_NODE_NUM) {
                                ul_write(fno, ds, MID_NODE_NUM*snode);
                                j = 0;
                                times++;
                        }
                        pnode = pnode->next;
                }
        }

        if (j != 0) {
                ul_write(fno, ds, j * snode);
        }

        ul_close(fno);

    /* save index1 */
        sprintf(tname, "%s.ind1", fname);
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ((fno = ul_open(fullpath, O_WRONLY|O_CREAT|O_TRUNC, 0644)) == -1) {
                ul_free(phi);
                ul_free(pnum);
                return -3;
        }
        fchmod(fno,0644);

        ul_write(fno, phi, h_num * sizeof(int));
        ul_write(fno, pnum, h_num * sizeof(int));
        ul_close(fno);

    /* check correction */
        if ( (times*MID_NODE_NUM+j != pdb->node_num)
          || (ioff != pdb->node_num) ){
                ul_writelog(UL_LOG_WARNING,"[db_syn_save] node_num error");
                ul_free(phi);
                ul_free(pnum);
                return -1;
        }

    /* free */
        ul_free(phi);
        ul_free(pnum);

        return 1;

}

/***********************************************************
 * Function:    db_syn_load
 * Description: load a dictionary for build
 * Input:
 *  path: path to find the file
 *  fname: dictionary file name
 * Output:
 *  the dictionary loaded
 **********************************************************/

Sdict_build_syn *db_syn_load( char *path, char *fname, int hash_n)
{
        int on_num, i;

        int h_num, n_num, n_buf, c_buf;
        int snode, rnum, rsize, full_size;
        int fno;
        int cur_h;

        char tname[256];

        Sdict_syn_snode sn[MID_NODE_NUM];
        Sdict_build_syn *pdb;

        char    fullpath[PATH_SIZE];

    /* init vars*/
        snode = sizeof(Sdict_syn_snode);

    /* read num from file */
        if (hash_n <= 0) {
                ul_readnum_oneint(path, fname, "hash_num", &h_num);
        } else {
        h_num = hash_n;
    }
        ul_readnum_oneint(path, fname, "node_num", &on_num);
        ul_readnum_oneint(path, fname, "word_buf", &n_buf);
    ul_readnum_oneint(path, fname, "cur_buf", &c_buf);

    /* creat dict */
        pdb = db_syn_creat(h_num);

    pdb->hash_num = h_num;
    pdb->nrbufblock = n_buf;
    pdb->cur_buf = c_buf;

        sprintf(tname, "%s.word", fname);
    pdb->pdword = db_load(path, tname, 0);
    pdb->words = pdb->pdword->node_num;

    // read the word buffers
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ((fno = ul_open(fullpath, O_RDONLY, 0644)) == -1)
                return NULL;
        ul_read(fno, pdb->bufblock->buf, WORDBUFSIZE);
    n_buf--;
        while (n_buf > 0) {
        if ((pdb->cur_bufblock->next = (Sdict_syn_buf_block *)
            ul_calloc (1, sizeof(Sdict_syn_buf_block))) == NULL)
            return NULL;
        pdb->cur_bufblock = pdb->cur_bufblock->next;
            ul_read(fno, pdb->cur_bufblock->buf, WORDBUFSIZE);
        n_buf--;
    }
        ul_close(fno);

    /* read items from file */
        sprintf(tname, "%s.ind2", fname);
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ((fno = ul_open(fullpath, O_RDONLY, 0644)) == -1)
                return NULL;

        n_num = 0;
        cur_h = 0;
        full_size = MID_NODE_NUM * snode;
        while ( (rsize = ul_read(fno, sn, full_size)) >0 ) {
                rnum = rsize / snode;

                for (i=0; i<rnum; i++) {
                        db_syn_add_snode(pdb, sn+i);
                }
                n_num += rnum;
        }
        ul_close(fno);

        if (n_num != on_num) {
                ul_writelog(UL_LOG_WARNING, "[db_syn_load] node_num error");
        }

        return pdb;

}

/***********************************************************
 * Function:    ds_syn_load
 * Description: load a dictionary for search
 * Input:
 *  path: path to find the file
 *  fname: dictionary file name
 * Output:
 *  the dictionary loaded
 **********************************************************/

Sdict_search_syn  *ds_syn_load( char *path, char *fname )
{
        int snode;
        int h_num;
        int n_num;
        int b_num;
        int fno;

        char    tname[256];

        Sdict_search_syn    *pds;

        char fullpath[PATH_SIZE];

    /* init vars */
        snode = sizeof(Sdict_snode);

    /* read num from file */
        ul_readnum_oneint(path, fname, "hash_num", &h_num);
        ul_readnum_oneint(path, fname, "node_num", &n_num);
        ul_readnum_oneint(path, fname, "word_buf", &b_num);

        if ((pds = (Sdict_search_syn *) ul_calloc(1, sizeof(Sdict_search_syn))) == NULL)
                return NULL;

        pds->hash_num = h_num;
        pds->node_num = n_num;

        if ((pds->hash = (u_int* )ul_malloc(h_num * sizeof(int))) == NULL) {
                ul_free(pds);
                return NULL;
        }
        if ((pds->num = (u_int* )ul_malloc(h_num * sizeof(int))) == NULL) {
                ul_free(pds->hash);
                ul_free(pds);
                return NULL;
        }

        if ((pds->node = (Sdict_syn_snode *)ul_malloc(n_num * sizeof(Sdict_syn_snode))) == NULL) {
                ul_free(pds->hash);
                ul_free(pds->num);
                ul_free(pds);
                return NULL;
        }

    /* read data from file */
        sprintf(tname, "%s.ind1", fname);
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ( (fno = ul_open(fullpath,O_RDONLY, 0644)) == -1 ) {
                ul_free(pds->node);
                ul_free(pds->hash);
                ul_free(pds->num);
                ul_free(pds);
                return NULL;
        }
        ul_read(fno, pds->hash, h_num * sizeof(int));
        ul_read(fno, pds->num, h_num * sizeof(int));    // read the number of nodes
                                                        // in hash entries
        ul_close(fno);

        sprintf(tname, "%s.ind2", fname);
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ( (fno = ul_open(fullpath, O_RDONLY, 0644)) == -1 ) {
                ul_free(pds->node);
                ul_free(pds->hash);
                ul_free(pds->num);
                ul_free(pds);
                return NULL;
        }
        ul_read(fno, pds->node, n_num * sizeof(Sdict_snode));
        ul_close(fno);

    /* load the word dictionary */
        sprintf(tname, "%s.word", fname);
    pds->pdword = ds_load(path, tname);
    if (pds->pdword == NULL) {
                ul_free(pds->node);
                ul_free(pds->hash);
                ul_free(pds->num);
                ul_free(pds);
                return NULL;
        }

    /* read the word buffer */
        CmpsPath(fullpath, path, tname, PATH_SIZE);
        if ( (fno = ul_open(fullpath, O_RDONLY, 0644)) == -1 ) {
                ul_free(pds->node);
                ul_free(pds->hash);
                ul_free(pds->num);
                ul_free(pds);
                return NULL;
        }
        if ((pds->wordbuffer = (char *)ul_malloc(b_num * WORDBUFSIZE)) == NULL) {
                ul_free(pds->node);
                ul_free(pds->hash);
                ul_free(pds->num);
                ul_free(pds);
        ds_del(pds->pdword);
                return NULL;
    }
    ul_read(fno, pds->wordbuffer, b_num * WORDBUFSIZE);
        ul_close(fno);
        return pds;
}


/***********************************************************
 * Function:    ds_syn_seek
 * Description: seek the synonyms of a word in a dictionary 
 * Input:
 *  pds: dictionary to seek
 *  word: word to seek
 *  outbuf: where to put the found synonyms
 *  len: the size of outbuf
 * Output:
 *  the number of found synonyms
 **********************************************************/

int ds_syn_seek( Sdict_search_syn *pds, char *word, char *outbuf, int len )
{
    u_int sign1, sign2;
    u_int sign3, sign4;

    int hkey;
    int ioff, inum, i;
    Sdict_syn_snode *pind2;
    int offset, wordlen, totallen = 0;
    int nrsyn = 0;  // number of synonyms found

    Sdict_snode pwnode;

    creat_sign_f64(word, strlen(word), &sign1, &sign2);

    /* get hash key */
    hkey = (sign1 + sign2) % pds->hash_num;

    ioff = pds->hash[hkey]; 
    inum = pds->num[hkey];

    /* searching in the index 2*/
    pind2 = pds->node + ioff;

    for (i=0; i<inum; i++) {
        if ((pind2[i].sign1 == sign1) &&
            (pind2[i].sign2 == sign2) ) {   // one found
            sign3 = pind2[i].sign3;
            sign4 = pind2[i].sign4;
            pwnode.sign1 = sign3;
            pwnode.sign2 = sign4;
            if (-1 == ds_op1(pds->pdword, &pwnode, SEEK))
                continue;
            offset = pwnode.other;
            #ifdef _DEBUG_
            ul_writelog(UL_LOG_TRACE, "offset in the word dictionary buffer: %d", offset);
            #endif
            wordlen = pwnode.code;
            if ((totallen + wordlen + 1) > len) {   // no room in outbuf any more
                break;
            } else {
                strncpy(outbuf+totallen, pds->wordbuffer+offset, wordlen+1);
                totallen += wordlen+1;
                nrsyn++;
            }
        }
    }
    return nrsyn;
}

/*******************************************************
 * Function:    ds_syn_del
 * Description:
 *  free the dictionary
 * Input:
 *      pds: the search dictionary
 *******************************************************/

int ds_syn_del( Sdict_search_syn *pds )
{
    /* free the hash & node & word buffer */
        ul_free(pds->hash);
        ul_free(pds->node);
    ul_free(pds->wordbuffer);

    /* free the dict */
        ul_free(pds);
        return 0;
}


/*******************************************************
 * Function:    ds_filter
 * Description:
 *      build a new dictionary from a search dictionary
 *      using nodes with maximum freq (not strictly)
 * Input:
 *      pds: the search dictionary
 *      number: node number to filter
 * Output:
 *      new built build dictionary
 *******************************************************/

Sdict_build *ds_filter(Sdict_search *pds, int number) 
{
        Sdict_build *pdb = NULL; 
        u_int maxother = 0;
        u_int minother = 0;
        u_int limit = 0;
        int interval;
        int *ostat; 
        int i, count, target, tmp;

        if (number > pds->node_num)
                return NULL;

        if ((ostat = (int *)ul_calloc(1, number*sizeof(int))) == NULL) { 
                ul_writelog(UL_LOG_FATAL, "ds_filter(): ul_calloc ostat failed");
                return NULL;
        }       

        // find the maximum of 'other' field
        for (i=0; i<pds->node_num; i++) {
                if (Dfreq(pds->node[i].other) > maxother)
                        maxother = Dfreq(pds->node[i].other);
        }       
        ul_writelog(UL_LOG_TRACE, "maximum 'freq' field: %d", maxother);

        // do statistics work
        interval = maxother / number + 1;
        for (i=0; i<pds->node_num; i++)
                ostat[Dfreq(pds->node[i].other)/interval]++;

        // find where is the limit to get enough nodes
        count = 0;
        for (i=number-1; i>=0; i--) {
                count += ostat[i];
                if (count >= number) {
                        limit = i;
                        break;
                }
        }

        // reuse the ostat array to record the offsets 
        count = 0;
        for (i=0; i<pds->node_num; i++) {
                if (Dfreq(pds->node[i].other)/interval >= limit) {
                        ostat[count] = i;
                        count++;
                        if (count >= number)
                                break;
                }
        }

        // scramble the order of the nodes
        for (i=0; i<number; i++) {
                target = random() % number;
                if (i != target) {      // swap them
                        tmp = ostat[target];
                        ostat[target] = ostat[i];
                        ostat[i] = tmp;
                }
        }

        // create a new dictionary
        pdb = db_creat(number+1, 0);
    if (pdb == NULL) { 
        ul_writelog(UL_LOG_FATAL, "db_creat failed");
            ul_free(ostat);
            return NULL;
    }
        minother = maxother;
        maxother = 0;
        for (i=0; i<number; i++) {
                if (Dfreq(pds->node[ostat[i]].other) < minother)
                        minother = Dfreq(pds->node[ostat[i]].other);
                if (Dfreq(pds->node[ostat[i]].other) > maxother)
                        maxother = Dfreq(pds->node[ostat[i]].other);
        pds->node[ostat[i]].code = -1;
                pds->node[ostat[i]].other = 0;
                db_op1(pdb, &pds->node[ostat[i]], ADD);
//              if (db_op1(pdb, &pds->node[ostat[i]], SEEK) == 0) {
//                      db_op1(pdb, &pds->node[ostat[i]], ADD);
//              } else {
//                      ul_writelog(UL_LOG_WARNING, "nodes with same sign found");
//              }
        }
        ul_writelog(UL_LOG_TRACE, "minimum 'freq' field in new dict: %d", minother);
        ul_writelog(UL_LOG_TRACE, "maximum 'freq' field in new dict: %d", maxother);

    ul_free(ostat);
        return pdb;
}

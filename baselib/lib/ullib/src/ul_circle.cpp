#include "ul_circle.h"

//***************************************************
struct circleB_t *pstCBinit( int npmBufSize )
{
	struct circleB_t *pstWkCB;

	if( npmBufSize < 2 )	/* size is too little */
		return((struct circleB_t *)0);

	pstWkCB=(struct circleB_t *)ul_malloc(sizeof(struct circleB_t));

	if(pstWkCB==(struct circleB_t *)0)	/* molloc system call error */
		return((struct circleB_t *)0);

	pstWkCB->sBufLen = npmBufSize + 1;

	pstWkCB->sTail=0;
	pstWkCB->sHead=0;

	pstWkCB->pbyBuf=(void *)ul_malloc( pstWkCB->sBufLen * sizeof(u_char) );
	if(pstWkCB->pbyBuf==(void *)0)/* molloc system call error */
	{
		ul_free(pstWkCB);
		return((struct circleB_t *)0);
	}
		

	return(pstWkCB);
}
//****************************************
void vCBfree( struct circleB_t * pstCB )
{

	if( pstCB == (struct circleB_t *)0 )
		return;

	if( pstCB->pbyBuf != (void *) 0 )
		ul_free(pstCB->pbyBuf);
	ul_free(pstCB );
}

//****************************************
int nCBput( struct circleB_t *pstCB, void *pbyPut, int nSize )
{
	if (pbyPut == NULL || nSize <= 0) {
		return DEF_ERR_INVALID_PARAM;
	}
	int sTmpLen;
#ifdef UL_COMMENT
	int sSavStartPos;
#endif

	if(pstCB==(struct circleB_t *)0)
		return(DEF_ERR_NOTINIT);

	/* Buff is not small, Can't put this data */
	if ( ( pstCB->sHead - 1 - pstCB->sTail + pstCB->sBufLen ) % pstCB->sBufLen < nSize )
	{
		return(DEF_ERR_CQFULL);	/* error */
	}
#ifdef UL_COMMENT
	sSavStartPos = pstCB->sTail;
#endif

	if( pstCB->sTail < pstCB->sHead ||
		( sTmpLen = pstCB->sBufLen - pstCB->sTail ) >= nSize ) 
	{
		memcpy( (u_char *)pstCB->pbyBuf + pstCB->sTail, pbyPut, (size_t)nSize );
		pstCB->sTail = ( pstCB->sTail + nSize ) % pstCB->sBufLen; 
	}
	else
	{
		memcpy( (u_char *)pstCB->pbyBuf + pstCB->sTail, pbyPut, (size_t)sTmpLen );
		memcpy( (u_char *)pstCB->pbyBuf, (u_char *)pbyPut + sTmpLen, (size_t)(nSize - sTmpLen) );
		pstCB->sTail = nSize - sTmpLen;
	}

#ifdef UL_COMMENT
	return( sSavStartPos );	
#endif
	return( 0 );	
} 

//*****************************************************
int nCBget( struct circleB_t *pstCB, void *pbyGet, int nSize ) 
{ 
	if (pbyGet == NULL || nSize <= 0) {
		return DEF_ERR_INVALID_PARAM;
	}

	int sTmpLen;

	if(pstCB==(struct circleB_t *)0)
		return(DEF_ERR_NOTINIT);

	/* queue has no enough data */ 
	if ( ( pstCB->sTail - pstCB->sHead + pstCB->sBufLen ) % pstCB->sBufLen < nSize )	
	{
		return(DEF_ERR_CQEMPTY); 	/* error */
	}

	if( pstCB->sTail > pstCB->sHead ||
		( sTmpLen = pstCB->sBufLen - pstCB->sHead ) >= nSize )
	{
		memcpy( (u_char *)pbyGet, (u_char *)pstCB->pbyBuf + pstCB->sHead, (size_t)nSize );
	
		pstCB->sHead = ( pstCB->sHead + nSize ) % pstCB->sBufLen; 
	}
	else
	{
		memcpy( pbyGet, (u_char *)pstCB->pbyBuf + pstCB->sHead, (size_t)sTmpLen );
		memcpy( (u_char *)pbyGet + sTmpLen, pstCB->pbyBuf, (size_t)(nSize - sTmpLen) );
		pstCB->sHead = nSize - sTmpLen; 
	}

	return(0);
}

//*****************************************************
int nCBlookup( struct circleB_t *pstCB, void *pbyGet, int nSize ) 
{
	if (pbyGet == NULL || nSize <= 0) {
		return DEF_ERR_INVALID_PARAM;
	}

	int sTmpLen = 0;
	int sSaveHead = 0;

	if(pstCB == NULL)
		return(DEF_ERR_NOTINIT);

	/* queue has no enough data */ 
	if ( ( pstCB->sTail - pstCB->sHead + pstCB->sBufLen ) % pstCB->sBufLen < nSize )	
	{
		return(DEF_ERR_CQEMPTY); 	/* error */
	}

	sSaveHead = pstCB->sHead;

	if( pstCB->sTail > pstCB->sHead ||
		( sTmpLen = pstCB->sBufLen - pstCB->sHead ) >= nSize )
	{
		memcpy( (u_char *)pbyGet, (u_char *)pstCB->pbyBuf + pstCB->sHead, (size_t)nSize );
	
		pstCB->sHead = ( pstCB->sHead + nSize ) % pstCB->sBufLen; 
	}
	else
	{
		memcpy( pbyGet, (u_char *)pstCB->pbyBuf + pstCB->sHead, (size_t)sTmpLen );
		memcpy( (u_char *)pbyGet + sTmpLen, pstCB->pbyBuf, (size_t)(nSize - sTmpLen) );
		pstCB->sHead = nSize - sTmpLen; 
	}

	pstCB->sHead = sSaveHead;

	return(0);
}

//*****************************************************
int nCBgetdatalen( struct circleB_t *pstCB )
{
    if(pstCB==(struct circleB_t *)0)
        return(DEF_ERR_NOTINIT);

    if ( pstCB->sTail == pstCB->sHead )
        return 0 ;
    return((pstCB->sTail-pstCB->sHead+pstCB->sBufLen)%pstCB->sBufLen);
}

//*****************************************************
int nCBgetbufreelen( struct circleB_t *pstCB )
{
	int freelen;
	if(pstCB==(struct circleB_t *)0)
		return(DEF_ERR_NOTINIT);

	//这个10没什么意义，就是防止一些错误操作导致的数据破坏
	if((freelen=pstCB->sBufLen - nCBgetdatalen( pstCB ) - 10) < 0 )
		freelen=0;
	return(freelen);
}


#include "stdafx.h"
#include "ObjectPool.h"

IMPLEMENT_SINGLETON(CBufPoolManager)

CBufPoolV::CBufPoolV(const char* pName, int nCategory)
{
    DLINK_INITIALIZE(&link);
    strncpy(m_name, pName, sizeof(m_name));
    m_nCategory = nCategory;
    m_nUnitSize = 0;
    g_pBufPoolManager->Register(this);
}

CBufPoolV::~CBufPoolV()
{
    g_pBufPoolManager->Deregister(this);
}

void CBufPoolManager::Register(CBufPoolV* pPool)
{
    m_List.push_back(pPool);
}

void CBufPoolManager::Deregister(CBufPoolV* pPool)
{
    m_List.remove(pPool);
}

struct PrintCtx {
    uint32 nCategoryMask;
    uint64 nAlloc;
    uint64 nFree;
    uint64 nAlloc_other;
    uint64 nFree_other;
};

bool CBufPoolManager::_DoPrint(CBufPoolV* pPool, ptr p)
{
    PrintCtx* pCtx = (PrintCtx*)p;
    if (pPool->m_nCategory & pCtx->nCategoryMask) {
        pPool->Print();
    }
    else {
        pCtx->nAlloc_other += (uint64)pPool->GetAllocCount() * pPool->m_nUnitSize;
        pCtx->nFree_other += (uint64)pPool->GetFreeCount() * pPool->m_nUnitSize;
    }
    pCtx->nAlloc += (uint64)pPool->GetAllocCount() * pPool->m_nUnitSize;
    pCtx->nFree += (uint64)pPool->GetFreeCount() * pPool->m_nUnitSize;
    return true;
}

void CBufPoolManager::Print(uint32 nCategoryMask)
{
    PrintCtx ctx;
    ZeroMemory(&ctx, sizeof(ctx));
    ctx.nCategoryMask = nCategoryMask;
    m_List.Enumerate(_DoPrint, &ctx);

    char buf1[64], buf2[64];
    if (nCategoryMask != (uint32)-1) {
        PrintSize(ctx.nFree_other, buf1, sizeof(buf1));
        PrintSize(ctx.nAlloc_other, buf2, sizeof(buf2));
        
        TRACE0("%16s: Free=%sB, Alloc=%lld KB (%sB)\n", "Others", buf1, ctx.nAlloc_other/1024, buf2);
    }

    PrintSize(ctx.nFree, buf1, sizeof(buf1));
    PrintSize(ctx.nAlloc, buf2, sizeof(buf2));
    TRACE0("%16s: Free=%sB, Alloc=%lld KB (%sB)\n", "Total", buf1, ctx.nAlloc/1024, buf2);
}

bool CBufPoolManager::_DoGarbageCollect(CBufPoolV* pPool, ptr p)
{
    pPool->GarbageCollect();
    return true;
}

void CBufPoolManager::GarbageCollect()
{
    m_List.Enumerate(_DoGarbageCollect, NULL);
}


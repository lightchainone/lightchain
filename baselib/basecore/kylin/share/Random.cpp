#include "stdafx.h"
#include "Random.h"

IMPLEMENT_SINGLETON_WITHOUT_CTOR(CRandom)

CRandom::CRandom()
{
	m_fSeed = 0;
}
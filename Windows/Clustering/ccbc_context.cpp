
#include "stdafx.h"

#include "ccbc_context.h"
#include <fstream>


ccbc_context::ccbc_context(uint32_t p_NextIdx, TCluster & p_Cluster, const vector<uint32_t> & p_IdxList, double p_Threshold)
	: m_NextIdx(p_NextIdx),
		m_Cluster(p_Cluster),
		m_Threshold(p_Threshold),
		m_IdxList(p_IdxList)
{
}


ccbc_context::~ccbc_context(void)
{
}


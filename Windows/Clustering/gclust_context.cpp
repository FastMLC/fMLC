
#include "stdafx.h"

#include "gclust_context.h"

gclust_context::gclust_context(uint32_t p_NextIdx, uint32_t p_FirstGroupIdx, TCluster & p_Cluster, vector<uint32_t> & p_IdxList, double p_Threshold)
	: m_NextIdx(p_NextIdx),
		m_FirstGroupIdx(p_FirstGroupIdx),
		m_Cluster(p_Cluster),
		m_Threshold(p_Threshold),
		m_IdxList(p_IdxList), 
		m_WaitHdl(0)
{
	//	for each Group, create a mutex that will protect access to each ClusterGroup.AddComparison(). note that you cannot resize a non-copyable object : m_MutexList.resize() is not allowed
	for (uint32_t g = 0, gmax = static_cast<uint32_t>(m_Cluster.GroupNo() - m_FirstGroupIdx); g < gmax; ++g) {
		m_MutexList.push_back(new std::mutex());
	}
	return;
}

gclust_context::~gclust_context(void)
{
	for(std::mutex * ptr : m_MutexList) {
		delete ptr;
	}
	m_MutexList.clear();
}

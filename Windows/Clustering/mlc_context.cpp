
#include "stdafx.h"

#include "mlc_context.h"
#include <algorithm>

mlc_context1::mlc_context1(TCluster & p_Cluster, const vector<uint32_t> & p_IdxList, double p_Threshold, uint32_t p_NumThread)
	: m_Cluster(p_Cluster), m_Threshold(p_Threshold), m_NextIdx(0), m_NumThread(p_NumThread), m_IdxList(p_IdxList)
{
	m_CentroidList.reserve(std::max(2000u, static_cast<uint32_t>(m_Cluster.Sequences().size() / 4)));	//	supposing there will be 4 sequences in each group
	return;
}


mlc_context1::~mlc_context1(void)
{
}


mlc_context2::mlc_context2(TCluster & p_Cluster, vector<uint32_t> & p_IdxList, double p_Threshold, uint32_t p_NumThread)
	:	m_Cluster(p_Cluster), m_Threshold(p_Threshold), m_NextIdx(0), m_NumThread(p_NumThread), m_IdxList(p_IdxList)
{
	//	for each Group, create a mutex that will protect access to each ClusterGroup.AddComparison(). note that you cannot resize a non-copyable object : m_MutexList.resize() is not allowed
	for (uint32_t g = 0, gmax = static_cast<uint32_t>(m_Cluster.GroupNo()); g < gmax; ++g) {
		m_MutexList.push_back(new std::mutex());
	}

	m_ScrapList.reserve(m_Cluster.Sequences().size() / 10);	//	max 10 % of scrap should be enough
	return;
}


mlc_context2::~mlc_context2(void)
{
	for (std::mutex * ptr : m_MutexList) {
		delete ptr;
	}
	m_MutexList.clear();
}



f_context::f_context(const TCluster & p_Cluster, const TCluster & p_RefCluster, uint32_t p_NumThread)
	: m_Cluster(p_Cluster), m_RefCluster(p_RefCluster), m_F(0.0), m_ItemNo(0), m_NextIdx(0), m_NumThread(p_NumThread)
{
}


f_context::~f_context()
{
}

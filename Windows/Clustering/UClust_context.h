
#pragma once

#include <mutex>
#include "Cluster.h"


class uclust_context
{
	public:
		uclust_context(uint32_t p_SMin, uint32_t p_GMin, TCluster & p_Cluster, vector<uint32_t> & m_IdxList, double p_Threshold);
		~uclust_context(void) ;

		//	multithreading indexes and mutex
		std::mutex	m_IdxMutex ;					//	lock access 

		//	members set from ctor parameters
		uint32_t		m_NextIdx;						//	the next sequence index to process
		uint32_t		m_FirstGroupIdx;				//	the index of the first group to consider

		TCluster &				m_Cluster;			//	the cluster to compute
		double					m_Threshold;

		vector<uint32_t> &	m_IdxList;			//	the index of the sequences to process. WARNING : the list will be destroyed during the computation !

		vector<std::mutex *> m_MutexList;		//	a map of mutexes that will protect access to each cluster.m_Groups item. The key in the map is the index of the group counted from g_min

		int	m_WaitHdl;								//	a handle to the task managing the progress bar

private :
	explicit uclust_context(const uclust_context & srce) = delete;
	uclust_context & operator=(const uclust_context & ) = delete ;	//	avoid implicit assignement operator
};


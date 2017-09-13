
#pragma once

#include <mutex>
#include "Cluster.h"

#ifdef BOOST
#include <boost/container/vector.hpp>
#else
#include <vector>
using namespace std;
#endif

class ccbc_context
{
public:
	ccbc_context(uint32_t p_SMin, TCluster & p_Cluster, const vector<uint32_t> & m_IdxList, double p_Threshold);
	~ccbc_context(void);

	//	multithreading indexes and mutex
	std::mutex				m_IdxMutex;			//	lock access 

	//	members set from ctor parameters
	uint32_t					m_NextIdx;			//	the next sequence index to process

	TCluster &				m_Cluster;			//	the cluster to compute
	double					m_Threshold;

	const vector<uint32_t> &	m_IdxList;			//	the index of the sequences to process

	//	computed in OperatorCcbc1 : nothing, it is just calling ref->InitRef() for each sequence

	//	computed by OperatorCcbc2 :
	double_matrix			m_Sims;				//	the similarities

	//	computed by the main thread :
	vector<uint32_t>		m_GroupIndexes;	//	use a vector to keep in mind the group of each sequence

private:
	explicit ccbc_context(const ccbc_context & srce) = delete;
	ccbc_context & operator=(const ccbc_context &) = delete;	//	avoid implicit assignement operator
};




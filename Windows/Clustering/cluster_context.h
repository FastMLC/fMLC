
#pragma once

#include <mutex>
#include "Cluster.h"

class cluster_context
{
	public:
		cluster_context(TCluster * p_Cluster, const vector<double> & p_Thresholds, AlgorithmEnum p_Algo);
		~cluster_context(void) ;

		//	members set from ctor parameters
		TCluster *			m_Cluster;			//	the cluster to compute
		vector<double>		m_Thresholds;
		AlgorithmEnum		m_Algo;

		int	m_WaitHdl;								//	a handle to the task managing the progress bar

private :
	explicit cluster_context(const cluster_context & srce) = delete;
	cluster_context & operator=(const cluster_context & ) = delete ;	//	avoid implicit assignement operator
};


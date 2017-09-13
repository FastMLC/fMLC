
#include "stdafx.h"

#include "cluster_context.h"

cluster_context::cluster_context(TCluster * p_Cluster, const vector<double> & p_Thresholds, AlgorithmEnum p_Algo)
	: m_Cluster(p_Cluster), m_Thresholds(p_Thresholds), m_Algo(p_Algo)
{
}


cluster_context::~cluster_context(void)
{
}

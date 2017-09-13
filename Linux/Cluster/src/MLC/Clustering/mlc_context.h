
#pragma once

#include <mutex>
#include "cluster.h"


//	the context of the first MLC step
class mlc_context1
{
	public:
		mlc_context1(TCluster & p_Cluster, const vector<uint32_t> & p_IdxList, double p_Threshold, uint32_t p_NumThread);
		~mlc_context1(void) ;

		//	multithreading indexes and mutex
		std::mutex	m_IdxMutex;							//	lock access to the index
		std::mutex	m_CentroidMutex;					//	lock access to the list of centroids

		TCluster &	m_Cluster;							//	the cluster to compute
		double		m_Threshold;						//	the current threshold used in UClust

		uint32_t		m_NextIdx;							//	the index of the first item in m_IdxList to process
		uint32_t		m_NumThread;						//	the number of threads

		const vector<uint32_t> &	m_IdxList;		//	the index of the sequences to process, must not be sorted
		vector<uint32_t> m_CentroidList;				//	the list of all sequences considered as centroids, to be completed
                vector<TCluster> m_Groups;

private :
	explicit mlc_context1(const mlc_context1 & srce) = delete;
	mlc_context1 & operator=(const mlc_context1 & ) = delete ;	//	avoid implicit assignement operator
};


//	the context of the second MLC step
class mlc_context2
{
public:
	mlc_context2(TCluster & p_Cluster, vector<uint32_t> & p_IdxList, double p_Threshold, uint32_t p_NumThread);
	~mlc_context2(void);

	//	multithreading indexes and mutex
	std::mutex	m_IdxMutex;					//	lock access to the index
	std::mutex	m_ScrapMutex;				//	lock access to the list of unclassified sequences

	TCluster &	m_Cluster;					//	the cluster to compute
	double		m_Threshold;				//	the number of threads

	uint32_t		m_NextIdx;					//	the index of the next sequence to process
	uint32_t		m_NumThread;				//	the number of threads

	vector<uint32_t> &	m_IdxList;		//	the index of the sequences to process
	vector<uint32_t> m_ScrapList;			//	the unclassified sequences

	vector<std::mutex *> m_MutexList;	//	a map of mutexes that will protect access to each cluster group. The key in the map is the index of the group

private:
	explicit mlc_context2(const mlc_context2 & srce) = delete;
	mlc_context2 & operator=(const mlc_context2 &) = delete;	//	avoid implicit assignement operator
};


//	teh context of the F-Measure computation
class f_context
{
public:
	f_context(const TCluster & p_Cluster, const TCluster & p_RefCluster, uint32_t p_NumThread);
	~f_context(void);

	//	multithreading indexes and mutex
	std::mutex	m_IdxMutex;				//	lock access to the index
	std::mutex	m_ResultMutex;			//	lock access to the global m_F value

	const TCluster &	m_Cluster;			//	the cluster to compute
	const TCluster &	m_RefCluster;		//	the reference cluster
	double		m_F;							//	the F-Measure
	double		m_ItemNo;					//	the number of items in the reference cluster

	uint32_t		m_NextIdx;				//	the index of the next sequence to process
	uint32_t		m_NumThread;			//	the number of threads

private:
	explicit f_context(const f_context & srce) = delete;
	f_context & operator=(const f_context &) = delete;	//	avoid implicit assignement operator
};


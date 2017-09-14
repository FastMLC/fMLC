
//#include "stdafx.h"

#include "clusterdb.h"
#include "clustering.h"
#include "nfieldbase.h"
#include "../MfcCluster/testfieldbase.h"

#include "double_matrix.h"
#include <algorithm>
#include <fstream>



ClusterDB::ClusterDB(void)
	: m_CacheLen(0)
{
}


void
ClusterDB::AllocateCache() const
{
#ifdef CACHING
	m_SimMutex.lock();
	if (m_CacheLen == 0) {
		//m_CacheLen = std::min(static_cast<uint32_t>(m_Sequences.size()), 54000u);	//	at most 30000� double => 7.2 GB, or 54000� double => 24 GB
		//m_CacheLen = std::min(static_cast<uint32_t>(m_Sequences.size()), 44000u);		//	44000� double => 16 GB
		m_CacheLen = std::min(static_cast<uint32_t>(m_Sequences.size()), 32000u);		//	32000� double => 8 GB
		m_Sims.resize(m_CacheLen, m_CacheLen, -1.0);		//	the cache for all similarity computations
	}
	m_SimMutex.unlock();
#endif
}


ClusterDB::~ClusterDB()
{
	for(TNFieldBase * ptr : m_Sequences) {
		delete ptr ;
		ptr = nullptr;
	}
	m_Sequences.clear();
}


int32_t
ClusterDB::LoadFastaFile(const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos)
{
	m_Sequences.clear();
	int32_t errorCode = clustering::LoadFastaFile(m_Sequences, p_FilePath, p_IsDna, p_NamePos);	//	allocation of the sequence, dont forget to delete it when finished
	AllocateCache();
	return errorCode;
}

double
ClusterDB::Compare( const TNFieldBase * srce, const TNFieldBase * ref) const
{
	uint32_t validNo = 0;		//	the number of valid comparisons
	uint32_t compNo = 0;			//	the number of comparisons

#ifdef CACHING
	uint32_t s = srce->RecordId();
	uint32_t r = ref->RecordId();
	if (s < m_CacheLen && r < m_CacheLen) {
		if (m_Sims[s][r] >= 0.0) {
			return m_Sims[s][r];
		}
	}
#endif
	//double sim = 0.5; // ComputeSimilarity(srce, ref, m_MinOverlap);
    double sim = ComputeSimilarity(srce, ref, m_MinOverlap);
#ifdef CACHING
	if (s < m_CacheLen && r < m_CacheLen) {	//	too far away ?
		m_SimMutex.lock();
		m_Sims[s][r] = sim;
		m_SimMutex.unlock();
	}
#endif
	////save sim into file
	//uint32_t error=clustering::SaveSimilarity(m_SimFilePath, srce, ref, sim);
	return sim;
}


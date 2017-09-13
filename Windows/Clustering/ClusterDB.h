
#pragma once

#include "NFieldBase.h"
#include <double_matrix.h>
#include <mutex>

#ifdef BOOST
#include <boost/container/vector.hpp>
#else
#include <vector>
using namespace std;
#endif

#define CACHING

//	a cluster is made of one reference sequence and a list of sequence IDs
class ClusterDB
{
public:
	ClusterDB(void);
	void AllocateCache() const;
	virtual ~ClusterDB(void);
	int32_t LoadFastaFile(const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos);
	int32_t m_MinOverlap = 100; //for optimizing the similarity score between two sequences
	int32_t m_MinSeqNoForMLC = 100; // for MLC: a minimum number of sequences for MLC. If the number of the sequence is smaller than this, a single clustering algorithm is used

	double Compare(const TNFieldBase * srce, const TNFieldBase * ref) const;
	//	in most case the sequences are not stored in the same direction, so we need to compare in forward and reverse-complement directions
	//	if all sequence stored are in the same direction, return false here
	static bool	ReverseComplement() { return true; }	

	//	the reference sequences and the cluster groups
	vector<TNFieldBase *> m_Sequences;			//	the list of all sequences used by this cluster. Shared will all sub-clusters. Allocated in LoadFastaFile(), deleted in the cluster dtor.
	mutable double_matrix	m_Sims;			//	the cache for all similarity computations
	mutable uint32_t			m_CacheLen;		//	the number of rows and columns in m_Sims
	mutable std::mutex		m_SimMutex;		//	lock access 
};




#pragma once

#include "nfieldbase.h"
#include "double_matrix.h"
#include <mutex>
/*
enum CompAlgoEnum {
    SW = 0,					//		Compare 2 sequences using Smith-Waterman algorithm
    BLAST = 1,					//	 	Compare 2 sequences using BLAST function
};
*/
#ifdef BOOST
#include <boost/container/vector.hpp>
#else
#include <vector>
using namespace std;
#endif


//#define CACHING Undefined caching

//	a cluster is made of one reference sequence and a list of sequence IDs
class ClusterDB
{
public:
	ClusterDB(void);
	void AllocateCache() const;
	virtual ~ClusterDB(void);
	int32_t LoadFastaFile(const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos);
        
	int32_t m_MinOverlap = 100; //for optimizing the similarity score between two sequences
	int32_t m_MinSeqNoForMLC = 100; // for MLC: a number of sequences that can be clustered quickly at a single level
//        CompAlgoEnum CompAlgo;

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



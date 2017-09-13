
#pragma once

#include <double_vector.h>

//	this is a container for two sequence IDs and the result of the comparison
//	the key always stored the smaller sequence ID first
class TComparison
{
public:
	TComparison();
	TComparison(uint32_t p_SrceIdx, uint32_t p_RefIdx, double p_Similarity);
	virtual ~TComparison(void);

	//	compare the value, useful to sort the TComparison by m_Value
	bool operator <(const TComparison & srce) const;
	bool operator <=(const TComparison & srce) const;
	bool operator >(const TComparison & srce) const;
	bool operator >=(const TComparison & srce) const;
	bool operator ==(const TComparison & srce) const;
	bool operator !=(const TComparison & srce) const;

	uint32_t		SrceIdx() const					{		return m_SrceIdx;				}
	uint32_t		RefIdx() const						{		return m_RefIdx;				}
	double		Sim() const							{ 		return m_Similarity;			}
	void			SetSim(double p_Sim) 			{ 		m_Similarity = p_Sim;		}
	void			SetRefIdx(uint32_t p_RefIdx)	{		m_RefIdx = p_RefIdx;			}

	void	SwapSrceAndRef();

	struct OnSrceIdxPr
	{
		bool
			operator()(const TComparison & a, const TComparison & b) const
		{
			return (a.m_SrceIdx < b.m_SrceIdx);	//	sort using the source index
		}
	};

	struct OnSimilarityPr
	{
		bool
			operator()(const TComparison & a, const TComparison & b) const
		{
			return (a.m_Similarity < b.m_Similarity);	//	sort using the source index
		}
	};


private :
	uint32_t m_SrceIdx;			//	the source sequence index
	uint32_t m_RefIdx;			//	the reference sequence index, usualy a Centroid sequence in a ClusterGroup
	double m_Similarity;			//	the similarity between the srce and the ref sequences

public:										//	the cluster relative coordinates, the reference being the parent cluster. Computed from Mds()
	double_vector m_Pos;			//	a three dimensional vector

};


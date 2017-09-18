
//#include "stdafx.h"

#include "cluster.h"
#include "clustering.h"
#include "clusterproperties.h"


#include "comparison.h"
#include "nfieldbase.h"
#include "eigenval.h"
#include "toolbox.h"
#include "toolbox_tl.h"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/filesystem.hpp>
#include "boost_plugin.h"
#include "double_matrix.h"
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <boost/algorithm/string.hpp>
//#include <mmsystem.h>	//	for timeGetTime(), needs library Winmm.lib

//	multi-threaded operations

#include "ccbc_context.h"
#include "gclust_context.h"
#include "mlc_context.h"

#include <sstream>
#include "wait.h"
#include <string>

#include <thread>

#include <sys/stat.h>
#include <sys/types.h>

TCluster::TCluster(const ClusterDB * p_ClusterDB)
	: TCluster(p_ClusterDB, UINT32_MAX)
{
}


//TCluster::TCluster(const ClusterDB * p_ClusterDB, uint32_t p_CentralSeqIdx)
//	: m_GroupsPerTour(50), m_SeqPerThread(30), m_SeqPerBloc(35), m_CentralSeqIdx(p_CentralSeqIdx), m_MinDist(0.0), m_MaxDist(0.0), m_DistFromParent(cNaN),
//	m_IsSortedGroups(false), m_ClusterDB(p_ClusterDB)
//{
//	m_NumThread = std::max(1u, std::thread::hardware_concurrency());	//	return 0 in case of an error
//	m_Pos.resize(3, 0.0);
//}

TCluster::TCluster(const ClusterDB * p_ClusterDB, uint32_t p_CentralSeqIdx)
	: m_GroupsPerTour(50), m_SeqPerThread(30), m_BlocNo(10), m_CentralSeqIdx(p_CentralSeqIdx), m_MinDist(0.0), m_MaxDist(0.0), m_DistFromParent(cNaN),
	m_IsSortedGroups(false), m_ClusterDB(p_ClusterDB)
{
    m_NumThread = std::max(1u, std::thread::hardware_concurrency());	
 //   m_NumThread =1;
    m_Pos.resize(3, 0.0);
}


TCluster::~TCluster()
{
}


size_t
TCluster::GroupNo() const	//	return the number of groups on level 0, not counting the following levels
{
	return m_Groups.size();
}


size_t
TCluster::FinalGroupNo() const	//	return the number of final groups : if a group contains group, call recursively, otherwise return 1
{
	if (m_Groups.size() > 0) { // we contain subgroups, call recursively our subgroups
		size_t len = 0;
		for (const TCluster & group : m_Groups) {
			len += group.FinalGroupNo();
		}
		return len;
	}
	return 1;
}

void
TCluster::Flatten()
{
	std::vector<TCluster> p_Clusters;
	p_Clusters.reserve(1000);

	FinalClusters(p_Clusters);
	m_Groups.clear();
	for (const TCluster & group : p_Clusters) {
		m_Groups.push_back(group);
	}
	BuildSortedIdList();
}

void
TCluster::FinalClusters(std::vector<TCluster> & p_Clusters) const //	return the final groups : if a group contains group, call recursively, otherwise return 1
{
	if (m_Groups.size() > 0) { // we contain subgroups, call recursively our subgroups
		for (const TCluster & group : m_Groups) {
			group.FinalClusters(p_Clusters);
		}
	}
	else {
		p_Clusters.push_back(TCluster(*this));
	}
}

//	see BuildSortedIdList() for explanation
//	this function is recursive in all child clusters
void
TCluster::BuildAllSortedIdList()
{
	BuildSortedIdList();

	// call recursively our child clusters
	for (TCluster & group : m_Groups) {
		group.BuildSortedIdList();
	}
}


//	build a list with our sequence indexes:
//	- the centroid is added to the list if it has been commputed (!= UINT32_MAX)
//	- if we contain child clusters, the list contains the centroids of each child cluster
//	- if we contain comparisons, the list contains all sequences found in the comparisons
//	The list is sorted and contains no duplicates
//	Function is recursive: child clusters are also called
void
TCluster::BuildSortedIdList()
{
	m_IdList.clear();

	if (m_CentralSeqIdx != UINT32_MAX) {
		m_IdList.emplace_back(m_CentralSeqIdx);
	}

	if(m_Groups.size() > 0 ) { // call recursively our child clusters
		for (TCluster & group : m_Groups) {
			if (group.m_CentralSeqIdx != UINT32_MAX) {
				m_IdList.emplace_back(group.m_CentralSeqIdx);
			}
		}
	}
	else {	//	use our comparisons
		m_IdList.reserve(1 + m_Comparisons.size());

		for (const auto & comp : m_Comparisons) {
			if (comp.SrceIdx() != m_CentralSeqIdx) {
				m_IdList.emplace_back(comp.SrceIdx());
			}
			if (comp.RefIdx() != m_CentralSeqIdx) {
				m_IdList.emplace_back(comp.RefIdx());
			}
		}
	}
	RemoveDuplicatesAndSort(m_IdList);
}

void
TCluster::GetIdList(vector<uint32_t> & p_IdxList)
{
	if (m_Groups.size() > 0) { // call recursively our child clusters
		for (TCluster & group : m_Groups) {
			if (group.m_CentralSeqIdx != UINT32_MAX) {
				p_IdxList.push_back(group.m_CentralSeqIdx);
				group.GetIdList(p_IdxList);
			}
		}
	}
	else {	//	use our comparisons
		for (const auto & comp : m_Comparisons) {
			if (comp.SrceIdx() != m_CentralSeqIdx) {
				p_IdxList.push_back(comp.SrceIdx());
			}
			if (comp.RefIdx() != m_CentralSeqIdx) {
				p_IdxList.push_back(comp.RefIdx());
			}
		}
	}
	RemoveDuplicatesAndSort(p_IdxList);
}

uint32_t TCluster::GetReferenceClusterIndex(std::string p_RecordName, uint32_t p_FieldNamePos) const
{
	size_t imax = m_ClusterDB->m_Sequences.size();
	if (imax == 0) {
		return 0;
	}
	uint32_t i = 0;
	bool found = false;
	do {
		found = (clustering::StrainName(m_ClusterDB->m_Sequences[i]->RecordName(),p_FieldNamePos) == clustering::StrainName(p_RecordName, p_FieldNamePos));
			i++;
	} while (!found);
	return i;
}

//	Save into a UTF-8 file
void
TCluster::Save(std::ostream & p_Stream, uint32_t p_FieldNamePos, uint32_t p_TabNo, uint32_t p_MaxTabNo, std::string  p_Extension) const
{
	
	if (m_Groups.size() > 0) {
		//recursively
		for (TCluster group : m_Groups) {
			TNFieldBase * seq = ClusterDatabase()->m_Sequences[group.CentralSeqIdx()];
			group.Save(p_Stream, p_FieldNamePos, p_TabNo+1,p_MaxTabNo, p_Extension + "Central id: " + std::to_string(seq->RecordId()) + "\t" );
		}
	}
	else {
		//	extend the record name with the centralidx of the clusters that the sequence belongs to.
		TNFieldBase * ref = ClusterDatabase()->m_Sequences[CentralSeqIdx()];
		for (uint32_t i = 0; i < p_MaxTabNo - p_TabNo; ++i) {
			p_Extension = p_Extension + "Central id: " + std::to_string(ref->RecordId()) + "\t" ;
		}
		p_Stream << ref->RecordId();
		p_Stream << "\t";
		std::string refrecordname = m_ClusterDB->m_Sequences[CentralSeqIdx()]->RecordName();
		p_Stream << refrecordname;
		p_Stream << "\t";
		uint32_t refclusterindex = GetReferenceClusterIndex(refrecordname,p_FieldNamePos);
		p_Stream << refclusterindex;
		p_Stream << "\t";
		p_Stream << p_Extension;
		p_Stream << "\r\n";
		if (m_Comparisons.size() > 0) {
			for (const TComparison & comp : m_Comparisons) {
				uint32_t s = comp.SrceIdx();
				TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
				p_Stream << srce->RecordId();
				p_Stream << "\t";
				std::string recordname = m_ClusterDB->m_Sequences[s]->RecordName();
				p_Stream << recordname;
				p_Stream << "\t";
				uint32_t clusterindex = 0;
				if (refrecordname == recordname) {
					clusterindex = refclusterindex;
				}
				else {
					clusterindex = GetReferenceClusterIndex(recordname, p_FieldNamePos);
				}
				p_Stream << refclusterindex;
				p_Stream << "\t";
				p_Stream << p_Extension;
				p_Stream << "\r\n";		
			}
		}
	}



	
}

void
TCluster::Save(std::wostringstream & p_Stream, uint32_t p_TabNo) const
{
	if(p_TabNo > 8) {
		p_TabNo = 8;
	}
	static const wchar_t tabs_array[][18] = {L"\r\n", L"\r\n\t", L"\r\n\t\t", L"\r\n\t\t\t", L"\r\n\t\t\t\t", L"\r\n\t\t\t\t\t", L"\r\n\t\t\t\t\t\t", L"\r\n\t\t\t\t\t\t\t", L"\r\n\t\t\t\t\t\t\t\t" };
	const wchar_t * begin_line = tabs_array[p_TabNo];

	//	save the central sequence
	if(m_CentralSeqIdx == UINT32_MAX) {
		p_Stream << begin_line << L"No centroid computed";
	}
	else {
		TNFieldBase * srce = m_ClusterDB->m_Sequences[m_CentralSeqIdx];
		p_Stream << begin_line << "Centroid[" << m_CentralSeqIdx << "].Id = " << srce->RecordId();
		p_Stream << L"\t";
		p_Stream << m_ClusterDB->m_Sequences[m_CentralSeqIdx]->RecordNameW();
	}
	p_Stream << L"\tRadius = " << std::to_wstring(m_MaxDist);

	if(m_Groups.size() > 0) {
		p_Stream << L"\tThere is " << m_Groups.size() << L" groups";
		for (const TCluster & group : m_Groups) {
			group.Save(p_Stream, p_TabNo + 1);
		}
	}
	else {
		//	save all sequence IDs of that group
		if (m_Comparisons.size() > 0) {
			for (const TComparison & comp : m_Comparisons) {
				p_Stream << begin_line << L"\t";
				uint32_t s = comp.SrceIdx();
				TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
				p_Stream << L"Seq[" << s << L"].Id = " << srce->RecordId();
				p_Stream << L"\t";

				uint32_t r = comp.RefIdx();
				TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
				p_Stream << L"Seq[" << r << L"].Id = " << ref->RecordId();
				p_Stream << L"\t";

				p_Stream << L"\t";
				p_Stream << m_ClusterDB->m_Sequences[s]->RecordNameW();
			}
		}
	}
}


//	Save into a UTF-8 file
bool
TCluster::SaveAsText(const wchar_t * p_DestFilePath, uint32_t p_FieldNamePos, uint32_t p_MaxTabNo ) const
{
	//	Save results into a cluster file
    
    char mbstr[256];
    std::wcstombs(mbstr, p_DestFilePath, 256);
    std::ofstream file(mbstr, std::ofstream::binary);
    if (file.fail()) {
	return true; //	error
    }
    file << "Sequence id" << "\t" << "Sequence name" << "\t"  << "Reference cluster index" << "\t" ;
    for (uint32_t i = 0; i < p_MaxTabNo ; ++i) {
	file << "Level" << " " << i + 1 << "\t";
    }
    file << "\r\n";
    for (const TCluster & group : m_Groups) {
	group.Save(file, p_FieldNamePos, 0, p_MaxTabNo,"");
    }
    bool isError = (file.goodbit != 0);
    file.close();
    return isError;
}

//	seems to fails if stream size > 16 kB
void
TCluster::OutputDebug()
{
	if (!m_IsSortedGroups) {
		SortAllGroups();
	}
	std::wostringstream stream;
	Save(stream, 0);
	//OutputDebugString(stream.str().c_str());
}


//	the recursive function
void
TCluster::SortAllGroups()
{
	SortGroups();

	for (TCluster & group : m_Groups) {
		group.SortGroups();
	}
}


//	this is a slow function, only called from debugging code, or from F_Measure() and F_Measure_Mt()
void
TCluster::SortGroups()
{
	for (TCluster & group : m_Groups) {
		group.BuildSortedIdList();						//	we need the child IdLIst to compare quickly the comparisons
		group.SortComparisonsBySrceIndex();
	}
	std::sort(m_Groups.begin(), m_Groups.end(), TCluster::OnGroupCentroidPr());	//	sort child clusters using the smallest record ID in m_IdList

	m_IsSortedGroups = true;
}


void
TCluster::SortComparisonsBySrceIndex()
{
	std::sort(m_Comparisons.begin(), m_Comparisons.end(), TComparison::OnSrceIdxPr());
}

void
TCluster::SortComparisonsBySimilarity()
{
	std::sort(m_Comparisons.begin(), m_Comparisons.end(), TComparison::OnSimilarityPr());
}


//	compute our centroids but also in all our child groups
void
TCluster::ComputeAllCentroids()
{
	ComputeCentroid();

	for (TCluster & group : m_Groups) {
		group.ComputeAllCentroids();
	}
}


//	compute the best representative of all sequences, using the already computed Comparisons
void
TCluster::ComputeCentroid()
{
	BuildSortedIdList();

	uint32_t oldCentralSeqIdx = m_CentralSeqIdx;
	//	these conditions must fit the case where len == 0 or len == 1
	m_CentralSeqIdx = UINT32_MAX;
	m_MinDist = 0.0;	//	radius of this group is 0.0 if no or only one sequence
	m_MaxDist = 0.0;

	size_t len = m_IdList.size();
	if (len == 0) {	//	should never happen
		return;
	}
	m_CentralSeqIdx = m_IdList.front();
        return;
	if (len > 2) {	//	most common case first
		size_t denom = len - 1;	//	as we compare each sequence with all the others, there is (len-1) comparisons

										//	store the summ of the similarities for each source sequence
		vector<double> simAvg(len, 0.0);

		//	compare everything with everything (n * n / 2 - n comparisons)
		for (size_t i = 0, imax = len - 1; i < imax; ++i) {
			uint32_t s = m_IdList[i];
			TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
			if (!srce->SrceReady()) {
				srce->InitSrce(ReverseComplement());
			}
			for (size_t j = i + 1, jmax = len; j < jmax; ++j) {
				uint32_t r = m_IdList[j];
				TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
				if (!ref->RefReady()) {
					ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is heavier than InitSrce()
				}
				double sim = m_ClusterDB->Compare(srce, ref);
				simAvg[i] += sim / denom;
				simAvg[j] += sim / denom;
			}
		}
		//	search the source sequence with the maximum similarity with all others
		double maxSimAvg = 0.0;
		for (size_t i = 0, imax = len; i < imax; ++i) {
			if (simAvg[i] > maxSimAvg) {
				m_CentralSeqIdx = m_IdList[i];
				maxSimAvg = simAvg[i];
			}
		}
		//	compute the max similarity
		m_MinDist = 1.0;
		m_MaxDist = 0.0;
		TNFieldBase * srce = m_ClusterDB->m_Sequences[m_CentralSeqIdx];
		if (!srce->SrceReady()) {
			srce->InitSrce(ReverseComplement());
		}
		for (size_t i = 0, imax = len; i < imax; ++i) {
			uint32_t r = m_IdList[i];
			if (r == m_CentralSeqIdx) {	//	Sim = 1.0
				continue;
			}
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is heavier than InitSrce()
			}
			double dist = 1.0 - m_ClusterDB->Compare(srce, ref);
			m_MinDist = std::min(m_MinDist, dist);
			m_MaxDist = std::max(m_MaxDist, dist);
			if(m_MinDist == 0.0 && m_MaxDist == 1.0) {	//	range cannot be wider
				break;
			}
		}
	}
	else if (len == 2) {	//	if containing only two sequences, each sequence is as close to the second one than the second one is close to the first one. The centroid is the longer sequence
		int s = m_IdList[0];
		TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
		if (!srce->SrceReady()) {
			srce->InitSrce(ReverseComplement());
		}
		int r = m_IdList[1];
		TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
		if (!ref->RefReady()) {
			ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is heavier than InitSrce()
		}
		if(srce->SequenceSize() > ref->SequenceSize()) {
			m_CentralSeqIdx = s;
		}
		else {
			m_CentralSeqIdx = r;
		}
		m_MinDist = m_MaxDist = 1.0 - m_ClusterDB->Compare(srce, ref);
	}

	//	if we replace the centroid from 15 to 20, the comparison where sim  = Compare(20, 15) must swap the srce and the reference, otherwise sequence 15 disapears (not in the comparisons srce nor a centroid)
	if (oldCentralSeqIdx != m_CentralSeqIdx) {
		for(auto & comp : m_Comparisons) {
			if(comp.SrceIdx() == m_CentralSeqIdx) {
				comp.SwapSrceAndRef();
				break;
			}
		}
		//	to be able to display it properly, if the reference is not the centroid any more, we must recompute the similarity betwween all members and the centroid
		TNFieldBase * ref = m_ClusterDB->m_Sequences[m_CentralSeqIdx];
		if (!ref->RefReady()) {
			ref->InitRef(true);
		}
		for (auto & comp : m_Comparisons) {
			if (comp.RefIdx() != m_CentralSeqIdx) {
				comp.SetRefIdx(m_CentralSeqIdx);
				TNFieldBase * srce = m_ClusterDB->m_Sequences[comp.SrceIdx()];
				if (!srce->SrceReady()) {
					srce->InitSrce(ReverseComplement());
				}
				comp.SetSim(Compare(srce, ref));	//	just read the already computed value
			}
		}
	}
}


//	called from OperatorMlc1
void
TCluster::PrepareCentroid() const
{
	//	the centroid will be used as a source in OperatorMlc2(), so we have to prepare the reverse complement, if any
	if (ReverseComplement()) {
		TNFieldBase * srce = m_ClusterDB->m_Sequences[m_CentralSeqIdx];
		if (!srce->SrceReady()) {
			srce->InitSrce(ReverseComplement());
		}
	}
}



//	compute the distance between my own centroid and all child groups, using their own centroid
void
TCluster::ComputeChildDistances()
{
	//	compute the distance between our centroid and the child centroids
	for (TCluster & group : m_Groups) {
		if (m_CentralSeqIdx != UINT32_MAX) {
			TNFieldBase * srce = m_ClusterDB->m_Sequences[m_CentralSeqIdx];
			if (!srce->SrceReady()) {
				srce->InitSrce(ReverseComplement());
			}
			TNFieldBase * ref = m_ClusterDB->m_Sequences[group.m_CentralSeqIdx];
			if (!ref->RefReady()) {
				ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is heavier than InitSrce()
			}
			group.m_DistFromParent = 1.0 - m_ClusterDB->Compare(srce, ref);
		}
		else {
			group.m_DistFromParent = cNaN;	//	we are the root
		}
	}
        
}


//	return the F-Measure between this clusters and the given reference cluster
//	this is a n� algorithm, but we use it only during testing
//	WARNING: this function is valid only for One-level clustering
double
TCluster::F_Measure(TCluster & p_RefCluster)
{
	double f = 0.0;			//	the F-Measure representing the quality of p_Groups compared to the reference groups p_RefGroups
	size_t itemNo = 0;		//	number of items in the reference cluster

	if(p_RefCluster.m_ClusterDB->m_Sequences.size() == 0) {
		return f ;	//	return 0.0
	}
	if(! m_IsSortedGroups) {
		SortAllGroups();
	}
	if (! p_RefCluster.m_IsSortedGroups) {
		p_RefCluster.SortAllGroups();
	}

	//	for each group in the reference cluster...
	for (const TCluster & refGroup : p_RefCluster.m_Groups) {
		const vector<uint32_t > & refIdList = refGroup.IdList();
		//	search for the group in the given cluster that seems to be the corresponding one
		double x = 0.0;
		for (const TCluster & group : m_Groups) {
			const vector<uint32_t > & srceIdList = group.IdList();
			size_t num = CountIntersection(srceIdList, refIdList);
			if (num == 0) {	//	most common case first
				continue;
			}
			size_t denom = srceIdList.size() + refIdList.size();
			x = std::max(x, 2.0 * num / denom);
			if(x == 1.0) {	//	cannot be better
				break;
			}
		}
		f += refIdList.size() * x ;
		itemNo += refIdList.size();
	}
	f /= itemNo;
	return std::min(1.0, f); //	protect against rounding errors
}


//	*****	Properties	*****

const ClusterDB *
TCluster::ClusterDatabase() const
{
	return m_ClusterDB;
}

const vector<TNFieldBase *> &
TCluster::Sequences() const
{
	return m_ClusterDB->m_Sequences;
}


size_t
TCluster::SequenceNo() const	//	return the number of sequences in the source list
{
	return Sequences().size();
}


//	return the number of sequences stored in groups, subgroups, subsubgroups, ... and comparisons
//	this function is very usefull to detect bug in algorithms, as the number of classified sequences should be the same as the number of sequences returned by SequenceNo()
size_t
TCluster::ClassifiedSequenceNo() const	//	return the number of sequences inclusing all subgroups
{
	size_t len = 0;
	//	add all subgroups sequences
	if (m_Groups.size() > 0) {
		//	don't save the centroid here, as it will be found again in a subgroup, either as a centroid, or as a comparison
		for (const auto & group : m_Groups) {
			len += group.ClassifiedSequenceNo();
		}
	}
	else {
		//	save the centroid, if not the root cluster, which has no centroid
		if (m_CentralSeqIdx != UINT32_MAX) {
			len += 1;
		}
		//	add all comparisons with the centroid
		for(const auto & comp : m_Comparisons) {
			if(comp.SrceIdx() != m_CentralSeqIdx) {
				++len;
			}
		}
	}
	return len;
}


uint32_t
TCluster::CentralSeqIdx() const
{
	return m_CentralSeqIdx;
}


const vector<uint32_t> &
TCluster::IdList() const
{
	return m_IdList;
}


const std::string &
TCluster::CentralName() const
{
	return m_ClusterDB->m_Sequences[m_CentralSeqIdx]->RecordName();
}


const vector<TComparison> &
TCluster::Comparisons() const
{
	return m_Comparisons;
}


vector<TComparison> &
TCluster::Comparisons()
{
	return m_Comparisons;
}

const vector<TCluster> &
TCluster::Groups() const
{
	return m_Groups;
}


vector<TCluster> &
TCluster::Groups()
{
	return m_Groups;
}


bool
TCluster::ReverseComplement() const
{
	return m_ClusterDB->ReverseComplement();
}

//	choose randomly p_GroupNo sequences in the given vector and return their IDs in refSeqIds
void GroupRandom(vector<int32_t> & p_RefSeqIds, vector<TNFieldBase *> & p_Sequences, int32_t p_GroupNo)
{
	p_RefSeqIds.reserve(p_GroupNo) ;
	for(int32_t i = 0; i < p_GroupNo; ++i)
	{
		int32_t idx = random(static_cast<int32_t>(p_Sequences.size()));
		p_RefSeqIds.push_back(idx);
	}
}


//	dispatch all sequences in groups according to their strain name
void
TCluster::ClusterByName(uint32_t p_FieldNamePosition)
{
	m_Groups.clear() ;
	m_IsSortedGroups = false;

	size_t imax = m_ClusterDB->m_Sequences.size();
	if(imax == 0) {
		return;
	}
	vector<uint32_t> idxList;
	idxList.reserve(imax);
	for (uint32_t i = 0; i < imax; ++i) {
		idxList.push_back(i);
	}

	//	sort the index list by decreasing size of the sequences
	clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	std::sort(idxList.begin(), idxList.end(), SeqSort);

	std::map<std::string, uint32_t> nameMap;	//	map the record name to the group index in m_Groups

	//	create the first group
	uint32_t r = idxList.front();
	const TNFieldBase & field0 = *(m_ClusterDB->m_Sequences[r]);
	m_Groups.emplace_back(m_ClusterDB, r);
	
	const std::string recordname = clustering::StrainName(field0.RecordName(), p_FieldNamePosition);
	nameMap[recordname] = 0;

	//	dispatch all sequences by strain or species name
	for(uint32_t i = 1; i < imax; ++i) {
		r = idxList[i];
		const TNFieldBase & field = *m_ClusterDB->m_Sequences[r];
		TCluster & lastGroup = m_Groups.back();

		if (clustering::StrainName(lastGroup.CentralName(),p_FieldNamePosition) == clustering::StrainName(field.RecordName(), p_FieldNamePosition)) { //	same name as the last group, very fast in case the database is sorted by name
			lastGroup.Comparisons().emplace_back(r, lastGroup.CentralSeqIdx(), NaN());
		}
		else if(nameMap.find(clustering::StrainName(field.RecordName(),p_FieldNamePosition)) != nameMap.end()) {
			auto & group = m_Groups[nameMap[clustering::StrainName(field.RecordName(),p_FieldNamePosition)]];
			group.Comparisons().emplace_back(r, group.CentralSeqIdx(), NaN());
		}
		else { //	new species name: add a new group
			nameMap[clustering::StrainName(field.RecordName(),p_FieldNamePosition)] = static_cast<uint32_t>(m_Groups.size());
			m_Groups.emplace_back(m_ClusterDB, r);
		}
	}
}


void
TCluster::GClust_St(double p_Threshold)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	GClust_St(p_Threshold, 0, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);
}


void
TCluster::GClust_St(double p_Threshold, uint32_t p_First, uint32_t p_Last)
{
	if(m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	p_First = std::max(0u, p_First);
	p_Last = std::min(p_Last, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);

	vector<uint32_t> idxList;
	idxList.reserve(p_Last - p_First + 1);
	for (uint32_t i = p_First; i <= p_Last; ++i) {
		idxList.push_back(i);
	}

	//	sort the index list by decreasing size of the sequences
	clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	std::sort(idxList.begin(), idxList.end(), SeqSort);
        GClust_SingleThread_Blast(p_Threshold, idxList);   
}


void
TCluster::GClust_St(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	vector<uint32_t> idxList(p_IdxList);

	//	sort the index list by decreasing size of the sequences
	clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	std::sort(idxList.begin(), idxList.end(), SeqSort);
        GClust_SingleThread_Blast(p_Threshold, idxList);
}


/*
Algorithm:
- take the first sequence and put it in the first group
- take all following sequences and put them in a group if the similarity is >= given threshold
*/
//	p_IdxSortedList must be sorted by sequence size, descending
void
TCluster::GClust_SingleThread_Blast(double p_Threshold, vector<uint32_t> & p_IdxSortedList)
{
	m_Groups.clear() ;
	m_IsSortedGroups = false;

	if(m_ClusterDB->m_Sequences.size() == 0 || p_IdxSortedList.size() == 0) {
		return;
	}

	//	put the first sequence in the first cluster group
	m_Groups.emplace_back(m_ClusterDB, p_IdxSortedList.front());
       
	//	create reference file        
        TNFieldBase * ref = m_ClusterDB->m_Sequences[p_IdxSortedList.front()];
        std::ofstream reffile;
        //std::string reffilename = std::to_string((int) p_Id) + "_" + std::to_string((int) ref-> RecordId()) + "_ref";
        std::string reffilename =  std::to_string((int) ref-> RecordId()) + "_ref";
        reffile.open(reffilename);
        reffile<< ">" <<  0 << "\r\n";
        reffile <<  ref-> Sequence2() << "\n";
        reffile.close();
       // std::remove("log");
        //std::remove("log.perf");
        //std::string makeblastdbcommand = "makeblastdb -in " + reffilename  + " -dbtype nucl -out " + reffilename + "db" + " -logfile log";
         //make blast db
       // system(makeblastdbcommand.c_str());  
                        
	for (vector<uint32_t>::const_iterator it = p_IdxSortedList.begin() + 1; it < p_IdxSortedList.end(); ++it) {
		uint32_t s = *it;
		TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
		if(! srce->RefReady()) {
			srce->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is lighter than InitSrce()
		}
		bool groupFound = false;
                //put r in the source list   
                double_matrix sims = OperatorBlast_GC(s,m_Groups.size(),reffilename);
                
		//	search in which group we will put it
		for (uint32_t g = 0, gmax = static_cast<uint32_t>(m_Groups.size()); g < gmax; ++g) {
			TCluster & group = m_Groups[g];
			uint32_t r = group.CentralSeqIdx();
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (! ref->SrceReady()) {
				ref->InitSrce(ReverseComplement());
			}
			double sim = sims[0][g];
			//double sim = Compare(srce, ref);
			if (sim >= p_Threshold) { //	put it in this group
				//group.AddComparison(TComparison(static_cast<int32_t>(r), s, sim)) ;
				group.Comparisons().emplace_back(r, s, sim);
				groupFound = true;
				break;
			}
		}
		if (! groupFound) { //	create a new group to hold this new sequences
			m_Groups.emplace_back(m_ClusterDB, s);
                        //put s into the reference file
                        reffile.open(reffilename, std::ios_base::app);
                        reffile<< ">" <<  m_Groups.size()-1 << "\r\n";
                        reffile <<  srce-> Sequence2() << "\n";
                        reffile.close();
                         //make blast db
  //                      std::remove("log");
  //                      std::remove("log.perf");
//                         system(makeblastdbcommand.c_str());  
                        
		}
	}
        //remove reffile
        std::remove(reffilename.c_str());
      //  std::remove((reffilename+"db.nin").c_str());
       // std::remove((reffilename+"db.nsq").c_str());
       // std::remove((reffilename+"db.nhr").c_str());
}

void
TCluster::GClust_SingleThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList)
{
	m_Groups.clear() ;
	m_IsSortedGroups = false;

	if(m_ClusterDB->m_Sequences.size() == 0 || p_IdxSortedList.size() == 0) {
		return;
	}

	//	put the first sequence in the first cluster group
	m_Groups.emplace_back(m_ClusterDB, p_IdxSortedList.front());
	//	scan the sequences only once
	for (vector<uint32_t>::const_iterator it = p_IdxSortedList.begin() + 1; it < p_IdxSortedList.end(); ++it) {
		uint32_t r = *it;
		TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
		//if (!ref->SrceReady()) {
		//	ref->InitSrce(m_Options.ReverseComplement()); //	we consider this one as a reference sequence
		//}
		if(! ref->RefReady()) {
			ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is lighter than InitSrce()
		}
		//if (!ref->SrceReady()) {
		//	ref->InitSrce(ReverseComplement()); 
		//}
		bool groupFound = false;
		//	search in which group we will put it
		for (uint32_t g = 0, gmax = static_cast<uint32_t>(m_Groups.size()); g < gmax; ++g) {
                        //cout << gmax <<"\n";
			TCluster & group = m_Groups[g];
			uint32_t s = group.CentralSeqIdx();
			TNFieldBase * srce = m_ClusterDB->m_Sequences[s];

			if (! srce->SrceReady()) {
				srce->InitSrce(ReverseComplement());
			}
			//
			// (!srce->RefReady()) { //this is for reserved comparison
			//	srce->InitRef(true); 
			//}
			double sim = Compare(srce, ref);
			if (sim >= p_Threshold) { //	put it in this group
				//group.AddComparison(TComparison(static_cast<int32_t>(r), s, sim)) ;
				group.Comparisons().emplace_back(r, s, sim);
				groupFound = true;
				break;
			}
		}
		if (! groupFound) { //	create a new group to hold this new sequences
			m_Groups.emplace_back(m_ClusterDB, r);
		}
	}
}

void
TCluster::Ccbc_St(double p_Threshold)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	Ccbc_St(p_Threshold, 0, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);
}


void
TCluster::Ccbc_St(double p_Threshold, uint32_t p_First, uint32_t p_Last)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	p_First = std::max(0u, p_First);
	p_Last = std::min(p_Last, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);

	vector<uint32_t> idxList;
	idxList.reserve(p_Last - p_First + 1);
	for (uint32_t i = p_First; i <= p_Last; ++i) {
		idxList.push_back(i);
	}
        Ccbc_St(p_Threshold, idxList);
}

void
TCluster::Ccbc_St(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	//vector<uint32_t> idxList(p_IdxList);

	//	sort the index list by decreasing size of the sequences
	//clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	//std::sort(idxList.begin(), idxList.end(), SeqSort);
        Ccbc_SingleThread_Blast(p_Threshold, p_IdxList);     
}

void
TCluster::Ccbc_SingleThread(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	m_Groups.clear();
	m_IsSortedGroups = false;

	if (m_ClusterDB->m_Sequences.size() == 0 || p_IdxList.size() == 0) {
		return;
	}

	uint32_t imax = static_cast<uint32_t>(p_IdxList.size());

	//	use a vector to keep in mind the group of each sequence
	vector<uint32_t> groupIndexes(imax, UINT32_MAX);

	//	compute everything with everything
	for (uint32_t i = 0; i < imax; ++i) {
		uint32_t s = p_IdxList[i];
		TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
		if (!srce->SrceReady()) {
			srce->InitSrce(ReverseComplement());
		}
		//	if that sequence is already in a group, use that group
		uint32_t g;	//	the group index
		if (groupIndexes[i] != UINT32_MAX) { // found
			g = groupIndexes[i];
		}
		else {	//	put that sequence in a new group
			m_Groups.emplace_back(m_ClusterDB, s);
			g = static_cast<uint32_t>(m_Groups.size()) - 1;	//	the group index
			groupIndexes[i] = g;
		}
		TCluster & group = m_Groups[g];

		//	put in that group all other m_Sequences that are close enough
		for (uint32_t j = i + 1; j < imax; ++j) {
			if (groupIndexes[j] != UINT32_MAX) { // already in a group, do nothing
				continue;
			}
			uint32_t r = p_IdxList[j];
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true);
			}
			double sim = Compare(srce, ref);
			if (sim >= p_Threshold) { //	put it in this group
				group.Comparisons().emplace_back(r, s, sim);
				//	and memorize that this sequence is in that group
				groupIndexes[j] = g;
			}
		}
	}
	return;
}

void
TCluster::Ccbc_SingleThread_Blast(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	m_Groups.clear();
	m_IsSortedGroups = false;

	if (m_ClusterDB->m_Sequences.size() == 0 || p_IdxList.size() == 0) {
		return;
	}

	uint32_t imax = static_cast<uint32_t>(p_IdxList.size());

	//	use a vector to keep in mind the group of each sequence
	vector<uint32_t> groupIndexes(imax, UINT32_MAX);
        std::string sourcefilename = std::to_string((int) p_IdxList[0]) + "_source"; //only when using Blast
        double_matrix sims = OperatorBlast_CCBC(p_IdxList,sourcefilename); //only when using Blast
        std::remove(sourcefilename.c_str());
        //	compute everything with everything
	for (uint32_t i = 0; i < imax; ++i) {
		uint32_t s = p_IdxList[i];
		TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
		if (!srce->SrceReady()) {
                	srce->InitSrce(ReverseComplement());
		}
		//	if that sequence is already in a group, use that group
		uint32_t g;	//	the group index
		if (groupIndexes[i] != UINT32_MAX) { // found
			g = groupIndexes[i];
		}
		else {	//	put that sequence in a new group
			m_Groups.emplace_back(m_ClusterDB, s);
			g = static_cast<uint32_t>(m_Groups.size()) - 1;	//	the group index
			groupIndexes[i] = g;
		}
		TCluster & group = m_Groups[g];

		//	put in that group all other m_Sequences that are close enough
		for (uint32_t j = i + 1; j < imax; ++j) {
			if (groupIndexes[j] != UINT32_MAX) { // already in a group, do nothing
				continue;
			}
			uint32_t r = p_IdxList[j];
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true);
			}
			//double sim = Compare(srce, ref);
                        double sim = sims[i][j];
			if (sim >= p_Threshold) { //	put it in this group
				group.Comparisons().emplace_back(r, s, sim);
				//	and memorize that this sequence is in that group
				groupIndexes[j] = g;
			}
		}
	}
        
	return;
}


//	compute the MLC of the current group using the threshold given by p_Thresholds[p_ParamIdx]
//	cluster all subgroups with threshold = p_Thresholds[++p_ParamIdx]
void
TCluster::MlcAll_Mt(const vector<double> & p_Thresholds, uint32_t p_ParamIdx)
{
	//	generate the next level of groups
	if(p_ParamIdx == 0) {	//	root level: use all sequences
		Mlc_Mt(p_Thresholds[p_ParamIdx]);
	}
	else {						//	not the root level, use the sequences found in our m_IdList
		//	Compute our m_IdList and use it to cluster
		BuildSortedIdList();
		Mlc_Mt(p_Thresholds[p_ParamIdx], m_IdList);
	}
	if (p_ParamIdx + 1 >= p_Thresholds.size()) {
		ComputeCentroid();
		//ComputeChildDistances();
		return;
	}
	//	cluster each child group
	for (TCluster & group : m_Groups) {
		group.MlcAll_Mt(p_Thresholds, p_ParamIdx + 1);
	}
	//	compute our centroid and the distance between our centroid and the centroid of each of our child clusters
	if(p_ParamIdx > 0) {	//	for the root, the centroid doesn't make sense, let it at NaN
		ComputeCentroid();
		//ComputeChildDistances();
	}
}


//	compute the MLC of the current group using the threshold given by p_Thresholds[p_ParamIdx]
//	cluster all subgroups with threshold = p_Thresholds[++p_ParamIdx]
void
TCluster::MlcAll_St(const vector<double> & p_Thresholds, uint32_t p_ParamIdx)
{
	//	generate the next level of groups
	if (p_ParamIdx == 0) {	//	root level: use all sequences
		Mlc_St(p_Thresholds[p_ParamIdx]);
	}
	else {						//	not the root level, use the sequences found in our m_IdList
									//	Compute our m_IdList and use it to cluster
		BuildSortedIdList();
		Mlc_St(p_Thresholds[p_ParamIdx], m_IdList);
	}

	if (p_ParamIdx + 1 >= p_Thresholds.size()) {
		ComputeCentroid();
		//ComputeChildDistances();
		return;
	}
	//	cluster each child group
	for (TCluster & group : m_Groups) {
		group.MlcAll_St(p_Thresholds, p_ParamIdx + 1);
	}
	//	compute our centroid and the distance between our centroid and the centroid of each of our child clusters
	if (p_ParamIdx > 0) {	//	for the root, the centroid doesn't make sense, let it at NaN
		ComputeCentroid();
	//	ComputeChildDistances();
	}
}


void
TCluster::Mlc_St(double p_Threshold)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	Mlc_St(p_Threshold, 0, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);
}


void
TCluster::Mlc_St(double p_Threshold, uint32_t p_First, uint32_t p_Last)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	p_First = std::max(0u, p_First);
	p_Last = std::min(p_Last, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);

	vector<uint32_t> idxList;
	idxList.reserve(p_Last - p_First + 1);
	for (uint32_t i = p_First; i <= p_Last; ++i) {
		idxList.push_back(i);
	}
	Mlc_SingleThread(p_Threshold, idxList);
}


void
TCluster::Mlc_St(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	vector<uint32_t> idxSortedList(p_IdxList);
	std::sort(idxSortedList.begin(), idxSortedList.end());
	Mlc_SingleThread(p_Threshold, idxSortedList);
}


//	p_IdxSortedList must be sorted by Idx
void
TCluster::Mlc_SingleThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList)
{
	m_Groups.clear();
	m_IsSortedGroups = false;
        
	//if (m_ClusterDB->m_Sequences.size() < 3 * m_SeqPerBloc || p_IdxSortedList.size() < 3 * m_SeqPerBloc) { //	doesn't make sense to run a MLC for a few sequences. I don't want to manage sequences spread over 1 bloc...
	//	GClust_St(p_Threshold, p_IdxSortedList);
	//	return;
	//}
	if (m_ClusterDB->m_Sequences.size() < m_ClusterDB->m_MinSeqNoForMLC || p_IdxSortedList.size() < m_ClusterDB->m_MinSeqNoForMLC) { //	doesn't make sense to run a MLC for a few sequences. I don't want to manage sequences spread over 1 bloc...
		Ccbc_St(p_Threshold, p_IdxSortedList);
		return;
	}
	//	securities
	p_Threshold = std::max(0.0, std::min(1.0, p_Threshold));

	//	create a list of centroid sequences found in step 1 below
	vector<uint32_t> centroidList;
	centroidList.reserve(2000);

	std::wostringstream stream;
	/*stream << L"\r\n ***** MLC(" << p_Threshold << L", " << m_SeqPerBloc << L") *****\r\n";*/
	stream << L"\r\n ***** MLC(" << p_Threshold << L", " << m_BlocNo << L") *****\r\n";

	vector<uint32_t> idxList;
	/*idxList.reserve(m_SeqPerBloc);*/
	uint32_t p_SeqPerBloc = (uint32_t)(m_ClusterDB->m_Sequences.size()/m_BlocNo);
	idxList.reserve(p_SeqPerBloc);
          
        //create a temp list of groups to store all the groups of the block
        vector<TCluster> tempgroups;
        
	//	dispatch the sequences into p_BlocNo cluster groups. Supposing the fasta file is already sorted, put together adjacent sequences in the source file
	for (uint32_t i = 0, imax = static_cast<int32_t>(p_IdxSortedList.size()); i < imax; i += p_SeqPerBloc) {
		idxList.clear();
		/*for (uint32_t j = i, jmax = std::min(imax, i + m_SeqPerBloc); j < jmax; ++j) {
			uint32_t idx = p_IdxSortedList[j];
			idxList.push_back(idx);
		}*/
		for (uint32_t j = i, jmax = std::min(imax, i + p_SeqPerBloc); j < jmax; ++j) {
			uint32_t idx = p_IdxSortedList[j];
			idxList.push_back(idx);
		}
		//	GClust each cluster and compute the central sequence for each cluster group
		//std::OutputDebugVector(stream, idxList);
		GClust_St(p_Threshold, idxList); //use GC to cluster the blocks
                //Ccbc_St(p_Threshold, idxList);     //use CCBC to cluster the blocks

		for(TCluster & group : m_Groups) {
			//stream << L"Computing centroid\r\n";
			//group.OutputDebug();
			group.ComputeCentroid();
			group.PrepareCentroid();
			//group.OutputDebug();
			//	add the centroid to the centroid list
			uint32_t idx = group.CentralSeqIdx();
			centroidList.push_back(idx);
			//stream << L"Adding centroid " << idx << L"\r\n";
                        //add each group to the temp list
                        tempgroups.push_back(group);
		}
	}

	std::sort(centroidList.begin(), centroidList.end());

	//	cluster the centroid sequences
	Ccbc_St(p_Threshold, centroidList);
        //Ccbc_St_Blast(p_Threshold, centroidList);//if using Blast
        
        //look for the final group of the temp groups
        uint32_t i = 0;
        vector<uint32_t> groupidxList;
        for(TCluster & tempgroup : tempgroups) {
            uint32_t r = tempgroup.CentralSeqIdx();
            //look for the final group of r
            uint32_t i = 0;
            for(TCluster & group : m_Groups) {
                if (r==group.CentralSeqIdx()){
                    groupidxList.push_back(i);
                    break;
                }
		if (group.Comparisons().size() > 0) {
                    for (const TComparison & comp : group.Comparisons()) {
                        uint32_t s = comp.SrceIdx();
                        if (r==s){
                            groupidxList.push_back(i);
                            break;
                        }
		    }
                } 
                for (uint32_t id: group.IdList()){
                    if (r==id){
                            groupidxList.push_back(i);
                            break;
                    }
                }
                i=i+1;
	    }
            
        }
        //merge temp groups in to thefinal groups
        i = 0;
        for(TCluster & tempgroup : tempgroups) {
            vector<uint32_t> reflist;
            TCluster & group = m_Groups[groupidxList[i]];
            uint32_t cenidx = group.CentralSeqIdx();
            reflist.push_back(group.CentralSeqIdx());
            if (group.Comparisons().size() > 0) {
                for (const TComparison & comp : group.Comparisons()) {
                        uint32_t s = comp.SrceIdx();
                        if(std::find(reflist.begin(), reflist.end(), s) == reflist.end()) {
                            reflist.push_back(s);
                        }
                         uint32_t r = comp.RefIdx();
                        if(std::find(reflist.begin(), reflist.end(), s) == reflist.end()) {
                            reflist.push_back(r);
                        }
		}
                for (uint32_t id: group.IdList()){
                    if(std::find(reflist.begin(), reflist.end(), id) == reflist.end()) {
                        reflist.push_back(id);
                    }
                }
            } 
            
            if (tempgroup.Comparisons().size() > 0) {
                for (const TComparison & comp : tempgroup.Comparisons()) {
                    uint32_t s = comp.SrceIdx();
                    if(std::find(reflist.begin(), reflist.end(), s) == reflist.end()) {
                         group.Comparisons().emplace_back(cenidx, s, comp.Sim());			
                    }  
                    uint32_t r = comp.RefIdx();
                    if(std::find(reflist.begin(), reflist.end(), r) == reflist.end()) {
                         group.Comparisons().emplace_back(cenidx, r, comp.Sim());                       
                    } 
                } 
                for (uint32_t id: tempgroup.IdList()){
                    if (std::find(reflist.begin(), reflist.end(), id) == reflist.end()) {
                        group.Comparisons().emplace_back(cenidx, id, 1);
                    }
                }
	    }
            i=i+1;
        }
    /*  
        return;
        
      
	//stream << L"There is " << centroidList.size() << L" centroids\r\n";
	//stream << L"Sorted centroids :\r\n";
	//std::OutputDebugVector(stream, centroidList);

	//stream << L"There is " << m_Groups.size() << L" groups after clustering the centroids\r\n";
	//OutputDebugGroups(stream);

	//	don't scan the centroids any more, they are already either a group centroid, or moved into another group.
	RemoveFromList(p_IdxSortedList, centroidList);

	//stream << L"Index list without centroids :\r\n";
	//std::OutputDebugVector(stream, p_IdxSortedList);

	//	dispatch all sequences into the existing groups

	uint32_t missingGroupNo = 0;	//	the number of groups added below

	uint32_t scrapNo = 0;
	int32_t g_min = static_cast<int32_t>(m_Groups.size());

	//	scan the sequences only once
	for (size_t i = 0, imax = p_IdxSortedList.size(); i < imax; ++i) {
		uint32_t r = p_IdxSortedList[i];
		TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
		if (!ref->RefReady()) {
			ref->InitRef(true);
		}
		bool groupFound = false;
		//	search in which group we will put it
		for (int32_t g = 0, gmax = static_cast<int32_t>(m_Groups.size()); g < gmax; ++g) {
			TCluster & group = m_Groups[g];
			uint32_t s = group.CentralSeqIdx();
			TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
			if (!srce->SrceReady()) {
				srce->InitSrce(ReverseComplement());
			}
			if (r == s) {
				groupFound = true;
				break;
			}
			double sim = Compare(srce, ref);
			if (sim >= p_Threshold) { //	put it in this group
				group.Comparisons().emplace_back(r, s, sim);
				groupFound = true;
				if (g >= g_min) {
					++scrapNo;
				}
				break;
			}
		}
		if (!groupFound) { //	create a new group to hold this new sequences
			m_Groups.emplace_back(m_ClusterDB, r);
			++scrapNo;
			++missingGroupNo;
		}
	}
	stream << L"Missing seq number: " << scrapNo << L"\r\n";
	stream << L"Missing group number: " << missingGroupNo << L"\r\n";

	//OutputDebugString(stream.str().c_str());
     */
}


void
TCluster::OutputDebugGroups(std::wostringstream & stream)
{
	for (const auto & group : m_Groups) {
		stream << group.CentralSeqIdx() << L", ";
	}
	stream << L"\r\n";
}


//	TODO: test
bool
TCluster::ExportSequences(const wchar_t * p_DestFilePath) const
{
	//	sort the index list by decreasing size of the sequences
	uint32_t imax = static_cast<uint32_t>(m_ClusterDB->m_Sequences.size());
	vector<uint32_t> idxList(boost::counting_iterator<uint32_t>(0), boost::counting_iterator<uint32_t>(imax));

	//	sort the index list by decreasing size of the sequences
	clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	std::sort(idxList.begin(), idxList.end(), SeqSort);

	bool IsError = true;
	try {
        char mbstr[256];
        std::wcstombs(mbstr, p_DestFilePath, 256);
		std::ofstream file(mbstr, std::ofstream::binary);
		if (file.fail()) {
			return true;	//	error
		}
		//	mark the file as utf-8
		file << cUtf8Header;

		//	save all values
		for (uint32_t r : idxList) {
			if (file.fail()) {
				break;
			}
			file << "\r\n";
			file << r << "\t";
			file << m_ClusterDB->m_Sequences[r]->RecordId() << "\t";
			file << m_ClusterDB->m_Sequences[r]->RecordName() << "\t";
			file << m_ClusterDB->m_Sequences[r]->Sequence();
		}
		IsError = (file.goodbit != 0);
		file.close();
	}
	catch (...) {
	}
	return IsError;
}

//	return the best similarity (srce vs ref AND reversed srce vs ref) with Sim correction for short sequences < 100
double
TCluster::Compare(const TNFieldBase * srce, const TNFieldBase * ref) const
{
	return m_ClusterDB->Compare(srce, ref);
}

bool CreateDirectory(char _filePath[256])
{
    // MAC SPECIFIC
    const char* path = _filePath;
    
    int result = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (result==-1)
    {
        return false;
    }
    /*
    boost::filesystem::path dir(path);
    if(boost::filesystem::create_directory(dir))
    {
        return true;
    }
     */
    
    return false;
}

//**********************BLAST functions**********************************

void
CreateBLastInputFiles(ccbc_context * ctx, uint32_t p_numThread)
{       
    //create reference file 
    const uint32_t seqNo = (ctx->m_Cluster.m_SeqPerThread);
    const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();
    std::ofstream reffile;
    std::ofstream sourcefile;
    uint32_t n = 0;
    if (ctx->m_IdxList.size() % seqNo == 0 ){
        n= (int )(ctx->m_IdxList.size() / seqNo);
    }
    else{
        n= ((int )(ctx->m_IdxList.size() / seqNo)) + 1;
    }
    vector<std::ofstream> sourcefiles;
    //create reference file
    std::string reffilename = std::to_string((int) ctx->m_IdxList[0]) + "_" + std::to_string((int) ctx->m_IdxList[ctx->m_IdxList.size()-1]) + "_ref";
    reffile.open(reffilename);
    std::string sourcefilename = "";
    
    uint32_t j = 0;
    for (uint32_t i = 0; i < ctx->m_IdxList.size(); ++i) {
        uint32_t r = ctx->m_IdxList[i];			//	the index in the sequence list
        TNFieldBase * ref = sequences[r];
        //if (!ref->SrceReady()) {
          // ref->InitSrce(ctx->m_Cluster.ReverseComplement());
        //}   
             //put the sequences into a fasta file
        reffile<< ">" <<  i << "\r\n";
        reffile <<  ref-> Sequence2() << "\n";
        
        if (i % seqNo ==0 ){
            if (i>0){
                sourcefile.close();
            }
            sourcefilename = std::to_string((int) i) + "_source.fasta";
            sourcefile.open(sourcefilename);
        }
        if (i % (seqNo * p_numThread)==0 ){
            j=0;
        }
             //put the sequences into a fasta file
        sourcefile << ">" <<  j << "\r\n";
        sourcefile <<  ref-> Sequence2() << "\n"; 
        j=j+1;
    } 
    sourcefile.close();
    reffile.close();     
 //   std::remove("log");
 //   std::remove("log.perf");
 //   std::string makeblastdbcommand = "makeblastdb -in " + reffilename  + " -dbtype nucl -out " + reffilename + "db" + " -logfile log";
 //   system(makeblastdbcommand.c_str());
}

void
DeleteBLastInputFiles(ccbc_context * ctx)
{   
    const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
    std::string reffilename = std::to_string((int)ctx->m_IdxList[0]) + "_" + std::to_string((int) ctx->m_IdxList[ctx->m_IdxList.size()-1]) + "_ref";
    std::remove(reffilename.c_str());
    uint32_t j = 0;
    for (uint32_t i = 0; i < ctx->m_IdxList.size(); ++i) {
        if (i % seqNo == 0){
            std::string sourcefilename = std::to_string((int) i) + "_source.fasta";
            std::remove(sourcefilename.c_str());
            std::string resultfilename = std::to_string((int) i) + "_result.txt";
            std::remove(resultfilename.c_str());
        }
    }
    //std::remove((reffilename+"db.nin").c_str());
    //std::remove((reffilename+"db.nsq").c_str());
    //std::remove((reffilename+"db.nhr").c_str());
}

double_matrix 
TCluster::OperatorBlast_CCBC(const vector<uint32_t> & p_IdxList, std::string sourcefilename)
{
    std::ofstream sourcefile;
    //std::string sourcefilename = std::to_string((int) p_IdxList[0]) + "_source";
    sourcefile.open(sourcefilename);  
    for (uint32_t i = 0; i < p_IdxList.size(); ++i) {
	uint32_t s = p_IdxList[i];			//	the index in the sequence list
	TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
        sourcefile<< ">" <<  i << "\r\n";
        sourcefile <<  srce-> Sequence2() << "\n";
    } // next sequence
    sourcefile.close();
  //  std::remove("log");
  //  std::remove("log.perf");
     //make blast db
   // std::string makeblastdbcommand = "makeblastdb -in " + sourcefilename  + " -dbtype nucl -out " + sourcefilename + "db" + " -logfile log";
   // system(makeblastdbcommand.c_str());
    return OperatorBlast_File(p_IdxList[0], p_IdxList.size(), p_IdxList.size(), sourcefilename,sourcefilename);
}

double_matrix 
TCluster::OperatorBlast_GC(uint32_t p_sourceIdx, uint32_t p_size, std::string p_reffilename)
{
    std::ofstream sourcefile;
    std::string sourcefilename = std::to_string((int) p_sourceIdx) + "_source.fasta";
    sourcefile.open(sourcefilename);  
    TNFieldBase * srce = m_ClusterDB->m_Sequences[p_sourceIdx];
    sourcefile<< ">" <<  0 << "\r\n";
    sourcefile <<  srce-> Sequence2() << "\n";
    sourcefile.close();
    return OperatorBlast_File(p_sourceIdx, 1, p_size, sourcefilename,p_reffilename);
    
}
double_matrix 
TCluster::OperatorBlast_File(uint32_t p_first, uint32_t p_RowCount, uint32_t p_ColCount, std::string p_sourcefilename, std::string p_reffilename)
{
    double_matrix simmatrix(p_RowCount, p_ColCount);	
    std::string blastresultfilename =  std::to_string((int) p_first) + "_blastresult.txt";
    //compare the sequences using BLast
    std::string blastcommand = "blastn -query " + p_sourcefilename + " -subject " + p_reffilename  + " -outfmt 6 " ;
    //std::string blastcommand = "blastn -query " + p_sourcefilename + " -db " + p_reffilename  + "db" + " -outfmt 6 " ;
    //get blast result
    std::string blastresult;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    blastcommand.append(" 2>&1");

    stream = popen(blastcommand.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) blastresult.append(buffer);
            pclose(stream);
    }
    
    if (blastresult !=""){
        std::vector<std::string> lines;
        boost::split(lines, blastresult, boost::is_any_of("\n"));
        for (uint32_t k = 0; k < lines.size(); ++k) {
            if (lines[k]!= ""){
                std::vector<std::string> strs;
                boost::split(strs, lines[k], boost::is_any_of("\t"));
                uint32_t i = std::atoi(strs[0].c_str());
                uint32_t j = std::atoi(strs[1].c_str());
                double sim = std::atof(strs[2].c_str());
                sim =sim/100;
                double overlap = std::atof(strs[3].c_str());
                if (overlap < 100){
                    sim = sim * overlap / 100;
                }
                simmatrix[i][j]=sim;
            }
        }    
    } 
    //delete source file
    std::remove(p_sourcefilename.c_str());
    
    return simmatrix;
}

double_matrix
OperatorBlast_GC_Mt(uint32_t p_sourceIdx, std::string p_Sequence, uint32_t p_size, std::string p_reffilename )
{
    double_matrix simmatrix(1, p_size);	
    std::ofstream sourcefile;
    std::string sourcefilename = std::to_string((int) p_sourceIdx) + "_source.fasta";
    std::string blastresultfilename =  std::to_string((int) p_sourceIdx) + "_blastresult.txt";
    sourcefile.open(sourcefilename);  
    sourcefile<< ">0" << "\r\n";
    sourcefile <<  p_Sequence << "\n";
    sourcefile.close();
     
    std::ofstream reffile;
    //compare the sequences using BLast
    std::string blastcommand = "blastn -query " + sourcefilename + " -subject " + p_reffilename  + " -outfmt 6 " ;
    //make blast command
   // std::string blastcommand = "blastn -query " + sourcefilename + " -db " + p_reffilename  + "db" + " -outfmt 6 " ;
    //get blast result
    std::string blastresult;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    blastcommand.append(" 2>&1");

    stream = popen(blastcommand.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) blastresult.append(buffer);
            pclose(stream);
    }
    if (blastresult !=""){
        std::vector<std::string> lines;
        boost::split(lines, blastresult, boost::is_any_of("\n"));
        for (uint32_t k = 0; k < lines.size(); ++k) {
            if (lines[k]!= ""){
                std::vector<std::string> strs;
                boost::split(strs, lines[k], boost::is_any_of("\t"));
                uint32_t i = std::atoi(strs[0].c_str());
                uint32_t j = std::atoi(strs[1].c_str());
                double sim = std::atof(strs[2].c_str());
                sim =sim/100.0;
                double overlap = std::atof(strs[3].c_str());
                if (overlap < 100){
                    sim = sim * overlap / 100;
                }
              //  cout << simmatrix.RowCount() << "row" << simmatrix.ColCount() << "col" << i <<"-"<< j << sim << "\n" ;
                simmatrix[i][j]=sim;
                
            }
        }    
    } 
    //delete source file
    std::remove(sourcefilename.c_str());
  
    return simmatrix;
}


//	****************************************************************************************************************************
//																																										*
//   Multi-threaded functions																	*
//																																										*
//	****************************************************************************************************************************


//	clusterize all sequences given by index list p_IdxList, but only in the ranges given by the context
void
OperatorGClust(uint32_t p_Id, gclust_context * ctx)
{
	const uint32_t seqSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
	const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();		//	simplify coding

	while (true) {
		ctx->m_IdxMutex.lock();										//	lock the indexes

		uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process
		uint32_t end = std::min(first + seqNo, seqSize);	//	compute the index of the next sequence to be process by the next thread
		ctx->m_NextIdx = end;										//	update the next index in the context

		ctx->m_IdxMutex.unlock();									//	unlock the indexes

		double x = ctx->m_NextIdx / static_cast<double>(seqSize);
		Wait(ctx->m_WaitHdl, x);

		int32_t len = end - first;				//	compute the real end, in case we have reached the end of the sequences
		if (len == 0) {							// check if job is finisheds
			break;
		}

		auto sit_first = ctx->m_IdxList.begin() + first;		//	get the first row to compute
		auto sit_end = ctx->m_IdxList.begin() + end;
                //create reference list
                std::string reffilename = std::to_string((int) (p_Id)) + "_" + std::to_string((int) (ctx->m_FirstGroupIdx)) + "_ref";
                std::ofstream reffile;
		//	scan the sequences
		for (auto sit = sit_first; sit < sit_end; ++sit) {			//	s is the index in m_IdxList
			uint32_t r = *sit;												//	the index in the sequence list
			TNFieldBase * ref = sequences[r];			//	the reference sequence
			if (!ref->RefReady()) {
				ref->InitRef(true); //	we consider this one as a reference sequence
			}
			//	search in which group we will put it
			//	g_min and g_max are constant during this process. No new group is added to the cluster groups
                        
                        reffile.open(reffilename);  
                        uint32_t n=0;                      
                        for (uint32_t g = ctx->m_FirstGroupIdx, gmax = static_cast<uint32_t>(ctx->m_Cluster.Groups().size()); g < gmax; ++g) {
                            TCluster & group = ctx->m_Cluster.Groups()[g];
                            uint32_t s = group.CentralSeqIdx();
                            TNFieldBase * srce =  sequences[s];
                            reffile << ">" <<  n << "\r\n";
                            reffile <<  srce-> Sequence2() << "\n";
                            n=n+1;
                        }    
                        reffile.close();
                         //make blast db
                       // std::remove("log");
                       // std::remove("log.perf");
                       // std::string makeblastdbcommand = "makeblastdb -in " + reffilename  + " -dbtype nucl -out " + reffilename + "db" + " -logfile log";
                       // system(makeblastdbcommand.c_str());
                        double_matrix sims = OperatorBlast_GC_Mt(r,ref-> Sequence2(),ctx->m_Cluster.Groups().size(),reffilename);
                        std::remove((reffilename.c_str()));
                        //std::remove((reffilename+".nin").c_str());
                        //std::remove((reffilename+".nsq").c_str());
                        //std::remove((reffilename+".nhr").c_str());
                        uint32_t j=0;
			for (uint32_t g = ctx->m_FirstGroupIdx, gmax = static_cast<uint32_t>(ctx->m_Cluster.Groups().size()); g < gmax; ++g) {
				TCluster & group = ctx->m_Cluster.Groups()[g];

				uint32_t s = group.CentralSeqIdx();
				TNFieldBase * srce = sequences[s];

				//double sim = ctx->m_Cluster.Compare(srce, ref);
                                double sim = sims[0][j];
                                j=j+1;
				if (sim >= ctx->m_Threshold) { //	put it in this group
					uint32_t t = g - ctx->m_FirstGroupIdx;
					ctx->m_MutexList[t]->lock();
					group.Comparisons().emplace_back(r, group.CentralSeqIdx(), sim);
					ctx->m_MutexList[t]->unlock();

					*sit = UINT32_MAX;	//	reset the index to be able to remove it from the list
					break;
				}
                                
			} // next group
		} // next sequence  
                
	} // next bloc of 'seqNo' sequences
}


/*
For the Medical fungi database, 2800 sequences in 421 groups, with 100 TNFieldResult.m_Denom correction (note that all four combinations of using (100 or 150) TNFieldResult.(m_Num or m_Denom) to correct the sim gives the same results)

Performance optimization vs m_GroupsPerTour and m_SeqPerThread


Algorithm:
- take the first sequence and put it in the first group
- take all following sequences and put them in a group if the similarity is >= given threshold
*/

/*	Greedy Clustering multi-threaded algorithm:
- sort all sequences by decreasing size
- tour #0:
- thread #0 will manage the first x sequences, creating up to y groups
- all remaining sequences can therefore be compared with the first y groups, this task is given to n threads (depending in the hardware)
- tour #1:
- thread #0 takes the x next remaining sequences and put them in the existing y groups + new groups, leading to z groups
- all remaining sequences are compared with these z groups, this task is given to n threads
-	etc until less than x sequences remain, which are treated by thread #0

*/


void
TCluster::GClust_Mt(double p_Threshold)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	GClust_Mt(p_Threshold, 0, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);
}


void
TCluster::GClust_Mt(double p_Threshold, uint32_t p_First, uint32_t p_Last)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	p_First = std::max(0u, p_First);
	p_Last = std::min(p_Last, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);

	//using namespace boost::numeric::ublas;
	vector<uint32_t> idxList;
	idxList.reserve(p_Last - p_First + 1);
	for (uint32_t i = p_First; i <= p_Last; ++i) {
		idxList.push_back(i);
	}
	//	sort the index list by decreasing size of the sequences
	clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	std::sort(idxList.begin(), idxList.end(), SeqSort);
        GClust_MultiThread_Blast(p_Threshold, idxList);
}


void
TCluster::GClust_Mt(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	vector<uint32_t> idxList(p_IdxList);

	//	sort the index list by decreasing size of the sequences
	clustering::OnSequenceSizeDescending_int32 SeqSort(m_ClusterDB->m_Sequences);
	std::sort(idxList.begin(), idxList.end(), SeqSort);
        GClust_MultiThread_Blast(p_Threshold, idxList); //if using Blast  
}


//	p_IdxSortedList must be sorted by sequence size, descending
void
TCluster::GClust_MultiThread_Blast(double p_Threshold, vector<uint32_t> & p_IdxSortedList)
{
	m_Groups.clear();
	m_IsSortedGroups = false;


	//	get the number of sequences to manage
	//uint32_t idxNo = static_cast<uint32_t>(p_IdxList.size());

	if (m_ClusterDB->m_Sequences.size() == 0 || p_IdxSortedList.size() == 0) {
		return;
	}
       
	int waitHdl = WaitOpen(L"GCLust", L"Clustering", true);

	unsigned long Start = static_cast<long int> (time(NULL));

	int32_t tour = 0;	//	the tour index
	while (p_IdxSortedList.size() > 0) {

		//	set the next group iterator
		uint32_t g_min = static_cast<uint32_t>(m_Groups.size());

		//	put the first sequence in the first cluster group
		m_Groups.emplace_back(m_ClusterDB, p_IdxSortedList.front());

		vector<uint32_t>::const_iterator sit = p_IdxSortedList.begin() + 1;

		//	Thread #0 scan at most s_max sequences
		uint32_t s_max = std::min(m_GroupsPerTour, static_cast<uint32_t>(p_IdxSortedList.size())); //	be carefull when reaching the end of the idxList
               
                //	scan the list of remaining sequence index
		for (vector<uint32_t>::const_iterator sit_max = p_IdxSortedList.begin() + s_max; sit < sit_max; ++sit) {
			uint32_t r = *sit;
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is lighter than InitSrce()
			}
			bool groupFound = false;
                       //create reference file
                        std::string reffilename = std::to_string((int) (m_Groups[0].CentralSeqIdx())) + "_ref";
                        std::ofstream reffile;
                        reffile.open(reffilename);  
                        uint32_t n=0;
                        for (vector<TCluster>::iterator git = m_Groups.begin() + g_min; git < m_Groups.end(); ++git) {
                            TCluster & group = *git;
                            uint32_t s = group.CentralSeqIdx();
                            TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
                            reffile << ">" <<  n << "\r\n";
                            reffile <<  srce-> Sequence2() << "\n";
                            n=n+1;
                        }                      
                        reffile.close();
                        //make blast db
                       // std::remove("log");
                        //std::remove("log.perf");
                        //std::string makeblastdbcommand = "makeblastdb -in " + reffilename  + " -dbtype nucl -out " + reffilename + "db"  + " -logfile log";
                        //system(makeblastdbcommand.c_str());  
                        //compare s with the reference sequences
                        double_matrix sims = OperatorBlast_GC_Mt(r,ref->Sequence(),m_Groups.size(),reffilename);
                        std::remove(reffilename.c_str());
                       // std::remove((reffilename+"db.nin").c_str());
                        //std::remove((reffilename+"db.nsq").c_str());
                        //std::remove((reffilename+"db.nhr").c_str());
                        uint32_t j=0;
			//	search in which group we will put it
			for (vector<TCluster>::iterator git = m_Groups.begin() + g_min; git < m_Groups.end(); ++git) {
				TCluster & group = *git;
				uint32_t s = group.CentralSeqIdx();
				TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
				if (!srce->SrceReady()) {
					srce->InitSrce(ReverseComplement());
				}
				double sim;
				//sim = Compare(srce, ref);
                                sim = sims[0][j];
				if (sim >= p_Threshold) { //	put it in this group
					group.Comparisons().emplace_back(r, group.CentralSeqIdx(), sim);
					groupFound = true;
					break;
				}
                                j=j+1;
			}
			if (!groupFound) { //	create a new group to hold this new sequences
				m_Groups.emplace_back(m_ClusterDB, r);
                                //put r into the reference file
                              //  reffile.open(reffilename, std::ios_base::app);
                               // reffile<< ">" <<  m_Groups.size()-1 << "\r\n";
                                //cout << "add" << r << "\n";
                                //reffile <<  ref-> Sequence2() << "\n";
                                //reffile.close();                             
				//	prepare it for further computations, otherwise the multiple thread will have to do it on the fly, with thread concurrency...
				if (!ref->SrceReady()) {
					ref->InitSrce(ReverseComplement());
				}
                                
			}
                        
		}
      
                //  delete reference file
                //std::remove(reffilename.c_str());
		uint32_t numThread = max_(1u, m_NumThread);
                
		//	now dispatch into numThread threads all sequences from s_max to end, analysing only from groups g_min to the end

		//	create the thread context and start computation after the sequences already processed
               
		gclust_context context(s_max, g_min, *this, p_IdxSortedList, p_Threshold);
		context.m_WaitHdl = waitHdl;

		if (numThread == 1) {
			OperatorGClust(0, &context);
		}
		else {
			vector<std::thread>	threads;
			threads.reserve(numThread);

			for (uint32_t i = 0; i < numThread; ++i) {
				threads.emplace_back(::OperatorGClust, i, &context);
			}
			//	catch each thread results and wait for all threads to finish
			for (std::thread & item : threads) {
				item.join();
			}
			//	clear all existing threads
			threads.clear();
		}
		if (IsTaskCanceled(waitHdl)) {
			break;
		}
                
		//	remove from idxList all items that are already moved into an existing group (they are set to UINT32_MAX by OperatorGClust)
		auto dit = p_IdxSortedList.begin();	//	the destination
		for (; sit < p_IdxSortedList.end(); ++sit) {
			if (*sit != UINT32_MAX) {
				*dit = *sit;
				++dit;
			}
		}
		p_IdxSortedList.resize(dit - p_IdxSortedList.begin());

		//std::wostringstream stream2;
		//stream2 << L"\r\nTour #" << tour << L" terminated with " << m_Groups.size() << L" groups and " << idxList.size() << " sequences \r\n";
		//OutputDebugString(stream2.str().c_str());

		//	next tour
		tour += 1;
	}
	//OutputDebug();
	WaitClose(waitHdl);

	std::wostringstream stream;
	unsigned long timeNeeded = (static_cast<long int> (time(NULL)) - Start);
	stream << std::endl << L"Clustering done in " << timeNeeded << L" ms";
	//OutputDebugString(stream.str().c_str());
	return;
}


void
TCluster::GClust_MultiThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList)
{
	m_Groups.clear();
	m_IsSortedGroups = false;


	//	get the number of sequences to manage
	//uint32_t idxNo = static_cast<uint32_t>(p_IdxList.size());

	if (m_ClusterDB->m_Sequences.size() == 0 || p_IdxSortedList.size() == 0) {
		return;
	}

	int waitHdl = WaitOpen(L"GCLust", L"Clustering", true);

	unsigned long Start = static_cast<long int> (time(NULL));

	int32_t tour = 0;	//	the tour index
	while (p_IdxSortedList.size() > 0) {

		//	set the next group iterator
		uint32_t g_min = static_cast<uint32_t>(m_Groups.size());

		//	put the first sequence in the first cluster group
		m_Groups.emplace_back(m_ClusterDB, p_IdxSortedList.front());

		vector<uint32_t>::const_iterator sit = p_IdxSortedList.begin() + 1;

		//	Thread #0 scan at most s_max sequences
		uint32_t s_max = std::min(m_GroupsPerTour, static_cast<uint32_t>(p_IdxSortedList.size())); //	be carefull when reaching the end of the idxList
																			//	scan the list of remaining sequence index
		for (vector<uint32_t>::const_iterator sit_max = p_IdxSortedList.begin() + s_max; sit < sit_max; ++sit) {
			uint32_t r = *sit;
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true); //	we consider this one as a reference sequence, as InitRef() is lighter than InitSrce()
			}
			bool groupFound = false;

			//	search in which group we will put it
			for (vector<TCluster>::iterator git = m_Groups.begin() + g_min; git < m_Groups.end(); ++git) {
				TCluster & group = *git;
				uint32_t s = group.CentralSeqIdx();
				TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
				if (!srce->SrceReady()) {
					srce->InitSrce(ReverseComplement());
				}
				double sim;
				sim = Compare(srce, ref);
				if (sim >= p_Threshold) { //	put it in this group
												  //group.AddComparison(TComparison(static_cast<int32_t>(r), group.CentralSeqIdx(), sim));
					group.Comparisons().emplace_back(r, group.CentralSeqIdx(), sim);
					groupFound = true;
					break;
				}
			}
			if (!groupFound) { //	create a new group to hold this new sequences
				m_Groups.emplace_back(m_ClusterDB, r);
				//	prepare it for further computations, otherwise the multiple thread will have to do it on the fly, with thread concurrency...
				if (!ref->SrceReady()) {
					ref->InitSrce(ReverseComplement());
				}
			}
		}

		uint32_t numThread = max_(1u, m_NumThread);

		//	now dispatch into numThread threads all sequences from s_max to end, analysing only from groups g_min to the end

		//	create the thread context and start computation after the sequences already processed
		gclust_context context(s_max, g_min, *this, p_IdxSortedList, p_Threshold);
		context.m_WaitHdl = waitHdl;

		if (numThread == 1) {
			OperatorGClust(0, &context);
		}
		else {
			vector<std::thread>	threads;
			threads.reserve(numThread);

			for (uint32_t i = 0; i < numThread; ++i) {
				threads.emplace_back(::OperatorGClust, i, &context);
			}
			//	catch each thread results and wait for all threads to finish
			for (std::thread & item : threads) {
				item.join();
			}
			//	clear all existing threads
			threads.clear();
		}
		if (IsTaskCanceled(waitHdl)) {
			break;
		}

		//	remove from idxList all items that are already moved into an existing group (they are set to UINT32_MAX by OperatorGClust)
		auto dit = p_IdxSortedList.begin();	//	the destination
		for (; sit < p_IdxSortedList.end(); ++sit) {
			if (*sit != UINT32_MAX) {
				*dit = *sit;
				++dit;
			}
		}
		p_IdxSortedList.resize(dit - p_IdxSortedList.begin());

		//std::wostringstream stream2;
		//stream2 << L"\r\nTour #" << tour << L" terminated with " << m_Groups.size() << L" groups and " << idxList.size() << " sequences \r\n";
		//OutputDebugString(stream2.str().c_str());

		//	next tour
		tour += 1;
	}
	//OutputDebug();
	WaitClose(waitHdl);

	std::wostringstream stream;
	unsigned long timeNeeded = (static_cast<long int> (time(NULL)) - Start);
	stream << std::endl << L"Clustering done in " << timeNeeded << L" ms";
	//OutputDebugString(stream.str().c_str());
	return;
}
 
//	call InitRef() on all sequences()
void
OperatorCcbc1(uint32_t p_Id, ccbc_context * ctx)
{
	const uint32_t seqSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
	const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();//	simplify coding

	while (true) {
		ctx->m_IdxMutex.lock();										//	lock the indexes

		uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process
		uint32_t end = std::min(first + seqNo, seqSize);	//	compute the index of the next sequence to be process by the next thread
		ctx->m_NextIdx = end;										//	update the next index in the context

		ctx->m_IdxMutex.unlock();									//	unlock the indexes

		int32_t len = end - first;				//	compute the real end, in case we have reached the end of the sequences
		if (len == 0) {							// check if job is finisheds
			break;
		}

		auto sit_first = ctx->m_IdxList.begin() + first;		//	get the first row to compute
		auto sit_end = ctx->m_IdxList.begin() + end;

		//	scan the sequences
		for (auto sit = sit_first; sit < sit_end; ++sit) {			//	s is the index in m_IdxList
		    uint32_t s = *sit;												//	the index in the sequence list
                    TNFieldBase * ref = sequences[s];			//	the reference sequence
		    if (!ref->RefReady()) {
			ref->InitRef(true);
		    }
		} // next sequence
	} // next bloc of 'seqNo' sequences
}

//	compare all sequences with all others on 1 row
void
OperatorCcbc2_Blast(uint32_t p_Id, ccbc_context * ctx)
{
    const uint32_t seqSize = static_cast<uint32_t>(ctx->m_IdxList.size());
    const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
    const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();		//	simplify coding

    ctx->m_IdxMutex.lock();										//	lock the indexes

    uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process in IdxList
    uint32_t end = std::min(first + seqNo, seqSize);	//	compute the index of the next sequence to be process by the next thread
    ctx->m_NextIdx = end;										//	update the next index in the context

    ctx->m_IdxMutex.unlock();	//	unlock the indexes
    //uint32_t simRowIdx = p_Id * seqNo;	    
    std::string reffilename = std::to_string((int) ctx->m_IdxList[0]) + "_" + std::to_string((int) ctx->m_IdxList[ctx->m_IdxList.size()-1]) + "_ref";
    //for (uint32_t i = first, imax = ctx->m_NextIdx; i < imax; ++i, ++simRowIdx) {
    // } // next sequence
    if (first >=end) {
        return;
    }
    std::string sourcefilename = std::to_string((int) first) + "_source.fasta";
    std::string blastresultfilename =  std::to_string((int) first) + "_result.txt";
    std::string blastcommand = "blastn -query " + sourcefilename + " -subject " + reffilename  + " -outfmt 6 -out " + blastresultfilename;
   // std::string blastcommand = "blastn -query " + sourcefilename + " -db " + reffilename  + "db -outfmt 6 -out " + blastresultfilename;
    system(blastcommand.c_str());
    std::ifstream file(blastresultfilename,std::ifstream::in);
    std::string line;
    while (file.good()) {
            //	read line
            line.clear() ;
            std::getline(file, line);
            if (line==""){
                continue;
            }
            std::vector<std::string> strs;
            boost::split(strs, line, boost::is_any_of("\t"));
            uint32_t i = std::atoi(strs[0].c_str());
            uint32_t j = std::atoi(strs[1].c_str());
            double sim = std::atof(strs[2].c_str());
            sim =sim/100.0;
            double overlap = std::atof(strs[3].c_str());
            if (overlap < 100){
                 sim = sim * overlap / 100;
            }
            ctx->m_Sims[i][j]=sim;
        //    cout << first << "i:" << i <<"j:" << j << "sim" << sim << "\n";
    }
            
    file.close();																	//	scan the sequences
}
    
void
OperatorCcbc2(uint32_t p_Id, ccbc_context * ctx)
{
	const uint32_t seqSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
	const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();		//	simplify coding

	ctx->m_IdxMutex.lock();										//	lock the indexes

	uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process in IdxList
	uint32_t end = std::min(first + seqNo, seqSize);	//	compute the index of the next sequence to be process by the next thread
	ctx->m_NextIdx = end;										//	update the next index in the context

	ctx->m_IdxMutex.unlock();									//	unlock the indexes

																		//	scan the sequences
	uint32_t simRowIdx = p_Id * seqNo;	//	thread o will fill rows 0 to 9, thread 1 will fill rows 10 to 19, etc. (supposing that m_SeqPerThread = 10)
	for (uint32_t i = first, imax = ctx->m_NextIdx; i < imax; ++i, ++simRowIdx) {
		uint32_t s = ctx->m_IdxList[i];			//	the index in the sequence list
		TNFieldBase * srce = sequences[s];			//	the source sequence
		if (!srce->SrceReady()) {
			srce->InitSrce(ctx->m_Cluster.ReverseComplement());
		}

		//	put in that group all other sequences that are close enough
		for (uint32_t j = i + 1; j < seqSize; ++j) {
			if (ctx->m_GroupIndexes[j] != UINT32_MAX) { // already in a group, do nothing
				continue;
			}
			uint32_t r = ctx->m_IdxList[j];
			TNFieldBase * ref = sequences[r];

			double sim = ctx->m_Cluster.Compare(srce, ref);
			ctx->m_Sims[simRowIdx][j] = sim;
		}
	} // next sequence
}
//	compare all sequences with all others on 1 row using BLast

void
TCluster::Ccbc_Mt(double p_Threshold)
{       
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	Ccbc_Mt(p_Threshold, 0, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);
}


void
TCluster::Ccbc_Mt(double p_Threshold, uint32_t p_First, uint32_t p_Last)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	p_First = std::max(0u, p_First);
	p_Last = std::min(p_Last, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);

	//using namespace boost::numeric::ublas;
	vector<uint32_t> idxList;
	idxList.reserve(p_Last - p_First + 1);
	for (uint32_t i = p_First; i <= p_Last; ++i) {
		idxList.push_back(i);
	}
	Ccbc_Mt(p_Threshold, idxList);
}


void
TCluster::Ccbc_Mt(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	m_Groups.clear();
	m_IsSortedGroups = false;

	if (m_ClusterDB->m_Sequences.size() == 0 || p_IdxList.size() == 0) {
		return;
	}
	//vector<uint32_t> idxList(p_IdxList);	//	a non-const copy

	uint32_t imax = static_cast<uint32_t>(p_IdxList.size());
	uint32_t numThread = max_(1u, m_NumThread);

	//	create the thread context
	ccbc_context ctx(0, *this, p_IdxList, p_Threshold);
	ctx.m_Sims.resize(numThread * m_SeqPerThread, imax);
	ctx.m_GroupIndexes.resize(imax, UINT32_MAX);

	//	call InitRef() on all sequences
	if (numThread == 1) {
		OperatorCcbc1(0, &ctx);
	}
	else {
		vector<std::thread>	threads;
		threads.reserve(numThread);

		for (uint32_t i = 0; i < numThread; ++i) {
			threads.emplace_back(::OperatorCcbc1, i, &ctx);
		}
		//	catch each thread results and wait for all threads to finish
		for (std::thread & item : threads) {
			item.join();
		}
		//	clear all existing threads
		threads.clear();
	}
        
        CreateBLastInputFiles(&ctx,numThread);//create input files if using blast
         
        //	Compare numThread rows of sequences
	//	reset the context start index
	ctx.m_NextIdx = 0;
	//	compute the similarity of the next numThread for all reference sequences that are not yet in a group
	while (ctx.m_NextIdx < imax) {
		//for (uint32_t firsti = 0; firsti < imax; firsti += numThread * m_SeqPerThread) {

		uint32_t firsti = ctx.m_NextIdx;
		if (numThread == 1) {          
                    OperatorCcbc2_Blast(0, &ctx); //if use Blast function
		}
		else {
			vector<std::thread>	threads;
			threads.reserve(numThread);
			for (uint32_t i = 0; i < numThread; ++i) {
                            threads.emplace_back(::OperatorCcbc2_Blast, i, &ctx);  //if use Blast function
			}
			//	catch each thread results and wait for all threads to finish
			for (std::thread & item : threads) {
				item.join();
			}
			//	clear all existing threads
			threads.clear();
		}
             ////   ReadBlastResults(&ctx);//read blast results and delete input files for blast

		//	typically : the 8 thread have filled in 80 rows with the similarities
		uint32_t simRowIdx = 0;
		//	get the comparison results and create the groups
		for (uint32_t i = firsti; i < ctx.m_NextIdx; ++i) {
			uint32_t s = p_IdxList[i];

			//	if that sequence is already in a group, use that group
			uint32_t g;	//	the group index
			if (ctx.m_GroupIndexes[i] != UINT32_MAX) { // found
				g = ctx.m_GroupIndexes[i];
			}
			else {	//	put that sequence in a new group
				m_Groups.emplace_back(m_ClusterDB, s);
				g = static_cast<uint32_t>(m_Groups.size()) - 1;	//	the group index
				ctx.m_GroupIndexes[i] = g;
			}
			TCluster & group = m_Groups[g];
			//	put in that group all other sequences that are close enough
			for (uint32_t j = i+1; j < imax; ++j) {
				if (ctx.m_GroupIndexes[j] != UINT32_MAX) { // already in a group, do nothing
					continue;
				}
				uint32_t r = p_IdxList[j];
				double sim = ctx.m_Sims[simRowIdx][j];  
                            
				if (sim >= p_Threshold) { //	put it in this group
					group.Comparisons().emplace_back(r, s, sim);
					//	and memorize that this sequence is in that group
					ctx.m_GroupIndexes[j] = g;
				}
			}
                        simRowIdx = simRowIdx + 1;
		}
	}
        DeleteBLastInputFiles(&ctx); //delete blast inputfile
	//OutputDebug();
	return;
}


//	first step of the MLC algorithm:
//	- dispatch the sequences into blocs of m_SeqPerBloc sequences
//	- GClust each bloc and, in each group created by GClust, compute the centroid
//	- add each centroid to the global list of centroids
void
OperatorMlc1(uint32_t p_Id, mlc_context1 * ctx)
{
	//const uint32_t seqSize = static_cast<uint32_t>(ctx->m_Cluster.Sequences().size());
	const uint32_t idxSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	//const uint32_t seqNo = ctx->m_Cluster.m_SeqPerBloc;
	const uint32_t seqNo = max((uint32_t)(ctx->m_IdxList.size()/ ctx->m_Cluster.m_BlocNo), ctx->m_Cluster.m_SeqPerThread);

	//	create a temporary list of centroids. Accessing directly the main centroid list would need to lock and unlock the mutex once for each sequence !
	vector<uint32_t> centroidList;
	centroidList.reserve(2000 + static_cast<uint32_t>(ctx->m_CentroidList.capacity() / ctx->m_NumThread));	//	supposing each thread will manage the same number of groups
        
        vector<TCluster> groups;
	groups.reserve(2000 + static_cast<uint32_t>(ctx->m_CentroidList.capacity() / ctx->m_NumThread));
        
	while (true) {
		ctx->m_IdxMutex.lock();										//	lock the indexes

		uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process
		uint32_t end = std::min(first + seqNo, idxSize);	//	compute the index of the next sequence to be process by the next thread
		ctx->m_NextIdx = end;										//	update the next index in the context

		ctx->m_IdxMutex.unlock();									//	unlock the indexes

		int32_t len = end - first;									//	compute the real end, in case we have reached the end of the indexes
		if (len == 0) {												// check if job is finisheds
			break;
		}
		TCluster cluster(ctx->m_Cluster.ClusterDatabase());		//	create a shallow copy of the main cluster

		vector<uint32_t> idxList;
		idxList.reserve(len);
		for (uint32_t j = first; j < end; ++j) {
			idxList.push_back(ctx->m_IdxList[j]);
		}
                //cluster.GClust_St(ctx->m_Threshold, idxList);			//	GClust the current bloc  
                cluster.Ccbc_St(ctx->m_Threshold, idxList);	

		for (auto & group : cluster.Groups()) {		//	for each group, compute the centroid
			group.ComputeCentroid();
			group.PrepareCentroid();
			centroidList.push_back(group.CentralSeqIdx());	//	add the centroid to the local centroid list
                        groups.push_back(group);
		}
	} // next bloc of 'seqNo' sequencess

	  //	copy our centroids into the global list
	ctx->m_CentroidMutex.lock();	//lock the indexes
        uint32_t i =0; 	
	for (uint32_t idx : centroidList) {
		ctx->m_CentroidList.push_back(idx);
                ctx->m_Groups.push_back(groups[i]);
                i=i+1;
	}
	ctx->m_CentroidMutex.unlock();				//	unlock the indexes
}


//	second step of the MLC algorithm:
//	- dispatch all sequences into the groups represented by each centroid
//	- keep in mind all sequences belonging to no group
void
OperatorMlc2(uint32_t p_Id, mlc_context2 * ctx)
{
	//const uint32_t seqSize = static_cast<uint32_t>(ctx->m_Cluster.Sequences().size());
	const uint32_t idxSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
	const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();		//	simplify coding

	while (true) {
		ctx->m_IdxMutex.lock();										//	lock the indexes

		uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process
		uint32_t end = std::min(first + seqNo, idxSize);	//	compute the index of the next sequence to be process by the next thread
		ctx->m_NextIdx = end;										//	update the next index in the context

		ctx->m_IdxMutex.unlock();									//	unlock the indexes

		int32_t len = end - first;									//	compute the real end, in case we have reached the end of the indexes
		if (len == 0) {												// check if job is finisheds
			break;
		}

		//	scan the sequences
		for (uint32_t j = first; j < end; ++j) {				//	r is the index in m_Sequences
			uint32_t r = ctx->m_IdxList[j];
			TNFieldBase * ref = sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true);									//	we consider this one as a reference sequence, the centroid is the source, which is heavier to pre-compute
			}
			bool groupFound = false;
                        
			//	search in which group we will put it
			for (uint32_t g = 0, gmax = static_cast<uint32_t>(ctx->m_Cluster.Groups().size()); g < gmax; ++g) {
				TCluster & group = ctx->m_Cluster.Groups()[g];
				uint32_t s = group.CentralSeqIdx();
				if (r == s) {											//	as all groups have already been created, we don't need to add the centroids to their own group
					groupFound = true;
					break;
				}
				TNFieldBase * srce = sequences[s];
				double sim = ctx->m_Cluster.Compare(srce, ref);
				if (sim >= ctx->m_Threshold) {					//	put it in this group
					ctx->m_MutexList[g]->lock();					//	each group has its own mutex created on the fly, so waiting will nearly never happen as two thread will nearly never add sequences to the same group at the same time
					group.Comparisons().emplace_back(r, s, sim);
					ctx->m_MutexList[g]->unlock();
					groupFound = true;
					break;
				}
			} // next group

			if (!groupFound) { //	keep in mind unclassified sequence indexes
				ctx->m_ScrapMutex.lock();
				ctx->m_ScrapList.push_back(r);
				ctx->m_ScrapMutex.unlock();
			}
		} // next sequence
	} // next bloc of 'seqNo' sequences
}



void
TCluster::Mlc_Mt(double p_Threshold)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}      
	Mlc_Mt(p_Threshold, 0, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);
}


void
TCluster::Mlc_Mt(double p_Threshold, uint32_t p_First, uint32_t p_Last)
{
	if (m_ClusterDB->m_Sequences.size() == 0) {
		return;
	}
	p_First = std::max(0u, p_First);
	p_Last = std::min(p_Last, static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1);

	vector<uint32_t> idxList;
	idxList.reserve(p_Last - p_First + 1);
	for (uint32_t i = p_First; i <= p_Last; ++i) {
		idxList.push_back(i);
	}
	Mlc_MultiThread(p_Threshold, idxList);
}


//	the multithreaded version of Multi-level clustering (MLC)
void
TCluster::Mlc_Mt(double p_Threshold, const vector<uint32_t> & p_IdxList)
{
	vector<uint32_t> idxSortedList(p_IdxList);	//	a non-const copy
	std::sort(idxSortedList.begin(), idxSortedList.end());
	Mlc_MultiThread(p_Threshold, idxSortedList);
}


//	the multithreaded version of Multi-level clustering (MLC)
//	p_IdxSortedList must be sorted by Idx
void
TCluster::Mlc_MultiThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList)
{       
	m_Groups.clear();
	m_IsSortedGroups = false;

	if(p_IdxSortedList.size() == 1) {
		m_CentralSeqIdx = p_IdxSortedList.front();
		return;
	}
	//if (m_ClusterDB->m_Sequences.size() < 3 * m_SeqPerBloc || p_IdxSortedList.size() < 3 * m_SeqPerBloc) { //	doesn't make sense to run a MLC for a few sequences. I don't want to manage sequences spread over 1 bloc...
	//	GClust_St(p_Threshold, p_IdxSortedList);
	//	return;
	//}
        
	if (m_ClusterDB->m_Sequences.size() < m_ClusterDB->m_MinSeqNoForMLC || p_IdxSortedList.size() < m_ClusterDB->m_MinSeqNoForMLC) { //	doesn't make sense to run a MLC for a few sequences. I don't want to manage sequences spread over 1 bloc...
		Ccbc_Mt(p_Threshold, p_IdxSortedList);
		return;
	}
	
	//	securities
	p_Threshold = std::max(0.0, std::min(1.0, p_Threshold));

	std::wostringstream stream;
	/*stream << L"\r\n ***** MLC(" << p_Threshold << L", " << m_SeqPerBloc << L") *****\r\n";*/
	stream << L"\r\n ***** MLC(" << p_Threshold << L", " << m_BlocNo << L") *****\r\n";
        
        
	uint32_t numThread = std::max(1u, m_NumThread);

	//	create the thread context
	mlc_context1 context(*this, p_IdxSortedList, p_Threshold, numThread);

	//	Divide the sequences into blocs of m_SeqPerBloc sequences
	//	For each bloc, GClust and find the centroid sequences

	if (numThread == 1) {
		OperatorMlc1(0, &context);
	}
	else {
		vector<std::thread>	threads;
		threads.reserve(numThread);

		for (uint32_t i = 0; i < numThread; ++i) {
                    threads.emplace_back(::OperatorMlc1, i, &context);
		}
		//	catch each thread results and wait for all threads to finish
		for (std::thread & item : threads) {
			item.join();
		}
		//	clear all existing threads
		threads.clear();
	}
        //create a temp list of groups to store all the groups of the block
        vector<TCluster> & tempgroups = context.m_Groups;
	//	make a list of all centroid computed by all threads, sorted and without duplicates
        
	//	get the list of centroids
	vector<uint32_t> & centroidList = context.m_CentroidList;

	std::sort(centroidList.begin(), centroidList.end());

	//	cluster the centroid sequences
	Ccbc_Mt(p_Threshold, centroidList);
        
        //look for the final group of the temp groups
        uint32_t i = 0;
        vector<uint32_t> groupidxList;
        for(TCluster & tempgroup : tempgroups) {
            uint32_t r = tempgroup.CentralSeqIdx();
            //look for the final group of r
            uint32_t i = 0;
            for(TCluster & group : m_Groups) {
                if (r==group.CentralSeqIdx()){
                    groupidxList.push_back(i);
                    break;
                }
		if (group.Comparisons().size() > 0) {
                    for (const TComparison & comp : group.Comparisons()) {
                        uint32_t s = comp.SrceIdx();
                        if (r==s){
                            groupidxList.push_back(i);
                            break;
                        }
		    }
                } 
                for (uint32_t id: group.IdList()){
                    if (r==id){
                            groupidxList.push_back(i);
                            break;
                    }
                }
                i=i+1;
	    }
            
        }
        //merge temp groups in to thefinal groups
        i = 0;
        for(TCluster & tempgroup : tempgroups) {
            vector<uint32_t> reflist;
            vector<uint32_t> srcelist;
            TCluster & group = m_Groups[groupidxList[i]];
            uint32_t cenidx = group.CentralSeqIdx();
            reflist.push_back(group.CentralSeqIdx());
            if (group.Comparisons().size() > 0) {
                for (const TComparison & comp : group.Comparisons()) {
                        uint32_t s = comp.SrceIdx();
                        if(std::find(reflist.begin(), reflist.end(), s) == reflist.end()) {
                            reflist.push_back(s);
                        }
                         uint32_t r = comp.RefIdx();
                        if(std::find(reflist.begin(), reflist.end(), s) == reflist.end()) {
                            reflist.push_back(r);
                        }
		}
                for (uint32_t id: group.IdList()){
                    if(std::find(reflist.begin(), reflist.end(), id) == reflist.end()) {
                        reflist.push_back(id);
                    }
                }
            } 
            
            if (tempgroup.Comparisons().size() > 0) {
                for (const TComparison & comp : tempgroup.Comparisons()) {
                    uint32_t s = comp.SrceIdx();
                    if(std::find(reflist.begin(), reflist.end(), s) == reflist.end() && std::find(srcelist.begin(), srcelist.end(), s) == srcelist.end()) {
                         group.Comparisons().emplace_back(s, cenidx, comp.Sim());		
                         reflist.push_back(s);
                    }  
                    uint32_t r = comp.RefIdx();
                    if(std::find(reflist.begin(), reflist.end(), r) == reflist.end() && std::find(srcelist.begin(), srcelist.end(), r) == srcelist.end()) {
                         group.Comparisons().emplace_back(r, cenidx, comp.Sim());    
                         reflist.push_back(r);
                    } 
                } 
                for (uint32_t id: tempgroup.IdList()){
                    if (std::find(reflist.begin(), reflist.end(), id) == reflist.end() && std::find(srcelist.begin(), srcelist.end(), id) == srcelist.end()) {
                        group.Comparisons().emplace_back(id, cenidx, 1);
                        reflist.push_back(id);
                    }
                }
	    }
            i=i+1;
        }
        
        return;
/*
	//stream << L"There is " << centroidList.size() << L" centroids\r\n";
	//stream << L"Sorted centroids :\r\n";
	//sz::OutputDebugVector(stream, centroidList);

	//stream << L"There is " << m_Groups.size() << L" groups after clustering the centroids\r\n";
	//OutputDebugGroups(stream);

	//	don't scan the centroids any more, they are already either a group centroid, or moved into another group.
	RemoveFromList(p_IdxSortedList, centroidList);

	//stream << L"Index list without centroids :\r\n";
	//sz::OutputDebugVector(stream, p_IdxSortedList);

	//	now dispatch all sequences into the created groups
	//	keep in mind the sequences not going into any group

	//	create the thread
	mlc_context2 context2(*this, p_IdxSortedList, p_Threshold, m_NumThread);

	if (numThread == 1) {
		OperatorMlc2(0, &context2);
	}
	else {
		vector<std::thread>	threads;
		threads.reserve(numThread);

		for (uint32_t i = 0; i < numThread; ++i) {
			threads.emplace_back(::OperatorMlc2, i, &context2);
		}
		//	catch each thread results and wait for all threads to finish
		for (std::thread & item : threads) {
			item.join();
		}
		//	clear all existing threads
		threads.clear();
	}

	//	this is the list of all unclassified sequences
	uint32_t scrapNo = static_cast<int32_t>(context2.m_ScrapList.size());
	int32_t g_min = static_cast<int32_t>(m_Groups.size());

	vector<uint32_t> & idxList = context2.m_ScrapList;	//	simplify code
																		//	sort the remaining sequence indexes by decreasing size of the sequences, otherwise the order we create these groups will vary from one computation to another, as the multiple thread give a slightly different list on each run
																		//clustering::OnSequenceSizeDescending_int32 SeqSort(m_Sequences);
	std::sort(idxList.begin(), idxList.end());// , SeqSort);

	uint32_t missingGroupNo = 0;
	//	add the next remaining sequence into a new group
	if (idxList.size() > 0) {
		TNFieldBase * srce0 = m_ClusterDB->m_Sequences[idxList.front()];
		if (!srce0->SrceReady()) {	//	they are already checked
			srce0->InitSrce(ReverseComplement());
		}

		//	scan the remaining sequences
		for (uint32_t r : idxList) {
			TNFieldBase * ref = m_ClusterDB->m_Sequences[r];
			if (!ref->RefReady()) {
				ref->InitRef(true); //	we consider this one as a reference sequence
			}
			bool groupFound = false;
			//	search in which group we will put it
			for (int32_t g = g_min, gmax = static_cast<int32_t>(m_Groups.size()); g < gmax; ++g) {
				TCluster & group = m_Groups[g];
				uint32_t s = group.CentralSeqIdx();
				if (r == s) {
					groupFound = true;
					break;
				}
				TNFieldBase * srce = m_ClusterDB->m_Sequences[s];
				double sim = Compare(srce, ref);
				if (sim >= p_Threshold) { //	put it in this group
												  //group.AddComparison(TComparison(r, s, sim));
					group.Comparisons().emplace_back(r, s, sim);
					groupFound = true;
					break;
				}
			}
			if (!groupFound) { //	create a new group to hold this new sequences
				m_Groups.emplace_back(m_ClusterDB, r);
				if (!ref->SrceReady()) {
					ref->InitSrce(ReverseComplement());
				}
				++missingGroupNo;
			}
		}
	}
	stream << L"Missing seq number: " << scrapNo << L"\r\n";
	stream << L"Missing group number: " << missingGroupNo << L"\r\n";

	//OutputDebugString(stream.str().c_str());
*/
}


void
OperatorFMeasure(uint32_t p_Id, f_context * ctx)
{
	const uint32_t groupSize = static_cast<uint32_t>(ctx->m_RefCluster.Groups().size());
	const uint32_t groupNo = ctx->m_RefCluster.m_SeqPerThread;

	double f = 0.0;							//	the local summ
	size_t itemNo = 0;

	while (true) {
		ctx->m_IdxMutex.lock();											//	lock the indexes

		uint32_t first = ctx->m_NextIdx;								//	read the index of the first group to process
		uint32_t end = std::min(first + groupNo, groupSize);	//	compute the index of the next group to be process by the next thread
		ctx->m_NextIdx = end;											//	update the next index in the context

		ctx->m_IdxMutex.unlock();										//	unlock the indexes

		int32_t len = end - first;				//	compute the real end, in case we have reached the end of the groups
		if (len == 0) {							// check if job is finisheds
			break;
		}

		for (uint32_t g = first; g < end; ++g) {
			const TCluster & refGroup = ctx->m_RefCluster.Groups()[g];
			const vector<uint32_t > & refIdList = refGroup.IdList();
			//	search for the group in the given cluster that seems to be the current group
			double x = 0.0;
			for (const auto & group : ctx->m_Cluster.Groups()) {
				const vector<uint32_t > & srceIdList = group.IdList();
				size_t num = CountIntersection(srceIdList, refIdList);
				if (num == 0) {	//	most common case first
					continue;
				}
				size_t denom = srceIdList.size() + refIdList.size();
				x = std::max(x, 2.0 * num / denom);
				if (x == 1.0) {	//	cannot be better
					break;
				}
			}
			f += refIdList.size() * x;
			itemNo += refIdList.size();
		} // next group
	} // next bloc of 'seqNo' sequences

	  //	add our F-Measure to the global value
	ctx->m_ResultMutex.lock();
	ctx->m_F += f;
	ctx->m_ItemNo += itemNo;
	ctx->m_ResultMutex.unlock();
}


//	return the F-Measure between this clusters and the given reference cluster
//	WARNING: this function is valid only for One-level clustering
double
TCluster::F_Measure_Mt(TCluster & p_RefCluster)
{
	if (p_RefCluster.m_ClusterDB->m_Sequences.size() == 0) {
		return 0.0;
	}

	if (!m_IsSortedGroups) {
		SortAllGroups();
	}
	if (!p_RefCluster.m_IsSortedGroups) {
		p_RefCluster.SortAllGroups();
	}

	uint32_t numThread = max_(1u, m_NumThread);

	//	create the thread context
	f_context context(*this, p_RefCluster, numThread);

	//	Divide the groups into blocs of m_SeqPerThread groups
	if (numThread == 1) {
		OperatorFMeasure(0, &context);
	}
	else {
		vector<std::thread>	threads;
		threads.reserve(numThread);

		for (uint32_t i = 0; i < numThread; ++i) {
			threads.emplace_back(::OperatorFMeasure, i, &context);
		}
		//	catch each thread results and wait for all threads to finish
		for (std::thread & item : threads) {
			item.join();
		}
		//	clear all existing threads
		threads.clear();
	}

	double f = context.m_F / context.m_ItemNo; //	the F-Measure representing the quality of p_Groups compared to the reference groups p_RefGroups
	return std::min(1.0, f); //	protect against rounding errors
}



//	very similar to OperatorCcbc1
//	call InitRef() on all sequences()
void
OperatorInitSrceRef(uint32_t p_Id, ccbc_context * ctx)
{
	const uint32_t seqSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
	const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();

	while (true) {
		ctx->m_IdxMutex.lock();										//	lock the indexes

		uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process
		uint32_t end = std::min(first + seqNo, seqSize);	//	compute the index of the next sequence to be process by the next thread
		ctx->m_NextIdx = end;										//	update the next index in the context

		ctx->m_IdxMutex.unlock();									//	unlock the indexes

		int32_t len = end - first;				//	compute the real end, in case we have reached the end of the sequences
		if (len == 0) {							// check if job is finisheds
			break;
		}

		auto sit_first = ctx->m_IdxList.begin() + first;		//	get the first row to compute
		auto sit_end = ctx->m_IdxList.begin() + end;

		//	scan the sequences
		for (auto sit = sit_first; sit < sit_end; ++sit) {			//	s is the index in m_IdxList
			uint32_t s = *sit;												//	the index in the sequence list
			TNFieldBase * srce = sequences[s];			//	the reference sequence
			if (!srce->SrceReady()) {
				srce->InitSrce(ctx->m_Cluster.ReverseComplement());
			}
			if (!srce->RefReady()) {
				srce->InitRef(true);
			}
		} // next sequence
	} // next bloc of 'seqNo' sequences
}



//	clusterize all sequences given by index list p_IdxList, but only in the ranges given by the context
void
OperatorCompareAll(uint32_t p_Id, ccbc_context * ctx)
{
	const uint32_t seqSize = static_cast<uint32_t>(ctx->m_IdxList.size());
	const uint32_t seqNo = ctx->m_Cluster.m_SeqPerThread;
	const vector<TNFieldBase *> & sequences = ctx->m_Cluster.Sequences();

	while (true) {
		ctx->m_IdxMutex.lock();										//	lock the indexes

		uint32_t first = ctx->m_NextIdx;							//	read the index of the first sequence to process
		uint32_t end = std::min(first + seqNo, seqSize);	//	compute the index of the next sequence to be process by the next thread
		ctx->m_NextIdx = end;										//	update the next index in the context

		ctx->m_IdxMutex.unlock();									//	unlock the indexes

		int32_t len = end - first;				//	compute the real end, in case we have reached the end of the sequences
		if (len == 0) {							// check if job is finisheds
			break;
		}

		//	scan the sequences
		for (uint32_t i = first, imax = ctx->m_NextIdx; i < imax; ++i) {
			uint32_t s = ctx->m_IdxList[i];			//	the index in the sequence list
			TNFieldBase * srce = sequences[s];			//	the source sequence
			ctx->m_Sims[i][i] = 1.0;

			//	put in that group all other sequences that are close enough
			for (uint32_t j = i + 1; j < seqSize; ++j) {
				uint32_t r = ctx->m_IdxList[j];
				TNFieldBase * ref = sequences[r];

				double sim = ctx->m_Cluster.Compare(srce, ref);
				ctx->m_Sims[i][j] = ctx->m_Sims[j][i] = sim;
			}
		} // next sequence

	} // next bloc of 'seqNo' sequences
}

void
TCluster::SaveFullSimilarity( std::string p_DestFilePath, double p_MinSim)
{
	//	Save results into a cluster file
   // char mbstr[11];
   // std::wcstombs(mbstr, p_DestFilePath, 11);
    std::ofstream file(p_DestFilePath.c_str(), std::ofstream::binary);
    if (file.fail()) {
	return; //	error
    }
    if (m_ClusterDB->m_Sequences.size() == 0) {
	return;
    }
    uint32_t smax = static_cast<uint32_t>(m_ClusterDB->m_Sequences.size());
	//using namespace boost::numeric::ublas;
    vector<uint32_t> idxList;
    idxList.reserve(smax);
    for (uint32_t i = 0; i < smax; ++i) {
	idxList.push_back(i);
    }
	//compare all  with each other
	//	step 2 : call InitSrce and InitRef on all sequences
    uint32_t numThread = max_(1u, m_NumThread);
   	//	create the thread context
    ccbc_context ctx(0, *this, idxList, 0.0);
    //ctx.m_Sims.resize(smax, smax);
    ctx.m_Sims.resize(numThread * m_SeqPerThread, smax);
    if (numThread == 1) {
		//OperatorInitSrceRef(0, &ctx);
        OperatorCcbc1(0, &ctx);
    }
    else {
	vector<std::thread>	threads;
	threads.reserve(numThread);

	for (uint32_t i = 0; i < numThread; ++i) {
                    //threads.emplace_back(::OperatorInitSrceRef, i, &ctx);
            threads.emplace_back(::OperatorCcbc1, i, &ctx);
	}
		//	catch each thread results and wait for all threads to finish
	for (std::thread & item : threads) {
            item.join();
	}
		//	clear all existing threads
        threads.clear();
    }

    CreateBLastInputFiles(&ctx,numThread);//create input files if using blast

	//	reset the context start index
    ctx.m_NextIdx = 0;
    //	compute the similarity of the next numThread for all reference sequences that are not yet in a group
    while (ctx.m_NextIdx < smax) {
		//for (uint32_t firsti = 0; firsti < imax; firsti += numThread * m_SeqPerThread) {
        uint32_t firsti = ctx.m_NextIdx;
        if (numThread == 1) {
            //OperatorCompareAll(0, &ctx);
           OperatorCcbc2_Blast(0, &ctx); //if use Blast function
         }   
         else {
            vector<std::thread>	threads;
            threads.reserve(numThread);

            for (uint32_t i = 0; i < numThread; ++i) {
                    //threads.emplace_back(::OperatorCompareAll, i, &ctx);
                threads.emplace_back(::OperatorCcbc2_Blast, i, &ctx);
             }
		//	catch each thread results and wait for all threads to finish
            for (std::thread & item : threads) {
                item.join();
            }
		//	clear all existing threads
            threads.clear();
        }

        // ctx.m_Sims[smax-1][smax-1]=1;												//double_matrix & sims = ctx.m_Sims;			//	simplify coding
    
        //for (uint32_t i = 0; i < smax; ++i) {
        uint32_t simRowIdx = 0;
        for (uint32_t i = firsti; i < ctx.m_NextIdx; ++i) {
            TNFieldBase * srce = ClusterDatabase()->m_Sequences[idxList[i]];
            uint32_t s = srce->RecordId();
            for (uint32_t j = i; j < smax; ++j) {
                TNFieldBase * ref = ClusterDatabase()->m_Sequences[idxList[j]];
                uint32_t r = ref->RecordId();
                //double sim = max(ctx.m_Sims[i][j], ctx.m_Sims[j][i]);
                double sim = ctx.m_Sims[simRowIdx][j];
                if (sim >= p_MinSim) {
                    file << r << " " << s << " " << std::to_string(sim) << "\r\n";
                }
            }   
            simRowIdx = simRowIdx+1;
        } 
    }
	/*TNFieldBase * srce = ClusterDatabase()->m_Sequences[idxList[smax-1]];
	uint32_t s = srce->RecordId();
	file << s << " " << s << " 1" << "\r\n";*/
    DeleteBLastInputFiles(&ctx); //delete blast inputfile
    bool isError = (file.goodbit != 0);
    file.close();
	//	return isError;
}

void
TCluster::SaveSimilarity(std::ostream & p_Stream, vector<uint32_t> &p_idxList, int32_t p_TabNo, int32_t p_KneighborNo,  double p_MinSim, double p_Threshold, int32_t & p_Count, int32_t p_Max )
{
    if (p_TabNo > 8) {
	p_TabNo = 8;
    }
    static const char tabs_array[][18] = { "\r\n", "\r\n\t", "\r\n\t\t", "\r\n\t\t\t", "\r\n\t\t\t\t", "\r\n\t\t\t\t\t", "\r\n\t\t\t\t\t\t", "\r\n\t\t\t\t\t\t\t", "\r\n\t\t\t\t\t\t\t\t" };
    const char * begin_line = tabs_array[p_TabNo];
    BuildSortedIdList();
    if (m_Groups.size() > 0) {
        uint32_t numThread = max_(1u, m_NumThread);
            //	create the thread context
        ccbc_context ctx(0, *this, m_IdList, 0.0);
            uint32_t smax = m_IdList.size();
        //ctx.m_Sims.resize(smax, smax);
        ctx.m_Sims.resize(numThread * m_SeqPerThread, smax);
        if (numThread == 1) {
		//OperatorInitSrceRef(0, &ctx);
            OperatorCcbc1(0, &ctx);
        }
        else {
            vector<std::thread>	threads;
            threads.reserve(numThread);

            for (uint32_t i = 0; i < numThread; ++i) {
                //threads.emplace_back(::OperatorInitSrceRef, i, &ctx);
                threads.emplace_back(::OperatorCcbc1, i, &ctx);
            }
		//	catch each thread results and wait for all threads to finish
            for (std::thread & item : threads) {
                item.join();
            }
		//	clear all existing threads
            threads.clear();
        }

        CreateBLastInputFiles(&ctx,numThread);//create input files if using blast
        	//	reset the context start index
        ctx.m_NextIdx = 0;        
        //	compute the similarity of the next numThread for all reference sequences that are not yet in a group
        while (ctx.m_NextIdx < smax) {
		//for (uint32_t firsti = 0; firsti < imax; firsti += numThread * m_SeqPerThread) {
            uint32_t firsti = ctx.m_NextIdx;
            if (numThread == 1) {
                //OperatorCompareAll(0, &ctx);
                OperatorCcbc2_Blast(0, &ctx); //if use Blast function
            }
            else {
                vector<std::thread>	threads;
                threads.reserve(numThread);
                for (uint32_t i = 0; i < numThread; ++i) {
                    //threads.emplace_back(::OperatorCompareAll, i, &ctx);
                    threads.emplace_back(::OperatorCcbc2_Blast, i, &ctx);
                }
                //	catch each thread results and wait for all threads to finish
                for (std::thread & item : threads) {
                    item.join();
                }
                //clear all existing threads
                threads.clear();
            }
            //ctx.m_Sims[smax-1][smax-1] = 0;												
            //for (uint32_t i = 0; i < smax; ++i) {
            uint32_t simRowIdx = 0;
            for (uint32_t i = firsti; i < ctx.m_NextIdx; ++i) {
                TNFieldBase * srce = ClusterDatabase()->m_Sequences[m_IdList[i]];
                uint32_t s = srce->RecordId();
                for (uint32_t j = i; j < smax; ++j) {
                    TNFieldBase * ref = ClusterDatabase()->m_Sequences[m_IdList[j]];
                    uint32_t r = ref->RecordId();
                    //double sim = max(ctx.m_Sims[i][j], ctx.m_Sims[j][i]);
                    double sim = ctx.m_Sims[simRowIdx][j];
                    if (sim >= p_MinSim) {
                        p_Stream << r << " " << s << " " << std::to_string(sim) << "\r\n";
                    }
                }
                simRowIdx = simRowIdx + 1;
            }  
        }    
        DeleteBLastInputFiles(&ctx); //delete blast inputfile
		//recursively
        for ( TCluster & group : m_Groups) {
            group.SaveSimilarity(p_Stream,  p_idxList,p_TabNo + 1,  p_KneighborNo, p_MinSim, p_Threshold,p_Count, p_Max );
        }   
    }
    else {		
        BuildSortedIdList();
		//compare sequences of the group
        if (m_IdList.size() > 1) {
            uint32_t numThread = max_(1u, m_NumThread);
                //	create the thread context
            ccbc_context ctx(0, *this, m_IdList, 0.0);
            uint32_t smax = m_IdList.size();
            //ctx.m_Sims.resize(smax, smax);
            ctx.m_Sims.resize(numThread * m_SeqPerThread, smax);
            if (numThread == 1) {
                    //OperatorInitSrceRef(0, &ctx);
                OperatorCcbc1(0, &ctx);
            }
            else {
                vector<std::thread>	threads;
                threads.reserve(numThread);

                for (uint32_t i = 0; i < numThread; ++i) {
                        //threads.emplace_back(::OperatorInitSrceRef, i, &ctx);
                    threads.emplace_back(::OperatorCcbc1, i, &ctx);
                }
                    //	catch each thread results and wait for all threads to finish
                for (std::thread & item : threads) {
                    item.join();
                }
                    //	clear all existing threads
                threads.clear();
            }

            CreateBLastInputFiles(&ctx,numThread);//create input files if using blast
        	//	reset the context start index
            ctx.m_NextIdx = 0;          
             //	compute the similarity of the next numThread for all reference sequences that are not yet in a group
            while (ctx.m_NextIdx < smax) {
                uint32_t firsti = ctx.m_NextIdx;
                if (numThread == 1) {
                    //OperatorCompareAll(0, &ctx);
                   OperatorCcbc2_Blast(0, &ctx); //if use Blast function
                }
                else {
                    vector<std::thread>	threads;
                    threads.reserve(numThread);
                    for (uint32_t i = 0; i < numThread; ++i) {
                        //threads.emplace_back(::OperatorCompareAll, i, &ctx);
                        threads.emplace_back(::OperatorCcbc2_Blast, i, &ctx);
                    }
                     //	catch each thread results and wait for all threads to finish
                    for (std::thread & item : threads) {
                        item.join();
                    }
                     //clear all existing threads
                    threads.clear();
                 }
                //ctx.m_Sims[smax-1][smax-1] = 0;												
                uint32_t simRowIdx = 0;
                for (uint32_t i = firsti; i < ctx.m_NextIdx; ++i) {
                    TNFieldBase * srce = ClusterDatabase()->m_Sequences[m_IdList[i]];
                    uint32_t s = srce->RecordId();
                    for (uint32_t j = i; j < smax; ++j) {
                        TNFieldBase * ref = ClusterDatabase()->m_Sequences[m_IdList[j]];
                        uint32_t r = ref->RecordId();
                        //double sim = max(ctx.m_Sims[simRowIdx][j], ctx.m_Sims[j][simRowIdx]);
                        double sim = ctx.m_Sims[simRowIdx][j];
                        if (sim >= p_MinSim) {
                            p_Stream << r << " " << s << " " << std::to_string(sim) << "\r\n";
                        }
                        if ((p_Count < p_Max) && (sim < p_Threshold)) {                       
                            if (m_IdList[i] != m_CentralSeqIdx && (p_idxList.size() == 0 || std::find(p_idxList.cbegin(), p_idxList.cend(), r) == p_idxList.cend())) {
                                p_idxList.push_back(r); //list of centrality idxes of each sub group
                                p_Count = p_Count + 1;
                            }
                            if ((m_IdList[j] != m_CentralSeqIdx) && (p_idxList.size() == 0 || std::find(p_idxList.cbegin(), p_idxList.cend(), s) == p_idxList.cend())) {
                                p_idxList.push_back(s); //list of centrality idxes of each sub group
                                p_Count = p_Count + 1;
                            }
                        }
                    }
                    simRowIdx=simRowIdx + 1;
                }
            }
            DeleteBLastInputFiles(&ctx);    									
        }
        else{
            TNFieldBase * srce = ClusterDatabase()->m_Sequences[m_CentralSeqIdx];
            uint32_t s = srce->RecordId();
            p_Stream << s << " " << s << " " << "1" << "\r\n";
        }
    }
}

void
TCluster::SaveSimilarity(std::string p_DestFilePath, int32_t p_KneighborNo, double p_MinSim, double p_Threshold, int32_t p_Max)
{
    std::ofstream file(p_DestFilePath.c_str(), std::ofstream::binary);
    if (file.fail()) {
	return; //	error
    }
    if (m_ClusterDB->m_Sequences.size() == 0) {
	return;
    }
    uint32_t p_First = 0;
    uint32_t p_Last = static_cast<uint32_t>(m_ClusterDB->m_Sequences.size()) - 1;
    vector<uint32_t> idxList;
    idxList.reserve(p_Last - p_First + 1);
    int32_t p_Count = 0;
    SaveSimilarity(file,  idxList,10,  p_KneighborNo, p_MinSim, p_Threshold, p_Count,p_Max);
    if (idxList.size() == 0){
	return;
    }
	//compare all  with each other
	//	step 2 : call InitSrce and InitRef on all sequences
    uint32_t numThread = max_(1u, m_NumThread);
    uint32_t smax = static_cast<uint32_t>(idxList.size());
	//	create the thread context
    ccbc_context ctx(0, *this, idxList, 0.0);
    //ctx.m_Sims.resize(smax, smax);
    ctx.m_Sims.resize(numThread * m_SeqPerThread, smax);
    if (numThread == 1) {
            //OperatorInitSrceRef(0, &ctx);
        OperatorCcbc1(0, &ctx);
    }
    else {
	vector<std::thread>	threads;
	threads.reserve(numThread);
	for (uint32_t i = 0; i < numThread; ++i) {
                    //threads.emplace_back(::OperatorInitSrceRef, i, &ctx);
            threads.emplace_back(::OperatorCcbc1, i, &ctx);
	}
		//	catch each thread results and wait for all threads to finish
	for (std::thread & item : threads) {
            item.join();
	}
		//	clear all existing threads
	threads.clear();
    }

    CreateBLastInputFiles(&ctx,numThread);
	//	reset the context start index
    ctx.m_NextIdx = 0;
    while (ctx.m_NextIdx < smax) {
		//for (uint32_t firsti = 0; firsti < imax; firsti += numThread * m_SeqPerThread) {
        uint32_t firsti = ctx.m_NextIdx;
        if (numThread == 1) {
        //OperatorCompareAll(0, &ctx);
            OperatorCcbc2_Blast(0, &ctx); //if use Blast function
        }
        else {
            vector<std::thread>	threads;
            threads.reserve(numThread);
            for (uint32_t i = 0; i < numThread; ++i) {
                //threads.emplace_back(::OperatorCompareAll, i, &ctx);
                threads.emplace_back(::OperatorCcbc2_Blast, i, &ctx);
            }
            //	catch each thread results and wait for all threads to finish
            for (std::thread & item : threads) {
                item.join();
            }
            //	clear all existing threads
            threads.clear();
        }
        uint32_t simRowIdx = 0;
        for (uint32_t i = firsti; i < ctx.m_NextIdx; ++i) {
            TNFieldBase * srce = ClusterDatabase()->m_Sequences[idxList[i]];
            uint32_t s = srce->RecordId();
            for (uint32_t j = i; j < smax; ++j) {			
                TNFieldBase * ref = ClusterDatabase()->m_Sequences[idxList[j]];
                uint32_t r = ref->RecordId();
                //double sim = max(ctx.m_Sims[i][j], ctx.m_Sims[j][i]);
                double sim = ctx.m_Sims[simRowIdx][j];
                if (sim >= p_MinSim) {
                    file << r << " " << s << " " << std::to_string(sim) << "\r\n";
                }
            }
        }
    }
    DeleteBLastInputFiles(&ctx); //delete blast inputfile
    bool isError = (file.goodbit != 0);
    file.close();
	//	return isError;
}


void
TCluster::ExtendRecordNames(uint32_t p_TabNo, uint32_t p_MaxTabNo, std::string  p_Extension) const
{
	if (m_Groups.size() > 0) {	
		//recursively
		for (TCluster group : m_Groups) {
			TNFieldBase * seq = ClusterDatabase()->m_Sequences[group.CentralSeqIdx()];
			group.ExtendRecordNames( p_TabNo + 1, p_MaxTabNo, p_Extension + "|Central id: " + std::to_string(seq->RecordId()));
		}
	}
	else {
		//	extend the record name with the centralidx of the clusters that the sequence belongs to.
		TNFieldBase * srce = ClusterDatabase()->m_Sequences[CentralSeqIdx()];
		for (uint32_t i = 0; i < p_MaxTabNo - p_TabNo; ++i) {
			p_Extension = p_Extension + "|Central id: " + std::to_string(srce->RecordId());;
		}
        std::string ziggo = srce->RecordName() + p_Extension;
		srce->SetRecordName(ziggo);
		if (m_Comparisons.size() > 0) {
			for (const TComparison & comp : m_Comparisons) {
				TNFieldBase * srce = ClusterDatabase()->m_Sequences[comp.SrceIdx()];				
				srce->SetRecordName(ziggo);				
			}
		}		
	}
}




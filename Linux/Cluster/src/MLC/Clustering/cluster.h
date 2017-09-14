
#pragma once

#include "nfieldbase.h"
#include "clusterdb.h"
#include "comparison.h"
#include "clusterproperties.h"
#include "double_matrix.h"
#include <list>

enum AlgorithmEnum {
    SLC = 0,					//		Single level clustering, multi-threaded
    MLC = 1,					//	 	Multi level clustering, multi-threaded
    CCBC = 2,				// 	CCBC, multi-threaded
    GC = 3,				//	 	greedy clustering, multi-threaded
    
    SLC_ST = 4,				// 	Single level clustering, single-threaded
    MLC_ST = 5,				// 	Multi level clustering, single-threaded
    CCBC_ST = 6,			// 	CCBC, single-threaded
    GC_ST = 7,			//		greedy clustering, single-threaded
};

//	a cluster is made of eventualy one reference sequence and either :
//	- a list of sub-clusters
//	- a list of sequence comparisons
class TCluster
{
public:
	explicit TCluster(const ClusterDB * p_ClusterDB);
	TCluster(const ClusterDB * p_ClusterDB, uint32_t p_CentralSeqIdx);
	virtual ~TCluster(void);

	//	saving and displaying

	bool SaveAsText(const wchar_t * p_DestFilePath, uint32_t p_FieldNamePos,uint32_t p_MaxTabNo ) const;
	void Save(std::ostream & p_Stream, uint32_t p_FieldNamePos, uint32_t p_TabNo, uint32_t p_MaxTabNo, std::string  p_Extension) const;
	void Save(std::wostringstream & p_Stream, uint32_t p_TabNo) const;
	void ExtendRecordNames( uint32_t p_TabNo, uint32_t p_MaxTabNo, std::string p_Extension) const;
	
	void SaveFullSimilarity(std::string p_DestFilePath, double p_MinSim);
	void SaveSimilarity(std::string p_DestFilePath, int32_t p_KneighborNo, double p_MinSim, double p_Threshold, int32_t p_Max);
	//void SaveSimilarity(vector<uint32_t> & p_idxList, uint32_t p_TabNo, double p_MinSim);
	void SaveSimilarity(std::ostream & p_Stream,  vector<uint32_t> &p_idxList,  int32_t p_TabNo, int32_t p_KneighborNo, double p_MinSim, double p_Threshold, int32_t & p_Count, int32_t p_Max);

	uint32_t GetReferenceClusterIndex(std::string p_RecordName, uint32_t p_FieldNamePos) const;
      //  int32_t CreateBLastInputFiles(const vector<uint32_t> & p_IdxList);
      // int32_t DeleteBLastInputFiles(const vector<uint32_t> & p_IdxList);

	//	functions

	void OutputDebug();
	void SortAllGroups();
	void ComputeAllCentroids();

	//	computations
	void SortGroups();
	void	BuildSortedIdList();
	void	GetIdList(vector<uint32_t> & p_IdxList);
	void	ComputeCentroid();
	void PrepareCentroid() const;
	void	SortComparisonsBySrceIndex();
	void	SortComparisonsBySimilarity();

	void GClust_St(double p_Threshold);
	void GClust_St(double p_Threshold, uint32_t p_First, uint32_t p_Last);
	void GClust_St(double p_Threshold, const vector<uint32_t> & p_IdxList);

	void Ccbc_St(double p_Threshold);
	void Ccbc_St(double p_Threshold, uint32_t p_First, uint32_t p_Last);
	void Ccbc_St(double p_Threshold, const vector<uint32_t> & p_IdxList);
        

	void GClust_Mt(double p_Threshold);
	void GClust_Mt(double p_Threshold, uint32_t p_First, uint32_t p_Last);
	void GClust_Mt(double p_Threshold, const vector<uint32_t> & p_IdxList);

        void Ccbc_Mt(double p_Threshold);
	void Ccbc_Mt(double p_Threshold, uint32_t p_First, uint32_t p_Last);
	void Ccbc_Mt(double p_Threshold, const vector<uint32_t> & p_IdxList);

	void Mlc_St(double p_Threshold);
	void Mlc_St(double p_Threshold, uint32_t p_First, uint32_t p_Last);
	void Mlc_St(double p_Threshold, const vector<uint32_t> & p_IdxSortedList);

	void Mlc_Mt(double p_Threshold);
	void Mlc_Mt(double p_Threshold, uint32_t p_First, uint32_t p_Last);
	void Mlc_Mt(double p_Threshold, const vector<uint32_t> & p_IdxSortedList);

	void MlcAll_Mt(const vector<double> & p_Thresholds, uint32_t p_ParamIdx);
	void MlcAll_St(const vector<double> & p_Thresholds, uint32_t p_ParamIdx);
        
        //double_matrix OperatorBlast_St(const vector<uint32_t> & p_sourceIdxList, const vector<uint32_t> & p_refIdxList);
        double_matrix OperatorBlast_CCBC(const vector<uint32_t> & p_IdxList, std::string sourcefilename);
        double_matrix OperatorBlast_GC(uint32_t p_sourceIdx, uint32_t p_size, std::string p_reffilename);
        double_matrix OperatorBlast_File(uint32_t p_first, uint32_t p_RowCount, uint32_t p_ColCount, std::string p_sourcefilename, std::string p_reffilename);

	void ComputeChildDistances();
	void ClusterByName(uint32_t p_FieldNamePosition);
	double F_Measure(TCluster & p_RefCluster);
	double F_Measure_Mt(TCluster & p_RefCluster);

	void OutputDebugGroups(std::wostringstream & stream);

	double Compare(const TNFieldBase * srce, const TNFieldBase * ref) const;        
        
	//bool	ExportToEScience(const std::wstring & p_DestFolder, uint32_t p_LevelNo) const;
	//bool	ExportToEScience(const std::wstring & p_DestFolder, const ClusterProperties & p_ParentProperties, std::ofstream & p_SmallDataStream, uint32_t p_LevelNo) const;
	bool	ExportSequences(const wchar_t * p_DestFilePath) const;
	//	properties
	const ClusterDB *				ClusterDatabase() const;
        
	const vector<TNFieldBase *> &	Sequences() const;
	size_t							SequenceNo() const;
	size_t							ClassifiedSequenceNo() const;
	uint32_t							CentralSeqIdx() const;
	size_t							GroupNo() const;
	size_t FinalGroupNo() const;
	void	FinalClusters(std::vector<TCluster> & p_Clusters) const;
	void	Flatten();	//	move all groups into a single level of depth
	void BuildAllSortedIdList();
	//	the sequence idx of our central sequence (centroid)
	const vector<uint32_t> &	IdList() const;
	const std::string &			CentralName() const;
	double							MinDist() const { return m_MinDist; }
	double							MaxDist() const { return m_MaxDist; }
	double							DistFromParent() const { return m_DistFromParent; }

	const vector<TComparison> &	Comparisons() const;
	vector<TComparison> &			Comparisons();

	//	the list of our sequence comparisons
	const vector<TCluster> & Groups() const;
	vector<TCluster> & Groups();

	bool ReverseComplement() const;

	//	GClust parameters, 
	uint32_t m_GroupsPerTour;	//	how many sequences are processed by thread #0 before starting all other thread. Best values : 40...80, best value for the medical database :  m_GroupsPerTour(50)

	//	global parameter used in GClust, Ccbc, MLC, F_Measure, InitSrce, etc.
	uint32_t m_SeqPerThread;	//	how many sequences are processed by a single thread when comparing sequences. Best values : 5...30

	//	MLC parameter
	//uint32_t m_SeqPerBloc;		//	how many sequences are stored in each bloc during step 1. Best values : 40...50, best value for the medical database : m_SeqPerBloc(35)
	uint32_t m_BlocNo;				//number of blocks for MLC, default is 10; m_SeqPerBloc is equal to the total number of sequences divided  by m_SeqPerBloc
private:
	//	the non-const versions for the p_IdxSortedList
	void Mlc_SingleThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList);
	void Mlc_MultiThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList);
        
	void GClust_SingleThread(double p_Threshold, vector<uint32_t> & p_IdxSortedList);
        void GClust_SingleThread_Blast(double p_Threshold, vector<uint32_t> & p_IdxSortedList);
        
        void Ccbc_SingleThread(double p_Threshold, const vector<uint32_t> & p_IdxSortedList);
        void Ccbc_SingleThread_Blast(double p_Threshold, const vector<uint32_t> & p_IdxList);
        
	void GClust_MultiThread(double p_Threshold, vector<uint32_t> & p_IdxList);
        void GClust_MultiThread_Blast(double p_Threshold, vector<uint32_t> & p_IdxList);

	uint32_t m_NumThread;					//	the max number of threads, set in ctor

	//	values computed in ComputeCentroid()
	uint32_t m_CentralSeqIdx;				//	the index in m_Sequences of our centroid sequence. Exist in all clusters except in the root cluster. UINT32_MAX if undefined.
	double	m_MinDist;						//	the smaller distance between the centroid and any other sequence in the groups or comparisons
	double	m_MaxDist;						//	the max distance...
	double	m_DistFromParent;				//	the distance between our centroid and the parent centroid

	//	is this cluster is a final one, it contains Comparisons, with all the included sequences results
	vector<TComparison>  m_Comparisons;					//	the list of our sequence comparisons
	vector<uint32_t>   m_IdList; //	the list of all sequences in this group, including the (future) centroid, used to compute the centroid, sorted, no duplicates

	//	otherwise it contains groups
	vector<TCluster> m_Groups;
	bool	m_IsSortedGroups;							//	true if the groups and subgroups have been sorted
													
	const ClusterDB * m_ClusterDB;				//	the source database. Shared will all sub-clusters. Allocated in LoadFastaFile(), deleted in the cluster dtor.

	//	the cluster relative coordinates, the reference being the parent cluster. Computed from Mds()
	double_vector m_Pos;								//	a three dimensional vector

	struct OnGroupIdPr
	{
		bool
			operator()(const TCluster & a, const TCluster & b) const
		{
			return (a.m_IdList.front() < b.m_IdList.front());	//	sort using the smallest record ID
		}
	};

	struct OnGroupCentroidPr
	{
		bool
			operator()(const TCluster & a, const TCluster & b) const
		{
			return (a.m_CentralSeqIdx < b.m_CentralSeqIdx);	//	sort using the centroid index
		}
	};

};



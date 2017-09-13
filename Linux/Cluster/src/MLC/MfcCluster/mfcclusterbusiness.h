#pragma once
#include <string>
#include "../Clustering/clusterdb.h"
#include "../Clustering/comparison.h"
//#include "ColumnTreeCtrl.h"

#include "../Clustering/boost_plugin.h"
#include "../Clustering/std_plugin.h"
#include "../Clustering/toolbox_tl.h"
#include "../Clustering/tostring.h" 

#include <fstream>
#include <locale>
//#include <codecvt>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>

// Visualize
#include <map>
#include "point.h"
#include <fstream>
#include <string>
#include <thread>  
#include <iostream>  

class TCluster;

class MfcClusterBusiness
{
	public:
		int			LoadSourceFile(std::string loadfilepath);
		void			CreateReferenceCluster(int32_t fieldNamePos);
		int			ReadThresholdsForOPTPrediction(wchar_t buffer1[264], wchar_t buffer2[264], wchar_t buffer3[264], int32_t fieldNamePos);
		uint64_t		m_InputFileWriteTime;

		std::wstring	m_OutputClusterFilePath;
		vector<double>	m_Thresholds;						//	thresholds, the threshold to use, one value for all algorithm except the Multi-level MLC. In the interface, use a semi-colon separated list of double in increasing order
		vector<double>	m_FromThresholds;
		double	m_FromThreshold;
		double	m_ToThreshold;
		double	m_OptThreshold;
		double			m_Step = 0.0001;
		int32_t			m_FieldNamePos;					//	the zero-based index of the field name in the fasta file sequence name. 6 for medical fungi, 3 for the other CBS database. -1 if undefined
		double m_MinSimForVisualization = 0.5;			//minimum similarity to be display in the visualization
		int32_t m_Max = 1000;			//maximum number of sequences to be recompared when saving a sparse similarity matrix for visualization
		double m_MaxDistance = 0.1;	//maximum distance from a sequence to the group. If two sequences in a group is very distant from each other whose similarity score is less than the similarity of the group minus this distance, they will be recompared when saving a sparse similarity matrix
		int32_t			m_KneighborNo = 150;
		int32_t			m_EdgeNo = 10;
		int32_t			m_VisDimension = 3;				// the number of dimensions for visualization 2D or 3D; default=3D
		ClusterDB		m_ClusterDatabase;				//	the unique cluster database
		TCluster		*	m_RefCluster;						//	the reference cluster
		TCluster		*	m_Cluster;							//	the working cluster
		vector<int32_t>	m_GroupSeqNumbers;

		int Visualize(std::string inputfilepath, std::string titlefilepath,std::string simfilepath, int32_t d, int32_t kneighbor);
		bool m_recordnameextended;
		double Cluster(const vector<double> & thresholds, int32_t algorithmPos, int32_t inputfieldPos, std::string inFilePath, int32_t minseqnoforMLC);


		bool file_exists(const char *fileName);
		int StringToWString(std::wstring &ws, const std::string &s);
		int WStringToString(const std::wstring &ws,  std::string &s);

		std::map<std::string, Point> pointMap;
		std::map<std::string, std::vector<std::string>> propertiesMap;

		void makePoints(std::string filename);
		int startLargeVisWaitForFinish(std::string parameters);
		std::vector<std::string> split(const std::string &s, char delim);
		void combinePointsWithProperties(std::string inputtitlefilename, std::string inputfilename, std::string outputfilename, std::string coordArchivePath);
		uint64_t GetFileWriteTime(const wchar_t * p_FilePath);
    
        void initBusiness();
    
    void saveClusterAsText(std::string outputfilename);
    void saveSimilarity(std::string simoutputfilename, double minsimilarity, double threshold, int32_t p_KneighborNo);
    void saveFullSimilarity(std::string inputfile, std::string simoutputfilename, double minsimilarity);
    double ComputeFmeasure(int nameposition, int32_t algorithmPos);

};
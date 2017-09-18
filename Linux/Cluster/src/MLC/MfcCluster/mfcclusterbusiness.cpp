#include <stdio.h>
#include <time.h>

#include "../Clustering/boost_plugin.h"
#include "../Clustering/clusterdb.h"
#include "../Clustering/cluster.h"
#include "../Clustering/clustering.h"
#include "mfcclusterbusiness.h"
//#include "OptionDlg.h"
#include "point.h"
#include "optimizer.h"
//#include "resource.h"
#include "../Clustering/std_plugin.h"
#include "../Clustering/toolbox_tl.h"
#include "../Clustering/tostring.h" 
//#include "Wait.h" 

#include <fstream>
#include <locale>
//#include <codecvt>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <vector>
#include <algorithm>

#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#define BOOST_NO_CXX11_SCOPED_ENUMS
//#include <boost/system>
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

void MfcClusterBusiness::initBusiness()
{
    m_RefCluster = NULL;
    m_Cluster = NULL;
}

bool MfcClusterBusiness::file_exists(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

int MfcClusterBusiness::StringToWString(std::wstring &ws, const std::string &s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

int MfcClusterBusiness::WStringToString(const std::wstring &ws, std::string &s)
{
	std::string wsTmp(ws.begin(), ws.end());

	s = wsTmp;

	return 0;
}

int MfcClusterBusiness::LoadSourceFile(std::string loadfilepath)
{
	int result = 0;
	std::wstring wescienceFolder;
	StringToWString(wescienceFolder, loadfilepath);
	return m_ClusterDatabase.LoadFastaFile(wescienceFolder.c_str(), true, m_FieldNamePos);
	//return result;
}

void MfcClusterBusiness::CreateReferenceCluster(int32_t fieldNamePos)
{
    if (m_RefCluster!=NULL)
        delete m_RefCluster;

	m_RefCluster = new TCluster(&m_ClusterDatabase);
	m_RefCluster->ClusterByName(fieldNamePos);
}

// 0 is a correct result
// -1 is The from threshold must be less or equal than the to threshold
// -2 is 
int MfcClusterBusiness::PredictOpt(int32_t algorithmPos, int32_t inputfieldPos, std::string inFilePath, int32_t minseqnoforMLC, std::string fromthreshold, std::string tothreshold, std::string step)
{
    vector<string> values = split(fromthreshold,',');
    m_Step = std::stod(step);
    if (m_Step <=0 || m_Step >1) {
	return -1;
    }
    AlgorithmEnum algo = static_cast<AlgorithmEnum>(algorithmPos);
    switch (algo) {
	default: {
            double x = std::stod(fromthreshold);
            x = std::max(0.0, std::min(x, 1.0));
            m_FromThresholds.push_back(x); //	we use only the last value for all algorithm, except for MLC
            m_FromThreshold = x;
            m_ToThreshold = std::stod(tothreshold);
            break;
	}
	case MLC: { //	Multi-level clustering
             //get tothreshold
            vector<std::string> tovalues = split(tothreshold, ',');
            m_ToThreshold = std::stod(tovalues[tovalues.size()-1]);
            
            m_FromThresholds.clear();
		//	split it
            vector<std::string> values = split(fromthreshold, ',');
            uint32_t i = 0;
            for (auto str : values) {
		double x = std::stod(str);
		x = std::max(0.0, std::min(x, 1.0));
		m_FromThresholds.push_back(x);
		if (i == values.size() - 1) {
                    m_FromThreshold = x;
		}
		i = +1;
            }
            RemoveDuplicatesAndSort(m_FromThresholds);
            if (m_FromThresholds.size() == 0) {
		m_FromThresholds.push_back(m_ToThreshold);
		m_FromThreshold = m_ToThreshold;
            }
            break;
	}
    }
    if (m_FromThreshold > m_ToThreshold) {
	return -1;
    }
   
    double bestf = 0.0;
    double threshold = m_FromThreshold;
    do {
	vector<double> thresholds;
	if (algo == MLC && m_FromThresholds.size() != 1) {
            for (uint32_t i = 0; i < m_FromThresholds.size() - 1; ++i) {
		thresholds.push_back(m_FromThresholds[i]);              
            }
	}
	thresholds.push_back(threshold);       
	Cluster(thresholds,algorithmPos,inputfieldPos,inFilePath,minseqnoforMLC);
        double f = ComputeFmeasure(inputfieldPos, algo);
        cout << "Threshold: " << threshold << "\t Fmeasure: " << f << endl;
	if (f > bestf) {
            bestf = f;
            m_OptThreshold = threshold;
	}
        threshold = threshold + m_Step;
    } while (threshold <= m_ToThreshold);
    cout << "OPT: " << m_OptThreshold << "\t Best Fmeasure: " << bestf << endl;
    return 0;
}

int MfcClusterBusiness::startLargeVisWaitForFinish(std::string parameters)
{
	if (system(NULL))
	{          
            system(("./LargeVis/LargeVis " + parameters).c_str());
            return 0;
	}
	else
	{
		return -1;
	}
}

void MfcClusterBusiness::makePoints(std::string filename)
{
	std::string id;
	double x, y, z;
	double dim = 0;
	bool firstline = true;
	std::ifstream infile(filename);
	std::string line;
	std::getline(infile, line);
	std::istringstream iss(line);
	if (iss >> x >> y) {
		dim = y;
	}
	else return;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		if (dim == 2)
		{
			if (iss >> id >> x >> y)
			{
				Point newpoint = Point(0.1, x, y);
				pointMap[id] = newpoint;
			}
		}
		else if (dim == 3)
		{
			if (iss >> id >> x >> y >> z)
			{
				Point newpoint = Point(x, y, z);
				pointMap[id] = newpoint;
			}
		}
		else break;
	}
}

std::vector<std::string>MfcClusterBusiness::split(const std::string &s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

void MfcClusterBusiness::combinePointsWithProperties(std::string inputtitlefilename, std::string inputfilename, std::string outputfilename, std::string coordArchivePath)
{
	std::ifstream infile(inputfilename,std::ios::binary );
	std::ofstream outfile(outputfilename,std::ios::binary);
	std::ofstream outfileArc(coordArchivePath,std::ios::binary );

	std::string line;
	std::map<std::string, Point>::iterator it;

	int fileindex = 0;

	outfile << "const data_all = {";
	outfileArc << "{";
	bool firstlinecreated = false;
	/////read title file
	if (file_exists(inputtitlefilename.c_str())==true) {
            std::ifstream titlefile(inputtitlefilename,std::ios::binary );
            std::getline(titlefile, line);
            if (line != "") {
                std::string outline;
		/*std::string firstline = L"\"";*/
		std::string firstline = "\"NamesOfProperties\":[";
		std::vector<std::string> properties = split(line, '|');
		int propertyindex = 0;
		for (std::vector<std::string>::iterator it = properties.begin(); it != properties.end(); ++it) {
                    std::string apostrofstr = "\"";
                    std::string apostrof(apostrofstr.c_str());
                    std::string activeproperty((*it).c_str());
                    propertyindex++;
                    if (propertyindex != 1)
                    {
			outline += ",";
			firstline += ",";
                    }
                    
                    outline += apostrof + activeproperty + apostrof;
                    firstline += apostrof + activeproperty + apostrof;
	        }
		outline += "]}";
		firstline += "], ";
			//CT2CA pszConvertedAnsiString1(firstline);
		std::string firstlinestring(firstline);
		outfile << firstlinestring;
		outfileArc << firstlinestring;
		firstlinecreated = true;
            }
            titlefile.close();
	}
	////
	while (std::getline(infile, line))
	{
		fileindex++;
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());//remove the end line character
		std::vector<std::string> properties = split(line, '|');
		/*std::string id(properties.at(0).c_str());
		std::string idstring(properties.at(0).c_str());*/
		std::string id(properties.at(0).erase(0, 1).c_str());
		std::string idstring(properties.at(0).c_str());

		std::string outline;
		std::string firstline = "\"";
		if (fileindex == 1) outline = "\"";
		else outline = ",\"";

		it = pointMap.find(id);

		if (it != pointMap.end())
		{
			Point pointmy = pointMap[id];
			std::ostringstream strs1;
			strs1 << pointmy.x();
			std::string str1 = strs1.str();
			std::string xcord(str1.c_str());

			std::ostringstream strs2;
			strs2 << pointmy.y();
			std::string str2 = strs2.str();
			std::string ycord(str2.c_str());

			std::ostringstream strs3;
			strs3 << pointmy.z();
			std::string str3 = strs3.str();
			std::string zcord(str3.c_str());

			outline = outline + idstring + "\":{\"Coordinates\":[" + xcord + ", " + ycord + ", " + zcord + "],\"Properties\" : [";
			if (!firstlinecreated)
				//firstline = firstline + "NamesOfProperties" + L"\":{\"Coordinates\":[" + xcord + L", " + ycord + L", " + zcord + L"],\"Properties\" : [";
			{
				firstline = firstline + "NamesOfProperties\":[";// +xcord + L", " + ycord + L", " + zcord + L"],\"Properties\" : [";
			}
		}

		int propertyindex = 0;
		for (std::vector<std::string>::iterator it = properties.begin(); it != properties.end(); ++it) {
			std::string apostrofstr = "\"";
			std::string apostrof(apostrofstr.c_str());
			std::string activeproperty((*it).c_str());
			propertyindex++;
			if (propertyindex != 1)
			{
				outline += ",";
				if (!firstlinecreated)
					firstline += ",";
			}
			outline += apostrof + activeproperty + apostrof;
			if (!firstlinecreated)
				firstline += apostrof + activeproperty + apostrof;
		}
		outline += "]}";
		if (!firstlinecreated)
			firstline += "], ";
		std::getline(infile, line);
		if (!firstlinecreated) {
			//CT2CA pszConvertedAnsiString1(firstline);
			std::string firstlinestring(firstline);
			outfile << firstlinestring;
			outfileArc << firstlinestring;
			firstlinecreated = true;
		}
		//CT2CA pszConvertedAnsiString(outline);
		std::string outlinestring(outline);
		outfile << outlinestring;
		outfileArc << outlinestring;
		outfile.flush();
		outfileArc.flush();
	}

	outfile << "}";
	outfileArc << "}";
	outfile.flush();
	outfile.close();
}

// resutl 0 is good
// result -1 is Please select a source fasta file and compute similarities first (cluster)
// result -2 is Please compute and save neccessary similarity values by first clustering the sequences!
// result -3 is Could not run LargeVis
int MfcClusterBusiness::Visualize(std::string inputfilepath, std::string titlefilepath, std::string simfilepath, int32_t d, int32_t kneighbor)
{
	std::string s2;
	s2 = std::string(inputfilepath);
        std::string filename = s2;
	std::string fastaFilePath(s2.c_str());
	s2 = std::string(titlefilepath);
	std::string titlefilename(s2.c_str());

	//we will extract only the filename from the filepath
	const size_t last_slash_idx = filename.find_last_of("\\/");
	if (std::string::npos != last_slash_idx)
	{
		filename.erase(0, last_slash_idx + 1);
	}
	// Remove extension if present.
	const size_t period_idx = filename.rfind('.');
	if (std::string::npos != period_idx)
	{
		filename.erase(period_idx);
	}
	std::string fastaFileName(filename.c_str());
        

	//std::string mfcOutFilePath = "..\\..\\..\\Working\\" + fastaFileName + ".sim";
        std::string mfcOutFilePath = simfilepath;
        
	std::string largeVisOutFilePath = "./LargeVis/" + fastaFileName + ".outLargeVis";
	std::string coordArchivePath =  "./DiVE/data/" + fastaFileName + "_coord.json";
	//std::string largeVIsInputParameters = L" -fea 0 -input " + mfcOutFilePath + L" -output " + largeVisOutFilePath + L" -outdim 3 -threads 4 -log 0";
	//std::string  s = to_string(m_KneighborNo);
        if (kneighbor ==0) {
            kneighbor = 150;
        }
        std::string  s = to_string(kneighbor);
	std::string neigs(s.c_str());
        //compute m_EdgeNo for LargeVis as the dataset size/100 as default
        if (m_ClusterDatabase.m_Sequences.size() ==0){
            //load sequences
            LoadSourceFile(inputfilepath);
        }
        
	int32_t edgeNo = (int32_t)(m_ClusterDatabase.m_Sequences.size() / 100);
        if (edgeNo <50){
            edgeNo =50;
        }
	s = to_string(edgeNo);
	std::string edges(s.c_str());
	//std::string largeVIsInputParameters = L" -fea 0 -input " + mfcOutFilePath + L" -output " + largeVisOutFilePath + L" -outdim 3 -threads 4 -log 1 -samples " + cs; 
	std::string largeVIsInputParameters = " -fea 0 -input " + mfcOutFilePath + " -output " + largeVisOutFilePath + " -outdim 3 -threads 4 -log 1 -samples " + edges + " -neigh " + neigs;
	if (d == 2) {
            largeVIsInputParameters = " -fea 0 -input " + mfcOutFilePath + " -output " + largeVisOutFilePath + " -outdim 2 -threads 4 -log 1 -samples 2 " + edges + " -neigh " + neigs;
	}
	std::string finalFilePath = "./DiVE/data/data.js";
	if (inputfilepath.size() == 0){
                cout << "No input file." << endl;
		return -1;
	}
	else {
		//std::wstring ws = mfcOutFilePath;// .GetBuffer(mfcOutFilePath.GetLength());
            std::string s = mfcOutFilePath;// std::string(ws.begin(), ws.end());
            if (file_exists(s.c_str()) == false) {
                cout << "Similarity file does not exist." << endl;
		return -2;
            }
	}
        //generate coordinates
        
        if (startLargeVisWaitForFinish(largeVIsInputParameters)!=0)
            return -3;
	makePoints(largeVisOutFilePath);
	combinePointsWithProperties(titlefilename, fastaFilePath, finalFilePath, coordArchivePath);
        system("firefox ./DiVE/index.html");
	//ShellExecute(0, 0, "..\\..\\..\\DiVE\\index.html", 0, 0, SW_SHOW);
    
    return 1;
}

//	return the FILETIME structure containing the last change date-time of the given file
uint64_t MfcClusterBusiness::GetFileWriteTime(const wchar_t * p_FilePath)
{
    
    char mbstr[256];
    std::wcstombs(mbstr, p_FilePath, 256);
    
    // MAC SPECIFIC
    struct stat t_stat;
    stat(mbstr, &t_stat);
    struct tm * timeinfo = localtime(&t_stat.st_ctime); // or gmtime() depending on what you want
    
    uint64_t fileWriteTime  = static_cast<long int>(mktime(timeinfo));
    //printf("File time and date: %s", asctime(timeinfo));
    
    //uint64_t fileWriteTime = boost::filesystem::last_write_time(mbstr);
    /*
	HANDLE hFile = CreateFile(p_FilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);

	uint64_t fileWriteTime = 0;
	FILETIME ftCreate, ftAccess, ftWrite;
	if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
		fileWriteTime = (static_cast<uint64_t>(ftWrite.dwHighDateTime) << 32) | static_cast<uint64_t>(ftWrite.dwLowDateTime);
	}
	CloseHandle(hFile);
     */
	return fileWriteTime;
}

int MfcClusterBusiness::Cluster(const vector<double> & thresholds, int32_t algorithmPos, int32_t inputfieldPos, std::string inFilePath, int32_t minseqnoforMLC)
{
    //wchar_t stringC[1032];	
    AlgorithmEnum algo = static_cast<AlgorithmEnum>(algorithmPos);
    //CompAlgoEnum compalgo = static_cast<CompAlgoEnum>(1);
    m_FromThresholds = thresholds;
    m_FieldNamePos = inputfieldPos;
    //int stepNo = 3;
    //double dx = 1.0 / stepNo;
    //	check
    // Retrieve the file times for the file.
    //std::wstring winfilepath = L""; // (inFilePath.c_str);
    uint64_t fileWriteTime = 0;// GetFileWriteTime(winfilepath.c_str);
    //if (m_ClusterDatabase.m_Sequences.size() == 0 || m_InputFileWriteTime != fileWriteTime || fieldNamePos != m_FieldNamePos) {	//	always reload
    
    m_InputFileWriteTime = fileWriteTime;
    if (inFilePath.size()==0) {
	return 0;
    }
    LoadSourceFile(inFilePath);
    //creat input files for BLAST
    //m_ClusterDatabase.CreateBLastInputFiles();
    double f=0;
    if (m_FieldNamePos >0){
        CreateReferenceCluster(m_FieldNamePos);
    }
    //}
    /*wchar_t buffer[264];
    EF_MinimumOverlap.GetWindowTextW(buffer, 256);
    double i = std::stod(buffer);
    m_ClusterDatabase.m_MinOverlap = i;*/
    //	Computations
    if (m_Cluster!=NULL)
        delete m_Cluster;
        //m_ClusterDatabase.CompAlgo =compalgo;
        if (minseqnoforMLC > 0){
           m_ClusterDatabase.m_MinSeqNoForMLC = minseqnoforMLC;
        }
	m_Cluster = new TCluster(&m_ClusterDatabase);
	switch (algo) {
		//	*****	Multi-threaded version	*****
            default:
            case SLC: { //	Single level clustering		
                m_Cluster->Mlc_Mt(thresholds.back());
		break;
            }
            case MLC: { //	Multi-level clustering
		
                m_Cluster->MlcAll_Mt(thresholds, 0);
		//take the last level
		break;
            }
            case CCBC: { //	CCBC             
                
                m_Cluster->Ccbc_Mt(thresholds.back());   
		break;
            }
            case GC: { //	Greedy clustering
		m_Cluster->GClust_Mt(thresholds.back());
		break;
            }
				 //	*****	Single-threaded version	*****
            case SLC_ST: { //	single-level clustering
                
		m_Cluster->Mlc_St(thresholds.back());
		break;
            }
            case MLC_ST: { //	Multi-level clustering
		m_Cluster->MlcAll_St(thresholds, 0);
		//take the last level	
		break;
            }
            case CCBC_ST: { //	CCBC, single thread       
		m_Cluster->Ccbc_St(thresholds.back());
            	break;
            }
            case GC_ST: { //	greedy clustering, single thread
		m_Cluster->GClust_St(thresholds.back());
		break;
            }
	}
        
	//	just to have a nicer display
	//m_Cluster->SortAllGroups();

	
/*
	//std::cout stream;
	std::cout << std::endl << "Threshold = " << thresholds.back();
	std::cout << ", Cluster contains " << m_Cluster->FinalGroupNo() << " groups";
	if (m_RefCluster) {
		std::cout << " and " << m_RefCluster->ClassifiedSequenceNo() << " classified sequences";
	}
         std::cout << std::endl;
*/
	return m_Cluster->FinalGroupNo();
}

void MfcClusterBusiness::saveClusterAsText(std::string outputfilename)
{
    std::wstring woutputfilename;// = new std::wstring(L"" + escienceFolder.c_str);// (escienceFolder.c_str);
    StringToWString(woutputfilename, outputfilename);
    m_Cluster->SortAllGroups();
    m_Cluster->SaveAsText(woutputfilename.c_str(), m_FieldNamePos, m_FromThresholds.size());
}

void MfcClusterBusiness::saveSimilarity(std::string simoutputfilename, double minsimilarity, double threshold, int32_t p_KneighborNo)
{
    int32_t maxseqnotoberecompared = (int32_t)(sqrt(p_KneighborNo * (m_ClusterDatabase.m_Sequences.size()))) ;
    m_Cluster->SaveSimilarity(simoutputfilename, p_KneighborNo, minsimilarity, threshold, maxseqnotoberecompared);
}

void MfcClusterBusiness::saveFullSimilarity(std::string inputfile, std::string simoutputfilename, double minsimilarity)
{
    if (m_ClusterDatabase.m_Sequences.size() == 0) {
        //load sequences
        LoadSourceFile(inputfile);
        m_Cluster = new TCluster(&m_ClusterDatabase);
	//return;
    }
    m_Cluster->SaveFullSimilarity(simoutputfilename, minsimilarity);
}
double MfcClusterBusiness::ComputeFmeasure(int nameposition,int32_t algorithmPos)
{
    //get the final groups
    AlgorithmEnum algo = static_cast<AlgorithmEnum>(algorithmPos);
    switch (algo) {
            case MLC: {
                m_Cluster->Flatten();
            }
            case MLC_ST: {
                m_Cluster->Flatten();
            }
    }
    //	compute and update the F-Measure
    
    double f;
    m_FieldNamePos = nameposition;
    if (m_FieldNamePos >0){
        f = m_Cluster->F_Measure_Mt(*m_RefCluster);
    }
    return f;
}
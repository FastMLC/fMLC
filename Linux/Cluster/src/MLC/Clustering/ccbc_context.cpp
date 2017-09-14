
//#include "stdafx.h"

#include "ccbc_context.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
ccbc_context::ccbc_context(uint32_t p_NextIdx, TCluster & p_Cluster, const vector<uint32_t> & p_IdxList, double p_Threshold)
	: m_NextIdx(p_NextIdx),
		m_Cluster(p_Cluster),
		m_Threshold(p_Threshold),
		m_IdxList(p_IdxList)
{
}


ccbc_context::~ccbc_context(void)
{
}
void ccbc_context::ReadBLastResult(std::string p_filename){
    std::ifstream file(p_filename , std::ifstream::in);
    std::string line;
    while (file.good()) {
        //	read line
        line.clear() ;
        std::getline(file, line) ;
        if (line.empty()) {
            continue ;
        }
        std::vector<std::string> strs;
        boost::split(strs, line, boost::is_any_of("\t"));
        uint32_t i = std::atoi(strs[0].c_str());
        uint32_t j = std::atoi(strs[1].c_str());
        double sim = std::atof(strs[2].c_str());
        sim =sim/100;
        double overlap = std::atof(strs[3].c_str());
        if (overlap < 100){
            sim = sim * overlap / 100;
        }
        m_Sims[i][j]=sim;
    }
    file.close();
}

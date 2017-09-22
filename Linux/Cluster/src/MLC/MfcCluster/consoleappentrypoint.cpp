//#define BOOST_SYSTEM_NO_DEPRECATED

#include <iostream>
#include "mfcclusterbusiness.h"
#include "../Clustering/cluster.h"
#include <boost/algorithm/string.hpp>

vector<double> convertThresholds(std::string input)
{
    vector<double> result;
    std::vector<std::string> strs;
    //std::string trimmedinput = input.substr(1,input.size()-2);
    //std::cout << trimmedinput;
    boost::split(strs, input, boost::is_any_of(","));
    
    for(int br=0; br < strs.size(); br++)
    {
        //std::cout << strs.at(br).c_str() << " " << std::atof(strs.at(br).c_str()) << "\n";
        result.push_back(std::atof(strs.at(br).c_str()));
    }
    return result;
}

int algorithToEnum(std::string input)
{
    int result = 0;
    if (input == "SLC")
    {
        result = 0;
    }
    else if (input == "MLC")
    {
        result = 1;
    }
    else if (input == "CCBC")
    {
        result = 2;
    }
    else if (input == "GC")
    {
        result = 3;
    }
    else if (input == "SLC_ST")
    {
        result = 4;
    }
    else if (input == "MLC_ST")
    {
        result = 5;
    }
    else if (input == "CCBC_ST")
    {
        result = 6;
    }
    else if (input == "GC_ST")
    {
        result = 7;
    }
    return result;
}

int main(int argc, char* argv[])
{
    
    AlgorithmEnum algorithm = MLC; // static_cast<AlgorithmEnum>(algorithmPos);
    vector<double> inputthresholds;
    std::string infilepath;
    std::string outfilepath;
    std::string propertytitlepath;
    int nameposition = 0;
    int namepositionforprediction =0;
   
    int32_t M = 100;
    int32_t K = 150;
    double minsim = 0.5;
    int32_t d =0;
    std::string fullmatrixfilepath;
    std::string sparsematrixfilepath;
    std::string simfilepath;
    std::string fromthreshold;
    std::string tothreshold;
    std::string step;
   
    //if (argc%2==1) // odd number of arguments,
    int br = 1;
    //for (int br1=1; br1 < argc; br1+=2){
    while (br < argc){
        std::string optionname = argv[br];
        if (optionname=="-predictOpt"){
            fromthreshold = argv[br+1];
            tothreshold = argv[br+2];
            step = argv[br+3];
            namepositionforprediction = std::stoi(argv[br+4]);
            br = br + 5;
        }
        else{
            std::string optionvalue =argv[br+1];
            if (optionname=="-input"){
                infilepath = optionvalue;
                //std::cout << "input OK " << optionvalue << std::endl;
            }
            else if (optionname=="-title"){                
                propertytitlepath =optionvalue;
                //std::cout << "output OK " << optionvalue << std::endl;
            }
            else if (optionname=="-output"){                
                outfilepath =optionvalue;
                //std::cout << "output OK " << optionvalue << std::endl;
            }
            else if (optionname=="-thresholds"){
                inputthresholds =convertThresholds(optionvalue);                
            }
            else if (optionname=="-algo"){              
                algorithm = static_cast<AlgorithmEnum>(algorithToEnum(optionvalue));
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-fmeasure"){                
                nameposition = std::atoi(optionvalue.c_str());
               // std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-saveCSM"){               
                fullmatrixfilepath = optionvalue;                
            }
            else if (optionname=="-saveSSM"){           
                sparsematrixfilepath = optionvalue;
            }
            else if (optionname=="-minsim"){                
                minsim = std::atof(optionvalue.c_str());
            }
            else if (optionname=="-K"){               
                K = std::atoi(optionvalue.c_str());
            }   
            else if (optionname=="-m"){                
                M = std::atoi(optionvalue.c_str());                
            }
            else if (optionname=="-visualize"){                
                d = std::atoi(optionvalue.c_str());               
            }
            else if (optionname=="-sim"){
                simfilepath = optionvalue;
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else{
                cout << "Wrong argument: " << optionname << endl;
                return -1;
            }
            br = br + 2;
        }
    }
   
 
    MfcClusterBusiness business;
    business.initBusiness();
        
    if (inputthresholds.size() > 0){
        int groupNo = business.Cluster(inputthresholds, algorithm, nameposition, infilepath,M);
        std::cout << "Number of obtained groups: " << groupNo << endl;
    }
    if (outfilepath != "") { //save clustering result
        business.saveClusterAsText(outfilepath);
        std::cout << "Clustering result saved in file " << outfilepath << endl;
    }
    if (fullmatrixfilepath != "") { //save full similarity matrix
        business.saveFullSimilarity(infilepath, fullmatrixfilepath, minsim);
        simfilepath = fullmatrixfilepath;
        std::cout << "Complete similarity matrix saved in file " << fullmatrixfilepath << endl;
    }
    if (sparsematrixfilepath != "") { //save sparse similarity matrix
        business.saveSimilarity(sparsematrixfilepath, minsim, inputthresholds.back(), K);
        simfilepath = sparsematrixfilepath;
        std::cout << "Sparse similarity matrix saved in file " << sparsematrixfilepath << endl;
    }
    if (nameposition >0){
        double f = business.ComputeFmeasure(nameposition, algorithm); 
        std::cout << "Fmeasure: " << f << endl; 
    } 
    if (namepositionforprediction>0){
        business.PredictOpt(algorithm, namepositionforprediction, infilepath,M,fromthreshold,tothreshold,step);            
    } 
    if (d>0 & (simfilepath !="")) {
        business.Visualize(infilepath,propertytitlepath,simfilepath, d,K);
    }
    
    return 0;
}
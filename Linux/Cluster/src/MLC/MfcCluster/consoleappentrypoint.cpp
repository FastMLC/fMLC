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
    
    AlgorithmEnum algorithm = CCBC; // static_cast<AlgorithmEnum>(algorithmPos);
    vector<double> inputthresholds;
    std::string infilepath;
    std::string outfilepath;
    std::string propertytitlepath;
    int nameposition = 0;
    bool correctarguments = true;
    int correctargumentscount = 0;
   // int correctargumentscount2 = 0;
    //bool exportsimilaritymatrix = false;
    //bool sparsematrix = false;
    int32_t M = 100;
    int32_t K = 150;
    double minsim = 0.5;
    int32_t d =0;
    std::string fullmatrixfilepath;
    std::string sparsematrixfilepath;
    std::string simfilepath;
    if (argc%2==1) // odd number of arguments,
    {
        for (int br1=1; br1 < argc; br1+=2)
        {
            std::string optionname = argv[br1];
            std::string optionvalue =argv[br1+1];
            if (optionname=="-input")
            {
                correctargumentscount++;
                infilepath = optionvalue;
                //std::cout << "input OK " << optionvalue << std::endl;
            }
            else if (optionname=="-title")
            {
                correctargumentscount++;
                propertytitlepath =optionvalue;
                //std::cout << "output OK " << optionvalue << std::endl;
            }
            else if (optionname=="-output")
            {
                correctargumentscount++;
                outfilepath =optionvalue;
                //std::cout << "output OK " << optionvalue << std::endl;
            }
            else if (optionname=="-thresholds")
            {
                inputthresholds =convertThresholds(optionvalue);
                //inputthresholds.push_back(0.52);
                if (inputthresholds.size()>0)
                {
                    correctargumentscount++;
                }
                //std::cout << "thresholds OK " << optionvalue << std::endl;
            }
            else if (optionname=="-algorithm")
            {
                correctargumentscount++;
                algorithm = static_cast<AlgorithmEnum>(algorithToEnum(optionvalue));
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-fmeasure")
            {
                correctargumentscount++;
                nameposition = std::atoi(optionvalue.c_str());
               // std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-saveCSM")
            {
                correctargumentscount++;
                fullmatrixfilepath = optionvalue;
                //if(optionvalue=="true")
                //{
                //    exportsimilaritymatrix = true;
                //}
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-saveSSM")
            {
                correctargumentscount++;
                sparsematrixfilepath = optionvalue;
            }
            else if (optionname=="-minsim")
            {
                correctargumentscount++;
                minsim = std::atof(optionvalue.c_str());
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-K") //for saving sparse similarity matrix
            {
                correctargumentscount++;
                K = std::atoi(optionvalue.c_str());
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
             else if (optionname=="-m") //for saving sparse similarity matrix
            {
                correctargumentscount++;
                M = std::atoi(optionvalue.c_str());
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-visualize") //for saving sparse similarity matrix
            {
                correctargumentscount++;
                d = std::atoi(optionvalue.c_str());
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else if (optionname=="-sim") //for saving sparse similarity matrix
            {
                correctargumentscount++;
                simfilepath = optionvalue;
                //std::cout << "algorithm OK " << optionvalue << std::endl;
            }
            else 
            {
                correctarguments = false;
                cout << "Wrong argument: " << optionname << endl;
            }
        }
         
    }
    
    if (correctarguments)
    {
        MfcClusterBusiness business;
        business.initBusiness();
        
        if (inputthresholds.size() > 0){
            business.Cluster(inputthresholds, algorithm, nameposition, infilepath,M);
        }
        if (outfilepath != "") { //save clustering result
             business.saveClusterAsText(outfilepath);
        }
        if (fullmatrixfilepath != "") { //save full similarity matrix
            business.saveFullSimilarity(infilepath, fullmatrixfilepath, minsim);
            simfilepath = fullmatrixfilepath;
        }
        if (sparsematrixfilepath != "") { //save sparse similarity matrix
            business.saveSimilarity(sparsematrixfilepath, minsim, inputthresholds.back(), K);
            simfilepath = sparsematrixfilepath;
        }
        if (nameposition >0){
            business.ComputeFmeasure(nameposition, algorithm);            
        } 
        if (d>0 & (simfilepath !="")) {
            business.Visualize(infilepath,propertytitlepath,simfilepath, d,K);
        }
    }
    else
    {
     //  std::cout << "Please check the arguments. \n";
     //   std::cout << "Usage: \n";
     //   std::cout << "./Cluster -input inputfilename -output outputfilename \n";
     //   std::cout << " -thresholds [t1,...] -algorithm SLC/MLC/CCBC/GC -fmeasure fieldnamenumber -saveSSM sparsesimmatrixfilename -saveFSM -fullsimmatrixfilename -visualize 3 \n";
        
    }
    
    /* //for testing
        std::cout << "Starting clustering \n";
        //int n;
        //std::cin >> n;

        MfcClusterBusiness business;
        business.initBusiness();

        // Set thresholds
        //std::vector<double> inputthresholds;
        inputthresholds.push_back(0.52);
        
        // Set algorithm
        algorithm = SLC;

        // Set fasta file
        infilepath="/home/ClusterLinux/DIVE MAC/example.fasta";
        
        // Set name position
        nameposition = 1;
        
        double result = business.Cluster(inputthresholds, algorithm, nameposition, infilepath);
        
        outfilepath="/home/ClusterLinux/DIVE MAC/output.clus";
        
        business.saveClusterAsText(outfilepath);
        
        std::cout << "Ending clustering with result " << result;
    //}
    */
    return 0;
}
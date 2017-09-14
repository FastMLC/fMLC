#include <iostream>
#include <fstream>
#include "testfieldbase.h"
#include <boost/algorithm/string.hpp>
#include <stdio.h>

void cstringcpy(const char *src, char * dest)
{
    while (*src) {
        *(dest++) = *(src++);
    }
    *dest = '\0';
}

TestFieldBase::TestFieldBase()
{
	//TNFieldBase(int32_t p_RecordId, const std::string & p_RecordName, NFieldSubTypeEnum p_SubType);
   // local_Sequence = NULL;
}

//Copy construtor
TestFieldBase::TestFieldBase(const TestFieldBase & p_Srce)
:local_Sequence(p_Srce.local_Sequence),
sequence_Id(p_Srce.sequence_Id),
sequence_Name(p_Srce.sequence_Name),
TNFieldBase(p_Srce)
{
}

TestFieldBase::~TestFieldBase()
{
}

bool TestFieldBase::FromSequence(const char * p_Sequence) 
{
    std::string l_str(p_Sequence);
    local_Sequence = l_str ;
	return true;
}

//	utf-8 version reading a text sequence
bool TestFieldBase::FromSequence(const wchar_t * p_Sequence) 
{
	return false;
}	//	utf-16 version reading a text sequence


void		TestFieldBase::InitSrce(bool p_ReverseComplement) {}
void		TestFieldBase::InitRef(bool p_Precise) {}

std::string TestFieldBase::Sequence2() const 
{
    std::string result(local_Sequence);
    return result;
}

std::string TestFieldBase::Sequence() 
{
	//std::string result(local_Sequence);
	return local_Sequence;
}	
//	return the sequence (as "cggatacgcggatagcatcg...") or the protein

void		TestFieldBase::ReverseComplement() {}				//	Swap Left-Right the source sequences (horizontal flip) and complement if possible

uint32_t	TestFieldBase::SequenceSize() const 
{ 
	uint32_t destination_size = sizeof(local_Sequence);
	return destination_size;  
}

TNFieldBase * TestFieldFactory(bool p_IsDna, uint32_t p_SequenceId, const std::string & p_SequenceName, const char * p_Sequence)
{
    TestFieldBase* result = new TestFieldBase();
    result->FromSequence(p_Sequence);
    result->sequence_Name = std::string(p_SequenceName);
    result->sequence_Id = p_SequenceId;
    std::string l_str(p_SequenceName);
    result->SetRecordName(l_str);
    result->SetRecordId(p_SequenceId);
    return result;
}

unsigned int edit_distance(const std::string& s1, const std::string& s2)
{
    const std::size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<unsigned int>> d(len1 + 1, std::vector<unsigned int>(len2 + 1));
    
    d[0][0] = 0;
    for(unsigned int i = 1; i <= len1; ++i) d[i][0] = i;
    for(unsigned int i = 1; i <= len2; ++i) d[0][i] = i;
    
    for(unsigned int i = 1; i <= len1; ++i)
        for(unsigned int j = 1; j <= len2; ++j)
            // note that std::min({arg1, arg2, arg3}) works only in C++11,
            // for C++98 use std::min(std::min(arg1, arg2), arg3)
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
    return d[len1][len2];
}

double ComputeSimilarity(const TNFieldBase * p_Srce, const TNFieldBase * p_Ref, int32_t p_MinOverLap)
{
    double sim =0;
    std::string filename1 =  std::to_string((int) p_Srce-> RecordId())+ ".txt";
    std::string filename2 =  std::to_string((int) p_Ref-> RecordId())+ ".txt";
    
    std::ofstream file1;
    file1.open(filename1);
    file1 << ">" <<  p_Srce-> RecordName() << "\r\n";
    file1 <<  p_Srce-> Sequence2() << "\r\n";
    file1.close();
    std::ofstream file2;
    file2.open(filename2);
    file2 << ">" <<  p_Ref-> RecordName() << "\r\n";
    file2 <<  p_Ref-> Sequence2() << "\r\n";
    file2.close();
    
    std::string blastcommand = "blastn -query " + filename1 + " -subject " + filename2  + " -outfmt 6";
    
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
            std::vector<std::string> strs;
            boost::split(strs, blastresult, boost::is_any_of("\t"));
            sim = std::atof(strs[2].c_str());
            sim =sim/100.0;
            double overlap = std::atof(strs[3].c_str());
            if (overlap < p_MinOverLap){
              sim = sim * overlap / p_MinOverLap;
            }
    }
    std::remove(filename1.c_str());
    std::remove(filename2.c_str());
    return sim;
    /*std::string srceseq(p_Srce->Sequence2());
    std::string destseq(p_Ref->Sequence2()); 

    double editdistance =edit_distance(srceseq, destseq);
    double result = 0.5;
    if(srceseq.size() >= destseq.size())
    {
        result = (editdistance/srceseq.size());
        //std::cout << srceseq << "-" << destseq << "-" << result;
    }
    else if(destseq.size() >= srceseq.size())
    {
        result = (editdistance/destseq.size());
        //std::cout << srceseq << "-" << destseq << "-" << result;
    }
     */
    
    
}
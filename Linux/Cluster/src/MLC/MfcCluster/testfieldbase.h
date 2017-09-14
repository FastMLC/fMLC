#pragma once
#include "../Clustering/nfieldbase.h"
class TestFieldBase :
public TNFieldBase
{
public:

    TestFieldBase();
    TestFieldBase(const TestFieldBase & p_Srce);

    ~TestFieldBase();
    bool FromSequence(const char * p_Sequence);
    bool FromSequence(const wchar_t * p_Sequence);
 
   
    void InitSrce(bool p_ReverseComplement);
    void InitRef(bool p_Precise);
    std::string Sequence();
    void ReverseComplement();
    uint32_t SequenceSize() const;

    std::string local_Sequence;
    uint32_t sequence_Id;
    std::string sequence_Name;
    
    std::string Sequence2() const;
};

TNFieldBase * TestFieldFactory(bool p_IsDna, uint32_t p_SequenceId, const std::string & p_SequenceName, const char * p_Sequence);

double ComputeSimilarity(const TNFieldBase * p_Srce, const TNFieldBase * p_Ref, int32_t p_MinOverLap);




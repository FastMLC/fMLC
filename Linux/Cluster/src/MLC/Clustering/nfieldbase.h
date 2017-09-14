
#pragma once

#include "std_plugin.h"

enum NFieldSubTypeEnum {
	DNA = 0,
	Protein = 1
};

//	******************************	TNFieldBase	*********************************

//	That's a container for a sequence of ADN (see TAdnField) or Amino acid (see TProtField)

class TNFieldBase {
protected :
	TNFieldBase();
	TNFieldBase(int32_t p_RecordId, const std::string & p_RecordName,  NFieldSubTypeEnum p_SubType);
	explicit TNFieldBase(const TNFieldBase & p_Srce);

private :
	TNFieldBase & operator=(const TNFieldBase &) = delete ; //	avoid implicit assignement operator

public :
	virtual ~TNFieldBase();

	virtual bool FromSequence(const char * p_Sequence) = 0;		//	utf-8 version reading a text sequence
	virtual bool FromSequence(const wchar_t * p_Sequence) = 0;	//	utf-16 version reading a text sequence
        

	virtual void		InitSrce(bool p_ReverseComplement) = 0;
	virtual void		InitRef(bool p_Precise) = 0;

	virtual std::string Sequence() = 0;			//	return the sequence (as "cggatacgcggatagcatcg...") or the protein
    
	virtual void		ReverseComplement() = 0;						//	Swap Left-Right the source sequences (horizontal flip) and complement if possible

	virtual uint32_t	SequenceSize() const = 0;

	NFieldSubTypeEnum		SubType() const	{		return m_SubType ;	} //	0 for DNA, 1 for protein

	bool						SrceReady() const		{		return m_bSrceReady ;								}
	bool						RefReady() const		{		return m_bRefReady ;									}
	uint32_t					RecordId() const		{		return m_RecordId;									}
	const std::string &	RecordName() const	{		return m_RecordName;									}
	std::wstring 			RecordNameW() const	{		return std::utf8_to_wstring(m_RecordName);	}
	

	std::string		m_Quality;
	void SetRecordName(std::string & p_RecordName);
        void SetRecordId(uint32_t p_RecordId);
    
    virtual std::string Sequence2() const = 0;
protected:
	uint32_t m_RecordId;
	std::string m_RecordName;
	

	NFieldSubTypeEnum m_SubType; //	0 for DNA, 1 for protein

	bool m_bSrceReady;	//	a sequence is loaded, ready for comparison as a source sequence
	bool m_bRefReady;		//	a sequence is loaded, ready for comparison as a reference sequence
	
};

//	this function creates the TNFieldBase instance 
//extern TNFieldBase * NFieldFactory(bool p_IsDna, uint32_t p_SequenceId, const std::string & p_SequenceName, const char * p_Sequence);

/*	To use your own Sequence class:
	- derive your sequence class from the TNFieldBase class above
	- write a factory allocating your class instances
	- this factory must return a pointer to the instance created with the given parameters
	- the function running in BioScience.x64.dll is given below, but the classes TAdnField and TProtField are very heavy classes not in public domain.

	TNFieldBase *	NFieldFactory(bool p_IsDna, uint32_t p_SequenceId, const std::string & p_SequenceName, const char * p_Sequence)
	{
		TNFieldBase * ptr;
		if (p_IsDna) {
			ptr = new TAdnField(p_SequenceId, p_SequenceName);
		}
		else {
			ptr = new TProtField(p_SequenceId, p_SequenceName);
		}
		ptr->FromSequence(p_Sequence);
		return ptr;
	}
*/

//	this function compares the given source and reference sequences, using the given alignment options
//extern double ComputeSimilarity(const TNFieldBase * p_Srce, const TNFieldBase * p_Ref, int32_t p_LowLimit = 0);

/*	To use your own comparing function:
- re-write a similar Compare function
- you can remove the p_Result parameter if you don't need it
- you can also remove the Options parameters
- the only constrain is to return a double value between 0.0 and 1.0, representing the similarity between the given source and reference sequences
*/

//	we call this function as startup, just to let the dll known where to save the log file
//extern void InitBioScience(const wchar_t * p_Path);



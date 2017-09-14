
#pragma once

#include "nfieldbase.h"
#include <vector>

enum TextFileTypeEnum {			//		BOM
	UnknownBom = 0,				//		cannot read file, or no BOM
	UTF_8 = 1,						//	 	EF BB BF
	UTF_16_Big_Endian = 2,		// 	FE FF
	UTF_16_Little_Endian = 3,	//	 	FF FE
	UTF_32_Big_Endian = 4,		// 	00 00 FE FF
	UTF_32_Little_Endian = 5,	// 	FF FE 00 00
	SCSU = 6,						// 	0E FE FF
	UTF_7 = 7,						// 	2B 2F 76 et l'une des s�quences d'octets suivantes: [ 38 | 39 | 2B | 2F ]
	UTF_EBCDIC = 8,				// 	DD 73 66 73
	BOCU_1 = 9						//		FB EE 28
} ;

namespace clustering
{
    int32_t LoadFastaFile(std::vector<TNFieldBase *> & p_Sequences, const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos);
    int32_t LoadFastaFileUtf8(std::vector<TNFieldBase *> & p_Sequences, const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos);
	//	this function save similariy in a file for visualization
	//uint32_t SaveSimilarity(const wchar_t * p_FilePath, const TNFieldBase * p_Srce, const TNFieldBase * p_Ref, double p_sim = 0); 
	//uint32_t SaveSimilarity(std::ostream & p_Stream,  const TNFieldBase * p_Srce, const TNFieldBase * p_Ref, double p_sim = 0);
	
	TextFileTypeEnum	TextFileType(const wchar_t* p_FilePath);	//	return the text file type
	uint32_t		StrainId(const std::string & srce);
	std::string StrainName(const std::string & srce, uint32_t p_NamePos) ;
	std::string	StrainNameFilter(const std::string & srce, int32_t p_NamePos);


	struct OnSequenceSizeAscending
	{
		bool
		operator()(const TNFieldBase * a, const TNFieldBase * b) const
		{
			return (a->SequenceSize() < b->SequenceSize());
		}
	};


	struct OnSequenceSizeDescending
	{
		bool
		operator()(const TNFieldBase * a, const TNFieldBase * b) const
		{
			return (a->SequenceSize() > b->SequenceSize());
		}
	};


	class OnSequenceSizeDescending_int32
	{
	public:
        OnSequenceSizeDescending_int32(const std::vector<TNFieldBase *> & p_Sequences)			:m_Sequences(p_Sequences)		{		}

		bool
			operator()(int32_t a, int32_t b) const
		{
			size_t a_size = m_Sequences[a]->SequenceSize();
			size_t b_size = m_Sequences[b]->SequenceSize();

			if(a_size == b_size) {
				int32_t a_recId = m_Sequences[a]->RecordId();
				int32_t b_recId = m_Sequences[b]->RecordId();

				return (a_recId < b_recId);	//	sort by record ID ascending
			}
			return (a_size > b_size);
		}
	protected:
        const std::vector<TNFieldBase *> & m_Sequences;
	};
}


//	normalize a 3D vector


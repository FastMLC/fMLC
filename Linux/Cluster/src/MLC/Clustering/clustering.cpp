
//#include "stdafx.h"

#include "clustering.h"
#include <fstream>

#include <iostream>
#include "nfieldbase.h"
#include "../MfcCluster/testfieldbase.h"
#include "toolbox_tl.h"


namespace clustering
{
	TextFileTypeEnum
	TextFileType(const wchar_t * p_FilePath)
	{
		//	open file
        
        char mbstr[256];
        std::wcstombs(mbstr, p_FilePath, 256);
		std::ifstream file(mbstr, std::ifstream::binary);
		if (file.bad()) {
			return TextFileTypeEnum::UnknownBom;
		}
		std::string line;
		std::getline(file, line) ;
		file.close() ;

		if (line.size() < 2) { //	too short to contain a BOM
			return TextFileTypeEnum::UnknownBom;
		}
		if (line[0] == 0xFE && line[1] == 0xFF) {
			return TextFileTypeEnum::UTF_16_Big_Endian ;
		}
		if (line[0] == 0xFE && line[1] == 0xFF) {
			return TextFileTypeEnum::UTF_16_Little_Endian ;
		}

		if (line.size() < 3) { //	too short to contain a 3-char BOM
			return TextFileTypeEnum::UnknownBom;
		}
		if (line[0] == 0xEF && line[1] == 0xBB && line[2] == 0xBF) {
			return TextFileTypeEnum::UTF_8 ;
		}
		if (line[0] == 0x0E && line[1] == 0xFE && line[2] == 0xFF) {
			return TextFileTypeEnum::SCSU ;
		}
		if (line[0] == 0xFB && line[1] == 0xEE && line[2] == 0x28) {
			return TextFileTypeEnum::BOCU_1 ;
		}

		if (line.size() < 4) { //	too short to contain a 3-char BOM
			return TextFileTypeEnum::UnknownBom;
		}
		if (line[0] == 0x00 && line[1] == 0x00 && line[2] == 0xFE && line[3] == 0xFF) {
			return TextFileTypeEnum::UTF_32_Big_Endian ;
		}
		if (line[0] == 0xFF && line[1] == 0xFE && line[2] == 0x00 && line[3] == 0x00) {
			return TextFileTypeEnum::UTF_32_Little_Endian ;
		}

		if (line[0] == 0xDD && line[1] == 0x73 && line[2] == 0x66 && line[3] == 0x73) {
			return TextFileTypeEnum::UTF_EBCDIC ;
		}
		if (line[0] == 0x2B && line[1] == 0x2F && line[2] == 0x76 && (line[3] == 0x38 || line[3] == 0x39 || line[3] == 0x2B || line[3] == 0x2F)) {
			return TextFileTypeEnum::UTF_7 ;
		}

		return TextFileTypeEnum::UnknownBom;
	}

	////saving similarity
	////	the static function
	//uint32_t SaveSimilarity(const wchar_t * p_FilePath, const TNFieldBase * srce, const TNFieldBase * ref, double sim)
	//{
	//	//	open file
	//	std::ofstream file(p_FilePath, std::ios_base::app | std::ios_base::out);
	//	file << srce->RecordId() << " " << ref->RecordId() << " " << std::to_string(sim) << "\n";
	//	return 0;
	//}

	

	//	the static function
    int LoadFastaFile(std::vector<TNFieldBase *> & p_Sequences, const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos)
	{
                TextFileTypeEnum fileType = TextFileType(p_FilePath);
		switch (fileType) {
			case TextFileTypeEnum::UnknownBom: //	probably ASCII
			case TextFileTypeEnum::UTF_8: {
				return LoadFastaFileUtf8(p_Sequences, p_FilePath, p_IsDna, p_NamePos);
			}

			default: //	TODO: implement the other formats
				return 0;

		}
	}

	//	load a fasta file and return the list of all loaded sequences
	//	>0|Acremonium acutatum|0|1|FMR 10368|0|Acremonium acutatum|0||0|0|0|0|True|0||False|0|0|0|False|0|562|-1||||
	//	ctccgttggtgaaccagcggagggatcattactgagtgtaaaaactcccaaaccctatgtgaacctaccacagttgcttcggcggcctaaatcgccccgggcgccccagcgcgccccgg...

	//	Load a UTF-8 file and add the loaded sequences to the given vector of TNFieldBase
	//	the static function
    int LoadFastaFileUtf8(std::vector<TNFieldBase *> & p_Sequences, const wchar_t * p_FilePath, bool p_IsDna, uint32_t p_NamePos)
	{
		//	open file
        char mbstr[256];
        std::wcstombs(mbstr, p_FilePath, 256);
        std::ifstream file(mbstr, std::ifstream::in);
	if (file.bad()) {
            return 33 ; //	error
	}
	std::string line;
	line.reserve(20000) ;

	std::string seqName;
	seqName.reserve(2000) ;

	std::string seqHeader;
	seqHeader.reserve(2000);

	uint32_t sequenceId = 0;

	std::string sequence;
	sequence.reserve(200000) ;

	//	initialize variables
	bool FirstLineLoaded = false;
        
        bool goodfile = file.good();
	//	parse file
	while (file.good()) {
            //	read line
            line.clear() ;
            std::getline(file, line) ;
            if (line.empty()) {
		continue ;
            }
            
            //	a non-empty line is loaded
            if (FirstLineLoaded == false) { //	check for the utf-8 header
		if (line.size() >= 3 && line[0] == cUtf8Header[0] && line[1] == cUtf8Header[1] && line[2] == cUtf8Header[2]) {
					//	a pure utf-8 file, remove the utf-8 BOM. If not, it may be an ascii file. Load it anyway.
                    line.erase(line.begin(), line.begin() + 3) ;
		}
		FirstLineLoaded = true ;
		if (line.empty()) {
                    continue ;
		}
            }

            if (line[0] == '>') //	a new sequence name
            {
		if (seqName.length() > 0 && sequence.length() > 0) {
                    //	save the already loaded sequence
                    TNFieldBase * p_sequence = TestFieldFactory(p_IsDna, sequenceId, seqHeader, sequence.c_str());
                    p_Sequences.push_back(p_sequence);
                    //p_Sequences.push_back(NFieldFactory(p_IsDna, sequenceId, seqName, sequence.c_str()));	//	allocation of the sequence, dont forget to delete it when finished
                    seqName = StrainName(line, p_NamePos);
                    seqHeader = line;
                    sequence.clear();
                 }
		//	remole trailing CR, LF, whitespaces, etc.
		while (line.length() > 0 && line.back() <= 32) {
                    line.pop_back();
		}
		//	remove leading '>' and other scrap
		while (line.length() > 0 && (line.front() == '>' || line.front() <= 32)) {
                    line.erase(line.begin(), line.begin() + 1) ;
		}
                sequenceId = StrainId(line);
                seqName = StrainName(line, p_NamePos) ;
                seqHeader = line;
            }
            else if (line[0] != ';') { //	not a comment, the sequence is given by multiple lines
		sequence.append(line);
			}
            } //	read next row from file

            //	save the last sequence
            if (seqName.length() > 0 && sequence.length() > 0) {
		TNFieldBase * p_sequence = TestFieldFactory(p_IsDna, sequenceId, seqHeader, sequence.c_str());
		p_Sequences.push_back(p_sequence);
		//p_Sequences.push_back(NFieldFactory(p_IsDna, sequenceId, seqName, sequence.c_str()));	//	allocation of the sequence, dont forget to delete it when finished
		sequence.clear();
            }
        file.close() ;
        return (0) ; //	no error
    }

	//	read an unsigned integer found at the beginning of the string
    uint32_t 
	StrainId(const std::string & srce)
	{
		uint32_t id = 0;
		const char * ptr = srce.c_str();
		if(*ptr == '>') {
			++ptr;
		}
		char c = *ptr;
		while(c >= '0' && c <= '9') {
			id = id * 10 + (c - '0');
			c = *(++ptr);
		}
		return id;
    }


	//	extract the strain name (Acremonium acutatum) from this :
	//	>0|Acremonium acutatum|0|1|FMR 10368|0|Acremonium acutatum|0||0|0|0|0|True|0||False|0|0|0|False|0|562|-1||||			name is in p_NamePos #6
	//	>1|CBS 14|CBS 14_ex19089_24514_ITS|Rhodosporidium toruloides|n3: rDNA sequences ITS												name is in p_NamePos #3
	//	p_NamePos gives the position of the name betweenthe pises. Shouldbe >= 1, as the ID is in location 0
    std::string
	StrainName(const std::string & srce, uint32_t p_NamePos)
	{
		uint32_t n = 0;
		std::string::size_type pipePos = srce.find('|');	//	search for the first pipe
		while(pipePos != srce.npos && pipePos < srce.length() - 1) {
			std::string::size_type pos = srce.find('|', pipePos + 1);	//	search for the next pipe
			if (pos == srce.npos || pos > srce.length() - 1) {	//	no more pipe
				return srce.substr(pipePos + 1);
			}
			if(++n == p_NamePos-1) {	//	found
				return srce.substr(pipePos + 1, pos - pipePos - 1);
			}
			pipePos = pos;
		}
		//	not found
		return srce;
	}


	//	return the given string keeping only the text at p_NamePos and replacing the rest of the text with dots
	std::string
		StrainNameFilter(const std::string & srce, int32_t p_NamePos)
	{
		std::string line;

		int32_t n = 0;
		std::string::size_type pipePos = srce.find('|');	//	search for the first pipe
		if(p_NamePos == 0) {
			if(pipePos == srce.npos) {
				return line;	//	no pipe
			}
			line = srce.substr(0, pipePos);
			return line;
		}

		while (pipePos != srce.npos && pipePos < srce.length() - 1) {
			std::string::size_type pos = srce.find('|', pipePos + 1);	//	search for the next pipe
			if (++n == p_NamePos) {	//	found
				if (pos == srce.npos || pos > srce.length() - 1) {	//	no more pipe
					line += srce.substr(pipePos+1);
					break;
				}
				for (size_t i = pipePos + 1; i < pos; ++i) {	//	the field name
					line += srce[i];
				}
				break;
			}
			pipePos = pos;
		}
		//	if not found, line is empty
		return line;
	}

}	//	end namespace


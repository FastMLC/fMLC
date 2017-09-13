
#include "stdafx.h"
#include "std_plugin.h"
#include <locale>
#include <codecvt>
#include <sstream>

//	*****	std extra functions	*****

namespace std {

// convert UTF-8 string to wstring
std::wstring utf8_to_wstring(const std::string & p_Srce)
{
	try {
		wstring_convert<codecvt_utf8<wchar_t>> myconv ;
		return myconv.from_bytes(p_Srce) ;
	}
	catch(std::exception & ) {	//	try to convert from ascii using the current thread page
		//size_t SrceSize = p_Srce.size() ;
		std::wstring dest ;
		//dest.resize(SrceSize + 1) ;
		//wchar_t * pDest = const_cast<wchar_t *> (dest.c_str()) ;
		//int32_t Len = MultiByteToWideChar(CP_THREAD_ACP,				//	Current thread code page
		//										MB_PRECOMPOSED,			//	ê => ê and not ^e
		//										p_Srce.c_str(),			//	source string
		//										(int32_t)(SrceSize + 1),		//	actual size of the source string + 1, include the null terminating character
		//										pDest,						//	destination w_char[]
		//										dest.size()) ;				//	max size to translate

		////	Warning : Len includes the null terminating character only if parameter 4 = -1
		//if(Len == 0) {
		//	switch(GetLastError()) {
		//		case ERROR_INSUFFICIENT_BUFFER :			dest = L"A supplied buffer size was not large enough, or it was incorrectly set to NULL" ;		break ;
		//		case ERROR_INVALID_FLAGS :					dest = L"The values supplied for flags were not valid" ;														break ;
		//		case ERROR_INVALID_PARAMETER :			dest = L" Any of the parameter values was invalid" ;															break ;
		//		case ERROR_NO_UNICODE_TRANSLATION :		dest = L" Invalid Unicode was found in a string" ;																break ;
		//		default :										dest = L"No error" ;																										break ;
		//	}
		//}
		//if(Len > 0) {
		//	--Len ;
		//}
		//pDest[Len] = 0 ;
		//dest.resize(Len) ;
		return dest ;
	}
}

// convert wstring to UTF-8 string
std::string wstring_to_utf8 (const std::wstring & p_Srce)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(p_Srce);
}


// convert ascii to wstring
std::wstring ascii_to_wstring(const std::string & p_Srce)
{
	size_t SrceSize = p_Srce.size() ;
	std::wstring dest ;
	dest.resize(SrceSize + 1) ;
	wchar_t * pDest = const_cast<wchar_t *> (dest.c_str()) ;
	int32_t Len = MultiByteToWideChar(CP_THREAD_ACP,					//	Current thread code page
											MB_PRECOMPOSED,				//	ê => ê and not ^e
											p_Srce.c_str(),				//	source string
											static_cast<int32_t>(SrceSize + 1),			//	actual size of the source string + 1, include the null terminating character
											pDest,							//	destination w_char[]
											static_cast<int32_t>(dest.size())) ;				//	max size to translate

	//	Warning : Len includes the null terminating character only if parameter 4 = -1
	//if(Len == 0) {
	//	switch(GetLastError()) {
	//		case ERROR_INSUFFICIENT_BUFFER :			dest = L"A supplied buffer size was not large enough, or it was incorrectly set to NULL" ;		break ;
	//		case ERROR_INVALID_FLAGS :					dest = L"The values supplied for flags were not valid" ;														break ;
	//		case ERROR_INVALID_PARAMETER :			dest = L" Any of the parameter values was invalid" ;															break ;
	//		case ERROR_NO_UNICODE_TRANSLATION :		dest = L" Invalid Unicode was found in a string" ;																break ;
	//		default :										dest = L"No error" ;																										break ;
	//	}
	//}
	if(Len > 0) {
		--Len ;
	}
	dest.resize(Len) ;
	return dest ;
}


// convert ascii to wstring
std::string wstring_to_ascii(const std::wstring & p_Srce)
{
	std::string dest ;
	char * pDest = const_cast<char *>(dest.c_str()) ;

	int32_t maxSize = static_cast<int32_t>(p_Srce.size()) ;
	//	get the minimum size of the destination buffer
	int32_t MinSize = WideCharToMultiByte(CP_THREAD_ACP,	//	Current thread code page
											0,							//	WC_COMPOSITECHECK would change "^e" => "ê"
											p_Srce.c_str(),		//	UNICODE source string
											maxSize,					//	actual size of the source string
											pDest,					//	destination char[]
											0,							//	=> compute the buffer size necessary to translate
											nullptr,						//	use system default char
											nullptr) ;					//	no custom default char
	//	reserve enough memory
	dest.resize(MinSize + 8) ;
	int32_t Size = WideCharToMultiByte(CP_THREAD_ACP,		//	Current thread code page
											0,							//	WC_COMPOSITECHECK would change "^e" => "ê"
											p_Srce.c_str(),		//	UNICODE source string
											maxSize,					//	actual size of the source string
											pDest,					//	destination char[]
											MinSize,					//	max size to translate
											nullptr,						//	use system default char
											nullptr) ;					//	no custom default char
	//	Warning : Len does not includes the null terminating character
	if(Size == 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER) {	//	should never happen
		Size = MinSize ;
	}
	dest.resize(Size) ;
	return dest ;
}


std::vector<std::string> &	split(const std::string & srce, char delim, std::vector<std::string> & elems)
{
    std::stringstream ss(srce) ;
    std::string item ;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item) ;
    }
    return elems ;
}


void reverse(std::string & srce)
{
	char * pi = const_cast<char *> (srce.c_str()) ;	//	the first char
	char * pj = pi + srce.size() - 1 ;					//	the last char

	for(; pi < pj; ++pi, --pj) {
		char c = * pi ;
		* pi = * pj ;
		* pj = c ;
	}
	return ;
}


vector<std::string> split(const char * srce, char delim)
{
	vector<std::string> elems;

	std::stringstream ss(srce);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}


vector<std::wstring> split(const wchar_t * srce, wchar_t delim)
{
	vector<std::wstring> elems;

	std::wstringstream ss(srce);
	std::wstring item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}



}	//	end namespace std

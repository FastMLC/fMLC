
//	include time.h only here to be sure there is no other call to it elsewhere

//#include "stdafx.h"

#include "toolbox.h"

#include "tostring.h"

#include <time.h>				//	for time(), etc.
#include <sstream>			//	for std::wostringstream
#include <string.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__ ;	//	so that #define DEBUG_NEW new(__FILE__, __LINE__) in afx.h displays the correct file name
#define new new(THIS_FILE, __LINE__)
#endif

//	************************************************************************************


//	Initialize random values
bool ToolboxInitialize() ;
bool ToolboxInitialize()
{
	srand(static_cast<uint32_t>(time(nullptr))) ;
	return(true) ;
}
bool ToolboxInitialized = ToolboxInitialize() ;


//	Return an integer value in the range [0, MaxValue[
//	Tested for maxValue = 0 to 100 on (100000 * maxValue) calls => distribution is OK � 2%
//	Initialize random values
//const double RandMax = RAND_MAX + 1 ;	//	RAND_MAX is defined as 0x7FFF
//int32_t __cdecl random(int32_t p_MaxValue) {
int32_t random(int32_t p_MaxValue) {
	//	return (ifloor(p_MaxValue / RandMax * rand())); //	note that rand() returns an integer.
	return rand() % (p_MaxValue + 1); //	note that rand() returns an integer.
}


//	*****************************	String Utilities	**********************************************

//	wcstod destroy the stack if EndPtr == &String !!!
//	EndPtr can be NULL

double	strtod_(const char * p_String, const char ** p_pEnd)
{
	char ** pEnd = const_cast<char **> (p_pEnd) ;	//	simply there because strtoul and strtod request a char ** EndPtr

	const char * ptr = p_String ;
	while(* ptr == ' ') {	//	 (isspace(*pt))	isspace() crashes in debug mode if _ASSERTE((unsigned)(c + 1) <= 256); in isctype.c line 56
		++ptr ;
	}
	ptr += (* ptr == '-') || (* ptr == '+') ;

	if(* ptr == '0') {
		switch (ptr[1]) {
			case 'x':
			case 'X':
				return(strtoul(ptr + 2, pEnd, 16)) ;	//	hexadecimal

			case 'b': // binary
			case 'B':
				return(strtoul(ptr + 2, pEnd, 2)) ;		//	Binary

			case '.': // float like 0.dd
				return (strtod(p_String, pEnd)) ;

			default: // decimal
				return (strtol(p_String, pEnd, 0)) ;
		}
	}
	//	as writing cNaN in a string gives "1.#QNAN", this function should recognize this string as cNaN
	if(strncmp(ptr, "1.#QNAN", 7) == 0) {
		* p_pEnd = ptr + 7 ;
		return(cNaN) ;
	}
	if(strncmp(ptr, "NaN", 3) == 0) {
		* p_pEnd = ptr + 3 ;
		return(cNaN) ;
	}
	//	Real
	return (strtod(p_String, pEnd)) ;
}


double	strtod_(const wchar_t * p_String, const wchar_t ** p_pEnd)
{
	wchar_t ** pEnd = const_cast<wchar_t **> (p_pEnd) ;	//	simply there because strtoul and strtod request a char ** EndPtr

	const wchar_t * ptr = p_String ;
	while(* ptr == ' ') {	//	 (isspace(*pt))	isspace() crashes in debug mode if _ASSERTE((unsigned)(c + 1) <= 256); in isctype.c line 56
		++ptr ;
	}
	ptr += (* ptr == '-') || (* ptr == '+') ;

	if(* ptr == '0') {
		switch (ptr[1]) {
			case 'x':
			case 'X':
				return(wcstoul(ptr + 2, pEnd, 16)) ;	//	hexadecimal

			case 'b': // binary
			case 'B':
				return(wcstoul(ptr + 2, pEnd, 2)) ;		//	Binary

			case '.': // float like 0.dd
				return (wcstod(p_String, pEnd)) ;

			default: // decimal
				return (wcstol(p_String, pEnd, 0)) ;
		}
	}
	//	as writing cNaN in a string gives "1.#QNAN", this function should recognize this string as cNaN
	if (wcsncmp(ptr, L"1.#QNAN", 7) == 0) {
		* p_pEnd = ptr + 7 ;
		return (cNaN) ;
	}
	if (wcsncmp(ptr, L"NaN", 3) == 0) {
		* p_pEnd = ptr + 3 ;
		return (cNaN) ;
	}
	//	Real
	return (wcstod(p_String, pEnd)) ;
}



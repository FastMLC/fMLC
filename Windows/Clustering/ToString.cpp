
#include "stdafx.h"
#include "Toolbox_tl.h" 

#include "ToString.h" 
#include <locale.h>

#ifdef _WINDOWS
//#define swprintf_s swprintf_s	//	_snwprintf_s
#else
	#define swprintf_s swprintf
	//#include "OSFix.h"
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__ ;	//	so that #define DEBUG_NEW new(__FILE__, __LINE__) in afx.h displays the correct file name
#define new new(THIS_FILE, __LINE__)
#endif



//	*************************************	Utilities	**************************************

char DecimalChar()
{
	return(* localeconv()->decimal_point) ;
}


//	**************************************	ToString	*****************************************


static const wchar_t wFFormat[][8] = {	L"%.0lf", L"%.1lf", L"%.2lf", L"%.3lf", L"%.4lf", L"%.5lf", L"%.6lf", 
												L"%.7lf", L"%.8lf", L"%.9lf", L"%.10lf", L"%.11lf", L"%.12lf", 
												L"%.13lf", L"%.14lf", L"%.15lf", L"%.16lf", L"%.17lf", L"%.18lf", L"%.19lf"} ;
static const wchar_t wGFormat[][8] = {	L"%.0lG", L"%.1lG", L"%.2lG", L"%.3lG", L"%.4lG", L"%.5lG", L"%.6lG", 
												L"%.7lG", L"%.8lG", L"%.9lG", L"%.10lG", L"%.11lG", L"%.12lG", 
												L"%.13lG", L"%.14lG", L"%.15lG", L"%.16lG", L"%.17lG", L"%.18lG", L"%.19lG"} ;

static const char FFormat[][8] = {	"%.0lf", "%.1lf", "%.2lf", "%.3lf", "%.4lf", "%.5lf", "%.6lf", 
												"%.7lf", "%.8lf", "%.9lf", "%.10lf", "%.11lf", "%.12lf", 
												"%.13lf", "%.14lf", "%.15lf", "%.16lf", "%.17lf", "%.18lf", "%.19lf"} ;
static const char GFormat[][8] = {	"%.0lG", "%.1lG", "%.2lG", "%.3lG", "%.4lG", "%.5lG", "%.6lG", 
												"%.7lG", "%.8lG", "%.9lG", "%.10lG", "%.11lG", "%.12lG", 
												"%.13lG", "%.14lG", "%.15lG", "%.16lG", "%.17lG", "%.18lG", "%.19lG"} ;

const char * ToString(char * dest, double x, int32_t decimal)
{
	double y = fabs(x) ;
	decimal = max_(0, min_(18, decimal)) ;
	
	if(y > 1E13 || isnan(x)) {	//	Do not show 1.2E121 with 122 characters ! Keep 1E13, as actual time in ms is greater than 1E12
		sprintf_s(dest, 256, "%lG", x) ;
	}
	else if(decimal >= 5 && y < 1.0) {	//	if we want at least 5 decimals and number is small : show it anyway
		sprintf_s(dest, 256, GFormat[decimal + 1], x) ;
	}
	else {
		sprintf_s(dest, 256, FFormat[decimal], x) ;
	}
	return(dest) ;
}

//const wchar_t * ToString(wchar_t * dest, double x, int32_t decimal)
//{
//	double y = fabs(x) ;
//	decimal = max_(0, min_(18, decimal)) ;
//	
//	//_swprintfs(dest, 256, "%.13lG", x) ;	//	at leat 13 digit for time values
//
//	if(y > 1E13 || IsNaN(x)) {	//	Do not show 1.2E121 with 122 characters ! Keep 1E13, as actual time in ms is greater than 1E12
//		swprintf_s(dest, 256, L"%lG", x) ;
//	}
//	else if(decimal >= 5 && y < 1.0) {	//	if we want at least 5 decimals and number is small : show it anyway
//		swprintf_s(dest, 256, wGFormat[decimal + 1], x) ;
//	}
//	else {
//		swprintf_s(dest, 256, wFFormat[decimal], x) ;
//	}
//	return(dest) ;
//}

//	CETIC version
const wchar_t * ToString(wchar_t * dest, double x, int32_t decimal) {
	double y = fabs(x);
	decimal = max_(0, min_(18, decimal));
	if (y > 1E13 || isnan(x))	//	Do not show 1.2E121 with 122 characters ! Keep 1E13, as actual time in ms is greater than 1E12
		swprintf_s(dest, 256, L"%lG", x);
	else if (decimal >= 5 && y < 1.0)	//	if we want at least 5 decimals and number is small : show it anyway
		swprintf_s(dest, 256, wGFormat[decimal + 1], x);
	else 
		swprintf_s(dest, 256, wFFormat[decimal], x);
	return (dest);
}


//	remove ' ', '-' and ':' from the source name
wchar_t * CleanServiceName(wchar_t * srce)
{
	for(uint32_t i = 0, j = 0; ; ++i) {
		switch(srce[i]) {
			case 0 : {
				srce[j] = 0 ;
				return(srce) ;
			}
			case ' ' : {
			case '-' :
			case ':' :
				break ;
			}
			default : {
				srce[j++] = srce[i] ;
				break ;
			}
		}
	}
}


//	Convert a double into a string in the "best" possible way
const wchar_t * ToCleanString(wchar_t * dest, double x, int32_t decimal)
{
	wchar_t * pAnswer = const_cast<wchar_t *> (ToString(dest, x, decimal)) ;

	//	clean up result
	if(wcschr(pAnswer, DecimalChar()) == nullptr) {	//	no decimal point
		return(pAnswer) ;
	}
	if(wcschr(pAnswer, 'e') != nullptr || wcschr(pAnswer, 'E') != nullptr) {	//	scientific notation
		return(pAnswer) ;
	}
	//	Suppress unecessary zeroes after decimal point
	return(CleanZero(pAnswer)) ;
}


//	Receive a string containing a floating value : "-1.235000000"
//	Suppress unecessary zeroes after decimal point
//	WARNING : if there is no decimal point : supress zeroes anyway : 1234000 => 1234
wchar_t * CleanZero(wchar_t * srce)
{
	wchar_t dc = DecimalChar() ;	//	this is the local decimal point (. or ,)
	if(! srce) {
		return(srce) ;
	}
	size_t i = wcslen(srce) - 1 ;
	for( ; i >= 0; --i) {
		if(srce[i] != '0') {
			break ;
		}
	}
	if(i >= 0 && srce[i] == dc) {		//	delete also a remaining dot : 12345. => 12345
		--i ;
	}
	srce[i + 1] = 0 ;
	if(srce[0] == '-' && srce[1] == '0' && srce[2] == 0) {	//	replace "-0" by "0"
		srce[0] = '0' ;
		srce[1] = 0 ;
	}
	return(srce) ;
}

//	******************************	Time utilities	************************************


double MakeDuration(int32_t hour, int32_t minute, int32_t sec, int32_t milli)
{
	//	Values may be negative !
	//	No limits in values : are accepted : 145:32:15.123 or 265:15, etc.
	return(hour * 3600000.0 + minute * 60000.0 + sec * 1000.0 + milli) ;
}


//	Input : a string like 11:45:22.123 or 11h42'18.254" or...
//	Return the number of milliseconds since today at 00:00:00, from 0.0 to ...
double	ReadDuration(const wchar_t * TimeString)
{
	wchar_t * StrPtr, StringC[264], Separator[] = L",;.:/hmsHMS'\"\t-_ " ;
	int32_t Hour, Minute, Second, Milli ;
	wcscpy_s(StringC, 256, TimeString) ;	//	do not destroy DateString !

	wchar_t * pContext = nullptr ;
		
	StrPtr = wcstok_s(StringC, Separator, & pContext) ;
	if(StrPtr == nullptr) {
		Hour = 0 ;
	}
	else if(_wcsicmp(StrPtr, L"INF") == 0) {
		return(cPlusInf) ;
	}
	else {
		Hour = _wtol(StrPtr) ;
	}

	StrPtr = wcstok_s(nullptr, Separator, & pContext) ;
	if(StrPtr == nullptr) {
		Minute = 0 ;
	}
	else {
		Minute = _wtol(StrPtr) ;
	}
	StrPtr = wcstok_s(nullptr, Separator, & pContext) ;
	if(StrPtr == nullptr) {
		Second = 0 ;
	}
	else {
		Second = _wtol(StrPtr) ;
	}
	StrPtr = wcstok_s(nullptr, Separator, & pContext) ;
	if(StrPtr == nullptr) {
		Milli = 0 ;
	}
	else {
		Milli = _wtol(StrPtr) % 1000 ;
	}
	return(max_(0.0, MakeDuration(Hour, Minute, Second, Milli))) ;
}


wchar_t *	ShowDuration(wchar_t * dest, double laps, bool showMilli)
{
	if(laps == cPlusInf) {
		wcscpy_s(dest, 8, L"INF") ;
		return(dest) ;
	}
	laps = max_(0.0, min_(floor(laps), 2E12)) ;	//	so laps / 1000 fits in an integer (max 2E9)
	int32_t SecLaps = static_cast<int32_t>(floor(laps / 1000.0)) ;

	int32_t Hour		= SecLaps / 3600 ;
	int32_t Minute	= (SecLaps % 3600) / 60 ;
	int32_t Sec		= SecLaps % 60 ;
	int32_t Milli	= static_cast<int32_t>(floor(laps - SecLaps * 1000.0)) ;

	if(showMilli) {
		swprintf_s(dest, 16, L"%02li:%02li:%02li.%03li", Hour, Minute, Sec, Milli) ;
	}
	else {
		Sec += (Milli >= 500) ;		//	round milliseconds
		if(Sec >= 60) {				//	avoid that 01:59:59.800 is written 01:60:60
			Sec = 0 ;
			++Minute ;
		}
		if(Minute >= 60) {
			Minute = 0 ;
			++Hour ;
		}
		swprintf_s(dest, 16, L"%02li:%02li:%02li", Hour, Minute, Sec) ;
	}
	return(dest) ;
}


wchar_t *	ShowRoundedDuration(wchar_t * dest, double laps)
{
	if(laps == cPlusInf) {
		wcscpy_s(dest, 8, L"INF") ;
		return(dest) ;
	}
	laps = max_(0.0, min_(floor(laps), 2E12)) ;	//	so laps / 1000 fits in an integer (max 2E9)
	int32_t SecLaps = static_cast<int32_t>(floor(laps / 1000.0)) ;

	int32_t Hour		= SecLaps / 3600 ;
	int32_t Minute	= (SecLaps % 3600) / 60 ;

	swprintf_s(dest, 16, L"%02li:%02li", Hour, Minute) ;
	return(dest) ;
}


wchar_t *	ShowDayDuration(wchar_t * dest, double laps, int32_t language)
{
	if(laps == cPlusInf) {
		wcscpy_s(dest, 256, L"INF") ;
		return(dest) ;
	}
	int32_t DayLaps = static_cast<int32_t>(floor(laps / 86400000.0)) ;
	//	see 	SZ_DAY
	wchar_t Days[][16] = {L"%li j.", L"%li d.", L"%li T.", L"%li d.", L"%li d." } ;

	swprintf_s(dest, 256, Days[language], DayLaps) ;
	return(dest) ;
}

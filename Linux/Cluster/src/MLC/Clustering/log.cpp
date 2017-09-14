
//#include "stdafx.h"

#include "log.h"
#include "std_plugin.h"

#include <mutex>
//#include <corecrt_io.h>
#include "toolbox_tl.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__ ;	//	so that #define DEBUG_NEW new(__FILE__, __LINE__) in afx.h displays the correct file name
#define new new(THIS_FILE, __LINE__)
#endif


//	***************************	Globals	****************************************

//	Do not forget to fill in g_TraceFile from InitInstance() or similar...
std::wstring	g_TraceFile ;
std::mutex		g_TraceMutex ;		//	lock access to the Trace File and to variable g_TraceFile


//	*****************************	Changing the log file	********************************

const std::wstring & TraceFile() {
	return g_TraceFile ;
}

void SetTraceFile(const std::wstring & p_NewFileName) {
	g_TraceFile = p_NewFileName ;
}

void SetEOF(FILE * p_pStream)
{
	if (!p_pStream) {
		return;
	}
	int32_t size = ftell(p_pStream);
        // TODO FIX ON LINUX/MAC
	//_chsize(fileno(p_pStream), size);
}

//	*****************************	Writing to log file	***********************************

//	WARNING : If an unmanaged DLL is called form ASP, all file-access function fail and lead to a System.DllNotFoundException

void
TraceDelete()
{
	std::lock_guard<std::mutex> lock(g_TraceMutex) ;

	try {
            // TODO FIX ON LINUX/MAC
		//::DeleteFile(g_TraceFile.c_str()) ;
	} catch(...) {
	}
}


void
Trace(int32_t p_MessageLevel, int32_t p_MessageType, const std::wstring & p_Message)
{
	std::lock_guard<std::mutex> lock(g_TraceMutex) ;

	static uint32_t FilePosition = 0 ;	//	Reset at application start

	FILE * pStream = nullptr ;
        // TODO FIX ON LINUX/MAC
	//errno_t err = _wfopen_s(& pStream, g_TraceFile.c_str(), L"r+b") ;
	//if(err != 0) {	//	file does not exist
	//	err = _wfopen_s(& pStream, g_TraceFile.c_str(), L"w+b") ;				//	create it
		//if(err == 0) {			//	Mark the file as unicode, just to get a right display when opening in Wordpad
		//	fwrite(& cUtf16Header, sizeof(cUtf16Header), 1, pStream) ;
		//	FilePosition = sizeof(cUtf16Header) ;
		//}
	//}
	
	// Get time as 64-bit integer.
        // TODO FIX ON LINUX/MAC
	//__time64_t long_time;
	//_time64(&long_time);

	// Convert to local time.
	struct tm newtime;
        // TODO FIX ON LINUX/MAC
	//_localtime64_s(&newtime, &long_time);
	wchar_t timebuf[32];
        // TODO FIX ON LINUX/MAC
	//wcsftime(timebuf, 32, L"%x %X", & newtime);

	std::wstring Line ;
	Line.reserve(256 + p_Message.size()) ;
	Line = L"\r\n[" ;
	Line += timebuf;
	Line += L"] " ;
	switch(p_MessageLevel) {
		case cErrorLevel				:	Line += L"[error]   " ;		break ;
		case cWarningLevel			:	Line += L"[warning] " ;		break ;
		case cInfoLevel				:	Line += L"[info]    " ;		break ;
		case cDebugLevel				:	Line += L"[debug]   " ;		break ;
		case cCommunicationLevel	:	Line += L"[com]     " ;		break ;
		default							:	Line += L"[msg]     " ;		break ;
	}
	switch(p_MessageType) {
		case cMessageType				:	Line += L"[msg]    " ;			break ;
		case cResourceType			:	Line += L"[res]    " ;			break ;
		case cOpcType					:	Line += L"[opc]    " ;			break ;
		case cDdeType					:	Line += L"[dde]    " ;			break ;
		case cTcpIpType				:	Line += L"[tcp/ip] " ;			break ;
		case cSerialType				:	Line += L"[serial] " ;			break ;
		case cConfigType				:	Line += L"[config] " ;			break ;
		case cDongleType				:	Line += L"[dongle] " ;			break ;
		case cDatabaseType			:	Line += L"[db]     " ;			break ;
		case cPerformanceType		:	Line += L"[perf]   " ;			break ;
		case cMemoryType				:	Line += L"[memory] " ;			break ;
		default							:	Line += L"[?]      " ;			break ;
	}
	Line += p_Message ;

	if(pStream) {
		fseek(pStream, 0, SEEK_END) ;
		FilePosition = std::max(static_cast<uint32_t>(sizeof(cUtf16Header)), std::min(FilePosition, static_cast<uint32_t>(ftell(pStream))) - 12) ;	//	-12 to overwrite "\n*** NOW ***"
		fseek(pStream, FilePosition, SEEK_SET) ;
		int32_t len = static_cast<int32_t>(Line.size()) * sizeof(wchar_t) ;
		fwrite(Line.c_str(), len, 1, pStream) ;
		FilePosition += len ;
		if(FilePosition > 1000000) { //	max 1MB
			SetEOF(pStream) ;	//	to cut a previous string overflowing the file
			fseek(pStream, sizeof(cUtf16Header), SEEK_SET) ;
			fputws(L"\r\n*** NOW ***", pStream) ;
			FilePosition = 12 ;
		}
		else {
			fputws(L"\r\n*** NOW ***", pStream) ;
			FilePosition += 12 ;
		}
		fclose(pStream) ;
	}
	#ifdef _DEBUG	//	LOG_TO_OUTPUT_WINDOWS
		OutputDebugStringW(Line.c_str()) ;
	#endif
}	

//	log the message
void echo(const char * p_0, const char * p_1, const char * p_2, const char * p_3, const char * p_4, const char * p_5, const char * p_6, const char * p_7)
{
	//	log message
	Trace(cWarningLevel, cComputingType, p_0, p_1, p_2, p_3, p_4, p_5, p_6, p_7) ;
	return ;
}


//	Copy this function on top of the main project file
void
Trace(int32_t p_MessageLevel, int32_t p_MessageType, const wchar_t * p_0, const wchar_t * p_1, const wchar_t * p_2, const wchar_t * p_3, const wchar_t * p_4, const wchar_t * p_5, const wchar_t * p_6, const wchar_t * p_7)
{
	std::wstring Line ;
	if(p_0) {
		Line += p_0 ;
		if(p_1) {
			Line += p_1 ;
			if(p_2) {
				Line += p_2 ;
				if(p_3) {
					Line += p_3 ;
					if(p_4) {
						Line += p_4 ;
						if(p_5) {
							Line += p_5 ;
							if(p_6) {
								Line += p_6 ;
								if(p_7) {
									Line += p_7 ;
								}
							}
						}
					}
				}
			}
		}
	}
	Trace(p_MessageLevel, p_MessageType, Line) ;
	return ;
}


//	utf-8 version
void
Trace(int32_t p_MessageLevel, int32_t p_MessageType, const char * p_0, const char * p_1, const char * p_2, const char * p_3, const char * p_4, const char * p_5, const char * p_6, const char * p_7)
{
	std::string Line ;
	if(p_0) {
		Line += p_0 ;
		if(p_1) {
			Line += p_1 ;
			if(p_2) {
				Line += p_2 ;
				if(p_3) {
					Line += p_3 ;
					if(p_4) {
						Line += p_4 ;
						if(p_5) {
							Line += p_5 ;
							if(p_6) {
								Line += p_6 ;
								if(p_7) {
									Line += p_7 ;
								}
							}
						}
					}
				}
			}
		}
	}
	Trace(p_MessageLevel, p_MessageType, std::utf8_to_wstring(Line)) ;
	return ;
}



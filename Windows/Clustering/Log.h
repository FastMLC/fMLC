
//	To log in the IDE Output window, uses this code :	OutputDebugString(L"blablabla") ;

//	WARNING :
//	This file is included in many toolbox files 
//	The implementation of these functions are located in Log.cpp, which is usefull when making a DLL

#pragma once
#include "asserte.h"
#include <string>


const int32_t	cErrorLevel				= 0 ;
const int32_t	cWarningLevel			= 1 ;
const int32_t	cInfoLevel				= 2 ;
const int32_t	cDebugLevel				= 3 ;
const int32_t	cCommunicationLevel	= 4 ;
const int32_t	cMaxMessageLevel		= 5 ;		//	could be anything else. If changed check their names in the language files

//	Message types
const int32_t	cMessageType			= 0 ;		//	All common messages: DLL started, thread started or stopped, etc.
const int32_t	cResourceType			= 1 ;		//	Missing resource, etc.
//	Acquisition group
const int32_t	cOpcType					= 2 ;		//	OPC
const int32_t	cDdeType					= 3 ;		//	DDE
const int32_t	cTcpIpType				= 4 ;		//	TCP/IP communication
const int32_t	cSerialType				= 5 ;		//	Serial port communication
//	Database group
const int32_t	cConfigType				= 6 ;		//	Configuration
const int32_t	cDongleType				= 7 ;		//	Hardware key
const int32_t	cRealTimeType			= 8 ;		//	Real-Time DB
const int32_t	cDatabaseType			= 9 ;		//	Database related message
const int32_t	cPerformanceType		= 10 ;	//	benchmarking messages
const int32_t	cMemoryType				= 11 ;	//	all kind of memory allocation problem
//	Computations
const int32_t	cComputingType			= 12 ;	//	computation

const int32_t	cMaxMessageType		= 12 ;	//	could be anything else. If changed check their names in MessageDlg.rc2

const std::wstring & TraceFile() ;
void	SetTraceFile(const std::wstring & p_NewFileName) ;
void	TraceDelete() ;

//	Functions defined in Log.cpp (or SdiFrame.cpp, or your main project file if it's a DLL)
//const wchar_t *	RString(int32_t resID, int32_t language = -1) ;
void	Trace(int32_t p_MessageLevel, int32_t p_MessageType, const std::wstring & p_Message) ;
void	Trace(int32_t messageLevel, int32_t messageType, const wchar_t * p_0, const wchar_t * p_1 = nullptr, const wchar_t * p_2 = nullptr, const wchar_t * p_3 = nullptr, const wchar_t * p_4 = nullptr, const wchar_t * p_5 = nullptr, const wchar_t * p_6 = nullptr, const wchar_t * p_7 = nullptr) ;
void	Trace(int32_t messageLevel, int32_t messageType, const char * p_0, const char * p_1 = nullptr, const char * p_2 = nullptr, const char * p_3 = nullptr, const char * p_4 = nullptr, const char * p_5 = nullptr, const char * p_6 = nullptr, const char * p_7= nullptr) ;

//	call Trace(cInfoLevel, cComputingType, ...)
void echo(const char * p_0, const char * p_1 = nullptr, const char * p_2 = nullptr, const char * p_3 = nullptr, const char * p_4 = nullptr, const char * p_5 = nullptr, const char * p_6 = nullptr, const char * p_7= nullptr) ;

//	This is the real function
void Asserte4(bool test, const wchar_t * file, int32_t line, const wchar_t * expr) ;

//	This one just map to the function above
//	In debug mode, the __include will work only if option inline expansion is set to: "only __inline"
//	But this option is not compatible with "Edit and Continue", so we can't use it
//	Only one solution remain : an awful macro
/*
__forceinline void	Asserte(bool test, const wchar_t * expr)
{
	Asserte(test, __FILE__, __LINE__, expr) ;
}
*/

//	This is the macro form :
//#define Asserte(test, expr)	Asserte4(test, TEXT(__FILE__), __LINE__, expr)


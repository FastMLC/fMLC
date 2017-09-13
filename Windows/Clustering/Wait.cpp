
#include "stdafx.h"

#include "std_plugin.h"
#include "Task.h"
#include "TaskMap.h"
// include this one in all files calling Wait() : #include "Wait.h"

#include <mutex>

//	**************************	globals	**********************************

//	the unique  task map
TTaskMap	g_TaskMap ;

//	a mutex to protect access to g_TaskMap
std::mutex	g_TaskMutex ;


//	************************************	Task management	*****************************

// open a new wait message
//	return the new task handle
int32_t WaitOpen(const std::wstring & p_Title, const std::wstring & p_Message, bool p_ShowCancelButton, int32_t p_TaskHdl = -1)
{
	g_TaskMutex.lock() ;	//	OutputDebugStringW(L"lock()") ;
	int32_t TaskHdl = g_TaskMap.OpenTask(p_Title, p_Message, p_ShowCancelButton, p_TaskHdl) ;
	g_TaskMutex.unlock() ;
	return TaskHdl ;	//	OutputDebugStringW(L"unlock()") ;
}


void Wait(int32_t p_TaskHdl, double p_Value)
{
	g_TaskMutex.lock() ;
	g_TaskMap.Wait(p_TaskHdl, p_Value) ;
	g_TaskMutex.unlock() ;
}


void Wait(int32_t p_TaskHdl, const std::wstring & p_Message)	//	utf16 version
{
	g_TaskMutex.lock() ;
	g_TaskMap.Wait(p_TaskHdl, p_Message) ;
	g_TaskMutex.unlock() ;
}


void Wait(int32_t p_TaskHdl, const std::string & p_Message)	//	utf8 version
{
	g_TaskMutex.lock() ;
	g_TaskMap.Wait(p_TaskHdl, std::utf8_to_wstring(p_Message)) ;
	g_TaskMutex.unlock() ;
}


void Wait(int32_t p_TaskHdl, const std::wstring & p_Message, double p_Value)
{
	g_TaskMutex.lock() ;
	g_TaskMap.Wait(p_TaskHdl, p_Message, p_Value) ;
	g_TaskMutex.unlock() ;
}


void Wait(int32_t p_TaskHdl, const std::string & p_Message, double p_Value)
{
	g_TaskMutex.lock() ;
	g_TaskMap.Wait(p_TaskHdl, std::utf8_to_wstring(p_Message), p_Value) ;
	g_TaskMutex.unlock() ;
}


bool IsTaskCanceled(int32_t p_TaskHdl)
{
	g_TaskMutex.lock() ;
	bool answer = g_TaskMap.IsTaskCanceled(p_TaskHdl) ;
	g_TaskMutex.unlock() ;
	return answer ;
}


void WaitClose(int32_t p_TaskHdl)
{
	g_TaskMutex.lock() ;
	g_TaskMap.CloseTask(p_TaskHdl) ;
	g_TaskMutex.unlock() ;
}

//	**************************************	Biolomics functions	*************************************

void GetWaitRange(int32_t * p_First, int32_t * p_Last)
{
	g_TaskMutex.lock() ;
	g_TaskMap.GetHdlRange(p_First, p_Last) ;
	g_TaskMutex.unlock() ;
	return ;
}


//	return true if message exist
int32_t GetWaitMessage(int32_t p_TaskHdl, double * p_Value, std::wstring & p_Message)
{
	g_TaskMutex.lock() ;
	int32_t answer = g_TaskMap.GetTaskMessage(p_TaskHdl, p_Value, p_Message) ;
	g_TaskMutex.unlock() ;
	return answer ;
}


void CancelTask(int32_t p_TaskHdl)
{
	g_TaskMutex.lock() ;
	g_TaskMap.CancelTask(p_TaskHdl) ;
	g_TaskMutex.unlock() ;
	return ;
}

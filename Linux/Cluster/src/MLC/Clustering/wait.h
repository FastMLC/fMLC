
#pragma once

#include <string>

//	update the global Wait text and value
int32_t WaitOpen(const std::wstring & p_Title, const std::wstring & p_Message, bool p_ShowCancelButton, int32_t p_TaskHdl = -1) ;
void Wait(int32_t p_TaskHdl, double p_Value) ;
void Wait(int32_t p_TaskHdl, const std::wstring & p_Message) ;
void Wait(int32_t p_TaskHdl, const std::string & p_Message) ;
void Wait(int32_t p_TaskHdl, const std::wstring & p_Message, double p_Value) ;
void Wait(int32_t p_TaskHdl, const std::string & p_Message, double p_Value) ;
bool IsTaskCanceled(int32_t p_TaskHdl) ;
void WaitClose(int32_t p_TaskHdl) ;

//	**************************************	Biolomics functions	*************************************

void	GetWaitRange(int32_t * p_First, int32_t * p_Last) ;
int32_t	GetWaitMessage(int32_t p_TaskHdl, double * p_Value, std::wstring & p_Message) ;
void	CancelTask(int32_t p_TaskHdl) ;
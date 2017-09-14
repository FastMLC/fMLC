
//#include "stdafx.h"

#include "task.h"
#include "taskmap.h"

#include "toolbox_tl.h"
#include <limits.h>

#include <cstdint>
#include <algorithm>

//	the min value of the task handle. Biolomics starts at 1
const int32_t g_MinHdl = 1001 ;


TTaskMap::TTaskMap(void)
	: m_ShouldOpenTaskViewer(false), m_ShouldCloseTaskViewer(false)
{
}


TTaskMap::~TTaskMap(void)
{
}


// Gets a free handle value
int32_t
TTaskMap::GetFreeHdl() const
{
	//	start at c_MinHdl to let value from 0 - 999 to Biolomics
	for(int32_t taskHdl = g_MinHdl; taskHdl < g_MinHdl + 100; ++taskHdl) { //	100 tasks on one computer should be enough...
		if(find(taskHdl) == end()) {
			return taskHdl ;
		}
	}
	return 0 ;	//	cannot happen
}


// open a new wait message
//	return the new message handle
int32_t
TTaskMap::OpenTask(const std::wstring & p_Title, const std::wstring & p_Message, bool p_ShowCancelButton, int32_t p_TaskHdl)
{
	if(p_TaskHdl < 0) {
		p_TaskHdl = GetFreeHdl() ;
	}
	//	if this task already exist, replace it => the title and ShowCancel buttons are updated by the TTask ctor
	this->operator[](p_TaskHdl) = TTask(p_TaskHdl,  p_Title, p_Message, p_ShowCancelButton) ;

	//	open and update the task viewer, if any
	m_ShouldOpenTaskViewer = true ;
	m_ShouldCloseTaskViewer = false ;

	//	remove it from the canceled list, as the handle are reused many times
	//std::vector<int32_t>::const_iterator cit = std::find(m_CanceledHdl.begin(), m_CanceledHdl.end(), p_TaskHdl) ;
        std::vector<int32_t>::iterator cit = std::find(m_CanceledHdl.begin(), m_CanceledHdl.end(), p_TaskHdl) ;
	if(cit != m_CanceledHdl.end()) { //	found
		m_CanceledHdl.erase(cit) ;
	}
	return p_TaskHdl ;
}


void
TTaskMap::Wait(int32_t p_TaskHdl, double p_Value)
{
	iterator_def it = find(p_TaskHdl) ;
	if(it != end()) { // found
		it->second.Wait(p_Value) ;
	}
	return ;
}


void
TTaskMap::Wait(int32_t p_TaskHdl, const std::wstring & p_Message)
{
	iterator_def it = find(p_TaskHdl) ;
	if(it != end()) { // found
		it->second.Wait(p_Message) ;
	}
	return ;
}


void
TTaskMap::Wait(int32_t p_TaskHdl, const std::wstring & p_Message, double p_Value)
{
	Wait(p_TaskHdl, p_Value) ;
	Wait(p_TaskHdl, p_Message) ;
	return ;
}


void
TTaskMap::CloseTask(int32_t p_TaskHdl)
{
	const_iterator_def it = find(p_TaskHdl) ;
	if(it != end()) { // found
		this->erase(p_TaskHdl) ;

		if(size() == 0) { //	no process remain, close the frmTaskViewer
			m_ShouldOpenTaskViewer = false ;
			m_ShouldCloseTaskViewer = true ;
		}
	}
}


void
TTaskMap::CancelTask(int32_t p_TaskHdl)	//	cancel a running process
{
	iterator_def it = find(p_TaskHdl) ;
	if(it != end()) { // found
		it->second.Cancel() ;						//	warn the task itself
		m_CanceledHdl.push_back(p_TaskHdl) ;	//	add this handle to the list of canceled handle
	}
}


bool
TTaskMap::IsTaskCanceled(int32_t p_TaskHdl)
{
	std::vector<int32_t>::const_iterator it = std::find(m_CanceledHdl.begin(), m_CanceledHdl.end(), p_TaskHdl) ;
	if(it != m_CanceledHdl.end()) { //	found
		return true ;
	}
	return false ;	//	not found
}


//	**************************************	Biolomics functions	*************************************

void
TTaskMap::GetHdlRange(int32_t * p_First, int32_t * p_Last)
{
	if(size() == 0) {
		* p_First = g_MinHdl ;
		* p_Last = g_MinHdl - 1 ;
	}
	else {
		* p_First = INT32_MAX ;
		* p_Last = INT32_MIN ;

		for(const_iterator it = begin(); it != end(); ++it) {
			* p_First = min_(* p_First, it->first) ;	// it->first = it->second.TaskHdl()
			* p_Last = max_(* p_Last, it->first) ;
		}
	}
	return ;
}


//	return 1 if message found
int32_t
TTaskMap::GetTaskMessage(int32_t p_TaskHdl, double * p_Value, std::wstring & p_Message)
{
	const_iterator it = find(p_TaskHdl) ;
	if(it == end()) {
		return 0 ;	// not found
	}
	//	found
	* p_Value = it->second.Value() ;
	p_Message = it->second.Message() ;
	return 1 ;	//	found
}
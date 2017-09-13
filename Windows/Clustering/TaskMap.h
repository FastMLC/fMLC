
#pragma once

#include "Task.h"

#include <vector>
#include <map>

/*	Design :
	'		- we call OpenWait() => a handle is created
	'		- we call n times Wait(...)
	'		- we call WaitClose() => the Process is removed
*/

class TTaskMap : public std::map<int32_t, TTask>
{
	typedef std::map<int32_t, TTask>::iterator iterator_def ;
	typedef const std::map<int32_t, TTask>::iterator const_iterator_def ;

public:
	TTaskMap(void) ;
	~TTaskMap(void) ;

	int32_t	OpenTask(const std::wstring & p_Title, const std::wstring & p_Message, bool p_ShowCancelButton, int32_t p_TaskHdl = -1) ;	//	open a new task
	void	Wait(int32_t p_TaskHdl, const std::wstring & p_Message, double p_Value) ;
	void	Wait(int32_t p_TaskHdl, const std::wstring & p_Message) ;
	void	Wait(int32_t p_TaskHdl, double p_Value) ;
	void	CloseTask(int32_t p_TaskHdl) ;

	void	CancelTask(int32_t p_TaskHdl) ;		//	'	cancel a running process
	bool	IsTaskCanceled(int32_t p_TaskHdl) ;	//	return true if the given process has been canceled

	//	BioloMICSS functions
	void	GetHdlRange(int32_t * p_First, int32_t * p_Last) ;
	int32_t	GetTaskMessage(int32_t p_TaskHdl, double * p_Value, std::wstring & p_Message) ;	//	GetMessage() is a windows macro...

	bool	ShouldOpenTaskViewer() const	{	return m_ShouldOpenTaskViewer ;	}
	bool	ShouldCloseTaskViewer() const	{	return m_ShouldCloseTaskViewer ;	}

private :
	int32_t	GetFreeHdl() const ;

	//	as IsTaskCanceled() may be called after WaitClose(), is is necessary to keep in mind all canceled processes
	std::vector<int32_t>	m_CanceledHdl ;	//	a short list of all canceled handle

	//	true if WaitOpen has been called
	bool	m_ShouldOpenTaskViewer ;	//	set when a process is added by WaitOpen()
	bool	m_ShouldCloseTaskViewer ;	//	set when the last process is closed by WaitClose()
	
} ;


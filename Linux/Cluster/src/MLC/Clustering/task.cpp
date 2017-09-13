
//#include "stdafx.h"
#include "task.h"

#include <string>

TTask::TTask()
	:	m_TaskHdl(0), 
		m_Title(L""), 
		m_Message(L""),
		m_ShowCancelButton(false),
		m_Value(0.0),
		m_Canceled(false),
		m_Updated(false)
{
}


TTask::TTask(int32_t p_Hdl, const std::wstring & p_Title, const std::wstring & p_Message, bool p_ShowCancelButton)
	:	m_TaskHdl(p_Hdl), 
		m_Title(p_Title), 
		m_Message(p_Message),
		m_ShowCancelButton(p_ShowCancelButton),
		m_Value(0.0),
		m_Canceled(false),
		m_Updated(false)
{
}


TTask::TTask(const TTask & p_Srce)
	:	m_TaskHdl(p_Srce.m_TaskHdl), 
		m_Title(p_Srce.m_Title), 
		m_Message(p_Srce.m_Message),
		m_ShowCancelButton(p_Srce.m_ShowCancelButton),
		m_Value(p_Srce.m_Value),
		m_Canceled(p_Srce.m_Canceled),
		m_Updated(p_Srce.m_Updated)
{
}


bool
TTask::operator !=(const TTask & p_Srce) const
{
	if(this == & p_Srce) {
		return false ;
	}
	if(m_TaskHdl != p_Srce.m_TaskHdl || m_Title != p_Srce.m_Title || m_Message != p_Srce.m_Message || m_ShowCancelButton != p_Srce.m_ShowCancelButton) {
		return true ;
	}
	if(m_Value != p_Srce.m_Value || m_Canceled != p_Srce.m_Canceled || m_Updated != p_Srce.m_Updated) {
		return true ;
	}
	return false ;
}


bool
TTask::operator ==(const TTask & p_Srce) const
{
	return ! this->operator != (p_Srce) ;
}


void
TTask::Wait(const std::wstring & p_Message, double p_Value)
{
	m_Message = p_Message ;
	m_Value = p_Value ;
}


void
TTask::Wait(const std::wstring & p_Message)
{
	m_Message = p_Message ;
}


void
TTask::Wait(double p_Value)
{
	m_Value = p_Value ;
}


void
TTask::Cancel()
{
	m_Canceled = true ;
}


int32_t
TTask::TaskHdl() const		
{
	return m_TaskHdl ;
}


const std::wstring &
TTask::Title() const 
{
	return m_Title ;
}


const std::wstring & 
TTask::Message() const 
{
	return m_Message ;
}


bool
TTask::ShowCancelButton() const 
{
	return m_ShowCancelButton ;
}

double
TTask::Value() const 
{
	return m_Value ;
}

double
TTask::Canceled() const 
{
	return m_Canceled ;
}

bool
TTask::Updated() const 
{
	return m_Updated ;
}



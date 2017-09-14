
#pragma once

#include <string>

class TTask {
public :
	TTask() ;
	TTask(int32_t p_TaskHdl, const std::wstring & p_Title, const std::wstring & p_Message, bool p_ShowCancelButton) ;
	TTask(const TTask & p_Srce) ;	//	cannot compile if explicit, due to the std::map

	bool	operator !=(const TTask & p_Srce) const ;
	bool	operator ==(const TTask & p_Srce) const ;

	void	Wait(const std::wstring & p_Message, double p_Value) ;
	void	Wait(const std::wstring & p_Message) ;
	void	Wait(double p_Value) ;
	void	Cancel() ;

	//	read only properties
	int32_t					TaskHdl() const ;
	const std::wstring &	Title() const ;
	const std::wstring &	Message() const ;
	bool						ShowCancelButton() const ;
	double					Value() const ;
	double					Canceled() const ;
	bool						Updated() const ;

protected :
	int32_t			m_TaskHdl ;					//	the unique handle used everywhere to recognize this message from others

	std::wstring	m_Title ;					//	a title used in the old frmSplah dialog
	std::wstring	m_Message ;					//	the progress information
	bool				m_ShowCancelButton ;		//	allow canceling or not

	double			m_Value ;					//	the progress value, from 0.0 to 1.0
	bool				m_Canceled ;				//	a flag telling if this process has been canceled by the user
	bool				m_Updated ;					//	a flag used for fast refresh of the TaskViewer grid
} ;


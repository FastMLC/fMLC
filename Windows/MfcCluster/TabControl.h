#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "afxeditbrowsectrl.h"
#include "MfcClusterDlg.h"

// TabControl dialog

class TabControl : public CDialogEx
{
	DECLARE_DYNAMIC(TabControl)

public:
	TabControl(CWnd* pParent = NULL);   // standard constructor
	virtual ~TabControl();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABCONTROL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};

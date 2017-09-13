#pragma once
#include "afxcmn.h"
#include "MfcClusterDlg.h"


// TabColtrolDlg dialog

class TabColtrolDlg : public CDialogEx
{
	DECLARE_DYNAMIC(TabColtrolDlg)

public:
	TabColtrolDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~TabColtrolDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TABCONTROL_DIALOG };
#endif
	// Implementation
protected:
	HICON m_hIcon;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
//	afx_msg void OnPaint();
//	afx_msg HCURSOR OnQueryDragIcon();

	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tabcontrol1;
	MfcClusterDlg m_tab1;
};
#pragma once
#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// OptionDlg dialog

class OptionDlg : public CDialogEx
{
	DECLARE_DYNAMIC(OptionDlg)

public:
	OptionDlg(CWnd* pParent = NULL);   // standard constructor
	OptionDlg(int32_t p_MinOverlap = 100, int32_t p_MaxNoForSingLevelClustering = 100, double p_MinSimForVisualization = 0.5, int32_t p_KneighborNo = 150,  int32_t p_VisDimension = 3, CWnd* pParent = NULL);
	virtual ~OptionDlg();
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTION_DIALOG };
#endif

protected:
	HICON m_hIcon;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support																
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	int32_t m_MinOverlap = 100;
	int32_t m_MinSeqNoForMLC = 100;
	double m_MinSimForVisualization = 0.5;
	int32_t m_KneighborNo = 150;
	int32_t m_VisDimension = 3;//2:2D;3:3D
	// The minimum overlap
	CEdit EF_MinimumOverlap;
	// The min. number of sequences for MLC
	CEdit EF_MinNoForMLC;
	// The minimum similarity saved for visualization
	CEdit EF_MinSimForVisualization;
	// The dimension for visualization
	CComboBox CO_VisDimension;
	// Kneighbor
	CEdit EF_Kneighbor;
};

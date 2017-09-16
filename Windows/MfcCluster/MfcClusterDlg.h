
// MfcClusterDlg.h : header file
//

#pragma once
#include <string>
#include "afxwin.h"
#include "afxcmn.h"
#include "ClusterDB.h"
#include "Comparison.h"
#include "ColumnTreeCtrl.h"
#include "afxeditbrowsectrl.h"

#include "boost_plugin.h"
#include "std_plugin.h"
#include "toolbox_tl.h"
#include "ToString.h" 

#include <fstream>
#include <locale>
#include <codecvt>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>

// Visualize
#include <map>
#include "Point.h"
#include <fstream>
#include <string>
#include <thread>  
#include <iostream>  

class TCluster;

// MfcClusterDlg dialog
class MfcClusterDlg :  public CDialogEx
{
// Construction
public:
	MfcClusterDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCCLUSTER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	void			LoadSourceFile();
	void			CreateReferenceCluster();
	void			DrawThresholds();
	void			DrawOptThresholds();
	void			ReadThresholds();
	bool			ReadThresholdsForOPTPrediction();
	void			SaveInGrid();
	void			DrawGroup(HTREEITEM & p_ParentNode, const TCluster & p_Cluster);
	void			DrawComparison(HTREEITEM & p_ParentNode, const TCluster & p_Cluster, const TComparison & p_Comp);
	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	void Wait(const wchar_t * p_Message, double x);
	void Wait(const wchar_t * p_Message);
	void Wait(double p_X);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	std::wstring	m_InputFilename;
	std::string	m_InputFilePath;
	std::string	m_Filename;
	std::string	m_TitleFilename;
	std::string	m_logfilename = "MLC.log";
	uint64_t		m_InputFileWriteTime;

	std::wstring	m_OutputClusterFilePath;
	vector<double>	m_Thresholds;						//	thresholds, the threshold to use, one value for all algorithm except the Multi-level MLC. In the interface, use a semi-colon separated list of double in increasing order
	vector<double>	m_FromThresholds;
	double	m_FromThreshold;
	double	m_ToThreshold;
	double	m_OptThreshold;
	double			m_Step = 0.0001;
	int32_t			m_FieldNamePos;					//	the zero-based index of the field name in the fasta file sequence name. 6 for medical fungi, 3 for the other CBS database. -1 if undefined
	double m_MinSimForVisualization = 0.5;			//minimum similarity to be display in the visualization
	int32_t			m_KneighborNo = 150; // as default by LargeVis
	int32_t			m_VisDimension = 3;				// the number of dimensions for visualization 2D or 3D; default=3D
	ClusterDB		m_ClusterDatabase;				//	the unique cluster database
	TCluster		*	m_RefCluster;						//	the reference cluster
	TCluster		*	m_Cluster;							//	the working cluster
	vector<int32_t>	m_GroupSeqNumbers;
	
	
	afx_msg void OnBnClickedCluster();
	afx_msg void OnEnChangeInputFilePath();
	void Visualize();
	bool m_recordnameextended;
	double Cluster(const vector<double> & thresholds);

	// The input field in the record name found in the input fasta file
	CComboBox CO_InputField;
	// The clustering algorithm
	CComboBox CO_Algorithm;
	// The thresholds to use
	CEdit EF_Threshold;
	// The starting threshold to predict the optimal threshold 
	CEdit EF_FromThreshold;
	// The ending threshold to predict the optimal threshold 
	CEdit EF_ToThreshold;
	// The step
	CEdit EF_Steps;
	// The number of groups found during clustering
	CEdit EF_GroupNo;
	// The F-Measure computed during clustering
	CEdit EF_FMeasure;	
	
	// The clustering results, 	//	see http://www.codeproject.com/Articles/23692/Tree-Control-with-Columns
	CColumnTreeCtrl	TR_Result;

	// The bottom information line
	CStatic St_Status;
	CProgressCtrl PR_Wait;

	// if checked, we compute the F-Measure
	CButton CB_ComputeFMeasure;
	afx_msg void OnCbnSelchangeAlgorithm();
	// Run computations
	CMFCButton PB_Cluster;
	// The path of the input fasta file
	CMFCEditBrowseCtrl EB_InputFilePath;
	// the name of the output file for the computed cluster
	CMFCEditBrowseCtrl EB_OutputFilePath;
	afx_msg void OnBnClickedExport();
	CButton PB_Export;
	afx_msg void OnBnClickedFinallevel();
	afx_msg void OnBnClickedComputefmeasure();
	afx_msg void OnBnClickedOptimize();
	afx_msg void OnBnClickedOption();
	afx_msg void OnBnClickedVisualize();
	afx_msg void OnBnClickedSavesimilarity();
	afx_msg void OnBnClickedSavefullsimilarity();
	afx_msg void OnBnClickedSave();
};

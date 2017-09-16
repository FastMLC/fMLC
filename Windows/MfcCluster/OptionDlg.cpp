// OptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MfcCluster.h"
#include "MfcClusterDlg.h"
#include "OptionDlg.h"
#include "afxdialogex.h"
#include "boost_plugin.h"
#include "ToString.h" 


// OptionDlg dialog

IMPLEMENT_DYNAMIC(OptionDlg, CDialogEx)

OptionDlg::OptionDlg(CWnd* pParent /*=NULL*/ )
	: CDialogEx(IDD_OPTION_DIALOG, pParent)
{
}

OptionDlg::OptionDlg(int32_t p_MinOverlap, int32_t p_MinSeqNoForMLC, double p_MinSimForVisualization, int32_t p_KneighborNo, int32_t p_VisDimension, CWnd* pParent)
	: CDialogEx(IDD_OPTION_DIALOG, pParent)
{	
	m_MinOverlap = p_MinOverlap;
	m_MinSeqNoForMLC = p_MinSeqNoForMLC;
	m_MinSimForVisualization = p_MinSimForVisualization;
	m_KneighborNo = p_KneighborNo;
	m_VisDimension = p_VisDimension;
}

OptionDlg::~OptionDlg()
{
}

enum DimensionEnum {
	D2 = 0,					//		2D
	D3 = 1,					//	 	3D
};

void OptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, EF_Min_Overlap, EF_MinimumOverlap);
	DDX_Control(pDX, EF_MAX_NUMBER, EF_MinNoForMLC);
	DDX_Control(pDX, EF_MIN_SIM_FOR_VISUALIZATION, EF_MinSimForVisualization);
	DDX_Control(pDX, EF_NUMBER_FOR_VISUALIZATION, EF_Kneighbor);
	DDX_Control(pDX, CO_VIS_DIMENSION, CO_VisDimension);
}


BEGIN_MESSAGE_MAP(OptionDlg, CDialogEx)
	ON_BN_CLICKED(PB_OK, &OptionDlg::OnBnClickedOk)
	ON_BN_CLICKED(PB_CANCEL, &OptionDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// OptionDlg message handlers
BOOL OptionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		CString strAboutMenu;
		// ReSharper disable once CppEntityNeverUsed
		BOOL bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	wchar_t stringC[264];
	EF_MinimumOverlap.SetWindowTextW(ToString(stringC, m_MinOverlap));
	EF_MinNoForMLC.SetWindowTextW(ToString(stringC, m_MinSeqNoForMLC));
	EF_MinSimForVisualization.SetWindowTextW(ToString(stringC, m_MinSimForVisualization));
	EF_Kneighbor.SetWindowTextW(ToString(stringC, m_KneighborNo));
	//	add dimensions
	CO_VisDimension.AddString(L"2D");
	CO_VisDimension.AddString(L"3D");
	if (m_VisDimension == 2) {
		CO_VisDimension.SetCurSel(0);
	}
	else {
		CO_VisDimension.SetCurSel(1);
	}
	return TRUE;  // return TRUE  unless you set the focus to a control
}


void OptionDlg::OnBnClickedOk()
{
	// Get Min overlap for DNA comparison
	wchar_t buffer[264];
	EF_MinimumOverlap.GetWindowTextW(buffer, 256);
	double i = std::stod(buffer);
	m_MinOverlap = (int32_t)i;
	//Get minimum similarity to be display in the visualization
	EF_MinSimForVisualization.GetWindowTextW(buffer, 256);
	i = std::stod(buffer);
	m_MinSimForVisualization = i;
	EF_Kneighbor.GetWindowTextW(buffer, 256);
	i = std::stod(buffer);
	m_KneighborNo = (int32_t)i;
	EF_MinNoForMLC.GetWindowTextW(buffer, 256);
	i = std::stod(buffer);
	m_MinSeqNoForMLC = (int32_t)i;
	DimensionEnum de = static_cast<DimensionEnum>(CO_VisDimension.GetCurSel());
	if (de == D2) {
		m_VisDimension = 2;
	}
	else
	{
		m_VisDimension = 3;
	}
	EndDialog(0);
}


void OptionDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	EndDialog(0);
}



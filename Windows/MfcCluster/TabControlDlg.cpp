// TabColtrolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MfcCluster.h"
#include "MfcClusterDlg.h"
#include "TabControlDlg.h"
#include "afxdialogex.h"
#include "afxcmn.h"
#include "afxdialogex.h"

// TabColtrolDlg dialog

IMPLEMENT_DYNAMIC(TabColtrolDlg, CDialogEx)

TabColtrolDlg::TabColtrolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TABCONTROL_DIALOG, pParent)
{

}

TabColtrolDlg::~TabColtrolDlg()
{
}

void TabColtrolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABCONTROL, m_tabcontrol1);
}


BEGIN_MESSAGE_MAP(TabColtrolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_MOVE()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// TabColtrolDlg message handlers

BOOL TabColtrolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

											// TODO: Add extra initialization here
	
	CTabCtrl* ptabcontrol = (CTabCtrl*)GetDlgItem(IDC_TABCONTROL);

	m_tab1.Create(IDD_MFCCLUSTER_DIALOG, m_tabcontrol1.GetWindow(IDD_MFCCLUSTER_DIALOG));
	
	TCITEM item1;
	item1.mask = TCIF_TEXT | TCIF_PARAM;
	item1.lParam = (LPARAM)& m_tab1;
	item1.pszText = _T("Clustering");
	ptabcontrol->InsertItem(0, &item1);

	//Dialog positioning
	CRect rcItem;
	ptabcontrol->GetItemRect(0, &rcItem);
	m_tab1.SetWindowPos(NULL, rcItem.left, rcItem.bottom + 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	m_tab1.ShowWindow(SW_SHOW);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


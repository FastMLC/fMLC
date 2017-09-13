// TabControl.cpp : implementation file
//

#include "stdafx.h"
#include "MfcCluster.h"
#include "TabControl.h"
#include "afxdialogex.h"
#include <string>
#include "afxwin.h"
#include "afxcmn.h"


// TabControl dialog

IMPLEMENT_DYNAMIC(TabControl, CDialogEx)

TabControl::TabControl(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_TABCONTROL, pParent)
{

}

TabControl::~TabControl()
{
}

void TabControl::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(TabControl, CDialogEx)
END_MESSAGE_MAP()


// TabControl message handlers

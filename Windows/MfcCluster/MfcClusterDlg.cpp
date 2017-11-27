
// MfcClusterDlg.cpp : implementation file
#include <stdio.h>
#include "stdafx.h"
#include <time.h>
#include "afxdialogex.h"

#include "boost_plugin.h"
#include "ClusterDB.h"
#include "Cluster.h"
#include "Clustering.h"
#include "MfcClusterDlg.h"
#include "OptionDlg.h"
#include "Point.h"
#include "Optimizer.h"
#include "resource.h"
#include "std_plugin.h"
#include "toolbox_tl.h"
#include "ToString.h" 
//#include "Wait.h" 

#include <fstream>
#include <locale>
#include <codecvt>
#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator> 

#ifdef _DEBUG
#define new DEBUG_NEW
#define use_CRT_SECURE_NO_WARNINGS
#endif
#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	
// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

enum AlgorithmEnum {
	SLC = 0,					//		Single level clustering, multi-threaded
	MLC = 1,					//	 	Multi level clustering, multi-threaded
	CCBC = 2,				// 	connected components based clustering, multi-threaded
	GC = 3,				//	 	greedy clustering, multi-threaded

	SLC_ST = 4,				// 	Single level clustering, single-threaded
	MLC_ST = 5,				// 	Multi level clustering, single-threaded
	CCBC_ST = 6,			// 	connected components based clustering, single-threaded
	GC_ST = 7,			//		greedy clustering, single-threaded
};

//	********************************************************************************************
//
//												MfcClusterDlg dialog
//
//	********************************************************************************************

//IMPLEMENT_DYNAMIC(MfcClusterDlg, CDialogEx)

MfcClusterDlg::MfcClusterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCCLUSTER_DIALOG, pParent), m_FieldNamePos(-1), m_RefCluster(nullptr), m_Cluster(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Thresholds.push_back(0.95);
	m_Thresholds.push_back(0.98);
}


void MfcClusterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, CO_INPUT_FIELD, CO_InputField);
	DDX_Control(pDX, CO_ALGORITHM, CO_Algorithm);
	DDX_Control(pDX, EF_THRESHOLD, EF_Threshold);
	DDX_Control(pDX, EF_FROM_THRESHOLD, EF_FromThreshold);
	DDX_Control(pDX, EF_TO_THRESHOLD, EF_ToThreshold);
	DDX_Control(pDX, EF_STEP, EF_Steps);
	DDX_Control(pDX, EF_GROUP_NO, EF_GroupNo);
	DDX_Control(pDX, EF_F_MEASURE, EF_FMeasure);
	DDX_Control(pDX, ST_STATUS, St_Status);
	DDX_Control(pDX, PROG_WAIT, PR_Wait);
	DDX_Control(pDX, TR_RESULT, TR_Result);
	DDX_Control(pDX, CB_F_MEASURE, CB_ComputeFMeasure);
	DDX_Control(pDX, PB_CLUSTER, PB_Cluster);
	DDX_Control(pDX, EB_INPUT_FILE_PATH, EB_InputFilePath);
	DDX_Control(pDX, EB_OUTPUT_PATH, EB_OutputFilePath);
	DDX_Control(pDX, PB_EXPORT, PB_Export);
}

BEGIN_MESSAGE_MAP(MfcClusterDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(PB_CLUSTER, &MfcClusterDlg::OnBnClickedCluster)
	ON_CBN_SELCHANGE(CO_ALGORITHM, &MfcClusterDlg::OnCbnSelchangeAlgorithm)
	ON_EN_CHANGE(EB_INPUT_FILE_PATH, &MfcClusterDlg::OnEnChangeInputFilePath)
	ON_BN_CLICKED(PB_EXPORT, &MfcClusterDlg::OnBnClickedExport)
	ON_BN_CLICKED(PB_FinalLevel, &MfcClusterDlg::OnBnClickedFinallevel)
	ON_BN_CLICKED(PB_ComputeFMeasure, &MfcClusterDlg::OnBnClickedComputefmeasure)
	ON_BN_CLICKED(PB_Optimize, &MfcClusterDlg::OnBnClickedOptimize)
	ON_BN_CLICKED(PB_Option, &MfcClusterDlg::OnBnClickedOption)
	ON_BN_CLICKED(PB_Visualize, &MfcClusterDlg::OnBnClickedVisualize)
	ON_BN_CLICKED(PB_SaveSimilarity, &MfcClusterDlg::OnBnClickedSavesimilarity)
	ON_BN_CLICKED(PB_SaveFullSimilarity, &MfcClusterDlg::OnBnClickedSavefullsimilarity)
	ON_BN_CLICKED(PB_SAVE, &MfcClusterDlg::OnBnClickedSave)
END_MESSAGE_MAP()


// MfcClusterDlg message handlers

BOOL MfcClusterDlg::OnInitDialog()
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

	//	add the algorithm, see AlgorithmEnum above
	CO_Algorithm.AddString(L"SLC (Single level clustering)");
	CO_Algorithm.AddString(L"MLC (Multi-level clustering)");
	CO_Algorithm.AddString(L"Ccbc");
	CO_Algorithm.AddString(L"GC");
	CO_Algorithm.AddString(L"SLC (single-threaded)");
	CO_Algorithm.AddString(L"MLC (single-threaded)");
	CO_Algorithm.AddString(L"Ccbc (single-threaded)");
	CO_Algorithm.AddString(L"GC (single-threaded)");
	CO_Algorithm.SetCurSel(1);

	//	update the threshold after choosing the algorithm
	DrawThresholds();

	PR_Wait.SetRange(0, 1000);
	PR_Wait.SetPos(0);

	//	see http://www.codeproject.com/Articles/23692/Tree-Control-with-Columns

	CWnd *pWnd = GetDlgItem(TR_RESULT);
	CRect rect;
	pWnd->GetWindowRect(&rect);
	double pc = rect.Width() / 100.0;
	int col = 0;
	TR_Result.InsertColumn(col++, L"Id", LVCFMT_LEFT, static_cast<int>(pc * 15));								//	sequence ID, LVCFMT_LEFT is mandatory otherwise the + and - symbols are disconected from the text
	TR_Result.InsertColumn(col++, L"Name", LVCFMT_LEFT, static_cast<int>(pc * 35));							//	sequence name
	TR_Result.InsertColumn(col++, L"Info", LVCFMT_LEFT, static_cast<int>(pc * 20));							//	info
	TR_Result.InsertColumn(col, L"Distance from centroid", LVCFMT_LEFT, static_cast<int>(pc * 20));		//	distance from parent centroid

	// set style for tree view
	CCustomTreeChildCtrl & ctrl = TR_Result.GetTreeCtrl();
	UINT uTreeStyle = TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_FULLROWSELECT;
	ctrl.ModifyStyle(0, uTreeStyle);

	PB_Cluster.ModifyStyle(0, BS_LEFT);
	HICON hIcon = static_cast<HICON>(LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_START), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));
	//HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_START));
	PB_Cluster.SetIcon(hIcon);
	//PB_Cluster.SetImage(LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_START)));

	EB_InputFilePath.EnableFileBrowseButton(L"fas", L"Fasta files (*.fas;*.fa)|*.fas;*.fa|All Files (*.*)|*.*|");
	EB_OutputFilePath.EnableFileBrowseButton(L"clus", L"Cluster result files (*.clus)|*.clus|All Files (*.*)|*.*|");
	wchar_t stringC[264];
	
	EF_Steps.SetWindowTextW(ToString(stringC, m_Step, 4));

	//SetTimer(1, 250, nullptr);

	St_Status.SetWindowTextW(L"Ready");
	return TRUE;  // return TRUE  unless you set the focus to a control
}


//	manage a single progress bar, so if there is multiple tasks running, display the last one
void 
MfcClusterDlg::OnTimer(UINT_PTR nIDEvent)
{
	//int32_t first, last;
	//GetWaitRange(&first, &last);	//	get the available messages
	//
	//if(first <= last) {
	//	double x = 0.0;
	//	std::wstring message;

	//	if (GetWaitMessage(last, &x, message)) {
	//		St_Status.SetWindowTextW(message.c_str());
	//		PR_Wait.SetPos(static_cast<int>(x) * 1000);
	//	}
	//}
	//else{
	//	St_Status.SetWindowTextW(L"Ready");
	//	PR_Wait.SetPos(0);
	//}

	// Call base class handler.
	//CDialogEx::OnTimer(nIDEvent);
}


void MfcClusterDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}


void
MfcClusterDlg::Wait(const wchar_t * p_Message, double x)
{
	Wait(p_Message);
	Wait(x);
}


void
MfcClusterDlg::Wait(const wchar_t * p_Message)
{
	St_Status.SetWindowTextW(p_Message);
}


void
MfcClusterDlg::Wait(double p_X)
{
	static int lastx = -1;
	int x = static_cast<int>(p_X * 1000);		//as we called PR_Wait.SetRange(0, 1000);
	if(lastx == x) {
		return;
	}
	lastx = x;
	PR_Wait.SetPos(x);
	PR_Wait.UpdateWindow();
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void MfcClusterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
// ReSharper disable once CppMemberFunctionMayBeConst
HCURSOR MfcClusterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

bool is_file_exist(const char *fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

void MfcClusterDlg::OnEnChangeInputFilePath()
{
	St_Status.SetWindowTextW(L"");
	CO_InputField.Clear();
	CO_InputField.ResetContent();
	//	read the first line and update the reference field combo box

	CString filePath;
	EB_InputFilePath.GetWindowTextW(filePath); // return full path and filename
	m_InputFilename = filePath.GetBuffer();


	//	open file
	std::ifstream file(m_InputFilename, std::ofstream::binary);
	if (file.bad()) {
		St_Status.SetWindowTextW(L"Unreadable input file");
		return ; //	error
	}

	std::string line;
	line.reserve(2000);

	std::string seqName;
	seqName.reserve(2000);

	//	initialize variables
	bool FirstLineLoaded = false;
	//	parse file
	while (file.good()) {
		//	read line
		line.clear();
		std::getline(file, line);

		//	remole trailing CR, LF, whitespaces, etc.
		while (line.length() > 0 && line.back() <= 32) {
			line.pop_back();
		}

		if (line.empty()) {
			continue;
		}
		//	a non-empty line is loaded
		if (FirstLineLoaded == false) { //	check for the utf-8 header
			if (line.size() >= 3 && line[0] == cUtf8Header[0] && line[1] == cUtf8Header[1] && line[2] == cUtf8Header[2]) {
				//	a pure utf-8 file, remove the utf-8 BOM. If not, it may be an ascii file. Load it anyway.
				line.erase(line.begin(), line.begin() + 3);
			}
			FirstLineLoaded = true;
			if (line.empty()) {
				continue;
			}
		}

		if (line[0] == '>') { //	a new sequence name
			//	remove leading '>' and other scrap
			while (line.length() > 0 && (line.front() == '>' || line.front() <= 32)) {
				line.erase(line.begin(), line.begin() + 1);
			}
			seqName = line;
			break;
		}
		
	} //	read next row from file
	file.close();

	if(seqName.empty()) {
		St_Status.SetWindowTextW(L"Sequence name is empty");
		return;
	}
	//	fill in the combo box
	//read title file
	std::string fullfilename;
	fullfilename = std::string(m_InputFilename.begin(), m_InputFilename.end());
	const size_t last_slash_idx = fullfilename.find_last_of("\\/");
	//get file path
	m_InputFilePath = fullfilename.substr(0, last_slash_idx + 1);
	// Remove extension if present.
	const size_t period_idx = fullfilename.rfind('.');
	if (std::string::npos != period_idx)
	{
		fullfilename.erase(period_idx);
	}
	//get filename
	m_Filename = fullfilename.substr(last_slash_idx+1);
	//get title file
	m_TitleFilename = fullfilename + ".title";
	if (is_file_exist(m_TitleFilename.c_str())) {
		std::ifstream titlefile(m_TitleFilename, std::ofstream::binary);
		std::getline(titlefile, line);
		if (line[0] == '>') { //	a new sequence name
									 //	remove leading '>' and other scrap
			while (line.length() > 0 && (line.front() == '>' || line.front() <= 32)) {
				line.erase(line.begin(), line.begin() + 1);
			}
		}
		titlefile.close();
	}
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	int32_t fieldIdx = 0;
	std::string comboLine = clustering::StrainNameFilter(line, fieldIdx);
	while(! comboLine.empty()) {
		//		std::string narrow = converter.to_bytes(wide_utf16_source_string);
		std::wstring wide = converter.from_bytes(comboLine);
		CO_InputField.AddString(wide.c_str());
		comboLine = clustering::StrainNameFilter(line, ++fieldIdx);
	}
	if (fieldIdx == 0){
		MessageBox(L"Please check the title file.", L"Warning", MB_ICONWARNING | MB_OK);
		Wait(L"", 0.0);
		return;
	}
	//	choose one of them, often the third one
	if(m_FieldNamePos >= 0 && m_FieldNamePos < fieldIdx) {	//	re-use the same index
		CO_InputField.SetCurSel(m_FieldNamePos);
	}
	else if (fieldIdx > 1) {	//	others
		CO_InputField.SetCurSel(1);
	}
	else {
		CO_InputField.SetCurSel(0);
	}
	m_recordnameextended = false;
}

//	return the FILETIME structure containing the last change date-time of the given file
uint64_t GetFileWriteTime(const wchar_t * p_FilePath)
{
	HANDLE hFile = CreateFile(p_FilePath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
	
	uint64_t fileWriteTime = 0;
	FILETIME ftCreate, ftAccess, ftWrite;
	if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
		fileWriteTime = (static_cast<uint64_t>(ftWrite.dwHighDateTime) << 32) | static_cast<uint64_t>(ftWrite.dwLowDateTime);
	}
	CloseHandle(hFile);
	return fileWriteTime;
}


void MfcClusterDlg::OnBnClickedCluster()
{
	wchar_t stringC[1032];

	//	clear outputs
	EF_GroupNo.SetWindowTextW(L"");
	EF_FMeasure.SetWindowTextW(L"");

	//	read inputs
	ReadThresholds();
	//	redraw it to show the real values after cleanup
	DrawThresholds();

	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	bool computeQuality = (CB_ComputeFMeasure.GetCheck() == BST_CHECKED);
	int32_t fieldNamePos = CO_InputField.GetCurSel();
	int stepNo = 3;
	if(computeQuality) {
		stepNo += 2;
	}
	double dx = 1.0 / stepNo, x = dx;
	//int hdl = WaitOpen(L"Clustering", L"Clustering", true);

	CString inFilePath;
	EB_InputFilePath.GetWindowTextW(inFilePath);

	//	check
	// Retrieve the file times for the file.
	uint64_t fileWriteTime = GetFileWriteTime(static_cast<const wchar_t *>(inFilePath));

	if (m_ClusterDatabase.m_Sequences.size() == 0 || m_InputFilename != inFilePath.GetBuffer() || m_InputFileWriteTime != fileWriteTime || fieldNamePos != m_FieldNamePos) {	//	reload
		m_InputFilename = inFilePath.GetBuffer();
		m_FieldNamePos = fieldNamePos;
		m_InputFileWriteTime = fileWriteTime;
		if (m_InputFilename.size() == 0) {
			MessageBox(L"Please select a source fasta file", L"Warning", MB_ICONWARNING | MB_OK);
			Wait( L"", 0.0);
			return;
		}
		Wait( L"Loading source file ...", x);		x += dx;
		LoadSourceFile();
	}

	if((algo == MLC || algo == MLC_ST ) && m_Thresholds.size() != 1) {
		computeQuality = false;
		//CB_ComputeFMeasure.SetCheck(BST_UNCHECKED);
	}

	//	Pre-computations
	if (computeQuality) {
		if (m_FieldNamePos < 0) {
			MessageBox(L"Please select the position of the sequence name in the fasta file", L"Warning", MB_ICONWARNING | MB_OK);
			Wait( L"", 0.0);
			return;
		}
		Wait( L"Creating reference cluster", x);		x += dx;
		CreateReferenceCluster();
	}

	//write to log file  the starting time of clustering
	time_t now = time(0);
	// Convert now to tm struct for local timezon
	struct tm timeinfo;
	localtime_s(&timeinfo, &now);
	char str[26];
	asctime_s(str, sizeof str, &timeinfo);
	std::ofstream logfile(m_logfilename.c_str(), std::ios_base::out | std::ios_base::app);
	logfile << "Start clustering " << m_Filename << ": " << str; 
	logfile.close();
	//	Computations
	delete m_Cluster;
	m_Cluster = new TCluster(&m_ClusterDatabase);

	Wait( L"Clustering ...", x);	x += dx;

	switch (algo) {
																							//	*****	Multi-threaded version	*****
		default:
		case SLC: { //	Single level clustering
			m_Cluster->Mlc_Mt(m_Thresholds.back());
			break;
		}
		case MLC: { //	Multi-level clustering
			m_Cluster->MlcAll_Mt(m_Thresholds, 0);
			break;
		}
		case CCBC: { //	CCBC
			m_Cluster->Ccbc_Mt(m_Thresholds.back());
			break;
		}
		case GC: { //	Greedy Clust
			m_Cluster->GClust_Mt(m_Thresholds.back());
			break;
		}
																							//	*****	Single-threaded version	*****
		case SLC_ST: { //	single-level clustering
			m_Cluster->Mlc_St(m_Thresholds.back());
			break;
		}
		case MLC_ST: { //	Multi-level clustering
			m_Cluster->MlcAll_St(m_Thresholds, 0);
			break;
		}
		case CCBC_ST: { //	CCBC
			m_Cluster->Ccbc_St(m_Thresholds.back());
			break;
		}
		case GC_ST: { //	GC
			m_Cluster->GClust_St(m_Thresholds.back());
			break;
		}
	}
	//write to log file the ending time 
	now = time(0);
	localtime_s(&timeinfo, &now);
	asctime_s(str, sizeof str, &timeinfo);
	logfile.open(m_logfilename.c_str(), std::ios_base::out | std::ios_base::app);
	logfile << "End: " << str << "\n";
	logfile.close();

	//	just to have a nicer display
	m_Cluster->SortAllGroups();

	std::wostringstream stream;
	stream << std::endl << L"Cluster contains " << m_Cluster->GroupNo() << L" groups, " << m_Cluster->Sequences().size();
	if(m_RefCluster) {
		stream << " and " << m_RefCluster->ClassifiedSequenceNo() << L" classified sequences";
	}
	OutputDebugString(stream.str().c_str());

	//	update the group number
	size_t len = m_Cluster->FinalGroupNo();
	EF_GroupNo.SetWindowTextW(ToString(stringC, len));
	m_GroupSeqNumbers.clear();
	m_GroupSeqNumbers.reserve(len);

	//	display that cluster in our grid
	Wait( L"Filling in the result grid ...", x);		x += dx;
	SaveInGrid();
	sort(m_GroupSeqNumbers.begin(), m_GroupSeqNumbers.end());
	size_t size = m_GroupSeqNumbers.size();
	int32_t sum = 0;
	int32_t count = 0;
	int32_t start = 0;
	for (uint32_t i = 0; i < m_GroupSeqNumbers.size(); ++i) {
		if( m_GroupSeqNumbers[i] > 1){
			sum = sum + m_GroupSeqNumbers[i];
			count = count + 1;
			//break;
		}
		//start = start + 1;
	}
	//	compute and update the F-Measure
	if (computeQuality) {
		Wait( L"Computing quality ...", x);		x += dx;
		double f = m_Cluster->F_Measure_Mt(* m_RefCluster);
		EF_FMeasure.SetWindowTextW(ToString(stringC, f, 3));
	}
	else {
		EF_FMeasure.SetWindowTextW(L"");
	}
	//	enable the export button
	PB_Export.EnableWindow(true);

	Wait( L"Ready", 1.0);
}



void MfcClusterDlg::OnBnClickedExport()
{
	if(m_Cluster == nullptr) {
		MessageBox(L"Please run a computation before exporting the resulting cluster", L"Warning", MB_ICONWARNING | MB_OK);
		Wait(L"", 0.0);
		return;
	}

	CString outFilePath;
	EB_OutputFilePath.GetWindowTextW(outFilePath);
	m_OutputClusterFilePath = outFilePath.GetBuffer();

	if (m_OutputClusterFilePath.size() == 0) {
		MessageBox(L"Please select a destination file for the cluster results", L"Warning", MB_ICONWARNING | MB_OK);
		Wait(L"", 0.0);
		return;
	}
	Wait(L"Saving cluster as text ...", 0.3);
	uint32_t fieldNamePos = CO_InputField.GetCurSel();
	m_Cluster->SaveAsText(m_OutputClusterFilePath.c_str(), fieldNamePos, (uint32_t)m_Thresholds.size());

	Wait(L"Ready", 0.0);
}

void MfcClusterDlg::LoadSourceFile()
{
	if(m_InputFilename.empty()) {
		St_Status.SetWindowTextW(L"Please choose an input fasta file as the source of the sequences to cluster");
		return;
	}
	St_Status.SetWindowTextW((L"Loading file " + m_InputFilename).c_str());
	//	load the reference cluster
	bool isdna = true;
	if(m_ClusterDatabase.LoadFastaFile(m_InputFilename.c_str(), isdna, m_FieldNamePos) != 0) {
		St_Status.SetWindowTextW(L"An error occured when loading the input fasta file");
		return;
	}
	St_Status.SetWindowTextW(L"Ready");
}


void MfcClusterDlg::CreateReferenceCluster()
{
	St_Status.SetWindowTextW(L"Creating reference cluster...");
	delete m_RefCluster;

	m_RefCluster = new TCluster(& m_ClusterDatabase);
	//m_RefCluster->ExportSequences(L"D:\\Clustering\\Cbs2800_sorted.txt");
	//m_RefCluster->ExportForDuong(L"D:\\Clustering\\Cbs2800_sim_list.txt");
	int32_t fieldNamePos = CO_InputField.GetCurSel(); 
	m_RefCluster->ClusterByName(fieldNamePos);
	//m_RefCluster->OutputDebug();

	std::wostringstream stream0;
	stream0 << std::endl << L"*******************************************************************************************************" << std::endl;
	stream0 << std::endl << L"Reference cluster contains " << m_RefCluster->GroupNo() << L" groups, " << m_RefCluster->Sequences().size() << " source sequences and " << m_RefCluster->ClassifiedSequenceNo() << L" classified sequences";
	stream0 << std::endl << L"Source file: " << m_InputFilename;
	OutputDebugString(stream0.str().c_str());

	St_Status.SetWindowTextW(L"Ready");
}


//	update the user interface
//	try to keep in mind the list of thresholds, sot that if user runs a MLC, then a SLC and again a MLC, the previous thresholds come back
void MfcClusterDlg::DrawThresholds()
{
	wchar_t stringC[264];
	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	switch (algo) {
		default: {
			EF_Threshold.SetWindowTextW(ToString(stringC, m_Thresholds.back(), 4));
			EF_FromThreshold.SetWindowTextW(ToString(stringC, m_Thresholds.back(), 4));
			EF_ToThreshold.SetWindowTextW(L"1");
			break;
		}
		case MLC: { //	Multi-level clustering
			std::wstring thresholdStr;
			std::wstring tothresholdStr;
			for (auto x : m_Thresholds) {	//	there is always at least one value
				thresholdStr += ToString(stringC, x, 4);
				thresholdStr += L"; ";
			}
			thresholdStr.resize(thresholdStr.size() - 2);	//	remove the "; "
			EF_Threshold.SetWindowTextW(thresholdStr.c_str());
			EF_FromThreshold.SetWindowTextW(thresholdStr.c_str());
			EF_ToThreshold.SetWindowTextW(L"1");
			break;
		}
		case MLC_ST: { //	Multi-level clustering
			std::wstring thresholdStr;
			std::wstring tothresholdStr;
			for (auto x : m_Thresholds) {	//	there is always at least one value
				thresholdStr += ToString(stringC, x, 4);
				thresholdStr += L"; ";
			}
			thresholdStr.resize(thresholdStr.size() - 2);	//	remove the "; "
			EF_Threshold.SetWindowTextW(thresholdStr.c_str());
			EF_FromThreshold.SetWindowTextW(thresholdStr.c_str());
			EF_ToThreshold.SetWindowTextW(L"1");
			break;
		}
	}
}

void MfcClusterDlg::DrawOptThresholds()
{
	wchar_t stringC[264];
	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	switch (algo) {
	default: {
		EF_Threshold.SetWindowTextW(ToString(stringC, m_OptThreshold, 4));
		break;
	}
	case MLC: { //	Multi-level clustering
		std::wstring thresholdStr;
		uint32_t i = 0;
		for (auto x : m_FromThresholds) {	//	there is always at least one value
			if (i < m_FromThresholds.size()-1) {
				thresholdStr += ToString(stringC, x, 4);
				thresholdStr += L"; ";
			}	
			i = +1;
		}
		thresholdStr += ToString(stringC, m_OptThreshold, 4);
		EF_Threshold.SetWindowTextW(thresholdStr.c_str());
		break;
	}
	}
}


//	read the user interface
void MfcClusterDlg::ReadThresholds()
{
	wchar_t buffer[264];
	EF_Threshold.GetWindowTextW(buffer, 256);
	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	switch (algo) {
		default: {
			double x = std::stod(buffer);
			x = std::max(0.0, std::min(x, 1.0));
			m_Thresholds.back() = x; //	we use only the last value for all algorithm, except for MLC
			break;
		}
		case MLC: { //	Multi-level clustering
			m_Thresholds.clear();	
			//	split it
			vector<std::wstring> values = split(buffer, L';');
			for (auto str : values) {
				double x = std::stod(str);
				x = std::max(0.0, std::min(x, 1.0));
				m_Thresholds.push_back(x);
			}
			RemoveDuplicatesAndSort(m_Thresholds);
			if (m_Thresholds.size() == 0) {
				St_Status.SetWindowTextW(L"The threshold must be in the range [0.0, 1.0]. For Multi-level clustering, use a ';' separated list such as \"0.4;0.8;0.98\"");
				m_Thresholds.push_back(0.98);
			}
			break;
		}
		case MLC_ST: { //	Multi-level clustering with single thread
			m_Thresholds.clear();
			//	split it
			vector<std::wstring> values = split(buffer, L';');
			for (auto str : values) {
				double x = std::stod(str);
				x = std::max(0.0, std::min(x, 1.0));
				m_Thresholds.push_back(x);
			}
			RemoveDuplicatesAndSort(m_Thresholds);
			if (m_Thresholds.size() == 0) {
				St_Status.SetWindowTextW(L"The threshold must be in the range [0.0, 1.0]. For Multi-level clustering, use a ';' separated list such as \"0.4;0.8;0.98\"");
				m_Thresholds.push_back(0.98);
			}
			break;
		}
	}
}

//	read the user interface
bool MfcClusterDlg::ReadThresholdsForOPTPrediction() 
{
	wchar_t buffer[264];
	EF_Steps.GetWindowTextW(buffer, 256);
	m_Step = std::stod(buffer);
	EF_ToThreshold.GetWindowTextW(buffer, 256);
	double x = std::stod(buffer);
	x = std::max(0.0, std::min(x, 1.0));
	if (x == 0) {
		m_ToThreshold = 1;
	}
	else{
		m_ToThreshold = x;
	}
	EF_FromThreshold.GetWindowTextW(buffer, 256);
	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	switch (algo) {
	default: {
		x = std::stod(buffer);
		x = std::max(0.0, std::min(x, 1.0));
		m_FromThresholds.push_back(x); //	we use only the last value for all algorithm, except for MLC
		m_FromThreshold = x;
		break;
	}
	case MLC: { //	Multi-level clustering
		m_FromThresholds.clear();
		//	split it
		vector<std::wstring> values = split(buffer, L';');
		uint32_t i = 0;
		for (auto str : values) {
			double x = std::stod(str);
			x = std::max(0.0, std::min(x, 1.0));
			m_FromThresholds.push_back(x);
			if (i == values.size()-1) {
				m_FromThreshold = x;
			}
			i = +1;
		}
		Sort(m_FromThresholds);
		if (m_FromThresholds.size() == 0) {
			St_Status.SetWindowTextW(L"The threshold must be in the range [0.0, 1.0]. For Multi-level clustering, use a ';' separated list such as \"0.4;0.8;0.98\"");
			m_FromThresholds.push_back(m_ToThreshold);
			m_FromThreshold = m_ToThreshold;
		}
		break;
	}
	}
	if (m_FromThreshold > m_ToThreshold) {
		MessageBox(L"The from threshold must be less or equal than the to threshold", L"Warning", MB_ICONWARNING | MB_OK);
		return false;
	}
	return true;
}

void
MfcClusterDlg::SaveInGrid()
{
	St_Status.SetWindowTextW(L"Completing the result grid");

	CCustomTreeChildCtrl & ctrl = TR_Result.GetTreeCtrl();
	ctrl.DeleteAllItems();

	HTREEITEM hRoot = nullptr; // parent item
	
	for (const TCluster & group : m_Cluster->Groups()) {
		DrawGroup(hRoot, group);
	}
	St_Status.SetWindowTextW(L"Ready");
}


//	display 4 columns: the group #, Record ID, Sequence name, info
void
MfcClusterDlg::DrawGroup(HTREEITEM & p_ParentNode, const TCluster & p_Cluster)
{
	wchar_t stringC[264];
	int col = 0;

	CCustomTreeChildCtrl & ctrl = TR_Result.GetTreeCtrl();
	HTREEITEM hItem;

	if (p_Cluster.CentralSeqIdx() == UINT32_MAX) {
		hItem = ctrl.InsertItem(L"-", p_ParentNode);																	//	No record ID
		TR_Result.SetItemText(hItem, ++col, L"No centroid computed");
	}
	else {
		TNFieldBase * srce = p_Cluster.ClusterDatabase()->m_Sequences[p_Cluster.CentralSeqIdx()];
		hItem = ctrl.InsertItem(std::to_wstring(srce->RecordId()).c_str(), p_ParentNode);				//	Record ID
		const std::string recordname = clustering::StrainName(srce->RecordName(), m_FieldNamePos);
		TR_Result.SetItemText(hItem, ++col, std::utf8_to_wstring(recordname).c_str());
		//TR_Result.SetItemText(hItem, ++col, std::utf8_to_wstring(srce->RecordName()).c_str());			//	Record name
	}
	std::wstringstream stream;
	if (p_Cluster.Groups().size() > 0) {																				//	additional info
		stream << p_Cluster.Groups().size() << (p_Cluster.Groups().size() > 1 ? L" child groups" : L" child group");
		TR_Result.SetItemText(hItem, ++col, stream.str().c_str());

		if(! isnan(p_Cluster.DistFromParent())) {
			TR_Result.SetItemText(hItem, ++col, ToString(stringC, p_Cluster.DistFromParent(), 3));
		}

		for (const TCluster & group : p_Cluster.Groups()) {
			DrawGroup(hItem, group);
		}
	}
	else if (p_Cluster.Comparisons().size() > 0) {	//	draw the comparisons
		stream << p_Cluster.Comparisons().size() << (p_Cluster.Comparisons().size() > 1 ? L" sequences" : L" sequence" );
		TR_Result.SetItemText(hItem, ++col, stream.str().c_str());

		if (!isnan(p_Cluster.DistFromParent())) {
			TR_Result.SetItemText(hItem, ++col, ToString(stringC, p_Cluster.DistFromParent(), 3));
		}

		for (const TComparison & comp : p_Cluster.Comparisons()) {
			DrawComparison(hItem, p_Cluster, comp);
		}
		m_GroupSeqNumbers.push_back((int32_t)p_Cluster.IdList().size());
	}
	else {	//	a group without any sequence in it except the centroid
		//TR_Result.SetItemText(hItem, ++col, L"Single sequence");
		++col;
		if (!isnan(p_Cluster.DistFromParent())) {
			TR_Result.SetItemText(hItem, ++col, ToString(stringC, p_Cluster.DistFromParent(), 3));
		}
		m_GroupSeqNumbers.push_back((int32_t)p_Cluster.IdList().size());
	}
}


//	display 3 columns: Record ID, Sequence name, additional information
void
MfcClusterDlg::DrawComparison(HTREEITEM & p_ParentNode, const TCluster & p_Cluster, const TComparison & p_Comp)
{
	wchar_t stringC[264];
	int col = 0;

	CCustomTreeChildCtrl & ctrl = TR_Result.GetTreeCtrl();

	uint32_t s = p_Comp.SrceIdx();
	TNFieldBase * srce = p_Cluster.ClusterDatabase()->m_Sequences[s];
	HTREEITEM hItem = ctrl.InsertItem(std::to_wstring(srce->RecordId()).c_str(), p_ParentNode);	//	Record ID
	const std::string recordname = clustering::StrainName(srce->RecordName(), m_FieldNamePos);
	TR_Result.SetItemText(hItem, ++col, std::utf8_to_wstring(recordname).c_str());
	//TR_Result.SetItemText(hItem,  ++col, utf8_to_wstring(srce->RecordName()).c_str());				//	Record name

	++col;	//	nothing in the Info column

	double dist = 1.0 - p_Comp.Sim();
	if (!isnan(dist)) {
		TR_Result.SetItemText(hItem, ++col, ToString(stringC, dist, 3));
	}
}


void MfcClusterDlg::OnCbnSelchangeAlgorithm()
{
	//CB_ComputeFMeasure.EnableWindow(true);

	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	switch (algo) {
		default:
		case SLC: {	//	Single level clustering
		case SLC_ST:
			break;
		}
		case MLC: { //	Multi-level clustering
		case MLC_ST:
			//CB_ComputeFMeasure.SetCheck(false);
			//CB_ComputeFMeasure.EnableWindow(false);
			break;
		}
		case CCBC: { //	CCBC
		case CCBC_ST:
			break;
		}
		case GC: { //	greedy clustering
		case GC_ST:
			break;
		}
	}
	DrawThresholds();
}





void MfcClusterDlg::OnBnClickedFinallevel()
{
	wchar_t stringC[1032];

	CreateReferenceCluster();

	// make a list of final vectors
	vector<const TCluster *> clusters;

	//TCluster * m_FinalCluster;
	//m_Cluster->FinalClusters(clusters); //compute groups at the final level
	m_Cluster->Flatten();

	//	display that cluster in our grid	
	SaveInGrid();

	//	update the group number
	size_t len = m_Cluster->FinalGroupNo();
	EF_GroupNo.SetWindowTextW(ToString(stringC, len));

	//	compute and update the F-Measure
	bool computeQuality = (CB_ComputeFMeasure.GetCheck() == BST_CHECKED);

	if (computeQuality) {
		double f = m_Cluster->F_Measure(*m_RefCluster);
		EF_FMeasure.SetWindowTextW(ToString(stringC, f, 3));
	}
	else {
		EF_FMeasure.SetWindowTextW(L"");
	}
	//	enable the export button
	PB_Export.EnableWindow(true);

}


void MfcClusterDlg::OnBnClickedComputefmeasure()
{
	CreateReferenceCluster();
	//	compute and update the F-Measure
	wchar_t stringC[1032];
	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	if ((algo == MLC || algo==MLC_ST) && m_Thresholds.size() != 1) {
		m_Cluster->Flatten();
	}
	//	just to have a nicer display
	m_Cluster->SortAllGroups();
	double f = m_Cluster->F_Measure_Mt(*m_RefCluster);
	EF_FMeasure.SetWindowTextW(ToString(stringC, f, 3));
}

double MfcClusterDlg::Cluster(const vector<double> & thresholds)
{
	wchar_t stringC[1032];

	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	int32_t fieldNamePos = CO_InputField.GetCurSel();

	int stepNo = 3;
	double dx = 1.0 / stepNo, x = dx;

	CString inFilePath;
	EB_InputFilePath.GetWindowTextW(inFilePath);

	//	check
	// Retrieve the file times for the file.
	uint64_t fileWriteTime = GetFileWriteTime(static_cast<const wchar_t *>(inFilePath));

	if (m_ClusterDatabase.m_Sequences.size() == 0 || m_InputFilename != inFilePath.GetBuffer() || m_InputFileWriteTime != fileWriteTime || fieldNamePos != m_FieldNamePos) {	//	reload
		m_InputFilename = inFilePath.GetBuffer();
		m_FieldNamePos = fieldNamePos;
		m_InputFileWriteTime = fileWriteTime;
		if (m_InputFilename.size() == 0) {
			MessageBox(L"Please select a source fasta file", L"Warning", MB_ICONWARNING | MB_OK);
			Wait(L"", 0.0);
			return 0.0;
		}
		Wait(L"Loading source file ...", x);		x += dx;
		LoadSourceFile();
		CreateReferenceCluster();
	}
	/*wchar_t buffer[264];
	EF_MinimumOverlap.GetWindowTextW(buffer, 256);
	double i = std::stod(buffer);
	m_ClusterDatabase.m_MinOverlap = i;*/
	//	Computations
	delete m_Cluster;
	m_Cluster = new TCluster(&m_ClusterDatabase);

	Wait(L"Clustering ...", x);	x += dx;

	switch (algo) {
		//	*****	Multi-threaded version	*****
	default:
	case SLC: { //	Single level clustering
		m_Cluster->Mlc_Mt(thresholds.back());
		break;
	}
	case MLC: { //	Multi-level clustering
		m_Cluster->MlcAll_Mt(thresholds, 0);
		//take the last level
		m_Cluster->Flatten();
		break;
	}
	case CCBC: { //	CCBC
		m_Cluster->Ccbc_Mt(thresholds.back());
		break;
	}
	case GC: { //	Greedy Clust
		m_Cluster->GClust_Mt(thresholds.back());
		break;
	}
					 //	*****	Single-threaded version	*****
	case SLC_ST: { //	single-level clustering
		m_Cluster->Mlc_St(thresholds.back());
		break;
	}
	case MLC_ST: { //	Multi-level clustering
		m_Cluster->MlcAll_St(thresholds, 0);
		//take the last level
		m_Cluster->Flatten();
		break;
	}
	case CCBC_ST: { //	CCBC
		m_Cluster->Ccbc_St(thresholds.back());
		break;
	}
	case GC_ST: { //	Greedy clustering
		m_Cluster->GClust_St(thresholds.back());
		break;
	}
	}
	//	just to have a nicer display
	m_Cluster->SortAllGroups();

	//	compute and update the F-Measure
	Wait(L"Computing quality ...", x);		x += dx;
	double f = m_Cluster->F_Measure_Mt(*m_RefCluster);
	EF_FMeasure.SetWindowTextW(ToString(stringC, f, 3));

	std::wostringstream stream;
	stream << std::endl << L"Threshold = " << thresholds.back() << L", F-Measure = " << f;
	stream << L", Cluster contains " << m_Cluster->GroupNo() << L" groups";
	if (m_RefCluster) {
		stream << " and " << m_RefCluster->ClassifiedSequenceNo() << L" classified sequences";
	}
	OutputDebugString(stream.str().c_str());

	Wait(L"Ready", 1.0);
	return f;
}


void MfcClusterDlg::OnBnClickedOptimize()
{
	CreateReferenceCluster();
	AlgorithmEnum algo = static_cast<AlgorithmEnum>(CO_Algorithm.GetCurSel());
	
	//vector<double> parameters;
	//parameters.push_back(0.9);// m_Thresholds.back());
	//vector<double> steps;
	//steps.push_back(0.02);
	//TOptimizer opt(parameters, steps, 0.00001);
	//double ptr[100];
	//std::memmove(ptr, &parameters[0], std::min(100ULL, parameters.size()) * sizeof(double));

	//save threshold and F-measure in a file
	CString outFilePath;
	EB_OutputFilePath.GetWindowTextW(outFilePath);
	m_OutputClusterFilePath = outFilePath.GetBuffer();
	const int result = MessageBox( L"Do you want to save the prediction in the result file?", L"Warning", MB_YESNOCANCEL);
	std::ofstream file(m_OutputClusterFilePath.c_str(), std::ofstream::binary);
	switch (result)
	{
	case IDYES:		
		if  (file.fail()) {
			MessageBox(L"Please enter the path for the output file.", L"Warning", MB_ICONWARNING | MB_OK);
			return; //	error
		}
		break;
	case IDNO:
		break;
	case IDCANCEL:
		// Do something
		return;
	}
	/*double err = 0.0;
	do {
	err = 1.0 - Cluster(parameters);
	} while (opt.Next(err));*/
	//	read inputs
	if (ReadThresholdsForOPTPrediction()==false) {
		return;
	}
	double bestf = 0.0;
	double threshold = m_FromThreshold;
	do {
		vector<double> thresholds;
		if (algo == MLC && m_FromThresholds.size() != 1) {
			for (uint32_t i = 0; i < m_FromThresholds.size() - 1; ++i) {
				thresholds.push_back(m_FromThresholds[i]);
			}
		}
		thresholds.push_back(threshold);
		double f = Cluster(thresholds);
		if (f > bestf) {
			bestf = f;
			m_OptThreshold = threshold;
		}
		//save in the result file
		if (result==IDYES) {
			file << threshold << "\t" <<  f << endl;
			bool isError = (file.goodbit != 0);
		}
		threshold = threshold + m_Step;
	} while (threshold <= m_ToThreshold);
	//close file
	
	file.close();

	//	display that cluster in our grid
	SaveInGrid();

	//	update the group number
	wchar_t stringC[264];
	size_t len = m_Cluster->FinalGroupNo();
	DrawOptThresholds();
	EF_FMeasure.SetWindowTextW(ToString(stringC, bestf, 3));
}





void MfcClusterDlg::OnBnClickedOption()
{
	//OptionDlg optdlg();
	int32_t m_MinOverlap = m_ClusterDatabase.m_MinOverlap;
	int32_t m_MinSeqNoForMLC = m_ClusterDatabase.m_MinSeqNoForMLC;
	OptionDlg optdlg( m_MinOverlap, m_MinSeqNoForMLC, m_MinSimForVisualization, m_KneighborNo, m_VisDimension);
	optdlg.DoModal();
	m_ClusterDatabase.m_MinOverlap = optdlg.m_MinOverlap;
	m_MinSimForVisualization = optdlg.m_MinSimForVisualization;
	m_KneighborNo = optdlg.m_KneighborNo;
	m_VisDimension = optdlg.m_VisDimension;
	m_ClusterDatabase.m_MinSeqNoForMLC = optdlg.m_MinSeqNoForMLC;
}


void MfcClusterDlg::OnBnClickedVisualize()
{
	// TODO: Add your control notification handler code here
	
	Visualize();
	
}

void startLargeVisWaitForFinish(CString parameters)
{	
	CString command = L"LargeVis\\LargeVis.exe " + parameters;
	if (system(NULL)) {
		system(CStringA(command));
	}
}

std::map<std::string, Point> pointMap;
std::map<std::string, std::vector<std::string>> propertiesMap;

void makePoints(CString filename)
{
	//std::string id;
	//double x, y, z;
	//std::ifstream infile(filename);
	//std::string line;
	//std::getline(infile, line);
	//while (std::getline(infile, line))
	//{
	//	std::istringstream iss(line);

	//	if (!(iss >> id >> x >> y >> z)) { break; } // error

	//	Point newpoint = Point(x, y, z);
	//	pointMap[id] = newpoint;
	//}
	std::string id;
	double x, y, z;
	double dim = 0;
	bool firstline = true;
	std::ifstream infile(filename);
	std::string line;
	std::getline(infile, line);
	std::istringstream iss(line);
	if (iss >> x >> y) {
		dim = y;
	}
	else return;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		if (dim == 2)
		{
			if (iss >> id >> x >> y)
			{
				Point newpoint = Point(0.1, x, y);
				pointMap[id] = newpoint;
			}
		}
		else if (dim == 3)
		{
			if (iss >> id >> x >> y >> z)
			{
				Point newpoint = Point(x, y, z);
				pointMap[id] = newpoint;
			}
		}
		else break;
	}
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

void combinePointsWithProperties(CString inputtitlefilename, CString inputfilename, CString outputfilename, CString coordArchivePath)
{
	std::ifstream titlefile(inputtitlefilename,std::ofstream::binary);
	//std::ifstream titlefile(m_TitleFilename, std::ofstream::binary);
	std::ifstream infile(inputfilename);
	std::ofstream outfile(outputfilename);
	std::ofstream outfileArc(coordArchivePath);

	std::string line;
	//std::getline(infile, line);

	std::map<std::string, Point>::iterator it;

	int fileindex = 0;

	outfile << "const data_all = {";
	outfileArc << "{";
	bool firstlinecreated = false;
	/////read title file
	if (titlefile.bad()) {
	}
	else{
		std::getline(titlefile, line);
		if (line != ""){
		CString outline;
		/*CString firstline = L"\"";*/
		CString firstline = L"\"NamesOfProperties\":[";
		std::vector<std::string> properties = split(line, '|');
		int propertyindex = 0;
		for (std::vector<std::string>::iterator it = properties.begin(); it != properties.end(); ++it) {
			std::string apostrofstr = "\"";
			CString apostrof(apostrofstr.c_str());
			CString activeproperty((*it).c_str());
			propertyindex++;
			if (propertyindex != 1)
			{
				outline += ",";				
				firstline += ",";
			}
			else
			{

			}
			outline += apostrof + activeproperty + apostrof;
			firstline += apostrof + activeproperty + apostrof;
		}
		outline += "]}";
		firstline += "], ";
		CT2CA pszConvertedAnsiString1(firstline);
		std::string firstlinestring(pszConvertedAnsiString1);
		outfile << firstlinestring;
		outfileArc << firstlinestring;
		firstlinecreated = true;
		}
		titlefile.close();
	}
	////
	while (std::getline(infile, line))
	{
		fileindex++;
		std::vector<std::string> properties = split(line, '|');
		/*std::string id(properties.at(0).c_str());
		CString idstring(properties.at(0).c_str());*/
		std::string id(properties.at(0).erase(0, 1).c_str());
		CString idstring(properties.at(0).c_str());

		CString outline;
		CString firstline = L"\"";
		if (fileindex == 1) outline = L"\"";
		else outline = L",\"";

		it = pointMap.find(id);

		if (it != pointMap.end())
		{
			Point pointmy = pointMap[id];
			std::ostringstream strs1;
			strs1 << pointmy.x();
			std::string str1 = strs1.str();
			CString xcord(str1.c_str());

			std::ostringstream strs2;
			strs2 << pointmy.y();
			std::string str2 = strs2.str();
			CString ycord(str2.c_str());

			std::ostringstream strs3;
			strs3 << pointmy.z();
			std::string str3 = strs3.str();
			CString zcord(str3.c_str());

			outline = outline + idstring + L"\":{\"Coordinates\":[" + xcord + L", " + ycord + L", " + zcord + L"],\"Properties\" : [";
			if (!firstlinecreated)
				//firstline = firstline + "NamesOfProperties" + L"\":{\"Coordinates\":[" + xcord + L", " + ycord + L", " + zcord + L"],\"Properties\" : [";
			{
				firstline = firstline + L"NamesOfProperties\":[";// +xcord + L", " + ycord + L", " + zcord + L"],\"Properties\" : [";
			}
		}
		
		int propertyindex = 0;
		for (std::vector<std::string>::iterator it = properties.begin(); it != properties.end(); ++it) {
			std::string apostrofstr = "\"";
			CString apostrof(apostrofstr.c_str());
			CString activeproperty((*it).c_str());
			propertyindex++;
			if (propertyindex != 1)
			{
				outline += ",";
				if (!firstlinecreated)
					firstline += ",";
			}
			outline += apostrof + activeproperty + apostrof;
			if (!firstlinecreated)
				firstline += apostrof + activeproperty + apostrof;
		}
		outline += "]}";
		if (!firstlinecreated)
			firstline += "], ";
		std::getline(infile, line);
		if (!firstlinecreated) {
			CT2CA pszConvertedAnsiString1(firstline);
			std::string firstlinestring(pszConvertedAnsiString1);
			outfile << firstlinestring;
			outfileArc << firstlinestring;
			firstlinecreated = true;
		}
		CT2CA pszConvertedAnsiString(outline);
		std::string outlinestring(pszConvertedAnsiString);
		outfile << outlinestring;
		outfileArc << outlinestring;
		outfile.flush();
		outfileArc.flush();
	}

	outfile << "}";
	outfileArc << "}";
	outfile.flush();
	outfile.close();
}

void MfcClusterDlg::Visualize()
{
	
	CString mfcOutFilePath = CString(m_InputFilePath.c_str()) + CString(m_Filename.c_str()) + L".sim";

	CString largeVisOutFilePath = CString(m_InputFilePath.c_str()) + CString(m_Filename.c_str()) + L".outLargeVis";
	//CString coordArchivePath = CString(m_InputFilePath.c_str()) + CString(m_Filename.c_str())  + L"_coord.json";
	
//	char buf[1024] = { 0 };
//	DWORD ret = GetModuleFileNameA(NULL, buf, sizeof(buf));
	
	TCHAR NPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, NPath);
	CString path(NPath);

	CString coordArchivePath = (CString)path + L"\\DiVE\\data\\" + CString(m_Filename.c_str()) + L"_coord.json";

	//CString largeVIsInputParameters = L" -fea 0 -input " + mfcOutFilePath + L" -output " + largeVisOutFilePath + L" -outdim 3 -threads 4 -log 0";
	std::string  s = to_string(m_KneighborNo);
	CString neigs(s.c_str());
	int32_t EdgeNo = (int32_t)(m_ClusterDatabase.m_Sequences.size() / 100);
	if (EdgeNo < 20) {
		EdgeNo = 20;
	}
	s = to_string(EdgeNo);
	CString edges(s.c_str());
	//CString largeVIsInputParameters = L" -fea 0 -input " + mfcOutFilePath + L" -output " + largeVisOutFilePath + L" -outdim 3 -threads 4 -log 1 -samples " + cs; 
	CString largeVIsInputParameters = L" -fea 0 -input " + mfcOutFilePath + L" -output " + largeVisOutFilePath + L" -outdim 3 -threads 4 -log 1 -samples " + edges + L" -neigh " + neigs;
	if (m_VisDimension == 2) {
		largeVIsInputParameters = L" -fea 0 -input " + mfcOutFilePath + L" -output " + largeVisOutFilePath + L" -outdim 2 -threads 4 -log 1 -samples 2 " + edges + L" -neigh " + neigs;
	}
	CString finalFilePath = path +  L"\\DiVE\\data\\data.js";

	if (m_InputFilename.size() == 0) {
		MessageBox(L"Please enter the input file to be cluster and cluster the file first.", L"Warning", MB_ICONWARNING | MB_OK);
		Wait(L"", 0.0);
		return;
	}
	else {
		std::wstring ws = mfcOutFilePath.GetBuffer(mfcOutFilePath.GetLength());
		std::string s= std::string(ws.begin(), ws.end());
		if (is_file_exist(s.c_str())==false)  {
			MessageBox(L"Please save a similarity matrix first.", L"Warning", MB_ICONWARNING | MB_OK);
			Wait(L"", 0.0);
			return;
		}
	}
	
	//std::wstring error(L"Starting generating coordinates. Please wait...");
	//check if we need to generate the coordinates
	fstream file;
	bool computecoordinates = false;
	file.open(largeVisOutFilePath, ios_base::out | ios_base::in);  // will not create file
	if (file.is_open())
	{
		if (MessageBox(L"The coordinates for visualization have been generated. Do you wish to recompute them?", L"Warning", MB_YESNOCANCEL) == IDYES) {
			computecoordinates = true;
		}
		file.close();
	}
	else {
		computecoordinates = true;
	}
	if (computecoordinates) {		
		//write to log file the starting time 
		time_t now = time(0);
		// Convert now to tm struct for local timezon
		struct tm timeinfo;
		localtime_s(&timeinfo, &now);
		char str[26];
		asctime_s(str, sizeof str, &timeinfo);
		std::ofstream file(m_logfilename.c_str(), std::ios_base::out | std::ios_base::app);
		std::string filename(m_InputFilename.begin(), m_InputFilename.end());
		file << "Start calculating coordinates for " << filename << ": " << str;

		//generate coordinates
		Wait(L"Computing sequence coordinates ...", 0.6);
		startLargeVisWaitForFinish(largeVIsInputParameters);
		Wait(L"Ready", 0.0);

		//write to log file the ending time 
		now = time(0);
		localtime_s(&timeinfo, &now);
		asctime_s(str, sizeof str, &timeinfo);
		file << "End: " << str << "\n";
		file.close();
	}
	makePoints(largeVisOutFilePath);
	combinePointsWithProperties(CString(m_TitleFilename.c_str()), CString(m_InputFilename.c_str()), finalFilePath, coordArchivePath);

	//std::wstring error(L"Finished generating 3d coordinates. Opening the browser.");
	//MessageBoxW(NULL, static_cast<LPCWSTR>(error.c_str()));

	//ShellExecute(0, 0, L"..\\..\\..\\DiVE\\index.html", 0, 0, SW_SHOW);
	//ShellExecute(0, 0, L"DiVE\\index.html", 0, 0, SW_SHOW);
	CString command = L"DiVE\\index.html";
	if (system(NULL)) {
		system(CStringA(command));
	}
	
}



void MfcClusterDlg::OnBnClickedSavesimilarity()
{
	if (m_ClusterDatabase.m_Sequences.size() == 0) {
		return;
	}
	CString fullfilename = CString(m_InputFilePath.c_str()) + CString(m_Filename.c_str()) + L".sim";
	//	Save similarity in a file
	std::ofstream file(static_cast<const wchar_t *>(fullfilename), std::ofstream::binary);
	
	Wait(L"Saving similarity ...", 0.3);

	/*if (m_Thresholds.size() > 1) {
		lowerboundthreshold = m_Thresholds[m_Thresholds.size()-2];
	}*/
	int32_t maxseqnotoberecompared = (int32_t)(sqrt(m_KneighborNo * m_ClusterDatabase.m_Sequences.size()));
	m_Cluster->SaveSimilarity(static_cast<const wchar_t *>(fullfilename), m_KneighborNo, m_MinSimForVisualization,m_Thresholds.back(), maxseqnotoberecompared);
	Wait(L"Ready", 0.0);
	
}


void MfcClusterDlg::OnBnClickedSavefullsimilarity()
{
	CString inFilePath;
	EB_InputFilePath.GetWindowTextW(inFilePath);
	uint64_t fileWriteTime = GetFileWriteTime(static_cast<const wchar_t *>(inFilePath));
	//get path for saving similarity 
	if(m_ClusterDatabase.m_Sequences.size() == 0 || m_InputFilename != inFilePath.GetBuffer() || m_InputFileWriteTime != fileWriteTime) {
		int stepNo = 3;
		double dx = 1.0 / stepNo, x = dx;
		m_InputFilename = inFilePath.GetBuffer();
		m_InputFileWriteTime = fileWriteTime;
		if (m_InputFilename.size() == 0) {
			MessageBox(L"Please select a source fasta file", L"Warning", MB_ICONWARNING | MB_OK);
			Wait(L"", 0.0);
			return;
		}
		Wait(L"Loading source file ...", x);		x += dx;
		LoadSourceFile();
		delete m_Cluster;
		m_Cluster = new TCluster(&m_ClusterDatabase);
		//return;
	}
	CString fullfilename = CString(m_InputFilePath.c_str()) + CString(m_Filename.c_str()) + L".sim";
	//	Save similarity in a file
//	std::ofstream file(static_cast<const wchar_t *>(fullfilename), std::ofstream::binary);

	Wait(L"Saving similarity ...", 0.3);
	m_Cluster->SaveFullSimilarity(static_cast<const wchar_t *>(fullfilename),m_MinSimForVisualization);

	Wait(L"Ready", 0.0);
}


void MfcClusterDlg::OnBnClickedSave()
{
	//	save clusters in Fasta file
	if (m_recordnameextended == true){
		if (MessageBox(L"You have saved the clustering result in the input file. Do you wish to continue?", L"Warning", MB_YESNOCANCEL) != IDYES) {
			Wait(L"", 0.0);
			return;
		}
	}
	uint32_t fieldNamePos = CO_InputField.GetCurSel();
	m_Cluster->ExtendRecordNames(0, (uint32_t)m_Thresholds.size() ,"", fieldNamePos);
	m_recordnameextended = true;
	//	Save results into the fasta file
	std::ofstream file(m_InputFilename.c_str(), std::ofstream::binary);
	Wait(L"Saving clusters in Fasta file...", 0.3);
	if (file.fail()) {
		Wait(L"Ready", 0.0);
		return;  //	error
	}
	for (TNFieldBase * seq : m_Cluster->Sequences()) {
		file << ">" << seq->RecordName() << "\r\n";
		file << seq->Sequence() << "\r\n";
	}
	Wait(L"Ready", 0.0);
	bool isError = (file.goodbit != 0);
	file.close();
	//	Save results into the title file
	fstream titlefile;
	titlefile.open(m_TitleFilename.c_str(), ios_base::out | ios_base::in);  
	if (titlefile.is_open())
	{
		std::string line;
		line.reserve(2000);
		std::getline(titlefile, line);
		titlefile.close();
		std::ofstream titlefile(m_TitleFilename.c_str(), std::ofstream::binary);
		titlefile << line + "|Suggested name";
		for (uint32_t i = 0; i < m_Thresholds.size(); ++i) {
			titlefile << "|Level" << to_string(i + 1) ;
		}
		titlefile.close();
	}
}

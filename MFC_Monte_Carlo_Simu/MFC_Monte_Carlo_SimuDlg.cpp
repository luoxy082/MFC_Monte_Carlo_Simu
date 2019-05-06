
// MFC_Monte_Carlo_SimuDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFC_Monte_Carlo_Simu.h"
#include "MFC_Monte_Carlo_SimuDlg.h"
#include "afxdialogex.h"
#include <omp.h>
#include <stdio.h>

extern "C" {
	#include "spa/spa.h"
}
extern "C" int spa(int max_iterations, double EbN0_dB, int *iterations);
extern "C" int H[row][column];
extern "C" double y[column];

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

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


// CMFCMonteCarloSimuDlg dialog



CMFCMonteCarloSimuDlg::CMFCMonteCarloSimuDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_MONTE_CARLO_SIMU_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCMonteCarloSimuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCMonteCarloSimuDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BUTTON_START_SPA, &CMFCMonteCarloSimuDlg::OnBnClickedButtonStartSpa)
	ON_BN_CLICKED(IDC_BUTTON_START_MONTE, &CMFCMonteCarloSimuDlg::OnBnClickedButtonStartMonte)
	ON_BN_CLICKED(IDC_BUTTON_H_matrix_choose, &CMFCMonteCarloSimuDlg::OnBnClickedButtonHmatrixchoose)
	ON_BN_CLICKED(IDC_BUTTON_y_matrix_choose, &CMFCMonteCarloSimuDlg::OnBnClickedButtonymatrixchoose)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_log, &CMFCMonteCarloSimuDlg::OnBnClickedButtonlog)
END_MESSAGE_MAP()


// CMFCMonteCarloSimuDlg message handlers

BOOL CMFCMonteCarloSimuDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
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

	//ShowWindow(SW_MAXIMIZE);

	ShowWindow(SW_MINIMIZE);

	// TODO: Add extra initialization here
	GetDlgItem(IDC_EDIT_N0)->SetWindowText(_T("1000"));
	GetDlgItem(IDC_EDIT_Nmax)->SetWindowText(_T("1e5"));
	GetDlgItem(IDC_EDIT_delta)->SetWindowText(_T("1e-7"));
	GetDlgItem(IDC_EDIT_r)->SetWindowText(_T("1e-1"));
	GetDlgItem(IDC_EDIT_alp)->SetWindowText(_T("0.10"));
	
	GetDlgItem(IDC_EDIT_max_Iterations)->SetWindowText(_T("20"));
	GetDlgItem(IDC_EDIT_EbN0_dB)->SetWindowText(_T("4.7"));

	pProg = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
	pProg->SetScrollRange(0, 1, 100);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCMonteCarloSimuDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCMonteCarloSimuDlg::OnPaint()
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
HCURSOR CMFCMonteCarloSimuDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMFCMonteCarloSimuDlg::OnBnClickedButtonStartSpa()
{
	// TODO: Add your control notification handler code here
	int max_iterations = 0;
	clock_t time_start, time_finish;
	CString str;
	int iterations;

	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	GetDlgItem(ID_BUTTON_START_SPA)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_EbN0_dB)->GetWindowText(str);
	double EbN0_dB = _wtof(str);
	GetDlgItem(IDC_EDIT_max_Iterations)->GetWindowText(str);
	max_iterations = _wtoi(str);

	time_start = clock();

	spa(max_iterations, EbN0_dB, &iterations);

	time_finish = clock();

	str.Format(_T("%d"), iterations);
	GetDlgItem(IDC_EDIT_ITERATION)->SetWindowText(str);

	str.Format(_T("%lf"), (double)(time_finish - time_start) / CLOCKS_PER_SEC);
	GetDlgItem(IDC_EDIT_TIME_COST)->SetWindowText(str);

	GetDlgItem(ID_BUTTON_START_SPA)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);

	return;
}

UINT proess_monte(LPVOID param);

void CMFCMonteCarloSimuDlg::OnBnClickedButtonStartMonte()
{
	// TODO: Add your control notification handler code here
	m_TestRes.hWnd = GetSafeHwnd();
	m_TestRes.pParam = this;

	m_pTestThread = AfxBeginThread(proess_monte, &m_TestRes, 0, 0, 0, NULL);

	GetDlgItem(IDC_BUTTON_START_MONTE)->EnableWindow(FALSE);

	return;
}


void CMFCMonteCarloSimuDlg::OnBnClickedButtonHmatrixchoose()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(TRUE, _T(".f"), NULL, 0, _T("(*.f)|*.f|All Files(*.*)|*.*||"));

	if (IDOK == fileDlg.DoModal())
	{
		CString des;
		des = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_H_matrix)->SetWindowText(des);

		FILE *pf_H = NULL;
		errno_t ret;
		char file_path[100];
		WideCharToMultiByte(CP_ACP, 0, des, des.GetLength() + 1, file_path, 100, NULL, NULL);
		ret = fopen_s(&pf_H, file_path, "r");
		if (ret != 0)
		{
			//Trace(_T("H_out.f open fail"));
			fclose(pf_H);

			return;
		}

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < column; j++)
			{
				int ch = fgetc(pf_H);
				if ((ch >= '0') && (ch <= '9'))
				{
					H[i][j] = ch - '0';
				}
				else
				{
					j--;
				}
			}
		}

		fclose(pf_H);
	}

	return;
}


void CMFCMonteCarloSimuDlg::OnBnClickedButtonymatrixchoose()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDlg(TRUE, _T(".f"), NULL, 0, _T("(*.f)|*.f|All Files(*.*)|*.*||"));

	if (IDOK == fileDlg.DoModal())
	{
		CString des;
		des = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_y_matrix)->SetWindowText(des);

		FILE *pf_y = NULL;
		errno_t ret;
		int x[column] = { 0 };
		char file_path[100];
		WideCharToMultiByte(CP_ACP, 0, des, des.GetLength() + 1, file_path, 100, NULL, NULL);

		ret = fopen_s(&pf_y, file_path, "r");
		if (ret != 0)
		{
			//printf("y_out.f open fail\n");
			fclose(pf_y);

			return;
		}

		for (int j = 0; j < column; j++)
		{
			x[j] = fgetc(pf_y) - '0';
			y[j] = pow(-1, x[j]);
		}

		fclose(pf_y);
	}

	return;
}


void CMFCMonteCarloSimuDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	int pos;
	CString str;

	switch (nIDEvent)
	{
	case 1:
		pos = this->pProg->GetPos();
		pos++;
		pProg->SetPos(pos % 100);

		
		str.Format(_T("%d"), monte_carol_count);
		GetDlgItem(IDC_EDIT_Cycles)->SetWindowText(str);
		break;
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

UINT proess_monte(PVOID pParam)
{
	CString str;
	int ret;
	double sum = 0;
	double y_mean = 0;
	double s = 0;
	double t_half_alp;
	int iterations;
	double sum_var = 0;
	int i;
	double delta_sample;
	double r_sample;
	int seg_start, seg_end;
	FILE *pf = NULL;
	char ch[100] = {0};

	TestResources *pRes = (TestResources *)pParam;
	HWND hWnd = pRes->hWnd;
	LPVOID m_PID = pRes->pParam;
	CMFCMonteCarloSimuDlg *dlg = (CMFCMonteCarloSimuDlg *)m_PID;

	dlg->GetDlgItem(IDC_EDIT_max_Iterations)->GetWindowText(str);
	dlg->SetTimer(1, 2000, nullptr);
	dlg->monte_carol_count = 0;

	//step 1:
	dlg->GetDlgItem(IDC_EDIT_max_Iterations)->GetWindowText(str);
	int max_iterations = _tstoi(str);

	dlg->GetDlgItem(IDC_EDIT_N0)->GetWindowText(str);
	int N0 = _tstoi(str);

	dlg->GetDlgItem(IDC_EDIT_Nmax)->GetWindowText(str);
	int Nmax = (int)_tstof(str);
	int *y = new int[Nmax];

	dlg->GetDlgItem(IDC_EDIT_delta)->GetWindowText(str);
	double delta = _tstof(str);

	dlg->GetDlgItem(IDC_EDIT_r)->GetWindowText(str);
	double r = _tstof(str);

	dlg->GetDlgItem(IDC_EDIT_alp)->GetWindowText(str);
	double alp = _tstof(str);

	if (0.10 == alp)
	{
		t_half_alp = 1.645;
	}
	else if (0.05 == alp)
	{
		t_half_alp = 1.96;
	}
	else
	{
		t_half_alp = 1.645;
	}

	dlg->GetDlgItem(IDC_EDIT_EbN0_dB)->GetWindowText(str);
	double EbN0_dB = _wtof(str);

	seg_start = 0;
	seg_end = N0;
	do {
		//step 2:
	#pragma omp parallel for
		for (i  = seg_start; i < seg_end; i++)
		{
			ret = spa(max_iterations, EbN0_dB, &iterations);
			if (iterations == max_iterations)
			{
				y[i] = 1;
			}
			else
			{
				y[i] = 0;
			}			
		}	
	#pragma omp parallel for reduction(+:sum)
		for (i = seg_start; i < seg_end; i++)
		{
			sum = sum + y[i];
		}

		//step 3:
		y_mean = sum / seg_end;

		for (i = seg_start; i < seg_end; i++)
		{
			sum_var = sum_var + pow(y[i] - y_mean, 2);
		}
		s = sqrt(sum_var / seg_end);

		delta_sample = s * t_half_alp / sqrt(seg_end - 1);
		r_sample = delta_sample / abs(y_mean);
		dlg->monte_carol_count = seg_end;

		time_t timer;
		timer = time(NULL);
		struct tm tblock;
		localtime_s(&tblock, &timer);
		char strTmp[100];
		asctime_s(strTmp, &tblock);

		sprintf_s(ch, "%f\t %d\t %f\t %f\t %s", 
			EbN0_dB, dlg->monte_carol_count, delta_sample, r_sample, strTmp);
		fopen_s(&pf, dlg->file_path, "a+");
		fputs(ch, pf);

		fclose(pf);

		seg_start = seg_end;
		seg_end = seg_end + 1000;
	} while ((delta_sample > delta) || (r_sample > r));




	



	//step 4:
	for (; dlg->monte_carol_count < Nmax; dlg->monte_carol_count++)
	{
		if (r_sample < r)
		{
			break;
		}
		spa(max_iterations, EbN0_dB, &iterations);
		if (iterations == max_iterations)
		{
			y[dlg->monte_carol_count] = 1;
		}
		else
		{
			y[dlg->monte_carol_count] = 0;
		}

		sum = sum + y[dlg->monte_carol_count];
		y_mean = sum / dlg->monte_carol_count;
		sum_var = sum_var + pow(y[dlg->monte_carol_count] - y_mean, 2);
		s = sqrt(sum_var / dlg->monte_carol_count);

		delta_sample = s * t_half_alp / sqrt(dlg->monte_carol_count - 1);
		str.Format(_T("%lf"), delta_sample);
		dlg->GetDlgItem(IDC_EDIT_delta_sample)->SetWindowText(str);

		r_sample = delta_sample / abs(y_mean);
		str.Format(_T("%lf"), r_sample);
		dlg->GetDlgItem(IDC_EDIT_r_sample)->SetWindowText(str);
	}

	delete[] y;

	dlg->GetDlgItem(IDC_BUTTON_START_MONTE)->EnableWindow(TRUE);
	dlg->KillTimer(1);

	return 0;
}


void CMFCMonteCarloSimuDlg::OnBnClickedButtonlog()
{
	// TODO: Add your control notification handler code here
	char * ch = "EbN0_dB\t Cycle\t delta\t r\t Time\n";
	CFileDialog fileDlg(TRUE, _T(".log"), NULL, 0, _T("(*.log)|*.log|All Files(*.*)|*.*||"));

	if (IDOK == fileDlg.DoModal())
	{
		CString des;
		des = fileDlg.GetPathName();
		GetDlgItem(IDC_EDIT_log)->SetWindowText(des);

		FILE *pf = NULL;
		errno_t ret;
		
		WideCharToMultiByte(CP_ACP, 0, des, des.GetLength() + 1, file_path, 100, NULL, NULL);

		ret = fopen_s(&pf, file_path, "w+");
		if (ret != 0)
		{
			fclose(pf);

			return;
		}

		fputs(ch, pf);

		fclose(pf);
	}

	return;
}

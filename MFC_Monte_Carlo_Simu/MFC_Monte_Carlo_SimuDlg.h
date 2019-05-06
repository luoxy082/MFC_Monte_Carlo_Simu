
// MFC_Monte_Carlo_SimuDlg.h : header file
//

#pragma once

struct TestResources {
	HWND hWnd;
	LPVOID pParam;
};

// CMFCMonteCarloSimuDlg dialog
class CMFCMonteCarloSimuDlg : public CDialogEx
{
// Construction
public:
	CMFCMonteCarloSimuDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_MONTE_CARLO_SIMU_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CProgressCtrl *pProg;
	
	//void proess_monte();
	CWinThread* m_pTestThread;
	TestResources m_TestRes;

	int init_H_matrix(void);
	int init_y_matrix();
	int y_matrix_add_noise(void);
	int y_matrix_create_Lj(void);
	int init_Lj2i_matrix(void);
	int CN_update(void);
	int VN_update(void);
	int LLR_update(void);
	int vj_update(void);
	int cycle_update(void);
	int check_vj(void);
public:
	afx_msg void OnBnClickedButtonStartSpa();
	afx_msg void OnBnClickedButtonStartMonte();
	afx_msg void OnBnClickedButtonHmatrixchoose();
	afx_msg void OnBnClickedButtonymatrixchoose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	int monte_carol_count;
	afx_msg void OnBnClickedButtonlog();
	char file_path[100];
};

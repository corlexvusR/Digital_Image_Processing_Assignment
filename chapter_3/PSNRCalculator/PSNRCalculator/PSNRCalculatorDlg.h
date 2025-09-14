
// PSNRCalculatorDlg.h: 헤더 파일
//

#pragma once


// CPSNRCalculatorDlg 대화 상자
class CPSNRCalculatorDlg : public CDialogEx
{
// 생성입니다.
public:
	CPSNRCalculatorDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PSNRCALCULATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 컨트롤 변수들
	CEdit m_editOriginalPath;
	CEdit m_editDamagedPath;
	CEdit m_editWidth;
	CEdit m_editHeight;
	CEdit m_editBitDepth;
	CEdit m_editResult;
	CButton m_btnOriginal;
	CButton m_btnDamaged;
	CButton m_btnCalculate;

	// 데이터 변수들
	CString m_strOriginalPath;
	CString m_strDamagedPath;
	int m_nWidth;
	int m_nHeight;
	int m_nBitDepth;

	afx_msg void OnBnClickedBtnOriginal();
	afx_msg void OnBnClickedBtnDamaged();
	afx_msg void OnBnClickedBtnCalculate();

	BOOL LoadImage(CString filePath, BYTE** ppImageData, int width, int height);
	double CalculateMSE(BYTE* original, BYTE* damaged, int width, int height);
	double CalculatePSNR(double mse, int maxValue);
	void DisplayResult(double psnr, double mse);
	afx_msg void OnEnChangeEditDamaged();
};

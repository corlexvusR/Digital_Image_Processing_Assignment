#pragma once
#include "afxdialogex.h"


// DCTDlg 대화 상자

class DCTDlg : public CDialogEx
{
	DECLARE_DYNAMIC(DCTDlg)

public:
	DCTDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DCTDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
	int m_DlgBlockSize;
	afx_msg void OnBnClickedOk();
};

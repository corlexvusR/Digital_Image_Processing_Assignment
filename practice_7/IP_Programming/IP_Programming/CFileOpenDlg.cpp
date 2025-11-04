// CFileOpenDlg.cpp: 구현 파일
//

#include "pch.h"
#include "IP_Programming.h"
#include "afxdialogex.h"
#include "CFileOpenDlg.h"


// CFileOpenDlg 대화 상자

IMPLEMENT_DYNAMIC(CFileOpenDlg, CDialogEx)

CFileOpenDlg::CFileOpenDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
	, m_DlgWidth(0)
	, m_DlgHeight(0)
{

}

CFileOpenDlg::~CFileOpenDlg()
{
}

void CFileOpenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WIDTH, m_DlgWidth);
	DDX_Text(pDX, IDC_EDIT_HEIGHT, m_DlgHeight);
}


BEGIN_MESSAGE_MAP(CFileOpenDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileOpenDlg 메시지 처리기

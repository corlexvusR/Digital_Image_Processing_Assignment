// DCTDlg.cpp: 구현 파일
//

#include "pch.h"
#include "IP_Programming.h"
#include "afxdialogex.h"
#include "DCTDlg.h"


// DCTDlg 대화 상자

IMPLEMENT_DYNAMIC(DCTDlg, CDialogEx)

DCTDlg::DCTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG2, pParent)
	, m_DlgBlockSize(0)
{

}

DCTDlg::~DCTDlg()
{
}

void DCTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_DlgBlockSize);
}


BEGIN_MESSAGE_MAP(DCTDlg, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT1, &DCTDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &DCTDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// DCTDlg 메시지 처리기

void DCTDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void DCTDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

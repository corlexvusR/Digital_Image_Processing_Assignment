
// PSNRCalculatorDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "PSNRCalculator.h"
#include "PSNRCalculatorDlg.h"
#include "afxdialogex.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CPSNRCalculatorDlg 대화 상자



CPSNRCalculatorDlg::CPSNRCalculatorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PSNRCALCULATOR_DIALOG, pParent)
	, m_nWidth(512)
	, m_nHeight(512)
	, m_nBitDepth(8)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPSNRCalculatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ORIGINAL, m_editOriginalPath);
	DDX_Control(pDX, IDC_EDIT_DAMAGED, m_editDamagedPath);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_editWidth);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_editHeight);
	DDX_Control(pDX, IDC_EDIT_BITDEPTH, m_editBitDepth);
	DDX_Control(pDX, IDC_EDIT_RESULT, m_editResult);
	DDX_Control(pDX, IDC_BTN_ORIGINAL, m_btnOriginal);
	DDX_Control(pDX, IDC_BTN_DAMAGED, m_btnDamaged);
	DDX_Control(pDX, IDC_BTN_CALCULATE, m_btnCalculate);
}

BEGIN_MESSAGE_MAP(CPSNRCalculatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_ORIGINAL, &CPSNRCalculatorDlg::OnBnClickedBtnOriginal)
	ON_BN_CLICKED(IDC_BTN_DAMAGED, &CPSNRCalculatorDlg::OnBnClickedBtnDamaged)
	ON_BN_CLICKED(IDC_BTN_CALCULATE, &CPSNRCalculatorDlg::OnBnClickedBtnCalculate)
	ON_EN_CHANGE(IDC_EDIT_DAMAGED, &CPSNRCalculatorDlg::OnEnChangeEditDamaged)
END_MESSAGE_MAP()


// CPSNRCalculatorDlg 메시지 처리기

BOOL CPSNRCalculatorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// 초기값 설정
	m_editWidth.SetWindowText(_T("256"));
	m_editHeight.SetWindowText(_T("256"));
	m_editBitDepth.SetWindowText(_T("8"));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CPSNRCalculatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CPSNRCalculatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CPSNRCalculatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CPSNRCalculatorDlg::OnBnClickedBtnOriginal()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(TRUE, _T("raw"), nullptr,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("RAW Files (*.raw)|*.raw|BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		m_strOriginalPath = dlg.GetPathName();
		m_editOriginalPath.SetWindowText(m_strOriginalPath);
	}
}


void CPSNRCalculatorDlg::OnBnClickedBtnCalculate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 입력값 검증
	CString strWidth, strHeight, strBitDepth;
	m_editWidth.GetWindowText(strWidth);
	m_editHeight.GetWindowText(strHeight);
	m_editBitDepth.GetWindowText(strBitDepth);

	m_nWidth = _ttoi(strWidth);
	m_nHeight = _ttoi(strHeight);
	m_nBitDepth = _ttoi(strBitDepth);

	if (m_nWidth <= 0 || m_nHeight <= 0 || m_nBitDepth <= 0)
	{
		AfxMessageBox(_T("올바른 영상 크기와 비트 심도를 입력하세요."));
		return;
	}

	if (m_strOriginalPath.IsEmpty() || m_strDamagedPath.IsEmpty())
	{
		AfxMessageBox(_T("원본 영상과 손상된 영상 파일을 선택하세요."));
		return;
	}

	// 이미지 데이터 로드
	BYTE* pOriginalData = nullptr;
	BYTE* pDamagedData = nullptr;

	if (!LoadImage(m_strOriginalPath, &pOriginalData, m_nWidth, m_nHeight) ||
		!LoadImage(m_strDamagedPath, &pDamagedData, m_nWidth, m_nHeight))
	{
		AfxMessageBox(_T("이미지 로드에 실패했습니다."));
		if (pOriginalData) delete[] pOriginalData;
		if (pDamagedData) delete[] pDamagedData;
		return;
	}

	// MSE 계산
	double mse = CalculateMSE(pOriginalData, pDamagedData, m_nWidth, m_nHeight);

	// PSNR 계산
	int maxValue = (1 << m_nBitDepth) - 1;  // 2^bitDepth - 1
	double psnr = CalculatePSNR(mse, maxValue);

	// 결과 출력
	DisplayResult(psnr, mse);

	// 메모리 해제
	delete[] pOriginalData;
	delete[] pDamagedData;
}

void CPSNRCalculatorDlg::OnBnClickedBtnDamaged()
{
	CFileDialog dlg(TRUE, _T("raw"), nullptr,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("RAW Files (*.raw)|*.raw|BMP Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		m_strDamagedPath = dlg.GetPathName();
		m_editDamagedPath.SetWindowText(m_strDamagedPath);
	}
}

BOOL CPSNRCalculatorDlg::LoadImage(CString filePath, BYTE** ppImageData, int width, int height)
{
	FILE* pFile = nullptr;
	errno_t err = _tfopen_s(&pFile, filePath, _T("rb"));

	if (err != 0 || !pFile)
	{
		return FALSE;
	}

	int imageSize = width * height;
	*ppImageData = new BYTE[imageSize];

	size_t readSize = fread(*ppImageData, sizeof(BYTE), imageSize, pFile);
	fclose(pFile);

	if (readSize != imageSize)
	{
		delete[] * ppImageData;
		*ppImageData = nullptr;
		return FALSE;
	}

	return TRUE;
}

double CPSNRCalculatorDlg::CalculateMSE(BYTE* original, BYTE* damaged, int width, int height)
{
	double sum = 0.0;
	int totalPixels = width * height;

	for (int i = 0; i < totalPixels; i++)
	{
		double diff = (double)original[i] - (double)damaged[i];
		sum += diff * diff;
	}

	return sum / totalPixels;
}

double CPSNRCalculatorDlg::CalculatePSNR(double mse, int maxValue)
{
	if (mse == 0.0)
	{
		return INFINITY;  // 완벽하게 동일한 이미지
	}

	double psnr = 10.0 * log10((maxValue * maxValue) / mse);
	return psnr;
}

void CPSNRCalculatorDlg::DisplayResult(double psnr, double mse)
{
	CString result;

	if (psnr == INFINITY)
	{
		result.Format(_T("MSE: %.6f\r\n")
			_T("PSNR: 무한대 (완벽하게 동일한 이미지)\r\n")
			_T("\r\n화질 평가: 완벽한 화질"),
			mse);
	}
	else
	{
		CString quality;
		if (psnr > 40)
			quality = _T("매우 좋은 화질 (거의 손실 없음)");
		else if (psnr > 30)
			quality = _T("좋은 화질 (약간의 손실)");
		else if (psnr > 20)
			quality = _T("보통 화질 (눈에 띄는 손실)");
		else
			quality = _T("나쁜 화질 (심각한 손실)");

		result.Format(_T("MSE: %.6f\r\n")
			_T("PSNR: %.4f dB\r\n")
			_T("\r\n화질 평가: %s"),
			mse, psnr, quality);
	}

	m_editResult.SetWindowText(result);
}
void CPSNRCalculatorDlg::OnEnChangeEditDamaged()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// ENM_CHANGE가 있으면 마스크에 ORed를 플래그합니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

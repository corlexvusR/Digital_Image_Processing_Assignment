
// IP_ProgrammingDoc.cpp: CIPProgrammingDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "IP_Programming.h"
#endif

#include "IP_ProgrammingDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIPProgrammingDoc

IMPLEMENT_DYNCREATE(CIPProgrammingDoc, CDocument)

BEGIN_MESSAGE_MAP(CIPProgrammingDoc, CDocument)
	ON_COMMAND(ID_FILTERING_SOBEL, &CIPProgrammingDoc::OnFilteringSobel)
	ON_COMMAND(ID_FILTERING_LOG, &CIPProgrammingDoc::OnFilteringLog)
	ON_COMMAND(ID_FILTERING_MEDIAN, &CIPProgrammingDoc::OnFilteringMedian)
	ON_COMMAND(ID_LINEDETECTION_HOUGHTRANSFORM, &CIPProgrammingDoc::OnLinedetectionHoughtransform)
	ON_COMMAND(ID_LINEDETECTION_CANNYHOUGHTRANSFORM, &CIPProgrammingDoc::OnLinedetectionCannyhoughtransform)
	ON_COMMAND(ID_CORNERDETECTION_HESSIANCORNERDETECTION, &CIPProgrammingDoc::OnCornerdetectionHessiancornerdetection)
END_MESSAGE_MAP()


// CIPProgrammingDoc 생성/소멸

CIPProgrammingDoc::CIPProgrammingDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CIPProgrammingDoc::~CIPProgrammingDoc()
{
}

BOOL CIPProgrammingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();

	if (pApp->toolbox != NULL && pApp->toolbox->io.m_Outputbuf != NULL)
	{
		// 출력 영상 설정
		toolbox.io.m_Height = pApp->toolbox->io.m_Height;
		toolbox.io.m_Width = pApp->toolbox->io.m_Width;
		
		// 출력 버퍼를 현재 Document의 입력으로 설정
		toolbox.io.m_Inputbuf = pApp->toolbox->io.m_Outputbuf;

		// BMP로 변환
		toolbox.io.IO_MakeGrayImagetoBMP(toolbox.io.m_Inputbuf);

		// 제목 설정
		this->SetTitle("Output Image");

		// 출력 버퍼 초기화
		pApp->toolbox->io.m_Outputbuf = NULL;
	}

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CIPProgrammingDoc serialization

void CIPProgrammingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CIPProgrammingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CIPProgrammingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CIPProgrammingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CIPProgrammingDoc 진단

#ifdef _DEBUG
void CIPProgrammingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIPProgrammingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CIPProgrammingDoc 명령


// OnOpenDocument 함수
BOOL CIPProgrammingDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CFileOpenDlg myDlg;
	if (myDlg.DoModal() == IDOK) {
		toolbox.io.m_Width = myDlg.GetWidth();
		toolbox.io.m_Height = myDlg.GetHeight();

		// 파일 열기
		errno_t err = _tfopen_s(&fpInputImage, lpszPathName, _T("rb"));
		if (err != 0 || !fpInputImage) {
			AfxMessageBox(_T("파일을 열 수 없습니다."));
			return FALSE;
		}

		// 메모리 할당
		toolbox.io.m_Inputbuf =
			toolbox.io.memory_alloc2D(toolbox.io.m_Width, toolbox.io.m_Height);

		// 파일 읽기
		fread(&toolbox.io.m_Inputbuf[0][0], sizeof(UCHAR),
			toolbox.io.m_Width * toolbox.io.m_Height, fpInputImage);

		// BMP 변환
		toolbox.io.IO_MakeGrayImagetoBMP(toolbox.io.m_Inputbuf);

		fclose(fpInputImage);
		return TRUE;
	}

	return FALSE;
}


void CIPProgrammingDoc::OnFilteringSobel()
{
	// 입력 영상이 없으면 리턴
	if (toolbox.io.m_Inputbuf == NULL)
		return;

	// 소벨 필터링 수행
	toolbox.sobel.SobelFiltering(toolbox.io.m_Inputbuf, toolbox.io.m_Height, toolbox.io.m_Width);

	// 출력 버퍼에 결과 저장
	toolbox.io.m_Outputbuf = toolbox.sobel.m_pucSobelFilteringImgbuf;

	// 새 창 열기
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox->io.m_Height = toolbox.io.m_Height;
	pApp->toolbox->io.m_Width = toolbox.io.m_Width;
	pApp->toolbox->io.m_Outputbuf = toolbox.sobel.m_pucSobelFilteringImgbuf;

	// 새 창 생성
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CIPProgrammingDoc::OnFilteringLog()
{
	if (toolbox.io.m_Inputbuf == NULL)
		return;

	double sigma = 1.4;  // 가우시안 표준편차 (표준 편차 조정 가능)

	toolbox.log.LoGFiltering(toolbox.io.m_Inputbuf,
		toolbox.io.m_Height,
		toolbox.io.m_Width,
		sigma);

	toolbox.io.m_Outputbuf = toolbox.log.m_pucLoGFilteringImgbuf;

	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

// Median 필터링
void CIPProgrammingDoc::OnFilteringMedian()
{
	if (toolbox.io.m_Inputbuf == NULL)
		return;

	int window_size = 3;  // 3x3, 5x5, 7x7 등으로 변경 가능

	toolbox.median.MedianFiltering(toolbox.io.m_Inputbuf, toolbox.io.m_Height, toolbox.io.m_Width, window_size);

	toolbox.io.m_Outputbuf = toolbox.median.m_pucMedianFilteringImgbuf;

	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

// 허프 변환
void CIPProgrammingDoc::OnLinedetectionHoughtransform()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	
	// 1. 입력 영상 확인
	if (toolbox.io.m_Inputbuf == NULL) {
		return;
	}

	// 2. 소벨 경계선 검출 수행
	toolbox.sobel.SobelFiltering(toolbox.io.m_Inputbuf, toolbox.io.m_Height, toolbox.io.m_Width);

	// 3. 허프 변환으로 직선 검출
	toolbox.hough.HoughTransform(toolbox.io.m_Inputbuf, toolbox.sobel.m_pucSobelFilteringImgbuf, toolbox.io.m_Height, toolbox.io.m_Width);

	// 4. 결과를 출력 버퍼에 설정
	toolbox.io.m_Outputbuf = toolbox.hough.m_pucLineDetectionImgBuf;

	// 5. 새 창으로 결과 표시
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CIPProgrammingDoc::OnLinedetectionCannyhoughtransform()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 1. 입력 영상 확인
	if (toolbox.io.m_Inputbuf == NULL) {
		return;
	}

	// 2. 캐니 경계선 검출 
	toolbox.canny.CannyEdgeDetection(toolbox.io.m_Inputbuf, toolbox.io.m_Height, toolbox.io.m_Width, 0.5, 1);

	// 3. 허프 변환으로 직선 검출
	toolbox.hough.HoughTransform(toolbox.io.m_Inputbuf, toolbox.canny.m_pucCannyEdgeImgBuf, toolbox.io.m_Height, toolbox.io.m_Width);

	// 4. 결과 출력
	toolbox.io.m_Outputbuf = toolbox.hough.m_pucLineDetectionImgBuf;

	// 5. 새 창으로 결과 표시
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

// 헤시안 매트릭스를 통한 코너점 추출하기
void CIPProgrammingDoc::OnCornerdetectionHessiancornerdetection()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (toolbox.io.m_Inputbuf == NULL) {
		AfxMessageBox(_T("먼저 영상을 열어주세요."));
		return;
	}

	// 헤시안 코너 검출 수행
	toolbox.hessian.HessianCornerDetection(toolbox.io.m_Inputbuf,
											toolbox.io.m_Height,
											toolbox.io.m_Width,
											0.01);  // threshold ratio

	// 검출된 코너 개수 출력
	CString msg;
	msg.Format(_T("검출된 코너 개수: %d"),
		(int)toolbox.hessian.m_detectedCorners.size());
	AfxMessageBox(msg);

	// 결과를 출력 버퍼에 설정
	toolbox.io.m_Outputbuf = toolbox.hessian.m_pucCornerImgBuf;

	// 새 창으로 결과 표시
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

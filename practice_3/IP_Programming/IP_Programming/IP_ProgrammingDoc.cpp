
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
	ON_COMMAND(ID_HISTOGRAM_EQUALIZATION, &CIPProgrammingDoc::OnHistogramEqualization)
	ON_COMMAND(ID_HISTOGRAM_BRIGHTEN, &CIPProgrammingDoc::OnHistogramBrighten)
	ON_COMMAND(ID_HISTOGRAM_DARKEN, &CIPProgrammingDoc::OnHistogramDarken)
	ON_COMMAND(ID_HISTOGRAM_SET_REFERENCE, &CIPProgrammingDoc::OnHistogramSetReference)
	ON_COMMAND(ID_HISTOGRAM_MATCHING, &CIPProgrammingDoc::OnHistogramMatching)
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

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	CIPProgrammingApp *pApp = (CIPProgrammingApp*)AfxGetApp();
	if (pApp->toolbox != NULL) {
		if (pApp->toolbox->io.m_Outputbuf) {
			toolbox.io.m_Height = pApp->toolbox->io.m_Height;
			toolbox.io.m_Width = pApp->toolbox->io.m_Width;
			toolbox.io.IO_MakeGrayImagetoBMP(pApp->toolbox->io.m_Outputbuf);

			this->SetTitle("Output Image");
			pApp->toolbox = NULL;
		}
	}

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


void CIPProgrammingDoc::OnHistogramEqualization()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	if (!toolbox.io.m_Inputbuf)
		return;

	// 원본 영상 히스토그램 생성
	float Histogram[256] = { 0, };
	toolbox.histogram.Histogram_MakeHistogram(toolbox.io.m_Inputbuf, Histogram, toolbox.io.m_Width, toolbox.io.m_Height);

	// 히스토그램 평활화 영상 생성
	memset(Histogram, 0, sizeof(float) * 256);
	toolbox.histogram.Histogram_MakeHistogramEqualization(toolbox.io.m_Inputbuf, toolbox.io.m_Width, toolbox.io.m_Height);
	toolbox.io.m_Outputbuf = toolbox.histogram.m_pucHistEqualImgBuf;

	// 히스토그램 평활화 영상을 새로운 창에 띄우기
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CIPProgrammingDoc::OnHistogramBrighten()
{
	if (!toolbox.io.m_Inputbuf) {
		AfxMessageBox(_T("입력 영상이 없습니다."));
		return;
	}

	// 밝기 증가 (50만큼 밝게)
	toolbox.histogram.Histogram_BrightnessShift(toolbox.io.m_Inputbuf,
		toolbox.io.m_Width,
		toolbox.io.m_Height,
		50);

	toolbox.io.m_Outputbuf = toolbox.histogram.m_pucHistEqualImgBuf;

	// 새 창에서 결과 보기
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CIPProgrammingDoc::OnHistogramDarken()
{
	if (!toolbox.io.m_Inputbuf) {
		AfxMessageBox(_T("입력 영상이 없습니다."));
		return;
	}

	// 밝기 감소 (50만큼 어둡게)
	toolbox.histogram.Histogram_BrightnessShift(toolbox.io.m_Inputbuf,
		toolbox.io.m_Width,
		toolbox.io.m_Height,
		-50);

	toolbox.io.m_Outputbuf = toolbox.histogram.m_pucHistEqualImgBuf;

	// 새 창에서 결과 보기
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CIPProgrammingDoc::OnHistogramSetReference()
{
	if (!toolbox.io.m_Inputbuf) {
		AfxMessageBox(_T("현재 영상을 참조 영상으로 설정할 입력 영상이 없습니다."));
		return;
	}

	// 현재 입력 영상을 참조 영상으로 설정
	toolbox.SetReferenceImage(toolbox.io.m_Inputbuf,
		toolbox.io.m_Width,
		toolbox.io.m_Height);

	AfxMessageBox(_T("현재 영상이 참조 영상으로 설정되었습니다."));
}

void CIPProgrammingDoc::OnHistogramMatching()
{
	if (!toolbox.io.m_Inputbuf) {
		AfxMessageBox(_T("입력 영상이 없습니다."));
		return;
	}

	if (!toolbox.m_ReferenceImage) {
		AfxMessageBox(_T("참조 영상이 설정되지 않았습니다.\n먼저 '참조 영상 설정' 메뉴를 사용하세요."));
		return;
	}

	// 크기 확인
	if (toolbox.io.m_Width != toolbox.m_RefWidth ||
		toolbox.io.m_Height != toolbox.m_RefHeight) {
		AfxMessageBox(_T("영상 크기가 참조 영상과 다릅니다."));
		return;
	}

	// 히스토그램 매칭 수행
	toolbox.histogram.Histogram_Matching(toolbox.io.m_Inputbuf,
		toolbox.m_ReferenceImage,
		toolbox.io.m_Width,
		toolbox.io.m_Height);

	toolbox.io.m_Outputbuf = toolbox.histogram.m_pucHistEqualImgBuf;

	// 새 창에서 결과 보기
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}
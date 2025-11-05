
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
	ON_COMMAND(ID_MORPHOLOGY_CLOSING, &CIPProgrammingDoc::OnMorphologyClosing)
	ON_COMMAND(ID_MORPHOLOGY_OPENING, &CIPProgrammingDoc::OnMorphologyOpening)
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

	// 앱 포인터 가져오기
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();

	if (pApp->toolbox != NULL) {
		if (pApp->toolbox->io.m_Outputbuf) {
			// 출력 이미지 정보 설정
			toolbox.io.m_Height = pApp->toolbox->io.m_Height;
			toolbox.io.m_Width = pApp->toolbox->io.m_Width;

			// BMP 형식으로 변환하여 표시
			toolbox.io.IO_MakeGrayImagetoBMP(pApp->toolbox->io.m_Outputbuf);

			// 창 제목 설정
			this->SetTitle("Output Image");

			// 툴박스 포인터 초기화
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


void CIPProgrammingDoc::OnMorphologyClosing()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 입력 영상 확인
	if (toolbox.io.m_Inputbuf == NULL)
		return;

	// 닫힘 연산 수행
	toolbox.morphology.Morphology_Closing(toolbox.io.m_Inputbuf,
		toolbox.io.m_Width,
		toolbox.io.m_Height);

	// 결과를 출력 버퍼에 설정
	toolbox.io.m_Outputbuf = toolbox.morphology.m_MorphologyBuf;

	// 새 창에 결과 표시
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

void CIPProgrammingDoc::OnMorphologyOpening()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	// 입력 영상 확인
	if (toolbox.io.m_Inputbuf == NULL)
		return;

	// 열림 연산 수행
	toolbox.morphology.Morphology_Opening(toolbox.io.m_Inputbuf,
		toolbox.io.m_Width,
		toolbox.io.m_Height);

	// 결과를 출력 버퍼에 설정
	toolbox.io.m_Outputbuf = toolbox.morphology.m_MorphologyBuf;

	// 새 창에 결과 표시
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	pApp->toolbox = &toolbox;
	AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
}

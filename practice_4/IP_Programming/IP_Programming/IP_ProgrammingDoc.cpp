
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
	ON_COMMAND(ID_DCT_TRANSFORM, &CIPProgrammingDoc::OnDctTransform)
END_MESSAGE_MAP()


// CIPProgrammingDoc 생성/소멸

CIPProgrammingDoc::CIPProgrammingDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CIPProgrammingDoc::~CIPProgrammingDoc()
{
}

// 새 창이 열리면 결과 영상을 출력하는 멤버 함수
BOOL CIPProgrammingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.
	
	CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
	if (pApp->toolbox != NULL) {
		if (pApp->toolbox->io.m_Outputbuf) {
			// 결과 이미지 데이터 복사 - 깊은 복사
			toolbox.io.m_Height = pApp->toolbox->io.m_Height;
			toolbox.io.m_Width = pApp->toolbox->io.m_Width;

			// 버퍼가 이미 존재하는지 확인 후 해제
			if (toolbox.io.m_Outputbuf) {
				free(toolbox.io.m_Outputbuf[0]);
				free(toolbox.io.m_Outputbuf);
				toolbox.io.m_Outputbuf = NULL;
			}

			// 새로 메모리 할당하고 데이터 복사
			toolbox.io.m_Outputbuf = toolbox.io.memory_alloc2D(toolbox.io.m_Width, toolbox.io.m_Height);
			for (int i = 0; i < toolbox.io.m_Height; i++) {
				for (int j = 0; j < toolbox.io.m_Width; j++) {
					toolbox.io.m_Outputbuf[i][j] = pApp->toolbox->io.m_Outputbuf[i][j];
				}
			}

			// BMP 변환
			toolbox.io.IO_MakeGrayImagetoBMP(toolbox.io.m_Outputbuf);
			this->SetTitle(_T("Output Image"));

			// 메모리 해제
			delete pApp->toolbox;
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

// DCT 메뉴를 선택했을 때 실행되는 동작을 정의하기 위한 이벤트 처리 함수
void CIPProgrammingDoc::OnDctTransform()
{
	DCTDlg dlg;

	if (dlg.DoModal() == IDOK) {
		int m_BlockSize = dlg.m_DlgBlockSize;

		// 블록 크기 유효성 검사
		bool validBlockSize = false;
		for (int i = 1; i <= 32; i *= 2) {
			if (m_BlockSize == i) {
				validBlockSize = true;
				break;
			}
		}

		if (!validBlockSize) {
			AfxMessageBox(_T("블록 크기는 1, 2, 4, 8, 16 또는 32만 가능합니다."));
			return;
		}

		// FDCT
		toolbox.dct.DCT_MakeBlock(m_BlockSize, 1, toolbox.io.m_Inputbuf,
			toolbox.io.m_Width, toolbox.io.m_Height);
		toolbox.dct.DCT_MakeFrequencytoGray(toolbox.dct.m_pucForwardDCTbuf,
			toolbox.dct.FDCTImgbuf, toolbox.io.m_Width, toolbox.io.m_Height);

		// 새 창에 표시하기 위한 준비
		CIPProgrammingApp* pApp = (CIPProgrammingApp*)AfxGetApp();
		if (pApp->toolbox != NULL) {
			delete pApp->toolbox;
			pApp->toolbox = NULL;
		}

		pApp->toolbox = new CIP_ProgrammingToolBox();

		// 필요한 데이터만 복사
		pApp->toolbox->io.m_Width = toolbox.io.m_Width;
		pApp->toolbox->io.m_Height = toolbox.io.m_Height;

		// 출력 버퍼 새로 할당 및 복사
		pApp->toolbox->io.m_Outputbuf = pApp->toolbox->io.memory_alloc2D(toolbox.io.m_Width, toolbox.io.m_Height);
		for (int i = 0; i < toolbox.io.m_Height; i++) {
			for (int j = 0; j < toolbox.io.m_Width; j++) {
				pApp->toolbox->io.m_Outputbuf[i][j] = toolbox.dct.FDCTImgbuf[i][j];
			}
		}

		// 새 창 열기
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);

		// IDCT 처리 (동일한 패턴으로)
		toolbox.dct.DCT_MakeBlock(m_BlockSize, 0, toolbox.io.m_Inputbuf,
			toolbox.io.m_Width, toolbox.io.m_Height);
		toolbox.dct.DCT_MakeFrequencytoGray(toolbox.dct.m_pucInverseDCTbuf,
			toolbox.dct.IDCTImgbuf, toolbox.io.m_Width, toolbox.io.m_Height);

		if (pApp->toolbox != NULL) {
			delete pApp->toolbox;
			pApp->toolbox = NULL;
		}

		pApp->toolbox = new CIP_ProgrammingToolBox();

		// 필요한 데이터만 복사
		pApp->toolbox->io.m_Width = toolbox.io.m_Width;
		pApp->toolbox->io.m_Height = toolbox.io.m_Height;

		// 출력 버퍼 새로 할당 및 복사
		pApp->toolbox->io.m_Outputbuf = pApp->toolbox->io.memory_alloc2D(toolbox.io.m_Width, toolbox.io.m_Height);
		for (int i = 0; i < toolbox.io.m_Height; i++) {
			for (int j = 0; j < toolbox.io.m_Width; j++) {
				pApp->toolbox->io.m_Outputbuf[i][j] = toolbox.dct.IDCTImgbuf[i][j];
			}
		}

		// 새 창 열기
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW);
	}
}


// ChildFrm.cpp: CChildFrame 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "IP_Programming.h"

#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWndEx)
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()

// CChildFrame 생성/소멸

CChildFrame::CChildFrame() noexcept
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CChildFrame::~CChildFrame()
{
}


BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서 Window 클래스 또는 스타일을 수정합니다.
	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

// CChildFrame 진단

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}
#endif //_DEBUG

// CChildFrame 메시지 처리기


void CChildFrame::SetWindowSize(int width, int height)
{
    // 클라이언트 영역 기준으로 창 크기 계산
    CRect clientRect(0, 0, width, height);
    CalcWindowRect(&clientRect);

    // 창 크기 설정
    SetWindowPos(NULL, 0, 0,
        clientRect.Width(), clientRect.Height(),
        SWP_NOMOVE | SWP_NOZORDER);
}

void CChildFrame::AutoResizeToImage(int imageWidth, int imageHeight)
{
    // 적절한 여백 추가
    int margin = 50;
    int windowWidth = imageWidth + margin;
    int windowHeight = imageHeight + margin + 100;

    // 화면 크기 제한
    CRect screenRect;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

    if (windowWidth > screenRect.Width()) {
        windowWidth = screenRect.Width() - 50;
    }
    if (windowHeight > screenRect.Height()) {
        windowHeight = screenRect.Height() - 50;
    }

    SetWindowSize(windowWidth, windowHeight);
    CenterWindow();
}

void CChildFrame::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    // 최소 크기 설정
    lpMMI->ptMinTrackSize.x = 400;
    lpMMI->ptMinTrackSize.y = 300;

    CMDIChildWndEx::OnGetMinMaxInfo(lpMMI);
}
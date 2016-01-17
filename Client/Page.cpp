#include "stdafx.h"
#include "Page.h"
#include <Windows.h>

#include "Button.h"
#include "Scene.h"

using namespace std;

//////////////////////////////////////////////////////////
//							Page

Page::Page()
{
	m_rcPageSize = m_rcButton = { 0, 0, 0, 0 };
	m_hWnd = 0;
	m_bOnView = true;
	m_bInitialize = m_bUseButton = false;
	m_strTag = TEXT("");
	m_bWriteNow = false;
}

Page::~Page()
{
}

void Page::Initialize(CGameFramework *parent, HWND hWnd, POINT startPos, bool View, RECT rc, bool bUseButton, RECT rcButton)
{
	m_pfmwkParent = parent;
	m_bInitialize = true;
	m_ptPageStartPos = startPos;
	m_rcPageSize = rc;
	m_bOnView = View;
	m_hWnd = hWnd;
	m_bUseButton = bUseButton;
	m_rcButton = rcButton;
}

bool Page::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	case WM_KEYUP:
		break;
	case WM_CHAR:
		break;
	default:
		return false;
	}
	return true;
}

bool Page::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	default:
		return false;
	}
	return true;
}

bool Page::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		break;

	default:
		return false;

	}
	return true;
}

bool Page::IsClickThisPage(POINT pt)
{
	bool br = false;
	if (m_bUseButton)
		br = (PtInRect(&m_rcButton, pt) ? true : false);
	if (!br && m_bOnView)
		br = (PtInRect(&(m_ptPageStartPos + m_rcPageSize), pt) ? true : false);

	return br;
}

void Page::Link(Page* pPage)
{
	if (!pPage)	return;
	for (auto p : m_Linkpages)
		if (p->findToTag(m_strTag) != nullptr) return;
	if (m_Linkpages.size() == 0)
		m_Linkpages.push_back(this);

	if (pPage->m_Linkpages.size() == 0)
		m_Linkpages.push_back(pPage);

	for (auto p : pPage->m_Linkpages)
		m_Linkpages.push_back(p);

	for (auto p : m_Linkpages)
		if (p->findToTag(m_strTag) == nullptr)
			p->m_Linkpages = m_Linkpages;

}

POINT Page::MousePoint(LPARAM lParam)
{
	static POINT Mp;
	POINT mainWindowSize = { CLIENT_WIDTH, CLIENT_HEIGHT };
	POINT pt = { -1, -1 };
	if (LOWORD(lParam) == sideStore(1, (int)LOWORD(lParam), (int)mainWindowSize.x))
		pt.x = Mp.x = LOWORD(lParam);
	if (HIWORD(lParam) == sideStore(1, (int)HIWORD(lParam), (int)mainWindowSize.y))
		pt.y = Mp.y = HIWORD(lParam);

	if (pt.x == -1)
		pt.x = Mp.x;
	if (pt.y == -1)
		pt.y = Mp.y;

	return Mp;
}

pageFileList::pageFileList()
{
	m_colorButton = RGB(155, 155, 155);
	m_bInitialize = false;
	m_bInvaildProcessing = false;
}

pageFileList::~pageFileList()
{


//	(길이) * (표시) / ((실제 길이 + 표시) * 2)

//	m_scrControlViewList.initialize(scroll::SCRType::SCR_H,
//		(m_lTileImageView->size() + 1) * 150.f - m_rcPageSize.right,	// 실제 길이
//		m_rcPageSize.right,			//	표시
//		m_rcPageSize.right - 20,	//	길이
//		m_ptPageStartPos + Point2F(10, 130),
//		0.75f, 
//		(m_rcPageSize.right - 20) * m_rcPageSize.right / ((m_lTileImageView->size() + 1) * 300.f));
//
}

bool pageFileList::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	case WM_CHAR:
		for (auto& p : m_vPageList)
			if (p->GetWirteable()) p->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYUP:
		for (auto& p : m_vPageList)
			if (p->GetWirteable()) p->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	default:
		return false;
	}
	return true;
}

bool pageFileList::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	if (m_bInvaildProcessing) return true;
	if (m_vffFileList.size() * 55 > (m_rcPageSize.bottom - m_rcPageSize.top) - 2 * m_ulspDownlist)
		m_Scroll.OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
//	{
//		if(nMessageID == WM_LBUTTONUP) m_bInvaildProcessing = false;
//		return true;
//	}
	auto mp = MousePoint(lParam);

	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		m_pThisPage = nullptr;
		for (auto& p : m_vPageList)
			if (p->IsClickThisPage(mp)) {
				m_pThisPage = p.get();
				p->SetWirteable(true);
				p->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
				break;
			}
			else
			{
				p->SetWirteable(false);
			}
		break;
	case WM_LBUTTONDOWN:

	{
		m_pThisPage = nullptr;
		if (PtInRect(&m_rcButton, mp))
		{
			for (auto p : m_Linkpages)
				if (p != this)p->SetViewMode(false);
				else SetViewMode(true);
		}
		for (auto& p : m_vPageList)
			if (p->IsClickThisPage(mp)) {
				m_pThisPage = p.get();
				p->SetWirteable(true);
				p->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
				break;
			}
			else
			{
				p->SetWirteable(false);
			}

		//	현재 페이지에서 행하는 작업
		if (nullptr == m_pThisPage)
		{

		}
	}
	break;

	case WM_MOUSEMOVE:
		if (m_pThisPage != nullptr)
			m_pThisPage->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		if (m_pThisPage != nullptr)
		{
			m_pThisPage->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			m_pThisPage = nullptr;
		}

	default:
		return false;

	}
	return true;
}

bool pageFileList::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	case WM_GETVALUE:
	{
		switch (lParam)
		{
		case 1:
			{
			
				m_ulspDownlist = 50;

				m_vffFileList = move(*((vector<FileFormat>*)wParam));
				for (auto &p : m_vffFileList) p.cibFilethumbnail.CreateHBitmap();

				RECT rcValidArea = m_ptPageStartPos + m_rcPageSize;
				rcValidArea.top += m_ulspDownlist;
				rcValidArea.bottom = rcValidArea.bottom - rcValidArea.top - m_ulspDownlist;

				if (m_vPageList.size() > 0) m_vPageList.clear();

				for (size_t sz = 0; sz < m_vffFileList.size(); ++sz)
				{
					m_vPageList.push_back(make_unique<pSelectFileButton>());
					pSelectFileButton* pCreatedButton = static_cast<pSelectFileButton*>(m_vPageList.back().get());
					pCreatedButton->Initialize(m_pfmwkParent, m_hWnd, true, &m_vffFileList[sz],
						MakeRect<LONG>(	10UL		, m_ulspDownlist		+ sz * 55,
										10UL + 350	, m_ulspDownlist + 50	+ sz * 55)
						);

					pCreatedButton->GetAdvanceInfo(0, &sz);
					pCreatedButton->GetAdvanceInfo(2, &rcValidArea);
				}

				LONG length = rcValidArea.bottom - m_ulspDownlist;
				m_Scroll.initialize(scroll::SCR_V, (50 + (m_vffFileList.size() - 1) * 55 - length),
					length, length * 0.95,
					MakePoint<LONG>(m_rcPageSize.right - 8, m_ulspDownlist * 1.25), 170
					);
					
				::ShowWindow(hWnd, SW_SHOW);
				::SetWindowPos(hWnd	, 0					//	ZORDER
									, 0, 0				//	WINPOS
									, 0, 0				//	WINSIZE
									, SWP_NOZORDER		//	NO ALTER Z-ORDER
									| SWP_NOSIZE		//	NO ALTER SIZE
									| SWP_NOMOVE		//	NO ALTER REPOSITION
					);

				SendMessage(hWnd, WM_GETVALUE, (WPARAM)&(m_vffFileList), 0);
			}
			break;
			
		case 2:
			{
				m_bInvaildProcessing = true;
				m_strWaitMessage = TEXT("작업이 끝날 때까지 기다려주세요...");
			}
			break;

		case 3:
			{
				m_bInvaildProcessing = false;
			}
			break;

		default:
			break;
		}
		
	}
	break;
	default:
		return false;
	}
	return true;
}

void pageFileList::Initialize(CGameFramework *parent, HWND hWnd, POINT startPos, bool View, RECT rc, bool bUseButton, RECT rcButton)
{
	m_pfmwkParent = parent;
	m_bInitialize = true;
	m_ptPageStartPos = startPos;
	m_rcPageSize = rc;
	m_bOnView = View;
	m_hWnd = hWnd;
	m_bUseButton = bUseButton;
	m_rcButton = rcButton;


}

void pageFileList::Update(float fTimeElapsed)
{
}

void pageFileList::OnDraw(HDC hDC)
{
	HFONT mainFont = CreateFont(24, FontSet_Arial_B);
	HFONT origFont = (HFONT)SelectObject(hDC, mainFont);
	SetTextColor(hDC, RGB(55, 55, 55));

	RECT rcPageWorldSize = m_ptPageStartPos + m_rcPageSize;

	RECT rcValidArea = m_ptPageStartPos + m_rcPageSize;
	rcValidArea.left = 10;
	rcValidArea.right = 360;
	rcValidArea.top += m_ulspDownlist;
	rcValidArea.bottom = rcValidArea.bottom - rcValidArea.top - m_ulspDownlist;

	RECT rcTitle = m_ptPageStartPos + m_rcPageSize;
	rcTitle.bottom = rcTitle.top + m_ulspDownlist;

	DrawLineBox(hDC, SizeDown(rcPageWorldSize, 2), RGB(255, 255, 255), RGB(155, 155, 155), 2, DL_FRAME);
	DrawLineBox(hDC, SizeDown(rcTitle, 5), RGB(255, 255, 255), RGB(155, 155, 155), 2, DL_FRAME);

	for (auto &p : m_vPageList) { float fLength = m_Scroll.GetNowSrcPosition(); p->GetAdvanceInfo(1, &fLength); p->OnDraw(hDC); }

	DrawText(hDC, TEXT("Download List"), -1, &rcTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if (m_vffFileList.size() * 55 > (m_rcPageSize.bottom - m_rcPageSize.top - 2 * m_ulspDownlist))
		m_Scroll.Draw(hDC, rcPageWorldSize, RGB(0, 0, 0));

	SetDCBrushColor(hDC, RGB(55, 55, 55));
	FrameRect(hDC, &rcValidArea, (HBRUSH)GetStockObject(DC_BRUSH));

	if (m_bInvaildProcessing)
	{
		SetTextColor(hDC, RGB(255, 255, 255));
		ColorBlt(hDC, rcPageWorldSize, RGB(0, 0, 0), 150);
		DrawText(hDC, m_strWaitMessage.c_str(), -1, &rcPageWorldSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
		
	}
	SelectObject(hDC, origFont);
	DeleteObject(mainFont);

}


pageDownloadInfo::pageDownloadInfo()
{
	m_colorButton = RGB(155, 155, 155);
	m_bInitialize = false;
	m_pDownloadFileInfo = nullptr;
}

pageDownloadInfo::~pageDownloadInfo()
{
	if(m_pPercentStruct) delete m_pPercentStruct;

}

bool pageDownloadInfo::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	case WM_KEYDOWN:
	case WM_CHAR:
		for (auto& p : m_vPageList)
			if (p->GetWirteable()) p->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYUP:
		for (auto& p : m_vPageList)
			if (p->GetWirteable()) p->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	default:
		return false;
	}
	return true;
}

bool pageDownloadInfo::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;

	auto mp = MousePoint(lParam);

	switch (nMessageID)
	{
	case WM_RBUTTONDOWN:
		m_pThisPage = nullptr;
		for (auto& p : m_vPageList)
			if (p->IsClickThisPage(mp)) {
				m_pThisPage = p.get();
				p->SetWirteable(true);
				p->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
				break;
			}
			else
			{
				p->SetWirteable(false);
			}
		break;
	case WM_LBUTTONDOWN:

	{
		m_pThisPage = nullptr;
		if (PtInRect(&m_rcButton, mp))
		{
			for (auto p : m_Linkpages)
				if (p != this)p->SetViewMode(false);
				else SetViewMode(true);
		}
		for (auto& p : m_vPageList)
			if (p->IsClickThisPage(mp)) {
				m_pThisPage = p.get();
				p->SetWirteable(true);
				p->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
				break;
			}
			else
			{
				p->SetWirteable(false);
			}

		//	현재 페이지에서 행하는 작업
		if (nullptr == m_pThisPage)
		{
			if (m_pPercentStruct && m_pPercentStruct->nowper >= 100)
			{
				m_bDownloading = false;
				delete m_pDownloadFileInfo;
				m_pDownloadFileInfo = nullptr;
				if (m_pPercentStruct) { delete m_pPercentStruct; m_pPercentStruct = nullptr; }
			}

		}
	}
	break;

	case WM_MOUSEMOVE:
		if (m_pThisPage != nullptr)
			m_pThisPage->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		if (m_pThisPage != nullptr)
		{
			m_pThisPage->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			m_pThisPage = nullptr;
		}

	default:
		return false;

	}
	return true;
}

bool pageDownloadInfo::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!m_bInitialize) return false;
	switch (nMessageID)
	{
	case WM_GETVALUE:
	{
		switch (lParam)
		{
		case 2:
			{
				m_pDownloadFileInfo = voidpointcasting(FileFormat, wParam); // (FileFormat*)wParam;
				MSG msg;
				msg.message = FILE_SELECT;
				msg.lParam = 0;
				msg.wParam = wParam;
				SendToServerMsg(msg);
				m_bDownloading = false;
			}
			break;
		case 4:	// File Downloading
			m_bDownloading = true;
			if (m_pPercentStruct) { delete m_pPercentStruct; m_pPercentStruct = nullptr; }
			m_pPercentStruct = voidpointcasting(percentStruct ,wParam);
			if (m_pPercentStruct->per_1secDownData < 0)	//	 fail
			{
				m_pDownloadFileInfo = nullptr;
				delete m_pPercentStruct;
				m_pPercentStruct = nullptr;
			}
			else if (m_pPercentStruct->nowper >= 100)	//	 success
			{
				m_pDownloadFileInfo = new FileFormat(*m_pDownloadFileInfo);
				SendMessage(hWnd, WM_GETVALUE, 0, 3);
			}
			break;
		case 5:
			m_pDownloadFileInfo = nullptr;
			m_bDownloading = false;
			SendMessage(hWnd, WM_GETVALUE, 0, 3);
			break;
		default:
			break;
		}
		
	}
	break;
	default:
		return false;
	}
	return true;
}

void pageDownloadInfo::Initialize(CGameFramework *parent, HWND hWnd, POINT startPos, bool View, RECT rc, bool bUseButton, RECT rcButton)
{
	m_pfmwkParent = parent;
	m_bInitialize = true;
	m_ptPageStartPos = startPos;
	m_rcPageSize = rc;
	m_bOnView = View;
	m_hWnd = hWnd;
	m_bUseButton = bUseButton;
	m_rcButton = rcButton;
}

void pageDownloadInfo::Update(float fTimeElapsed)
{
	if (!m_pDownloadFileInfo) return;
}

void pageDownloadInfo::OnDraw(HDC hDC)
{
	HFONT mainFont = CreateFont(24, FontSet_Arial_B);
	HFONT origFont = (HFONT)SelectObject(hDC, mainFont);

	RECT rcPageWorldSize = m_ptPageStartPos + m_rcPageSize;

//	LineTo(hDC, rcPageWorldSize.left, 10);
//	MoveToEx(hDC, rcPageWorldSize.left, rcPageWorldSize.bottom - 10, NULL);

	if (!m_pDownloadFileInfo)
	{

		ColorBlt(hDC, rcPageWorldSize, RGB(0, 0, 0), 150);
		SetTextColor(hDC, RGB(255, 255, 255));
		DrawText(hDC, TEXT("파일을 선택해주세요"), -1, &rcPageWorldSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	}
	else
	{
		POINT ptIconCenter = m_ptPageStartPos;
		ptIconCenter.x += 25;
		ptIconCenter.y += 35;
		RECT rcFileName = rcPageWorldSize;
		rcFileName.left += 50; rcFileName.right -= 100;
		rcFileName.top += 10; rcFileName.bottom = rcFileName.top + 50;
		for (auto &p : m_vPageList) p->OnDraw(hDC);

		//	Draw Caption
		{
			HDC hDCIcon = CreateCompatibleDC(hDC);
			SelectObject(hDCIcon, m_pDownloadFileInfo->cibFilethumbnail.GetHBitmap());

			BLENDFUNCTION bf;

			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.SourceConstantAlpha = 0xff;
			bf.AlphaFormat = AC_SRC_ALPHA;
			::AlphaBlend(hDC, ptIconCenter.x - 16, ptIconCenter.y - 16, 32, 32, hDCIcon, 0, 0, 32, 32, bf);

			DeleteDC(hDCIcon);
			COLORREF oriColor = SetTextColor(hDC, RGB(55, 55, 55));
			DrawText(hDC, m_pDownloadFileInfo->strFileName.c_str(), -1, &rcFileName, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			SetTextColor(hDC, oriColor);
		}

		//	퍼센테이지 표시
		if (m_pPercentStruct)
		{			
			/*
			퍼센테이지 바는 400 px.
			퍼센테이지 바의 옆에 사이즈 바에 현재 진행률 표시
			퍼센테이지 밑에 전송률과 남은 시간 표시
			*/
			DeleteObject(mainFont);
			mainFont = CreateFont(14, FontSet_Arial_B);
			SelectObject(hDC, mainFont);

			LONG percentageLength = 400;
			RECT rcPercentage = rcPageWorldSize;
			rcPercentage.left += 50; rcPercentage.right = rcPercentage.left + percentageLength;
			rcPercentage.top += 100; rcPercentage.bottom = rcPercentage.top + 25;

			RECT rcNowPercentage = rcPercentage;
			rcNowPercentage.right = rcPercentage.left + max(0, percentageLength * m_pPercentStruct->nowper) / 100;

			rcNowPercentage = SizeDown(rcNowPercentage, 2);

			COLORREF oriColor = SetTextColor(hDC, RGB(55, 55, 55));
			FrameRect(hDC, &rcPercentage, (HBRUSH)GetStockObject(BLACK_BRUSH));
			if(rcNowPercentage.right > rcNowPercentage.left)
				FillRect(hDC, &rcNowPercentage, (HBRUSH)GetStockObject(BLACK_BRUSH));
		
			wstring strMessage;
			
			strMessage = to_wstring(m_pPercentStruct->nowper) + TEXT(" ％");
			RECT rcViewPercent = rcPercentage;
			rcViewPercent.left = rcViewPercent.right + 10;
			rcViewPercent.right = rcViewPercent.left + 50;
			DrawText(hDC, strMessage.c_str(), -1, &rcViewPercent, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			RECT rcViewByteperSec = rcPercentage;
			rcViewByteperSec.top = rcViewByteperSec.bottom + 10;
			rcViewByteperSec.bottom = rcViewByteperSec.top + 50;
			strMessage = TEXT("현재 전송률 : ") + FormatData(static_cast<size_t>(m_pPercentStruct->per_1secDownData)) + TEXT(" / sec");
			DrawText(hDC, strMessage.c_str(), -1, &rcViewByteperSec, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			rcViewByteperSec.left = rcViewByteperSec.right - 50;
			rcViewByteperSec.right = rcViewByteperSec.right + 50;
			strMessage = TEXT("남은 시간 : ") + FormatTime(m_pPercentStruct->losstime);
			DrawText(hDC, strMessage.c_str(), -1, &rcViewByteperSec, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
			
			SetTextColor(hDC, oriColor);

			if (m_pPercentStruct->nowper >= 100)
			{
				ColorBlt(hDC, rcPageWorldSize, RGB(0, 0, 0), 210);
				SetTextColor(hDC, RGB(255, 255, 255));
				DrawText(hDC, TEXT("전송이 완료됐습니다!"), -1, &rcPageWorldSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

			}
		}


	}
	
	SelectObject(hDC, origFont);
	DeleteObject(mainFont);
}

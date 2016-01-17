#include "stdafx.h"
#include "Page.h"
#include <Windows.h>
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

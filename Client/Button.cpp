#include "stdafx.h"
#include "Button.h"
#include "Page.h"
#include <Windows.h>
#include <iostream>
#include <chrono>

#include "ServerMain.h"

using namespace std;

//////////////////////////////////////////////////////////
//							pButton

pButton::pButton()
{
	m_colorButton = RGB(155, 155, 155);
	m_bInitialize = false;
	m_bPush = false;
}

pButton::~pButton()
{
}

void pButton::Initialize(CGameFramework *parent, HWND hWnd, bool View, LPCWSTR Name, RECT rcButton)
{
	Page::Initialize(parent, hWnd, MakePoint(0, 0), View, MakeRect(0, 0, 0, 0), true, rcButton);

	lstrcpy(m_strName, Name);
}


bool pButton::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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
	return(true);
}

bool pButton::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	POINT mp = MousePoint(lParam);
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:

		if (PtInRect(&m_rcButton, mp))
			m_bPush = m_bStay = true;
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		if (m_bPush && PtInRect(&m_rcButton, mp))
			m_bStay = true;
		else m_bStay = false;
		break;

	case WM_LBUTTONUP:
		//	func
		if (m_bStay && m_bPush)
		{
		}
		m_bStay = false;
		m_bPush = false;
		break;

	default:
		return false;

	}
	return(true);
}

void pButton::Update(float fTimeElapse)
{

}

void pButton::OnDraw(HDC hDC)
{
	HFONT mainFont = CreateFont(25, FontSet_Arial_B);
	SelectObject(hDC, mainFont);

	DrawLineBox(hDC, m_rcButton, (!m_bStay) ? m_colorButton : RGB(55, 55, 55), RGB(255, 255, 255), 2, DL_FRAME);
	SetTextColor(hDC, RGB(255, 255, 255));
	DrawText(hDC, _T("Button"), -1, &m_rcButton, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	DeleteObject(mainFont);
}

//////////////////////////////////////////////////////////
//							pSelectFileButton

pSelectFileButton::pSelectFileButton()
{
	m_colorButton = RGB(155, 155, 155);
	m_bInitialize = false;
	m_bPush = false;
	m_fLengthScrolling = 0;
}

pSelectFileButton::~pSelectFileButton()
{
}

void pSelectFileButton::Initialize(CGameFramework *parent, HWND hWnd, bool View, FileFormat* Name, RECT rcButton)
{
	pButton::Initialize(parent, hWnd, View, Name->strFileName.c_str(), rcButton);

	m_FileFormat = Name;

}

bool pSelectFileButton::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
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
	return(true);
}

bool pSelectFileButton::IsClickThisPage(POINT pt)
{
	RECT rcButton = m_rcButton;
	rcButton.top -= m_fLengthScrolling;
	rcButton.bottom -= m_fLengthScrolling;
	bool br = false;
	if (m_bUseButton)
		br = (PtInRect(&rcButton, pt) ? true : false);
	if (!br && m_bOnView)
		br = (PtInRect(&(m_ptPageStartPos + m_rcPageSize), pt) ? true : false);

	return br;
}

bool pSelectFileButton::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	POINT mp = MousePoint(lParam);
	RECT rcButton = m_rcButton;
	rcButton.top -= m_fLengthScrolling;
	rcButton.bottom -= m_fLengthScrolling;
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:

		if (PtInRect(&rcButton, mp))
			m_bPush = m_bStay = true;
		break;

	case WM_RBUTTONDOWN:
		break;

	case WM_MOUSEMOVE:
		if (m_bPush && PtInRect(&rcButton, mp))
			m_bStay = true;
		else m_bStay = false;
		break;

	case WM_LBUTTONUP:
		//	func
		if (m_bStay && m_bPush)
		{
			SendMessage(hWnd, WM_GETVALUE, m_IdxFileList, 2);
		}
		m_bStay = false;
		m_bPush = false;
		break;

	default:
		return false;

	}
	return(true);
}

void pSelectFileButton::GetAdvanceInfo(unsigned int index, LPVOID extraInfo)
{
	switch (index)
	{
	case 0:	//	Get Index
		m_IdxFileList = voidcasting(size_t, extraInfo);
		break;
	case 1:
		m_fLengthScrolling = voidcasting(float, extraInfo);
		break;
	case 2:
		m_rcValidArea = voidcasting(RECT, extraInfo);
		break;
	}


}

void pSelectFileButton::Update(float fTimeElapse)
{

}

void pSelectFileButton::OnDraw(HDC hDC)
{
	HDC hdc = CreateCompatibleDC(hDC);
	HBITMAP lay = CreateCompatibleBitmap(hDC, m_rcValidArea.right - m_rcValidArea.left, m_rcValidArea.bottom - m_rcValidArea.top);
	SelectObject(hdc, lay);
	{
		SetBkMode(hdc, TRANSPARENT);
		RECT rcLocal = m_rcValidArea;
		rcLocal.right = rcLocal.right - rcLocal.left;
		rcLocal.bottom = rcLocal.bottom - rcLocal.top;
		rcLocal.left = rcLocal.top = 0;

		FillRect(hdc, &rcLocal, (HBRUSH)GetStockObject(WHITE_BRUSH));
		HFONT mainFont = CreateFont(18, FontSet_Arial_B);
		RECT rcButton = MakePoint(-m_rcValidArea.left, -m_rcValidArea.top) + m_rcButton;
		rcButton.top -= m_fLengthScrolling;
		rcButton.bottom -= m_fLengthScrolling;
		RECT rcText = rcButton;
		rcText.left += 50;
		POINT ptIconCenter;
		ptIconCenter.x = (rcButton.left + 25);
		ptIconCenter.y = ((rcButton.top + rcButton.bottom) / 2);

		SelectObject(hdc, mainFont);
		DrawLineBox(hdc, rcButton, (!m_bStay) ? m_colorButton : RGB(55, 55, 55), RGB(255, 255, 255), 2, DL_FRAME);
		SetTextColor(hdc, RGB(255, 255, 255));

		TCHAR strView[80];
		lstrcpy(strView, m_FileFormat->strFileName.c_str());
		PathCompactPath(hdc, strView, rcText.right - rcText.left - 20);	// 20 : ¿©¹é
		DrawText(hdc, strView, -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		HDC hDCIcon = CreateCompatibleDC(hdc);
		SelectObject(hDCIcon, m_FileFormat->cibFilethumbnail.GetHBitmap());

		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		::AlphaBlend(hdc, ptIconCenter.x - 16, ptIconCenter.y - 16, 32, 32, hDCIcon, 0, 0, 32, 32, bf);

		DeleteDC(hDCIcon);
		DeleteObject(mainFont);

		TransparentBlt(hDC, m_rcValidArea.left, m_rcValidArea.top,
			m_rcValidArea.right - m_rcValidArea.left, m_rcValidArea.bottom - m_rcValidArea.top,
			hdc, 0, 0, m_rcValidArea.right - m_rcValidArea.left, m_rcValidArea.bottom - m_rcValidArea.top, RGB(255, 255, 255));
	}
	DeleteObject(lay);
	DeleteDC(hdc);

}

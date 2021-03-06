#include "stdafx.h"
#include "GameFramework.h"

#include <Windows.h>
#include <commdlg.h>

#include <iostream>
#include <memory>
#include <chrono>
#include <string>
#include <algorithm>
#include <fstream>
#include <iterator>

#include "Page.h"
#include "Button.h"
#include "Scene.h"

using namespace std;

CGameFramework::CGameFramework()
{
	lstrcpy(m_pszBuffer, Win32TitleName);

#ifdef _DEBUG
	lstrcat(m_pszBuffer, _T(" ("));
#endif

	m_nTitleLen = lstrlen(m_pszBuffer);
	m_hInstance = NULL;
	m_hWnd = NULL;
	m_hMenu = NULL;

	m_hDCFrameBuffer = NULL;
	m_hBitmapFrameBuffer = NULL;
	m_hBitmapSelect = NULL;

	m_cxViewport = CLIENT_WIDTH;
	m_cyViewport = CLIENT_HEIGHT;

	m_cCurrentTime = chrono::system_clock::now();
	m_fps = 0;
	m_bBackgroundColor = 0x00FFFFFF;

	m_pNowScene = nullptr;
}

CGameFramework::~CGameFramework()
{
	OnDestroy();
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	RECT rc;
	GetClientRect(m_hWnd, &rc);
	m_xViewport = rc.left;
	m_yViewport = rc.top;
	m_cxViewport = rc.right - rc.left;
	m_cyViewport = rc.bottom - rc.top;

	BuildFrameBuffer();
	BuildObjects();

	return(true);

}

void CGameFramework::OnDestroy()
{
	ReleaseObjects();
}

void CGameFramework::BuildFrameBuffer()
{
	HDC hDC = ::GetDC(m_hWnd);

	if (!m_hDCFrameBuffer) m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	if (m_hBitmapFrameBuffer)
	{
		::SelectObject(m_hDCFrameBuffer, NULL);
		::DeleteObject(m_hBitmapFrameBuffer);
		m_hBitmapFrameBuffer = NULL;
	}
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC, m_cxViewport, m_cyViewport);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);

	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor)
{
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_xViewport, m_yViewport, m_cxViewport, m_cyViewport);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer(HDC hDC)
{

	::BitBlt(hDC, m_xViewport, m_yViewport, m_cxViewport, m_cyViewport, m_hDCFrameBuffer, m_xViewport, m_yViewport, SRCCOPY);
}

void CGameFramework::BuildObjects()
{

	m_lScene.push_back(make_unique<CScene>());
	m_pNowScene = m_lScene.back().get();

	m_pNowScene->BuildObjects(TEXT("MainScene"), m_hWnd, this);

}

void CGameFramework::ReleaseObjects()
{
}

bool CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_CHAR:
		break;
		
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			::DestroyWindow(hWnd);
			::PostQuitMessage(0);
			break;
		}
		break;
	case WM_KEYUP:
		break;
	}
	return(true);
}

bool CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	POINT mp = MousePoint(lParam);
	if (m_pNowScene && m_pNowScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) return true;

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:

		m_bGrip = false;
		m_ptLast = mp;

		break;
	case WM_RBUTTONDOWN:

		break;
	case WM_MOUSEMOVE:
		if (m_bGrip)
		{
			m_ptLast = mp;
		}
		break;
	case WM_LBUTTONUP:

		m_bGrip = false;
		break;
	}
	
	return(true);
}

bool CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_DROPFILES:
		if (m_pNowScene)m_pNowScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_UPDATEMESSAGE:
		switch (wParam)
		{
		case FAILUPLOAD:
		{
			RECT rcClient;
			GetClientRect(ghWndMain, &rcClient);
			POINT ptCenter;
			ptCenter.x = (rcClient.left + rcClient.right) / 2;
			ptCenter.y = (rcClient.top + rcClient.bottom) / 2;
			LusMsg Msg;
			Msg.hWnd = ghWndMain;
			Msg.Title = L"실패";
			Msg.MsgStartPos = ptCenter;
			Msg.Msg = L"\n업로드에 실패했습니다.\n불안정한 데이터는 삭제됩니다.";
			Msg.InBoxMsg_1 = L"확인";
			Msg.thisClass = (void*)lParam;
			Msg.Msg_1_Func = [](void* Class) {
				DestroyWindow((HWND)Class);
				close_LusMsgProc();
				//	static_cast<CScene*>(Class);
			};

			LusMsgBox(Msg.hWnd, Msg, hInst, 175);
		
		}	break;
			
		case REBUILDSCENE:
			m_pNowScene->ReBuild();
			break;

		}
		break;
	case WM_CREATEWINDOW:
	{
		RECT getClientSize;
		GetWindowRect(hWnd, &getClientSize);

		HWND hNew = CreateWindow(TEXT("FileDownloadBox"), NULL, WS_POPUP | WS_VISIBLE
					, (getClientSize.left + getClientSize.right - 500) / 2
					, (getClientSize.top + getClientSize.bottom - 200) / 2
					, 500
					, 200
					, ghWndMain
					, NULL
					, hInst
					, NULL
				);

		SetWindowLong(hNew, GWL_EXSTYLE,
			GetWindowLong(hNew, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hNew, 0, 180, LWA_ALPHA);

		ShowWindow(hNew, SW_SHOW);
		UpdateWindow(hNew);

		DataInfo &df = RegistWndInfo(hNew);
		df.SetHwnd(hNew);
		df.SetFileName((LPWSTR)lParam);
		
		MSG msg;

		msg.hwnd = hNew;
		msg.lParam = lParam;
		SendToServerMsg(msg);
	}
		break;
	default:
		return false;
	}
	return true;
}

POINT CGameFramework::MousePoint(LPARAM lParam)
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

void CGameFramework::AnimateObjects(float fTimeElapsed)
{
	if (m_pNowScene)m_pNowScene->AnimateObjects(fTimeElapsed);

}

void CGameFramework::OnDraw()
{
	ClearFrameBuffer(m_bBackgroundColor);

	if (m_pNowScene)m_pNowScene->Draw(m_hDCFrameBuffer);
}

void CGameFramework::FrameAdvance()
{
	chrono::system_clock::time_point m_nCurrentTime = chrono::system_clock::now();
	
	AnimateObjects(m_cftimeElapsed.count());
	OnDraw();

	InvalidateRect(m_hWnd, NULL, FALSE);

	m_cftimeElapsed = chrono::system_clock::now() - m_cCurrentTime;

	if (m_cftimeElapsed.count() > 0.33333)
	{

		chrono::duration<double> fTimeElapsed = chrono::system_clock::now() - m_nCurrentTime;
		m_cCurrentTime = chrono::system_clock::now();
		if (fTimeElapsed.count() > 0.000001)
			m_fps = 1 / fTimeElapsed.count();
	}

#ifdef _DEBUG
	_itow_s(m_fps, m_pszBuffer + m_nTitleLen, 50 - m_nTitleLen, 10);
	wcscat_s(m_pszBuffer + m_nTitleLen, m_nTitleLen, _T(" FPS)"));
#endif
	::SetWindowText(m_hWnd, m_pszBuffer);
}

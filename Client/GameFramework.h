#pragma once

#include "stdafx.h"
#include "Scene.h"

class CGameFramework
{
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void AnimateObjects(float fTimeElapsed);
	void FrameAdvance();
	void OnDraw();

	void BuildObjects();
	void ReleaseObjects();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer(HDC hDC);

	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	POINT MousePoint(LPARAM lParam);
	POINT GetCenterPosition(){ return{ m_cxViewport / 2, m_cyViewport / 2 }; }
	HINSTANCE GethInstance(){ return m_hInstance; }

private:
	//	GameTimer
	_TCHAR								m_pszBuffer[50];
	int									m_nTitleLen;
	chrono::system_clock::time_point	m_cCurrentTime;
	chrono::duration<double>			m_cftimeElapsed;
	double								m_fps;
	//	windows handle
	HINSTANCE							m_hInstance;
	HWND								m_hWnd;
	HMENU								m_hMenu;
	//	Render Bitmap
	HDC									m_hDCFrameBuffer;
	HBITMAP								m_hBitmapFrameBuffer;
	HBITMAP								m_hBitmapSelect;
	DWORD								m_bBackgroundColor;
	//	Viewport
	int									m_xViewport;
	int									m_yViewport;
	int									m_cxViewport;
	int									m_cyViewport;
	//	Mouse Cursor
	bool								m_bGrip;
	POINT								m_ptLast;

	list < unique_ptr< CScene> > 		m_lScene;
	CScene								*m_pNowScene;
};


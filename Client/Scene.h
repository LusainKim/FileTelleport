//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "ServerMain.h"
#include "Page.h"
#include "Button.h"

using namespace std;

class CGameFramework;

class CScene
{
public:
    CScene();
	virtual ~CScene();

	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	virtual void BuildObjects(wstring Tag, HWND hWnd, CGameFramework *pMasterFramework);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Draw(HDC hDC);
	bool FindTag(wstring str){ return (str == m_strTag); }

protected:
	wstring						m_strTag;
	HWND						m_hWnd;
	CGameFramework				*m_pMasterFramework;

	vector<FileFormat>			*m_vffFileList;

	Page						*m_pThisPage;
	vector<unique_ptr<Page>>	m_vPageList;
};

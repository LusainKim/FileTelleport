#pragma once


#include "Page.h"


class pButton : public Page{
protected:
	bool		m_bPush;
	bool		m_bStay;
	TCHAR		m_strName[80];
public:
	pButton();
	~pButton();
	virtual void Initialize(CGameFramework *parent, HWND hWnd, bool View, LPCWSTR Name, RECT rcButton);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void GetAdvanceInfo(unsigned int index, LPVOID extraInfo) = 0;
	virtual void Update(float fTimeElapsed);
	virtual void OnDraw(HDC hDC);
	virtual bool IsClickThisPage(POINT pt) { return Page::IsClickThisPage(pt); }
};

class pSelectFileButton : public pButton {

public:
	pSelectFileButton();
	~pSelectFileButton();
	virtual void Initialize(CGameFramework *parent, HWND hWnd, bool View, FileFormat* Name, RECT rcButton);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void GetAdvanceInfo(unsigned int index, LPVOID extraInfo);
	virtual void Update(float fTimeElapsed);
	virtual void OnDraw(HDC hDC);
	virtual bool IsClickThisPage(POINT pt);
private:
	size_t			m_IdxFileList;
	float			m_fLengthScrolling;
	FileFormat*		m_FileFormat;
	RECT			m_rcValidArea;
};

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <list>

using namespace std;

class Page{
protected:
	wstring					m_strTag;
	POINT					m_ptPageStartPos;
	RECT					m_rcPageSize;
	HWND					m_hWnd;
	bool					m_bOnView;
	bool					m_bInitialize;

	RECT					m_rcButton;
	bool					m_bUseButton;
	COLORREF				m_colorButton;
	vector<Page*>			m_Linkpages;
	class CGameFramework	*m_pfmwkParent;
	bool					m_bWriteNow;
public:
	Page();
	virtual ~Page();
	virtual void Initialize(CGameFramework *parent, HWND hWnd, POINT startPos, bool View, RECT rc, bool bUseButton = false, RECT rcButton = { 0, 0, 0, 0 });
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void Update(float fTimeElapsed) = 0;
	virtual void OnDraw(HDC hDC) = 0;
	virtual void GetAdvanceInfo(unsigned int index, LPVOID extraInfo) {};
	virtual bool IsClickThisPage(POINT pt);
	void SetViewMode(bool mode){ m_bOnView = mode; }
	void SetTag(const wstring& str){ m_strTag = str; }
	Page* findToTag(const wstring& str){ if (str == m_strTag) return this; else return nullptr; }
	//	Link는 하나의 페이지를 다른 페이지에 붙이는 역할입니다.
	void Link(Page* pPage);
	void SetWirteable(bool mode){ m_bWriteNow = mode; }
	bool GetWirteable(){ return m_bWriteNow; }
	POINT MousePoint(LPARAM lParam);
};

class pageFileList : public Page {
protected:

	vector<FileFormat>			m_vffFileList;

	vector<unique_ptr<Page>>	m_vPageList;
	Page						*m_pThisPage;

	bool						m_bInvaildProcessing;
	wstring						m_strWaitMessage;
	scroll						m_Scroll;
	ULONG						m_ulspDownlist;
public:
	pageFileList();
	virtual ~pageFileList();
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void Initialize(CGameFramework *parent, HWND hWnd, POINT startPos, bool View, RECT rc, bool bUseButton = false, RECT rcButton = { 0, 0, 0, 0 });
	void Update(float fTimeElapsed);
	void OnDraw(HDC hDC);

};

class pageDownloadInfo : public Page {
protected:

	FileFormat*					m_pDownloadFileInfo;

	vector<unique_ptr<Page>>	m_vPageList;
	Page						*m_pThisPage;

	double						m_pDownloadPercent;
	percentStruct				*m_pPercentStruct;
	bool						m_bDownloading;
public:
	pageDownloadInfo();
	virtual ~pageDownloadInfo();
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual void Initialize(CGameFramework *parent, HWND hWnd, POINT startPos, bool View, RECT rc, bool bUseButton = false, RECT rcButton = { 0, 0, 0, 0 });
	void Update(float fTimeElapsed);
	void OnDraw(HDC hDC);

};
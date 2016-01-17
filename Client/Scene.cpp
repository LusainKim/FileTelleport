//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "GameFramework.h"
#include "Scene.h"

#include "Page.h"
#include "Button.h"

using namespace std;

CScene::CScene()
{

}

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	auto mp = m_pMasterFramework->MousePoint(lParam);

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		m_pThisPage = nullptr;
		for (auto& p : m_vPageList)
			if (p->IsClickThisPage(mp)) {
				m_pThisPage = p.get();
				p->SetWirteable(true);
				p->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			}
			else
			{
				p->SetWirteable(false);
			}
		break;
	case WM_MOUSEMOVE:
		if (m_pThisPage != nullptr)
			m_pThisPage->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		for (auto& p : m_vPageList)
			if (p->GetWirteable()) p->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		if (m_pThisPage != nullptr)
		{
			m_pThisPage->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
			m_pThisPage = nullptr;
		}
		break;
	}

	return(true);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(false);
}

bool CScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_GETVALUE:
		switch(lParam)
		{
		case 0:
			m_vffFileList = voidpointcasting(vector<FileFormat>, wParam);
			return true;
		case 2:	//	Select DownloadFile

			wParam = (WPARAM)&((*m_vffFileList)[wParam]);
			break;
		}

		for (auto &p : m_vPageList)
			//if (p->findToTag(TEXT("FileListMain")))
			p->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam);
		
		break;
	default:
		return false;
	}
	return true;
}

void CScene::BuildObjects(wstring Tag, HWND hWnd, CGameFramework *pMasterFramework)
{
	m_strTag = Tag;
	m_hWnd = hWnd;
	m_pMasterFramework = pMasterFramework;

	RECT rcClient,rcAdjust;
	DWORD style = (DWORD)::GetWindowLong(hWnd, GWL_STYLE);
	GetWindowRect(hWnd, &rcClient);
	rcAdjust = rcClient;
	AdjustWindowRect(&rcAdjust, style, false);
	rcAdjust = rcAdjust - rcClient;
	rcClient = rcClient - rcAdjust;
	m_vPageList.push_back(make_unique<pageFileList>());
	{
		pageFileList* p = static_cast<pageFileList*>(m_vPageList.back().get());

		p->Initialize(m_pMasterFramework, hWnd, MakePoint(0, 0), true,
			MakeRect<LONG>(0, 0, 370, rcClient.bottom - rcClient.top));

		p->SetTag(TEXT("FileListMain"));
	}
	
	m_vPageList.push_back(make_unique<pageDownloadInfo>());
	{
		pageDownloadInfo* p = static_cast<pageDownloadInfo*>(m_vPageList.back().get());

		p->Initialize(m_pMasterFramework, hWnd, MakePoint<LONG>(370, 0), true,
			MakeRect<LONG>(0, 0, rcClient.right - rcClient.left - 370, rcClient.bottom - rcClient.top));

		p->SetTag(TEXT("DownloadInfo"));

	}
	
}

void CScene::ReleaseObjects()
{
}

void CScene::AnimateObjects(float fTimeElapsed)
{

}

void CScene::Draw(HDC hDC)
{
	for (auto &p : m_vPageList) p->OnDraw(hDC);

//	HDC hdcIcon = CreateCompatibleDC(hDC);
//	TCHAR strView[80];
//	if (m_vffFileList.size() > 0)
//	{
//		for (size_t i = 0; i < m_vffFileList.size(); ++i)
//		{
//			SelectObject(hdcIcon, m_vffFileList[i].second.GetHBitmap());
//
//			BLENDFUNCTION bf;
//
//			bf.BlendOp = AC_SRC_OVER;
//			bf.BlendFlags = 0;
//			bf.SourceConstantAlpha = 0xff;
//			bf.AlphaFormat = AC_SRC_ALPHA;
//			::AlphaBlend(hDC, 10, 10 + i * 35, 32, 32, hdcIcon, 0, 0, 32, 32, bf);
//			lstrcpy(strView, m_vffFileList[i].first.c_str());
//			PathCompactPath(hDC, strView, 250);
//			TextOut(hDC, 50, 10 + i * 35, strView, lstrlen(strView));
//		}
//	}
//	DeleteDC(hdcIcon);
}

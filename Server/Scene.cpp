//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"

#include "GameFramework.h"
#include "Scene.h"

#include "Page.h"
#include "Button.h"

CScene::CScene()
{

}

CScene::~CScene()
{
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_Scroll.OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
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
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		TCHAR szPathFileName[MAX_PATH + 1];
		DragQueryFile(hDrop, 0, szPathFileName, MAX_PATH);

		if (m_cImage) { DeleteObject(m_cImage); m_cImage = nullptr; }
		if (m_hIcon) { DestroyIcon(m_hIcon); m_hIcon = nullptr; }
		
		std::wstring strExtension(PathFindExtension(szPathFileName));
		if (   strExtension == TEXT(".png")
			|| strExtension == TEXT(".jpg")
			|| strExtension == TEXT(".bmp")
			|| strExtension == TEXT(".jpeg")
			|| strExtension == TEXT(".PNG")
			|| strExtension == TEXT(".JPG")
			|| strExtension == TEXT(".JPEG")
			|| strExtension == TEXT(".BMP")
			)
		{
			CImage m_cImageSumnail;
			m_cImageSumnail.Load(szPathFileName);

			float height = (float)m_cImageSumnail.GetHeight();
			float width = (float)m_cImageSumnail.GetWidth();
			bool bStand = false;
			int space = 0;
			if (height > width) { bStand = true; width /= height; height = 1; }
			else if (height < width) { bStand = false; height /= width; width = 1; }
			else { height = width = 1; }

			height *= 64.f;
			width *= 64.f;

			space = 32 - (bStand ? width : height) * 0.5f;

			CImage cImage; cImage.Create(64, 64, 32, CImage::createAlphaChannel);

			HDC hdc = cImage.GetDC();
			RECT rc = { 0,0,64,64 };
			SetDCBrushColor(hdc, RGB(255, 0, 255));
			FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
			if (bStand)	m_cImageSumnail.TransparentBlt(hdc, int(space), 0, int(width), int(height), RGB(255, 0, 255));
			else		m_cImageSumnail.TransparentBlt(hdc, 0, int(space), int(width), int(height), RGB(255, 0, 255));
			cImage.ReleaseDC();

			///////////////////////////////////////////////////////
			
			BITMAPINFO m_bif;
			ZeroMemory(&m_bif, sizeof(BITMAPINFO));

			// 비트맵(DDB) 정보 얻기
			BITMAP bmp;
			GetObject(cImage, sizeof(BITMAP), &bmp);

			// 비트맵(DIB) 정보 설정
			BITMAPINFOHEADER& bmih = m_bif.bmiHeader;
			ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
			bmih.biSize = sizeof(BITMAPINFOHEADER);
			bmih.biWidth = bmp.bmWidth;    // 가로
			bmih.biHeight = bmp.bmHeight;  // 세로
			bmih.biPlanes = 1;
			bmih.biBitCount = cImage.GetBPP();          // 픽셀당 비트수(BPP)
			bmih.biCompression = BI_RGB;

			// 비트맵(DIB) 데이터 추출
			// 데이터의 크기를 알아낸다
			HDC hDC = GetDC(NULL);
			GetDIBits(hDC, cImage, 0, bmp.bmHeight, NULL,
				(LPBITMAPINFO)&bmih, DIB_RGB_COLORS);

			DWORD m_dwImageSize = bmih.biSizeImage;

			// 데이터 저장 공간 확보
			LPBYTE m_bytes = new BYTE[m_dwImageSize];
			GetDIBits(hDC, cImage, 0, bmp.bmHeight, m_bytes, (LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
			ReleaseDC(NULL, hDC);

			size_t cnt = 0;
			for (int y = 0; y < m_bif.bmiHeader.biHeight; y++)  for (int x = 0; x < m_bif.bmiHeader.biWidth; x++)
			{
				if (!(m_bytes[cnt] == 255 && m_bytes[cnt + 1] == 0 && m_bytes[cnt + 2] == 255))
					m_bytes[cnt + 3] = 255;
				cnt += 4;
			}

			LPVOID pNewBytes;
			m_cImage = ::CreateDIBSection(NULL, &m_bif, DIB_RGB_COLORS, &pNewBytes, 0, 0);

			hDC = GetDC(NULL);

			SetDIBits(hDC, m_cImage, 0, m_bif.bmiHeader.biHeight, m_bytes, &m_bif, DIB_RGB_COLORS);

			ReleaseDC(NULL, hDC);


			m_cImageSumnail.Destroy();
			cImage.Destroy();
			delete[] m_bytes;


		}
		else
		{
			SHFILEINFO sfi;
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo(szPathFileName, 0, &sfi, sizeof(SHFILEINFO),
				SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_TYPENAME);

			m_hIcon = sfi.hIcon;

		}
	}
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
	ReBuild();
}

void CScene::ReleaseObjects()
{
}

void CScene::AnimateObjects(float fTimeElapsed)
{

}

void CScene::ReBuild()
{
	vector<wstring> fileList;

	GetFiles(fileList, TEXT(".\\Download\\"), false);
	TCHAR szCurrentDirectory[MAX_PATH];
	TCHAR szDownloadDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH + 1, szCurrentDirectory);

	if (m_vffFileList.size() > 0) m_vffFileList.clear();

	for (size_t sz = 0; sz < fileList.size(); ++sz)
	{
		wsprintf(szDownloadDirectory, TEXT("%s\\Download\\%s"), szCurrentDirectory, fileList[sz].c_str());

		ifstream fs(szDownloadDirectory, ios::binary);
		// size check;
		fs.seekg(0, fstream::end);
		size_t szData = fs.tellg();
		fs.close();
		m_vffFileList.push_back(FileFormat(fileList[sz], CImageBits(szDownloadDirectory), szData));
	}

	m_ulspDownlist = 50;

	RECT rcWorldSize;
	RECT rcLocalSize;
	GetClientRect(m_hWnd, &rcWorldSize);
	rcLocalSize = rcWorldSize;
	rcLocalSize.right -= rcLocalSize.left;
	rcLocalSize.bottom -= rcLocalSize.top;
	rcLocalSize.left = rcLocalSize.top = 0;

	RECT rcValidArea = rcLocalSize;
	rcValidArea.top += m_ulspDownlist;
	rcValidArea.bottom = rcValidArea.top + CLIENT_HEIGHT - m_ulspDownlist * 3;

	for (size_t sz = 0; sz < m_vffFileList.size(); ++sz)
	{
		m_vffFileList[sz].cibFilethumbnail.CreateHBitmap();

		m_vPageList.push_back(make_unique<pSelectFileButton>());
		pSelectFileButton* pCreatedButton = static_cast<pSelectFileButton*>(m_vPageList.back().get());
		pCreatedButton->Initialize(m_pMasterFramework, m_hWnd, true, &m_vffFileList[sz],
			MakeRect<LONG>(10UL, m_ulspDownlist + sz * 55,
				10UL + 350, m_ulspDownlist + 50 + sz * 55)
			);

		pCreatedButton->GetAdvanceInfo(0, &sz);
		pCreatedButton->GetAdvanceInfo(2, &rcValidArea);
	}

	LONG length = CLIENT_HEIGHT - m_ulspDownlist * 3;
	m_Scroll.initialize(scroll::SCR_V, (50 + (m_vffFileList.size() - 1) * 55 - length),
		length, length * 0.95,
		MakePoint<LONG>(CLIENT_WIDTH - 30, m_ulspDownlist * 1.25), 170
		);

	m_hIcon = NULL;
	m_cImage = NULL;
}

void CScene::Draw(HDC hDC)
{
	RECT rcWorldSize;
	RECT rcLocalSize;
	GetClientRect(m_hWnd, &rcWorldSize);
	rcLocalSize = rcWorldSize;
	rcLocalSize.right -= rcLocalSize.left;
	rcLocalSize.bottom -= rcLocalSize.top;
	rcLocalSize.left = rcLocalSize.top = 0;

	RECT rcValidArea = rcLocalSize;
	rcValidArea.left = 10;
	rcValidArea.right = 360;
	rcValidArea.top += m_ulspDownlist;
	rcValidArea.bottom = rcValidArea.top + CLIENT_HEIGHT - m_ulspDownlist * 3;

	RECT rcTitle = SizeDown(rcLocalSize, 2);
	rcTitle.bottom = rcTitle.top + m_ulspDownlist;

	DrawLineBox(hDC, SizeDown(rcLocalSize, 2), RGB(255, 255, 255), RGB(155, 155, 155), 2, DL_FRAME);
	DrawLineBox(hDC, SizeDown(rcTitle, 5), RGB(255, 255, 255), RGB(155, 155, 155), 2, DL_FRAME);

	HFONT mainFont = CreateFont(24, FontSet_Arial_B);
	HFONT origFont = (HFONT)SelectObject(hDC, mainFont);
	SetTextColor(hDC, RGB(55, 55, 55));

	DrawText(hDC, TEXT("Service File List"), -1, &rcTitle, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	SelectObject(hDC, origFont);
	DeleteObject(mainFont);

	m_Scroll.Draw(hDC, rcLocalSize, RGB(0, 0, 0));
	for (auto &p : m_vPageList) { float fLength = m_Scroll.GetNowSrcPosition(); p->GetAdvanceInfo(1, &fLength); p->OnDraw(hDC); }

	SetDCBrushColor(hDC, RGB(55, 55, 55));
	FrameRect(hDC, &rcValidArea, (HBRUSH)GetStockObject(DC_BRUSH));

	HDC hdcIcon = CreateCompatibleDC(hDC);
	if (m_hIcon) DrawIcon(hDC, 10, 550, m_hIcon);
	else if (m_cImage)
	{
		SelectObject(hdcIcon, m_cImage);

		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		::AlphaBlend(hDC, 10, 550, 64, 64, hdcIcon, 0, 0, 64, 64, bf);

	}
//	TCHAR strView[80];
//	CImage cimg;
//	if (m_vffFileList.size() > 0)
//	{
//		for (size_t i = 0; i < m_vffFileList.size(); ++i)
//		{
//			SetBkMode(hDC, TRANSPARENT);
//
//			SelectObject(hdcIcon, m_vffFileList[i].second.GetHBitmap());
//		
//			BLENDFUNCTION bf;
//		
//			bf.BlendOp = AC_SRC_OVER;
//			bf.BlendFlags = 0;
//			bf.SourceConstantAlpha = 0xff;
//			bf.AlphaFormat = AC_SRC_ALPHA;
//			::AlphaBlend(hDC, 10, 10 + i * 35, 32, 32, hdcIcon, 0, 0, 32, 32, bf);
//
//			lstrcpy(strView, m_vffFileList[i].first.c_str());
//			PathCompactPath(hDC, strView, 250);
//			TextOut(hDC, 50, 10 + i * 35, strView, lstrlen(strView));
//		}
//	}
//	cimg.Destroy();
	DeleteDC(hdcIcon);
}

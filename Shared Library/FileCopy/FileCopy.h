#pragma once

#include <windows.h>
#include <atlImage.h>
#include <string>
#include <vector>
#include <shellapi.h>

#define BUFSIZE		16384

//#define DO_NOT_TRANSPORT_DATA
//#define DO_NOT_USE_CRITICAL_SECTION

#define EXIT_CLIENT (UINT)0x0001
#define FILE_SELECT (UINT)0x0002
#define DOWN_CANCEL (UINT)0x0003
#define FILE_UPLOAD (UINT)0x0004

typedef struct Packet
{
	size_t	_size;
	char	_str[BUFSIZE];

	Packet(char* p, size_t size = 0) { _size = size; memcpy(_str, p, BUFSIZE); }
	Packet(size_t size = 0) { _size = size; ZeroMemory(_str, BUFSIZE); }

}Packet;

class CImageBits {
private:
	LPBYTE		m_bytes;
	DWORD		m_dwImageSize;
	BITMAPINFO	m_bif;
	HBITMAP		m_hBitmap;
	
	inline void CreateFromCImage(const CImage& Cimg)
	{
		m_hBitmap = NULL;
		m_bytes = NULL;
		ZeroMemory(&m_bif, sizeof(BITMAPINFO));

		// 비트맵(DDB) 정보 얻기
		BITMAP bmp;
		GetObject(Cimg, sizeof(BITMAP), &bmp);

		// 비트맵(DIB) 정보 설정
		BITMAPINFOHEADER& bmih = m_bif.bmiHeader;
		ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = bmp.bmWidth;    // 가로
		bmih.biHeight = bmp.bmHeight;  // 세로
		bmih.biPlanes = 1;
		bmih.biBitCount = Cimg.GetBPP();          // 픽셀당 비트수(BPP)
		bmih.biCompression = BI_RGB;

		// 비트맵(DIB) 데이터 추출
		// 데이터의 크기를 알아낸다
		HDC hDC = GetDC(NULL);
		GetDIBits(hDC, Cimg, 0, bmp.bmHeight, NULL,
			(LPBITMAPINFO)&bmih, DIB_RGB_COLORS);

		m_dwImageSize = bmih.biSizeImage;

		// 데이터 저장 공간 확보
		m_bytes = new BYTE[m_dwImageSize];
		GetDIBits(hDC, Cimg, 0, bmp.bmHeight, m_bytes,
			(LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
		ReleaseDC(NULL, hDC);
	}
	
public:

	static LPVOID GetHBitmapBits(const CImage& Cimg)
	{
		// 비트맵(DDB) 정보 얻기
		BITMAP bmp;
		GetObject(Cimg, sizeof(BITMAP), &bmp);

		// 비트맵(DIB) 정보 설정
		BITMAPINFOHEADER bmih;
		ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = bmp.bmWidth;    // 가로
		bmih.biHeight = bmp.bmHeight;  // 세로
		bmih.biPlanes = 1;
		bmih.biBitCount = Cimg.GetBPP();          // 픽셀당 비트수(BPP)
		bmih.biCompression = BI_RGB;

		// 비트맵(DIB) 데이터 추출
		// 데이터의 크기를 알아낸다
		HDC hDC = GetDC(NULL);
		GetDIBits(hDC, Cimg, 0, bmp.bmHeight, NULL,
			(LPBITMAPINFO)&bmih, DIB_RGB_COLORS);

		// 데이터 저장 공간 확보
		LPVOID lpDIBits = new BYTE[bmih.biSizeImage];
		GetDIBits(hDC, Cimg, 0, bmp.bmHeight, lpDIBits,
			(LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
		ReleaseDC(NULL, hDC);

		return lpDIBits;
	}

	static DWORD GetBitmapSize(const CImage& Cimg)
	{
		// 비트맵(DDB) 정보 얻기
		BITMAP bmp;
		GetObject(Cimg, sizeof(BITMAP), &bmp);

		// 비트맵(DIB) 정보 설정
		BITMAPINFOHEADER bmih;
		ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
		bmih.biSize = sizeof(BITMAPINFOHEADER);
		bmih.biWidth = bmp.bmWidth;			// 가로
		bmih.biHeight = bmp.bmHeight;		// 세로
		bmih.biPlanes = 1;
		bmih.biBitCount = Cimg.GetBPP();	// 픽셀당 비트수(BPP)
		bmih.biCompression = BI_RGB;

		// 비트맵(DIB) 데이터 추출
		// 데이터의 크기를 알아낸다
		HDC hDC = GetDC(NULL);
		GetDIBits(hDC, Cimg, 0, bmp.bmHeight, NULL,
			(LPBITMAPINFO)&bmih, DIB_RGB_COLORS);
		ReleaseDC(NULL, hDC);

		return bmih.biSizeImage;
	}

	static HBITMAP BuildHBitmapFromBits(const LPVOID pBytes, const BITMAPINFO& bif)
	{
		if (!pBytes) return NULL;

		LPVOID pNewBytes = NULL;
		HBITMAP hbit = ::CreateDIBSection(NULL, &bif, DIB_RGB_COLORS, &pNewBytes, 0, 0);

		HDC hDC = GetDC(NULL);

		SetDIBits(hDC, hbit, 0, bif.bmiHeader.biHeight, pBytes,
			&bif, DIB_RGB_COLORS);

		ReleaseDC(NULL, hDC);

		return hbit;
	}
	
public:

	CImageBits(const CImage& Cimg) { CreateFromCImage(Cimg); }

	CImageBits(const LPCWSTR szFilePath)
	{
		std::wstring strExtension(PathFindExtension(szFilePath));
		if (   strExtension == TEXT(".png")
			|| strExtension == TEXT(".jpg")
			|| strExtension == TEXT(".bmp")
			|| strExtension == TEXT(".gif")
			|| strExtension == TEXT(".jpeg")
			|| strExtension == TEXT(".PNG")
			|| strExtension == TEXT(".JPG")
			|| strExtension == TEXT(".JPEG")
			|| strExtension == TEXT(".BMP")
			|| strExtension == TEXT(".GIF")
			)
		{
			//	Bitmap으로 빼기
			CImage imgSumnail;
			imgSumnail.Load(szFilePath);
			
			float height = (float)imgSumnail.GetHeight();
			float width = (float)imgSumnail.GetWidth();
			bool bStand = false;
			int space = 0;
			if (height > width) { bStand = true; width /= height; height = 1; }
			else if (height < width) { bStand = false; height /= width; width = 1; }
			else { height = width = 1; }

			height	*=	32.f;
			width	*=	32.f;

			space = 16 - (bStand ? width : height) * 0.5f;


			CImage img;
			img.Create(32, 32, 32, CImage::createAlphaChannel);
			
			HDC hdc = img.GetDC();
			RECT rc = { 0,0,32,32 };
			SetDCBrushColor(hdc, RGB(255, 0, 255));
			FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
			if (bStand)	imgSumnail.TransparentBlt(hdc, int(space), 0, int(width), int(height), RGB(0, 0, 0));
			else		imgSumnail.TransparentBlt(hdc, 0, int(space), int(width), int(height), RGB(0, 0, 0));
			img.ReleaseDC();

			///////////////////////////////////////////////////////
			CreateFromCImage(img);
			size_t cnt = 0;
			for (int y = 0; y < m_bif.bmiHeader.biHeight; y++)  for (int x = 0; x < m_bif.bmiHeader.biWidth; x++)
			{
				if (!(m_bytes[cnt] == 255 && m_bytes[cnt + 1] == 0 && m_bytes[cnt + 2] == 255))
					m_bytes[cnt + 3] = 255;
				cnt += 4;
			}

			imgSumnail.Destroy();
			img.Destroy();
		}
		else
		{
			SHFILEINFO sfi;
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo(szFilePath, 0, &sfi, sizeof(SHFILEINFO),
				SHGFI_USEFILEATTRIBUTES | SHGFI_ICON | SHGFI_SYSICONINDEX | SHGFI_TYPENAME);

			//	Bitmap으로 빼기
			CImage img;
			img.Create(32, 32, 32, CImage::createAlphaChannel);

			HDC hdc = img.GetDC();
			DrawIcon(hdc, 0, 0, sfi.hIcon);
			img.ReleaseDC();

			///////////////////////////////////////////////////////
			CreateFromCImage(img);

			DestroyIcon(sfi.hIcon);
			img.Destroy();
		}

	}

	inline void BuildExistResource(DWORD szFile, BITMAPINFO bif, LPVOID bytes)
	{
		m_bytes = NULL;
		m_dwImageSize = szFile;
		m_bif = bif;
		m_bytes = (BYTE*)bytes;

		CreateHBitmap();
	}

	CImageBits(DWORD szFile, BITMAPINFO bif, LPVOID bytes)
	{
		BuildExistResource(szFile,bif,bytes);
	}

	CImageBits()
	{
		m_bytes = NULL;
		m_hBitmap = NULL;
	}

	CImageBits(CImageBits&& other)
	{
		m_dwImageSize = other.m_dwImageSize;
		m_bif = other.m_bif;
		m_bytes = other.m_bytes;
		m_hBitmap = other.m_hBitmap;
		other.m_bytes = nullptr;
		other.m_hBitmap = nullptr;
	}

	CImageBits(const CImageBits& other)
	{
		m_hBitmap = nullptr;
		m_dwImageSize = other.m_dwImageSize;
		m_bif = other.m_bif;
		m_bytes = new BYTE[m_dwImageSize];
		memcpy(m_bytes, other.m_bytes, m_dwImageSize);
	}

	HBITMAP BuildHBitmapFromBits()
	{
		return BuildHBitmapFromBits(m_bytes, m_bif);
	}

	~CImageBits()
	{
		if (m_bytes) { delete[] m_bytes; m_bytes = NULL; }
		if (m_hBitmap) { ::DeleteObject(m_hBitmap); m_hBitmap = NULL; }
	}

	void CreateHBitmap() { if (m_hBitmap) DeleteObject(m_hBitmap); m_hBitmap = BuildHBitmapFromBits(); }

	DWORD GetSize() const { return m_dwImageSize; }
	LPVOID GetFileBytes() { return m_bytes; }
	BITMAPINFO GetBIF() { return m_bif; }
	HBITMAP GetHBitmap() { return m_hBitmap; }
};

/*
* 서버 - 클라이언트 간 통신 구조체
* 출력될 첫 번째 위치를 인자로 가진다.
* SOCKET 구조체도 가진다.
*/
typedef struct ThreadStructToFileCopy {

	int m_iStartY;
	SOCKET sock;
	ThreadStructToFileCopy(int y, SOCKET &s) :
		m_iStartY(y), sock(s) 
	{}

}Trd_FC;

int gotoxy(int x, int y);
int wherex();
int wherey();
int bufferMaxX();
int bufferMaxY();

#ifdef UNICODE
std::wstring FormatData(size_t data);
std::wstring FormatTime(double times);

template<typename NumType>
std::wstring FixValue(NumType fixnum)
{
	std::wstring strfix;
	int count = 0;

	while (fixnum > 0)
	{
		strfix += (TEXT('0') + (fixnum % 10));
		fixnum /= 10;
		if (fixnum > 0 && ++count % 3 == 0)
			strfix += TEXT(',');
	}

	if (count == 0)
		return std::wstring(TEXT("0"));
	return std::wstring(strfix.rbegin(), strfix.rend());
}
#else
std::string FormatData(size_t data);
std::string FormatTime(double times);

template<typename NumType>
std::string FixValue(NumType fixnum)
{
	std::string strfix;
	int count = 0;

	while (fixnum > 0)
	{
		strfix += ('0' + (fixnum % 10));
		fixnum /= 10;
		if (fixnum > 0 && ++count % 3 == 0)
			strfix += ',';
	}

	if (count == 0)
		return std::string("0");
	return std::string(strfix.rbegin(), strfix.rend());
}
#endif

#ifdef _UNICODE

// 소켓 함수 오류 출력 후 종료
inline void err_quit(LPWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCWSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
inline void err_displayBox(LPWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPWSTR)lpMsgBuf, msg, MB_OK);
	LocalFree(lpMsgBuf);
}

#else

// 소켓 함수 오류 출력 후 종료
inline void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
inline void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

#endif

#ifndef UNICODE
#ifndef DO_NOT_USE_CRITICAL_SECTION
inline void ErrorProcess(LPSTR str, int &y, CRITICAL_SECTION &cs)
#else
inline void ErrorProcess(LPSTR str, int &y)
#endif
{
	if(y > 0) gotoxy(0, y);
#ifndef DO_NOT_USE_CRITICAL_SECTION
	EnterCriticalSection(&cs);
	err_display(str);
	LeaveCriticalSection(&cs);
#else
	err_display(str);
#endif
	if (y > 0) y = wherey();
}
#endif

inline void ErrorProcess(LPWSTR str)
{
	err_displayBox(str);
}


void GetFiles(std::vector<std::wstring> &vList, std::wstring sPath, bool bAllDirectories);
#pragma once

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include "FileCopy.h"
#include <cmath>

#define SERVERPORT 9000
#define WM_CREATEWINDOW		(UINT)0x401
#define WM_UPDATEMESSAGE	(UINT)0x402

#define FAILUPLOAD		(WPARAM)0x0001
#define REBUILDSCENE	(WPARAM)0x0002

typedef struct ThreadStructToFileCopy_GUI {

	HWND m_hWnd;
	SOCKET sock;
	ThreadStructToFileCopy_GUI(HWND hWnd, SOCKET &s) :
		m_hWnd(hWnd), sock(s)
	{}

}GUI_Trd_FC;

typedef struct FileFormat {
	wstring		strFileName;
	CImageBits	cibFilethumbnail;
	size_t		szFileSize;
	FileFormat(wstring str, CImageBits cib, size_t size) :
		strFileName(str), cibFilethumbnail(cib), szFileSize(size)
	{

	}
}FileFormat;

class DataInfo {
private:
	TCHAR				m_szfileName[260];
	
	size_t				m_szFile;
	size_t				m_szGetData;
	size_t				m_szGetPacketNumperTime;
	
	double				m_szFile_div100;
	double				m_dLosstime;
	double				m_per_1secDownData;
	double				nowper;

	HWND				m_hWnd;
	FileFormat			*m_pFileFormat;
	POINT				m_ptDrag;
	POINT				m_ptOld;
	POINT				m_ptNow;
	bool				m_bMove;
	bool				m_bDragFlag;
	chrono::system_clock::time_point	m_cStarterTime;
	chrono::system_clock::time_point	m_nCurrentTime;
	chrono::duration<double>			m_cftimeElapsed;

public:
	
	DataInfo()
	{
		m_ptDrag = m_ptOld = m_ptNow = MakePoint(0, 0);
		m_szFile_div100 = 0;
		m_dLosstime = 0;
		m_per_1secDownData = 0;
		nowper = 0;
		m_szFile = 0;
		m_szGetData = 0;
		m_szGetPacketNumperTime = 0;

		ZeroMemory(m_szfileName, sizeof(m_szfileName));
		m_hWnd = NULL;
		m_bDragFlag = false;
		m_pFileFormat = nullptr;
	}
	explicit DataInfo(HWND hWnd, LPWSTR str)
	{
		m_ptDrag = m_ptOld = m_ptNow = MakePoint(0, 0);
		m_szFile_div100 = 0;
		m_dLosstime = 0;
		m_per_1secDownData = 0;
		nowper = 0;
		m_szFile = 0;
		m_szGetData = 0;
		m_szGetPacketNumperTime = 0;

		m_bDragFlag = false;
		m_pFileFormat = nullptr;

		lstrcpy(m_szfileName, str);
		// TODO MORE
	}
	~DataInfo()
	{
		if (m_pFileFormat) delete m_pFileFormat;
	}

	HWND GetHwnd() const { return m_hWnd; }
	void SetHwnd(HWND hWnd) { if(!m_hWnd) m_hWnd = hWnd; }

	void SetFileName(LPCWSTR str) { lstrcpy(m_szfileName, str); }
	LPCWSTR GetFileName() const { return m_szfileName; }

	void SetFileSize(size_t size) 
	{
		m_szFile = size; 
		m_szFile_div100 = m_szFile / 100.0;
		m_dLosstime = 0;
		m_per_1secDownData = 0;
		nowper = 0;
		m_szGetData = 0;
		m_szGetPacketNumperTime = 0;
		m_per_1secDownData = 0;
		StartTimer();
	}
	void AddSize(size_t size) { m_szGetPacketNumperTime += size; count(); }
	void SetCIB(LPCWSTR path) 
	{
		m_pFileFormat = new FileFormat(m_szfileName, CImageBits(path), m_szFile);
		m_pFileFormat->cibFilethumbnail.CreateHBitmap();
	}

	void SetCIB(CImageBits&& cib)
	{
		m_pFileFormat = new FileFormat(m_szfileName, move(cib), m_szFile);
		m_pFileFormat->cibFilethumbnail.CreateHBitmap();
	}

	FileFormat* GetCIB() { return m_pFileFormat; }
	size_t GetFileSize() const { return m_szFile; }
	void StartTimer() { m_cStarterTime = m_nCurrentTime = chrono::system_clock::now(); }
	void count() 
	{
		m_cftimeElapsed = chrono::system_clock::now() - m_nCurrentTime;

		if (m_cftimeElapsed.count() > 0.3333)
		{
			Renewal();
			m_nCurrentTime = chrono::system_clock::now();
		}
	}
	void Renewal()
	{
		// 초당 데이터
		m_per_1secDownData = m_szGetPacketNumperTime / m_cftimeElapsed.count();
		m_per_1secDownData = round(m_per_1secDownData);
		m_szGetData += m_szGetPacketNumperTime;
		nowper = m_szGetData / m_szFile_div100;
		m_szGetPacketNumperTime = 0;
		//	남은 시간
		m_dLosstime = double(m_szFile - m_szGetData) / m_per_1secDownData;
		InvalidateRect(m_hWnd, NULL, false);
		if (nowper >= 99.9999) nowper = 100.0;
	}
	bool IsDownloadEnd() { return (nowper >= 99.9999); }

public:

	void StartDrag(LPARAM lParam)
	{

		ReleaseCapture();
		SetCapture(m_hWnd);

		GetCursorPos(&m_ptNow);

		m_ptDrag = m_ptNow;
		m_ptOld = m_ptNow;
		m_bDragFlag = true;

		m_ptNow.x = LOWORD(lParam);
		m_ptNow.y = HIWORD(lParam);

		m_ptOld = m_ptOld - m_ptNow;
		m_bMove = false;

	}

	void Drag(LPARAM lParam)
	{
		if (m_bDragFlag)
		{
			GetCursorPos(&m_ptNow);
			m_ptOld.x = m_ptOld.x + m_ptNow.x - m_ptDrag.x;
			m_ptOld.y = m_ptOld.y + m_ptNow.y - m_ptDrag.y;

			SetWindowPos(m_hWnd, 0, m_ptOld.x, m_ptOld.y,
				0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOZORDER);

			m_ptDrag = m_ptNow;

			UpdateWindow(m_hWnd);
			m_bMove = true;
		}
	}

	bool ReleaseDrag(LPARAM lParam)
	{
		m_bDragFlag = false;
		ReleaseCapture();
		return m_bMove;
	}

	void Draw(HDC hDC);

};

extern DWORD WINAPI ServerMain(LPVOID arg);
extern DWORD WINAPI ProcessClient(LPVOID arg);
extern LRESULT CALLBACK DownloadProc(HWND hMsg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void SendToServerMsg(MSG msg);
extern DataInfo* GetWndInfo(HWND hWnd);
extern DataInfo& RegistWndInfo(HWND hWnd);

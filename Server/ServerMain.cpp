#include "stdafx.h"
#include <windows.h>
#include <cstdio>
#include <io.h>

#include "ServerMain.h"
#include <winsock2.h>
#include "FileCopy.h"
using namespace std;

namespace {
	CRITICAL_SECTION cs;

	map<HWND, DataInfo> DownloadData;
	deque<MSG> ServerMessage;
	
	// 사용자 정의 데이터 수신 함수
	int recvn(SOCKET s, char *buf, int len, int flags)
	{
		int received;
		char *ptr = buf;
		int left = len;

		while (left > 0) {
			received = recv(s, ptr, left, flags);
			if (received == SOCKET_ERROR)
				return SOCKET_ERROR;
			else if (received == 0)
				break;
			left -= received;
			ptr += received;
		}

		return (len - left);
	}

};

void SendToServerMsg(MSG msg)
{

	EnterCriticalSection(&cs);
	ServerMessage.push_back(msg);
	LeaveCriticalSection(&cs);
}

DataInfo* GetWndInfo(HWND hWnd)
{
	return &DownloadData[hWnd];
}

DataInfo& RegistWndInfo(HWND hWnd)
{
	return DownloadData[hWnd];
}

LRESULT CALLBACK DownloadProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//	접근 방법 
//	DownloadData[hWnd];
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg)
	{

	case WM_CREATE:
		break;

	case WM_LBUTTONDOWN:

		EnterCriticalSection(&cs);
		DownloadData[hWnd].StartDrag(lParam);
		LeaveCriticalSection(&cs);
		
		break;

	case WM_MOUSEMOVE:
		
		EnterCriticalSection(&cs);
		DownloadData[hWnd].Drag(lParam);
		LeaveCriticalSection(&cs);

		break;

	case WM_LBUTTONUP:

		EnterCriticalSection(&cs);
		if (DownloadData[hWnd].ReleaseDrag(lParam)) LeaveCriticalSection(&cs);
		else if (DownloadData[hWnd].IsDownloadEnd())
		{
			LeaveCriticalSection(&cs);

			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			POINT ptCenter;
			ptCenter.x = (rcClient.left + rcClient.right) / 2;
			ptCenter.y = (rcClient.top + rcClient.bottom) / 2;
			LusMsg Msg;
			Msg.hWnd = hWnd;
			Msg.Title = L"종료";
			Msg.Msg = L"\n보고서를 종료합니다.";
			Msg.MsgStartPos = ptCenter;
			Msg.InBoxMsg_1 = L"확인";
			Msg.InBoxMsg_2 = L"취소";
			Msg.bUseMsg_2 = true;
			Msg.thisClass = hWnd;
			Msg.Msg_1_Func = [](void* Class) {
				close_LusMsgProc();
				DestroyWindow((HWND)Class);
				//	static_cast<CScene*>(Class);
			};
			Msg.Msg_2_Func = [](void* Class) {
				close_LusMsgProc();
				//	static_cast<CScene*>(Class);
			};

			LusMsgBox(Msg.hWnd, Msg, hInst, 175);


		}
		else LeaveCriticalSection(&cs);

		break;

	case WM_KEYDOWN:

		break;

	case WM_GETVALUE:
		switch (wParam)
		{
		case 0:

			break;
		}
		break;

	case WM_PAINT:
		
		hdc = BeginPaint(hWnd, &ps);
		{
			RECT rcSize;
			GetClientRect(hWnd, &rcSize);
			HDC LayDC = CreateCompatibleDC(hdc);
			HBITMAP Lay = CreateCompatibleBitmap(hdc, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top);
			SelectObject(LayDC, Lay);

			EnterCriticalSection(&cs);
			DownloadData[hWnd].Draw(LayDC);
			LeaveCriticalSection(&cs);

			BitBlt(hdc, 0, 0, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top, LayDC, 0, 0, SRCCOPY);

			DeleteObject(Lay);
			DeleteDC(LayDC);
		}
		EndPaint(hWnd, &ps);

		break;

	case WM_DESTROY:
		
		EnterCriticalSection(&cs);
		DownloadData.erase(hWnd);
		LeaveCriticalSection(&cs);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
{

	// 임계 영역 생성
#ifndef DO_NOT_USE_CRITICAL_SECTION
	InitializeCriticalSection(&cs);
#endif
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit(TEXT("socket()"));

	// bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit(TEXT("bind()"));

	// listen()
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit(TEXT("listen()"));

	// 데이터 통신에 사용할 변수
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	int len;

	list<HANDLE> lhTrd;
	HANDLE hThread;
	while (1)
	{
		// accept()
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR *)&clientaddr, &addrlen);
		Trd_FC trd_fc(0, client_sock);

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)&trd_fc, 0, NULL);

		if (hThread == NULL) { closesocket(client_sock); }
		else
		{
			//		lhTrd.push_back(hThread);
			CloseHandle(hThread);
		}
	}

	// closesocket()
	closesocket(listen_sock);

	// 윈속 종료
	WSACleanup();

#ifndef DO_NOT_USE_CRITICAL_SECTION
	// 임계 영역 해제
	DeleteCriticalSection(&cs);
#endif

	return 0;
}

void RenewalFileList(SOCKET& client_sock)
{
	int retval = 0;
	size_t len;
	vector<wstring> fileList;
	GetFiles(fileList, TEXT(".\\Download\\"), false);
	TCHAR szCurrentDirectory[MAX_PATH];
	TCHAR szDownloadDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH + 1, szCurrentDirectory);

	TCHAR fileName[80];

	len = fileList.size();
	//	리스트 수 보내기
	retval = send(client_sock, (char *)&len, sizeof(size_t), 0);

	for (size_t i = 0; i < fileList.size(); ++i)
	{
		len = sizeof(TCHAR) * 80;

		// 데이터 보내기(고정 길이)
		retval = send(client_sock, (char *)&len, sizeof(size_t), 0);
		lstrcpy(fileName, fileList[i].c_str());
		// 1. 리스트 보내기
		retval = send(client_sock, reinterpret_cast<char*>(fileName), len, 0);

		wsprintf(szDownloadDirectory, TEXT("%s/Download/%s"), szCurrentDirectory, fileName);
		CImageBits cib(szDownloadDirectory);
		len = cib.GetSize();
		retval = send(client_sock, (char *)&len, sizeof(DWORD), 0);
		retval = send(client_sock, reinterpret_cast<char*>(&cib.GetBIF()), sizeof(BITMAPINFO), 0);
		retval = send(client_sock, reinterpret_cast<char*>(cib.GetFileBytes()), len, 0);
		ifstream fs(szDownloadDirectory, ios::binary);
		// size check;
		fs.seekg(0, fstream::end);
		size_t szData = fs.tellg();
		fs.close();
		retval = send(client_sock, (char *)&szData, sizeof(size_t), 0);
	}
	SendMessage(ghWndMain, WM_UPDATEMESSAGE, REBUILDSCENE, 0);
}

DWORD WINAPI ProcessClient(LPVOID arg)
{
	GUI_Trd_FC fc = (*reinterpret_cast<GUI_Trd_FC*>(arg));

	SOCKET client_sock = (fc.sock);
	SOCKADDR_IN clientaddr;

	int retval;
	// get client info
	int addrlen = sizeof(clientaddr);
	getpeername(client_sock, (SOCKADDR *)&clientaddr, &addrlen);

	size_t len;

	// 보낼 데이터
	fstream		SendData;
	bool		bOpenSucess = false;

	// 패킷을 받을 임시 변수
	Packet GetPacket;

	size_t	szDataBlock = 0;
	size_t	szGetPacketNum = 0;
	size_t	szGetPacketNumperTime = 0;

	// 패킷 받은 횟수
	size_t nRecv = 0;
	size_t szData = 0;

	// draw percent
	int nowper = 0;
	size_t szDataBlock_div_100 =0;
	// file name
	TCHAR strFilename[92];
	// 시간 측정
	chrono::system_clock::time_point	m_cStarterTime;
	chrono::system_clock::time_point	m_nCurrentTime;
	chrono::duration<double>			m_cftimeElapsed;

	RenewalFileList(client_sock);

	//	GetSelectedFileInfo
	while(1)
	{

		UINT ServerState = 0;
		retval = recvn(client_sock, reinterpret_cast<char*>(&ServerState), sizeof(UINT), 0);
		switch (ServerState)
		{
		case EXIT_CLIENT: 
			// closesocket()
			closesocket(client_sock);

			return 0;
			break;
		case FILE_UPLOAD: 
		{
			szGetPacketNum = 0;
			retval = recvn(client_sock, reinterpret_cast<char*>(&len), sizeof(size_t), 0);
			if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); closesocket(client_sock); return 0; }
			else if (retval == 0) { closesocket(client_sock); return 0; }
			retval = recvn(client_sock, reinterpret_cast<char*>(strFilename), len, 0);
			if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); closesocket(client_sock); return 0; }
			else if (retval == 0) { closesocket(client_sock); return 0; }

			len /= sizeof(TCHAR);
			strFilename[len] = 0;

			wstring strLink = TEXT("Download\\") + wstring(strFilename);
			DWORD szImgLength = 0;
			BITMAPINFO bif;
			BYTE* szImgBits;

			retval = recvn(client_sock, (char*)&szImgLength, sizeof(DWORD), 0);
			retval = recvn(client_sock, (char*)&bif, sizeof(BITMAPINFO), 0);
			szImgBits = new BYTE[szImgLength];
			retval = recvn(client_sock, (char*)szImgBits, szImgLength, 0);
			retval = recvn(client_sock, (char*)&szDataBlock, sizeof(size_t), 0);
			CImageBits cib(szImgLength, bif, szImgBits);

			HWND hWnd = nullptr;
			SendMessage(ghWndMain, WM_CREATEWINDOW, 0, (LPARAM)strFilename);
			while (!hWnd)
			{
				EnterCriticalSection(&cs);
				if (ServerMessage.size() > 0)
				{
					if (lstrcmp((LPWSTR)ServerMessage.front().lParam, strFilename) == 0)
					{
						hWnd = ServerMessage.front().hwnd;
						ServerMessage.pop_front();

						lstrcpy(strFilename, strLink.c_str());

						SendData.open(strLink, ios::binary | ios::out);

						DownloadData[hWnd].SetFileSize(szDataBlock);
						DownloadData[hWnd].SetCIB(move(cib));
					}
				}
				LeaveCriticalSection(&cs);
			}

			// size check;

			InvalidateRect(ghWndMain, NULL, false);
			InvalidateRect(hWnd, NULL, false);

			szGetPacketNum = 0;
			// 클라이언트와 데이터 통신
			while (szGetPacketNum < szDataBlock)
			{
				// 데이터 받기(고정 길이)
				retval = recvn(client_sock, (char *)&len, sizeof(size_t), 0);
				if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); break; }
				else if (retval == 0) break;

				retval = recvn(client_sock, reinterpret_cast<char*>(&GetPacket), len, 0);
				if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); break; }
				else if (retval == 0)break;

				szGetPacketNum += GetPacket._size;

				// Download main procedure
				SendData.write(GetPacket._str, GetPacket._size);

				EnterCriticalSection(&cs);
				DownloadData[hWnd].AddSize(GetPacket._size);
				LeaveCriticalSection(&cs);

			}	//end while

			EnterCriticalSection(&cs);
			DownloadData[hWnd].Renewal();
			LeaveCriticalSection(&cs);
			InvalidateRect(hWnd, NULL, false);

			if (szGetPacketNum < szDataBlock)
			{
				SendMessage(ghWndMain, WM_UPDATEMESSAGE, FAILUPLOAD, (LPARAM)hWnd);
				SendData.close();
				_wremove(strLink.c_str());
				break;
			}

			if(SendData.is_open()) SendData.close();
			RenewalFileList(client_sock);

		}	break;
		case FILE_SELECT:
		{
			retval = recvn(client_sock, reinterpret_cast<char*>(&len), sizeof(size_t), 0);
			if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); closesocket(client_sock); return 0; }
			else if (retval == 0) { closesocket(client_sock); return 0; }
			retval = recvn(client_sock, reinterpret_cast<char*>(strFilename), len, 0);
			if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); closesocket(client_sock); return 0; }
			else if (retval == 0) { closesocket(client_sock); return 0; }

			len /= sizeof(TCHAR);
			strFilename[len] = 0;

			HWND hWnd = nullptr;
			SendMessage(ghWndMain, WM_CREATEWINDOW, 0, (LPARAM)strFilename);
			while (!hWnd)
			{
				EnterCriticalSection(&cs);
				if (ServerMessage.size() > 0)
				{
					if (lstrcmp((LPWSTR)ServerMessage.front().lParam, strFilename) == 0)
					{
						hWnd = ServerMessage.front().hwnd;
						ServerMessage.pop_front();

						wstring strLink = TEXT("Download\\") + wstring(strFilename);
						lstrcpy(strFilename, strLink.c_str());

						SendData.open(strFilename, ios::binary | ios::in);
						SendData.seekg(0, fstream::end);
						szData = SendData.tellg();
						SendData.seekg(0);

						DownloadData[hWnd].SetFileSize(szData);
						DownloadData[hWnd].SetCIB(strFilename);
					}
				}
				LeaveCriticalSection(&cs);
			}

			// size check;

			InvalidateRect(ghWndMain, NULL, false);
			InvalidateRect(hWnd, NULL, false);

			// 클라이언트와 데이터 통신

			len = sizeof(Packet);
			size_t szNowSendData = 0;
			while (szNowSendData < szData)
			{
				Packet pStr(min(szData - szNowSendData, BUFSIZE));
				SendData.read(pStr._str, pStr._size);

				szNowSendData += pStr._size;

				retval = send(client_sock, (char *)&len, sizeof(size_t), 0);
				if (retval == SOCKET_ERROR) { cout << endl; err_displayBox(TEXT("send()")); break; }
				retval = send(client_sock, reinterpret_cast<char*>(&pStr), len, 0);
				if (retval == SOCKET_ERROR) { cout << endl; err_displayBox(TEXT("send()")); break; }

				EnterCriticalSection(&cs);
				DownloadData[hWnd].AddSize(pStr._size);
				LeaveCriticalSection(&cs);
				InvalidateRect(hWnd, NULL, false);

			}

			EnterCriticalSection(&cs);
			DownloadData[hWnd].Renewal();
			LeaveCriticalSection(&cs);
			InvalidateRect(hWnd, NULL, false);

			SendData.close();
			RenewalFileList(client_sock);
		}
			break;

		}

	} //end while
	
	// closesocket()
	closesocket(client_sock);

	return 0;
}

void DataInfo::Draw(HDC hDC)
{

	RECT rcWorldSize;
	RECT rcLocalSize;
	GetClientRect(m_hWnd, &rcWorldSize);
	rcLocalSize = rcWorldSize;
	rcLocalSize.right -= rcLocalSize.left;
	rcLocalSize.bottom -= rcLocalSize.top;
	rcLocalSize.left = rcLocalSize.top = 0;

	SetBkMode(hDC, TRANSPARENT);
	DrawLineBox(hDC, SizeDown(rcLocalSize, 5), RGB(255, 255, 255), RGB(55, 55, 55), 2, DL_FRAME);

	if (!m_pFileFormat) return;

	HFONT mainFont = CreateFont(24, FontSet_Arial_B);
	HFONT origFont = (HFONT)SelectObject(hDC, mainFont);

	//	LineTo(hDC, rcPageWorldSize.left, 10);
	//	MoveToEx(hDC, rcPageWorldSize.left, rcPageWorldSize.bottom - 10, NULL);
	
	POINT ptIconCenter;
	ptIconCenter.x = 25;
	ptIconCenter.y = 35;
	RECT rcFileName = rcWorldSize;
	rcFileName.left += 50; rcFileName.right -= 100;
	rcFileName.top += 10; rcFileName.bottom = rcFileName.top + 50;

	//	Draw Caption
	{
		HDC hDCIcon = CreateCompatibleDC(hDC);
		SelectObject(hDCIcon, m_pFileFormat->cibFilethumbnail.GetHBitmap());

		BLENDFUNCTION bf;

		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 0xff;
		bf.AlphaFormat = AC_SRC_ALPHA;
		::AlphaBlend(hDC, ptIconCenter.x - 16, ptIconCenter.y - 16, 32, 32, hDCIcon, 0, 0, 32, 32, bf);

		DeleteDC(hDCIcon);
		COLORREF oriColor = SetTextColor(hDC, RGB(55, 55, 55));
		DrawText(hDC, m_pFileFormat->strFileName.c_str(), -1, &rcFileName, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		SetTextColor(hDC, oriColor);
	}

	//	퍼센테이지 표시
	{

		/*
		퍼센테이지 바는 400 px.
		퍼센테이지 바의 옆에 사이즈 바에 현재 진행률 표시
		퍼센테이지 밑에 전송률과 남은 시간 표시
		*/
		DeleteObject(mainFont);
		mainFont = CreateFont(14, FontSet_Arial_B);
		SelectObject(hDC, mainFont);

		LONG percentageLength = 400;
		RECT rcPercentage = rcWorldSize;
		rcPercentage.left += 50; rcPercentage.right = rcPercentage.left + percentageLength;
		rcPercentage.top += 100; rcPercentage.bottom = rcPercentage.top + 25;

		RECT rcNowPercentage = rcPercentage;
		rcNowPercentage.right = rcPercentage.left + max(0, percentageLength * nowper) / 100;

		rcNowPercentage = SizeDown(rcNowPercentage, 2);

		COLORREF oriColor = SetTextColor(hDC, RGB(55, 55, 55));
		FrameRect(hDC, &rcPercentage, (HBRUSH)GetStockObject(BLACK_BRUSH));
		if (rcNowPercentage.right > rcNowPercentage.left)
			FillRect(hDC, &rcNowPercentage, (HBRUSH)GetStockObject(BLACK_BRUSH));

		wstring strMessage;

		strMessage = to_wstring(int(nowper)) + TEXT(" ％");
		RECT rcViewPercent = rcPercentage;
		rcViewPercent.left = rcViewPercent.right + 10;
		rcViewPercent.right = rcViewPercent.left + 50;
		DrawText(hDC, strMessage.c_str(), -1, &rcViewPercent, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		RECT rcViewByteperSec = rcPercentage;
		rcViewByteperSec.top = rcViewByteperSec.bottom + 10;
		rcViewByteperSec.bottom = rcViewByteperSec.top + 50;
		strMessage = TEXT("현재 전송률 : ") + FormatData(static_cast<size_t>(floor(m_per_1secDownData))) + TEXT(" / sec");
		DrawText(hDC, strMessage.c_str(), -1, &rcViewByteperSec, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
		rcViewByteperSec.left = rcViewByteperSec.right - 50;
		rcViewByteperSec.right = rcViewByteperSec.right + 50;
		strMessage = TEXT("남은 시간 : ") + FormatTime(m_dLosstime);
		DrawText(hDC, strMessage.c_str(), -1, &rcViewByteperSec, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		SetTextColor(hDC, oriColor);

		if (nowper >= 99.9999)
		{
			SelectObject(hDC, origFont);
			DeleteObject(mainFont);
			DeleteObject(mainFont);
			mainFont = CreateFont(28, FontSet_Arial_B);
			SelectObject(hDC, mainFont);
			ColorBlt(hDC, rcWorldSize, RGB(0, 0, 0), 210);
			SetTextColor(hDC, RGB(255, 255, 255));
			DrawText(hDC, TEXT("전송이 완료됐습니다!"), -1, &rcWorldSize, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		}
	}


	SelectObject(hDC, origFont);
	DeleteObject(mainFont);
}

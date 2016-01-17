#include "stdafx.h"
#include <windows.h>
#include <commdlg.h>
#include <cstdio>
#include <io.h>
#include "ServerMain.h"
#include <winsock2.h>
#include "FileCopy.h"
using namespace std;

namespace {

	CRITICAL_SECTION cs;
	string strServerIP;

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
	/*
	
	접근은 동기화 필요
	
	HWND : 영향을 끼치지 않음.
	UINT : 메시지 종류. 윈도우 메시지랑 무관

	*/
	deque<MSG> ServerMessage;
	DataInfo* DownloadData = nullptr;
};

void SendToServerMsg(MSG msg)
{

	EnterCriticalSection(&cs);
	ServerMessage.push_back(msg);
	LeaveCriticalSection(&cs);
}

DataInfo* GetWndInfo(HWND hWnd)
{
	return DownloadData;
}

DataInfo* RegistWndInfo(HWND hWnd)
{
	if (DownloadData) delete DownloadData;
	DownloadData = new DataInfo();
	return DownloadData;
}

LRESULT CALLBACK DownloadProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//	접근 방법 
	//	DownloadData[hWnd];
	if (!DownloadData) return DefWindowProc(hWnd, uMsg, wParam, lParam);
	HDC hdc;
	PAINTSTRUCT ps;
	switch (uMsg)
	{

	case WM_CREATE:
		break;

	case WM_LBUTTONDOWN:

		EnterCriticalSection(&cs);
		DownloadData->StartDrag(lParam);
		LeaveCriticalSection(&cs);

		break;

	case WM_MOUSEMOVE:

		EnterCriticalSection(&cs);
		DownloadData->Drag(lParam);
		LeaveCriticalSection(&cs);

		break;

	case WM_LBUTTONUP:

		EnterCriticalSection(&cs);
		if (DownloadData->ReleaseDrag(lParam)) LeaveCriticalSection(&cs);
		else if (DownloadData->IsDownloadEnd())
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
			DownloadData->Draw(LayDC);
			LeaveCriticalSection(&cs);

			BitBlt(hdc, 0, 0, rcSize.right - rcSize.left, rcSize.bottom - rcSize.top, LayDC, 0, 0, SRCCOPY);

			DeleteObject(Lay);
			DeleteDC(LayDC);
		}
		EndPaint(hWnd, &ps);

		break;

	case WM_DESTROY:

		EnterCriticalSection(&cs);
		delete DownloadData;
		DownloadData = nullptr;
		LeaveCriticalSection(&cs);
		return DefWindowProc(hWnd, uMsg, wParam, lParam);

	default: return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void renewalFileList(SOCKET& sock)
{
	int retval;
	size_t len;

	vector<FileFormat> vFileFormat;
	size_t szFileList = 0;
	retval = recvn(sock, (char*)&szFileList, sizeof(size_t), 0);

	TCHAR fileName[80];
	DWORD szImgLength = 0;
	BITMAPINFO bif;
	BYTE* szImgBits;
	size_t Filesize;

	for (size_t i = 0; i < szFileList; ++i)
	{
		retval = recvn(sock, (char*)&len, sizeof(size_t), 0);
		retval = recvn(sock, (char*)fileName, len, 0);


		retval = recvn(sock, (char*)&szImgLength, sizeof(DWORD), 0);
		retval = recvn(sock, (char*)&bif, sizeof(BITMAPINFO), 0);
		szImgBits = new BYTE[szImgLength];
		retval = recvn(sock, (char*)szImgBits, szImgLength, 0);
		retval = recvn(sock, (char*)&Filesize, sizeof(size_t), 0);
		CImageBits cib(szImgLength, bif, szImgBits);
		vFileFormat.push_back(FileFormat(wstring(fileName), cib, Filesize));
	}

	SendMessage(ghWndMain, WM_GETVALUE, reinterpret_cast<WPARAM>(&vFileFormat), 1);

}

void SetServerIP(std::string wstr)
{
	strServerIP = wstr;
}

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
{
	int retval;
	
	InitializeCriticalSection(&cs);

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit(TEXT("socket()"));

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(strServerIP.c_str());
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit(TEXT("connect()"));

	//	길이 받기
	size_t len;

	TCHAR lpstrFile[MAX_PATH] = L"";
	TCHAR szFileTitle[MAX_PATH] = L"";

	renewalFileList(sock);

	bool DownloadingNow = false;
	bool UploadNow = false;
	FileFormat* downloadFileInfo = nullptr;
	// 서버와 데이터 통신
	while (1) 
	{
#pragma region Download

		if (DownloadingNow)
		{
			if (!downloadFileInfo) _DEBUG_ERROR("파일이 제대로 로드되지 않았습니다!");
			// 저장 데이터
			ofstream	ofDownload;
			bool		bOpenSucess = false;

			// 패킷을 받을 임시 변수
			Packet GetPacket;

			size_t	szDataBlock = 0;
			size_t	szGetPacketNum = 0;
			size_t	szGetPacketNumperTime = 0;

			// draw percent
			int nowper = 0;
			double szDataBlock_div_100;

			// 시간 측정
			chrono::system_clock::time_point	m_cStarterTime;
			chrono::system_clock::time_point	m_nCurrentTime;
			chrono::duration<double>			m_cftimeElapsed;

			ofDownload.open(lpstrFile, ios::binary);
			//	if (!ofDownload.is_open())
			//	{
			//		SendMessage(ghWndMain, WM_UPDATEMESSAGE, NoSaveFile, 1);
			//		SendMessage(ghWndMain, WM_GETVALUE, 0, 5);
			//		downloadFileInfo = nullptr;
			//		DownloadingNow = false;
			//		break;
			//	}
			bOpenSucess = true;

			szDataBlock = downloadFileInfo->szFileSize;
			szDataBlock_div_100 = szDataBlock / 100.0;

			// 시간 측정 시작
			m_cStarterTime = m_nCurrentTime = chrono::system_clock::now();

			percentStruct *sendPackper = new percentStruct(0, 0, 0);
			SendMessage(ghWndMain, WM_GETVALUE, (WPARAM)sendPackper, 4);
			// 클라이언트와 데이터 통신
			while (szGetPacketNum < szDataBlock)
			{
				// 데이터 받기(고정 길이)
				retval = recvn(sock, (char *)&len, sizeof(size_t), 0);
				if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); break; }
				else if (retval == 0) break;

				retval = recvn(sock, reinterpret_cast<char*>(&GetPacket), len, 0);
				if (retval == SOCKET_ERROR) { ErrorProcess(TEXT("recv()")); break; }
				else if (retval == 0)break;

				// Download main procedure
				ofDownload.write(GetPacket._str, GetPacket._size);
				szGetPacketNumperTime += GetPacket._size;

				// 현재 시간 확인
				m_cftimeElapsed = chrono::system_clock::now() - m_nCurrentTime;

				// draw percent
				if (m_cftimeElapsed.count() > 0.3333
					|| (nowper != int((szGetPacketNum + szGetPacketNumperTime) / szDataBlock_div_100)))
				{
					szGetPacketNum += szGetPacketNumperTime;

					if (nowper != int(szGetPacketNum / szDataBlock_div_100)) nowper++;
					// 초당 데이터
					double per_1secDownData = szGetPacketNumperTime / m_cftimeElapsed.count();
					//	남은 시간
					double losstime = double(szDataBlock - szGetPacketNum) / per_1secDownData;
					//	Draw
					{
						/*
						보낼 데이터	: int		nowper
						초당 데이터	: double	per_1secDownData
						남은 시간		: double	losstime
						*/
						percentStruct *sendPackper = new percentStruct(nowper, per_1secDownData, losstime);
						SendMessage(ghWndMain, WM_GETVALUE, (WPARAM)sendPackper, 4);

					}
					szGetPacketNumperTime = 0;
					// Time renewal
					m_nCurrentTime = chrono::system_clock::now();
				}


			}	//end while

			if (szGetPacketNumperTime > 0) szGetPacketNum += szGetPacketNumperTime;

			if (szDataBlock > 0 && szGetPacketNum == szDataBlock)
			{
				nowper = 100;
				m_cftimeElapsed = chrono::system_clock::now() - m_cStarterTime;
				percentStruct *sendPackper = new percentStruct(nowper, double(szGetPacketNum), m_cftimeElapsed.count());
				SendMessage(ghWndMain, WM_GETVALUE, (WPARAM)sendPackper, 4);

				ofDownload.close();
				DownloadingNow = false;
			}
			else
			{
				percentStruct *sendPackper = new percentStruct(nowper, -1, 0);
				SendMessage(ghWndMain, WM_GETVALUE, (WPARAM)sendPackper, 4);

				ofDownload.clear();
				ofDownload.close();
				if (bOpenSucess) _wremove(downloadFileInfo->strFileName.c_str());
				DownloadingNow = false;
			}

			renewalFileList(sock);

		}

#pragma endregion

#pragma region Upload

		else if (UploadNow)
		{
			ifstream	SendData;
			size_t szData;

			HWND hWnd = nullptr;
			while (!hWnd)
			{
				EnterCriticalSection(&cs);
				if (ServerMessage.size() > 0)
				{
					if (ServerMessage.front().message == WM_CREATEWINDOW)
					{
						MSG msg = ServerMessage.front();
						ServerMessage.pop_front();
						hWnd = msg.hwnd;
						LPWSTR strFilename = (LPWSTR)msg.lParam;
						wstring strLink = wstring(strFilename);
						//	잘라내기
						PathStripPath(strFilename);
						SendData.open(strLink, ios::binary);
						SendData.seekg(0, fstream::end);
						szData = SendData.tellg();
						SendData.seekg(0);

						DownloadData->SetFileSize(szData);
						DownloadData->SetCIB(strLink.c_str());
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

				retval = send(sock, (char *)&len, sizeof(size_t), 0);
				if (retval == SOCKET_ERROR) { cout << endl; err_displayBox(TEXT("send()")); break; }
				retval = send(sock, reinterpret_cast<char*>(&pStr), len, 0);
				if (retval == SOCKET_ERROR) { cout << endl; err_displayBox(TEXT("send()")); break; }

				EnterCriticalSection(&cs);
				DownloadData->AddSize(pStr._size);
				LeaveCriticalSection(&cs);
				InvalidateRect(hWnd, NULL, false);

			}

			EnterCriticalSection(&cs);
			DownloadData->Renewal();
			LeaveCriticalSection(&cs);
			InvalidateRect(hWnd, NULL, false);

			SendData.close();
			DestroyWindow(hWnd);
			UploadNow = false;

			renewalFileList(sock);

		}

#pragma endregion
		//	Get Message
		else 
		{
			EnterCriticalSection(&cs);
			if (ServerMessage.size() > 0)
			{
				
				MSG msg = ServerMessage.front();
				ServerMessage.pop_front();
				LeaveCriticalSection(&cs);
				switch (msg.message)
				{
				case FILE_UPLOAD:
				{	
					LPWSTR strUpload = (LPWSTR)msg.wParam;
					UINT ServerState = FILE_UPLOAD;
					send(sock, (char*)&ServerState, sizeof(UINT), 0);

					TCHAR SendFileName[MAX_PATH + 1];
					lstrcpy(SendFileName, strUpload);
					PathStripPath(SendFileName);
					len = lstrlen(SendFileName) * sizeof(TCHAR);

					retval = send(sock, (char*)&len, sizeof(size_t), 0);
					retval = send(sock, (char*)SendFileName, len, 0);

					CImageBits cib(strUpload);
					len = cib.GetSize();
					retval = send(sock, (char*)&len, sizeof(DWORD), 0);
					retval = send(sock, reinterpret_cast<char*>(&cib.GetBIF()), sizeof(BITMAPINFO), 0);
					retval = send(sock, reinterpret_cast<char*>(cib.GetFileBytes()), len, 0);

					ifstream SendData(strUpload, ios::binary);
					SendData.seekg(0, fstream::end);
					size_t szData = SendData.tellg();
					SendData.close();

					retval = send(sock, (char*)&szData, sizeof(size_t), 0);

					DownloadingNow = false;
					UploadNow = true;

					SendMessage(ghWndMain, WM_CREATEWINDOW, 0, (LPARAM)strUpload);
				}
					break;
					//	WPARAM : pointer FileFormat
				case FILE_SELECT:
					downloadFileInfo = voidpointcasting(FileFormat, msg.wParam);
					if (msg.lParam == 0)
					{
						OPENFILENAME ofn;
						TCHAR szCurrentDirectory[MAX_PATH + 1];
						memset(szCurrentDirectory, 0, MAX_PATH + 1);
						GetCurrentDirectory(MAX_PATH + 1, szCurrentDirectory);
						lstrcpy(lpstrFile, szCurrentDirectory);
						lstrcpy(lpstrFile, downloadFileInfo->strFileName.c_str());
						memset(&ofn, 0, sizeof(OPENFILENAME));
						ofn.lStructSize = sizeof(OPENFILENAME);
						ofn.hwndOwner = ghWndMain;
						ofn.lpstrFilter = L"모든 파일\0*.*\0";
						ofn.nFilterIndex = 1;
						ofn.lpstrFile = lpstrFile; // 파일열기공통대화상자에서선택한파일명
						ofn.nMaxFile = MAX_PATH;
						ofn.lpstrTitle = L"File Save";
						ofn.nMaxFileTitle = MAX_PATH;
						ofn.lpstrDefExt = PathFindExtension(downloadFileInfo->strFileName.c_str());
						ofn.lpstrFileTitle = szFileTitle;
						if (GetSaveFileName(&ofn))
						{
							if (0 == _waccess(downloadFileInfo->strFileName.c_str(), 0))
							{
								SendMessage(ghWndMain, WM_UPDATEMESSAGE, AlreadyExistFile, msg.wParam);
								downloadFileInfo = nullptr;
								break;
							}

							UINT ServerState = FILE_SELECT;
							retval = send(sock, (char*)&ServerState, sizeof(UINT), 0);

							len = downloadFileInfo->strFileName.length() * sizeof(TCHAR);
							retval = send(sock, (char*)&len, sizeof(size_t), 0);
							retval = send(sock, (char*)downloadFileInfo->strFileName.c_str(), len, 0);
							DownloadingNow = true;

							SetCurrentDirectory(szCurrentDirectory);
						}
						else
						{
							SendMessage(ghWndMain, WM_UPDATEMESSAGE, NoSaveFile, 0);
							SendMessage(ghWndMain, WM_GETVALUE, 0, 5);
							downloadFileInfo = nullptr;
							SetCurrentDirectory(szCurrentDirectory);
						}
					}
					else // must be Download 
					{
						UINT ServerState = FILE_SELECT;
						retval = send(sock, (char*)&ServerState, sizeof(UINT), 0);

						len = downloadFileInfo->strFileName.length() * sizeof(TCHAR);
						retval = send(sock, (char*)&len, sizeof(size_t), 0);
						retval = send(sock, (char*)downloadFileInfo->strFileName.c_str(), len, 0);
						DownloadingNow = true;
					}

					break;

				case DOWN_CANCEL:
					SendMessage(ghWndMain, WM_UPDATEMESSAGE, NoSaveFile, 0);
					SendMessage(ghWndMain, WM_GETVALUE, 0, 5);
					downloadFileInfo = nullptr;
					break;

				case EXIT_CLIENT:
					UINT ServerState = EXIT_CLIENT;
					retval = send(sock, (char*)&ServerState, sizeof(UINT), 0);
					// closesocket()
					closesocket(sock);

					// 윈속 종료
					WSACleanup();

					DeleteCriticalSection(&cs);

					return 0;

					//case:
					//	break;
				}

			}
			else LeaveCriticalSection(&cs);

		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	
	DeleteCriticalSection(&cs);

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

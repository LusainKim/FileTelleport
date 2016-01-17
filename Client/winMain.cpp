
#include "stdafx.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE	hInst;								// 현재 인스턴스입니다.
HWND		ghWndMain;
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
												// 소켓 통신 스레드 함수
DWORD WINAPI ServerMain(LPVOID arg);

CGameFramework		gGameFramework;
// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.
	MSG msg;

	wsprintf(szTitle, Win32TitleName);
	wsprintf(szWindowClass, _T("winMain"));

	// 전역 문자열을 초기화합니다.
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			gGameFramework.FrameAdvance();
		}
	}
	gGameFramework.OnDestroy();

	return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = DownloadProc;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = TEXT("FileDownloadBox");

	RegisterClassEx(&wc);

	return RegisterClassEx(&wcex);
}

void CallServerIPInputBox(POINT ptClientWorld)
{
	LusInputMsg Msg;
	Msg.hWnd = ghWndMain;
	Msg.Title = L"IP 주소 입력";
	Msg.Msg = L"\n서버의 IP 주소를 입력해주세요.\n아무것도 입력하지 않으면\n루프백 주소가 입력됩니다.";
	Msg.MsgStartPos = ptClientWorld;
	Msg.thisClass = nullptr;
	Msg.InputMax = 16;
	Msg.InputSize = 100;
	Msg.IsOnlyUseNum = false;
	Msg.MsgSize = { 0, 0, 300, DEFAULT_MSGHEIGHT };

	Msg.resultFunc = [](void* Class, LPWSTR str) {
		close_LusMsgProc();
		string serverIP("127.0.0.1");
		if (lstrlen(str) > 0)
		{
			wstring wstr = str;
			wstring p1, p2, p3, p4;
			auto p = wstr.begin();
			bool bInvalidIP = false;
			for (; p != wstr.end() && *p != '.' && p1.length() < 4; ++p) p1 += *p;
			if (p1.length() >= 4 || to_wstring(stoi(p1)) != p1) bInvalidIP = true;
			else ++p;
			if (!bInvalidIP)
			{
				for (; p != wstr.end() && *p != '.' && p2.length() < 4; ++p) p2 += *p;
				if (p2.length() >= 4 || to_wstring(stoi(p2)) != p2) bInvalidIP = true;
				else ++p;
				if (!bInvalidIP)
				{
					for (; p != wstr.end() && *p != '.' && p3.length() < 4; ++p) p3 += *p;
					if (p3.length() >= 4 || to_wstring(stoi(p3)) != p3) bInvalidIP = true;
					else ++p;
					if (!bInvalidIP)
					{
						for (; p != wstr.end() && *p != '.' && p4.length() < 4; ++p) p4 += *p;
						if (p4.length() >= 4 || to_wstring(stoi(p4)) != p4) bInvalidIP = true;
					}
				}
			}

			if (bInvalidIP)
			{
				//	데스크톱 윈도우 사이즈
				RECT getWinSize;
				GetWindowRect(GetDesktopWindow(), &getWinSize);

				//	클라이언트 사이즈
				RECT rc; rc.left = rc.top = 0; rc.right = CLIENT_WIDTH; rc.bottom = CLIENT_HEIGHT;
				AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER, FALSE);

				//	클라이언트 절대좌표(left, top)
				POINT ptClientWorld;
				ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
				ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

				wsprintf(Lus_strMsg, L"\n잘못된 주소입니다.");
				LusMsg Msg;
				Msg.hWnd = ghWndMain;
				Msg.Title = L"실패";
				Msg.Msg = Lus_strMsg;
				Msg.MsgStartPos = ptClientWorld;
				Msg.InBoxMsg_1 = L"확인";
				Msg.thisClass = nullptr;
				Msg.Msg_1_Func = [](void* Class) {
					close_LusMsgProc();
					//	데스크톱 윈도우 사이즈
					RECT getWinSize;
					GetWindowRect(GetDesktopWindow(), &getWinSize);

					//	클라이언트 사이즈
					RECT rc; rc.left = rc.top = 0; rc.right = CLIENT_WIDTH; rc.bottom = CLIENT_HEIGHT;
					AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER, FALSE);

					//	클라이언트 절대좌표(left, top)
					POINT ptClientWorld;
					ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
					ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

					CallServerIPInputBox(ptClientWorld);
				};
				LusMsgBox(Msg.hWnd, Msg, hInst, 175);
				return;
			}
			else serverIP = string(to_string(stoi(p1)) + '.' + to_string(stoi(p2)) + '.' + to_string(stoi(p3)) + '.' + to_string(stoi(p4)));
		}

		SetServerIP(serverIP);

		// 소켓 통신 스레드 생성
		CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);
		Sleep(100);
		
	};
	Msg.undoFunc = [](void* Class) {
		close_LusMsgProc();
		DestroyWindow(ghWndMain);
	};

	LusInputMsgBox(Msg.hWnd, Msg, hInst, 175);
}


//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	//	윈도우 스타일
	DWORD dwStyle = 
				WS_OVERLAPPED 	//	
			|	WS_CAPTION 		//
			|	WS_MINIMIZEBOX 	//
			|	WS_SYSMENU 		//
			|	WS_BORDER			//
			;					//	end

	//	인스턴스 핸들을 전역 변수에 저장
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	//	데스크톱 윈도우 사이즈
	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	//	클라이언트 사이즈
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	AdjustWindowRect(&rc, dwStyle, FALSE);

	//	클라이언트 절대좌표(left, top)
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	윈도우 생성
	ghWndMain = CreateWindow(
				  szWindowClass			//	윈도우 클래스 명
				, szTitle				//	캡션 표시 문자열
				, dwStyle				//	윈도우 스타일
				, ptClientWorld.x		//	부모 윈도우 기반 윈도우 시작좌표 : x
				, ptClientWorld.y		//	부모 윈도우 기반 윈도우 시작좌표 : y
				, rc.right				//	윈도우 사이즈 : width
				, rc.bottom				//	윈도우 사이즈 : height
				, NULL					//	부모 윈도우.
				, NULL					//	메뉴 핸들
				, hInstance				//	인스턴스 핸들
				, NULL					//	추가 파라메터 : NULL
		);

	if (!gGameFramework.OnCreate(hInstance, ghWndMain)) return(FALSE);
	//	생성 실패시 프로그램 종료
	//	확인 : WndProc의 default msg handler가 DefWindowProc 함수를 반환하는가?
	if (!ghWndMain)	return FALSE;

	//	윈도우 표시
	//	서버에 연결됐을 경우에만 표시한다.
//	ShowWindow(ghWndMain, nCmdShow);
	::DragAcceptFiles(ghWndMain, TRUE);
	UpdateWindow(ghWndMain);

	CallServerIPInputBox(ptClientWorld);

	//	성공 반환
	return TRUE;
}
//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDBLCLK:
	case WM_MOUSELEAVE:
	case WM_MBUTTONDOWN:
		gGameFramework.OnProcessingMouseMessage(hWnd, message, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_IME_COMPOSITION:
	case WM_IME_STARTCOMPOSITION:
		gGameFramework.OnProcessingKeyboardMessage(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		gGameFramework.PresentFrameBuffer(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		if (!gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam))
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
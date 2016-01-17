
#include "stdafx.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100

// ���� ����:
HINSTANCE	hInst;								// ���� �ν��Ͻ��Դϴ�.
HWND		ghWndMain;
TCHAR szTitle[MAX_LOADSTRING];					// ���� ǥ���� �ؽ�Ʈ�Դϴ�.
TCHAR szWindowClass[MAX_LOADSTRING];			// �⺻ â Ŭ���� �̸��Դϴ�.
												// ���� ��� ������ �Լ�
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

CGameFramework		gGameFramework;
// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;

	wsprintf(szTitle, Win32TitleName);
	wsprintf(szWindowClass, _T("winMain"));

	// ���� ���ڿ��� �ʱ�ȭ�մϴ�.
	MyRegisterClass(hInstance);

	// ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{

	//	������ ��Ÿ��
	DWORD dwStyle = 
					WS_OVERLAPPED 	//	
				|	WS_CAPTION 		//
				|	WS_MINIMIZEBOX 	//
				|	WS_SYSMENU 		//
				|	WS_BORDER			//
			;					//	end

	//	�ν��Ͻ� �ڵ��� ���� ������ ����
	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	//	����ũ�� ������ ������
	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	//	Ŭ���̾�Ʈ ������
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	
	//	Ŭ���̾�Ʈ ������ǥ(left, top)
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - getWinSize.left - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - getWinSize.top - CLIENT_HEIGHT) / 2;

	//	������ ����
	ghWndMain = CreateWindow(
				  szWindowClass			//	������ Ŭ���� ��
				, szTitle				//	ĸ�� ǥ�� ���ڿ�
				, dwStyle				//	������ ��Ÿ��
				, ptClientWorld.x		//	�θ� ������ ��� ������ ������ǥ : x
				, ptClientWorld.y		//	�θ� ������ ��� ������ ������ǥ : y
				, rc.right				//	������ ������ : width
				, rc.bottom				//	������ ������ : height
				, NULL					//	�θ� ������.
				, NULL					//	�޴� �ڵ�
				, hInstance				//	�ν��Ͻ� �ڵ�
				, NULL					//	�߰� �Ķ���� : NULL
		);

	if (!gGameFramework.OnCreate(hInstance, ghWndMain)) return(FALSE);
	//	���� ���н� ���α׷� ����
	//	Ȯ�� : WndProc�� default msg handler�� DefWindowProc �Լ��� ��ȯ�ϴ°�?
	if (!ghWndMain)	return FALSE;

	//	������ ǥ��
	ShowWindow(ghWndMain, nCmdShow);
	UpdateWindow(ghWndMain);

	::DragAcceptFiles(ghWndMain, TRUE);
	// ���� ��� ������ ����
	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	//	���� ��ȯ
	return TRUE;
}
//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND	- ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT	- �� â�� �׸��ϴ�.
//  WM_DESTROY	- ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
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
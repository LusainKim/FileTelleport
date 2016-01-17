
#include <Windows.h>
#include "LusDef.h"

////////////////////////////////////////////////
//

POINT operator+(POINT a, const POINT b)	{ return MakePoint( a.x + b.x	, a.y + b.y );	}
POINT operator-(POINT a, const POINT b)	{ return MakePoint( a.x - b.x	, a.y - b.y );	}
POINT operator*(float b, POINT a)		{ return MakePoint( a.x *b		, a.y *b	);	}
bool operator==(POINT p1, POINT p2){ return (p1.x == p2.x && p1.y == p2.y) ? true : false; }
bool operator!=(POINT p1, POINT p2){ return (p1.x == p2.x && p1.y == p2.y) ? false: true; }

RECT operator+(RECT rc, POINT pt)	{ return MakeRect(pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom); }
RECT operator+(POINT pt, RECT rc)	{ return MakeRect(pt.x + rc.left, pt.y + rc.top, pt.x + rc.right, pt.y + rc.bottom); }
RECT operator+(RECT rc_1, RECT rc_2) { return MakeRect(rc_1.left + rc_2.left, rc_1.top + rc_2.top, rc_1.right + rc_2.right, rc_1.bottom + rc_2.bottom); }
RECT operator-(RECT rc_1, RECT rc_2) { return MakeRect(rc_1.left - rc_2.left, rc_1.top - rc_2.top, rc_1.right - rc_2.right, rc_1.bottom - rc_2.bottom); }

void DrawLineBox(HDC mainDC, RECT rc,
	COLORREF main_RGB, COLORREF Line_RGB, int LineLen, bool Type)
{
	SetDCBrushColor(mainDC, main_RGB);
	FillRect(mainDC, &rc, (HBRUSH)GetStockObject(DC_BRUSH));

	rc = SizeDown(rc, LineLen);
	SetDCBrushColor(mainDC, Line_RGB);

	FrameRect(mainDC, &rc, (HBRUSH)GetStockObject(DC_BRUSH));

	if (Type == true)
	{
		rc = SizeDown(rc, 2);
		FillRect(mainDC, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
	}
}

void FixValue(LPWSTR DestMsg, int fixnum)
{
	//	초기값 0
	TCHAR Fix[20] = TEXT("0\0");
	//	반한될 문자열
	TCHAR returnMsg[20];
	int count = 0;
	int endnum;

	while (fixnum > 0)
	{
		endnum = fixnum % 10;
		fixnum /= 10;
		Fix[count] = '0' + endnum;
		count++;
		if (count % 4 == 3 && fixnum > 0)
		{
			Fix[count] = ',';
			count++;
		}
	}
	if (count == 0) count++;
	for (int i = 0; i < count; i++) 
		returnMsg[i] = Fix[count - i - 1];
	returnMsg[count] = '\0';

	lstrcpy(DestMsg, returnMsg);
	
}



//////////////////////////////////////////////////////////////////////////////////////////////


LusMsg		Lus_MsgStorage;
LusInputMsg	Lus_InputBoxStorage;
bool		Flag_LusMsgBox;
HWND		hLMg;

TCHAR		Lus_strMsg[4096];
TCHAR		Lus_strMsg_1[128];
TCHAR		Lus_strMsg_2[128];
int			iMsgAlpha;

void close_LusMsgProc()
{
	DestroyWindow(hLMg);
	hLMg = nullptr;
}

///////////////////////////////////////////////
//		LusMsg Proc

LRESULT CALLBACK LusMsgProc(HWND hMsg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	// Msg
	static LusMsg Msg;
	const static LusMsg nullMsg;
	static RECT rc_Msg_1, rc_Msg_2;
	static int sel_Msg = 0;
	static bool IsSel = false;

	// Draw Vaule
	PAINTSTRUCT ps;
	HDC hdc;

	// calc Value
	RECT wndRect;

	// Move Pointer
	static POINT Mp, old_pt, pt_for_drag;
	static bool flag_for_Drag;

	GetWindowRect(hMsg, &wndRect);
	POINT pt_for_wndFix = { -wndRect.left, -wndRect.top };

	wndRect = wndRect + pt_for_wndFix;

	switch (uMsg)
	{
	case WM_CREATE:
		flag_for_Drag = false;
		Msg = nullMsg;
		sel_Msg = 0;
		Mp = old_pt = pt_for_drag = { 0, 0 };
		IsSel = false;
		Flag_LusMsgBox = true;
		break;

	case WM_GETVALUE:
		switch (wParam)
		{
		case wGETMSG:
			Msg = Lus_MsgStorage;
			Lus_MsgStorage = nullMsg;

			if (Msg.bUseMsg_2)
			{
				rc_Msg_1 = {
					MsgBlank + 2,
					Msg.MsgSize.bottom - ButtonHeight - 4 - MsgBlank,
					MsgBlank + 2 + (Msg.MsgSize.right - 2 * MsgBlank - 4) / 2,
					Msg.MsgSize.bottom - 2 - MsgBlank
				};
				rc_Msg_2 = {
					rc_Msg_1.right,
					Msg.MsgSize.bottom - ButtonHeight - 4 - MsgBlank,
					Msg.MsgSize.right - (MsgBlank + 2),
					Msg.MsgSize.bottom - 2 - MsgBlank
				};

			}
			else
			{
				rc_Msg_1 = {
					MsgBlank + 2,
					Msg.MsgSize.bottom - ButtonHeight - 4 - MsgBlank,
					Msg.MsgSize.right - (MsgBlank + 2),
					Msg.MsgSize.bottom - 2 - MsgBlank
				};
			}
		case wGETRENEWALSCREEN:
		{
			hdc = GetDC(hMsg);
			HDC LayDC = CreateCompatibleDC(hdc);
			HBITMAP Lay = CreateCompatibleBitmap(hdc, wndRect.right, wndRect.bottom);
			SelectObject(LayDC, Lay);
			HFONT mainFont = CreateFont((Msg.FontSize == -1) ? DEFAULT_FontSize : Msg.FontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
			SelectObject(LayDC, mainFont);
			SetTextColor(LayDC, Msg.TextColor);
			SelectObject(LayDC, (HPEN)GetStockObject(WHITE_PEN));
			SetBkMode(LayDC, TRANSPARENT);
			RECT lprc;

			TEXTMETRIC tm;
			GetTextMetrics(LayDC, &tm);
			int fontSize = tm.tmHeight;

			SetDCBrushColor(LayDC, Msg.MsgColor);
			FillRect(LayDC, &wndRect, (HBRUSH)GetStockObject(DC_BRUSH));

			lprc = SizeDown(wndRect, MsgBlank);
			FrameRect(LayDC, &lprc, (HBRUSH)GetStockObject(WHITE_BRUSH));

			lprc = { MsgBlank + 2, MsgBlank + 2,
				Msg.MsgSize.right - MsgBlank - 2, lprc.top + fontSize + 2 };

			DrawText(LayDC, Msg.Title, -1, &lprc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			MoveToEx(LayDC, MsgBlank + 1, lprc.bottom, NULL);
			LineTo(LayDC, Msg.MsgSize.right - MsgBlank - 1, lprc.bottom);

			lprc = { MsgBlank + 2, lprc.bottom + 2,
				Msg.MsgSize.right - MsgBlank - 2,
				Msg.MsgSize.bottom - ButtonHeight - 2 };

			DrawText(LayDC, Msg.Msg, -1, &lprc,
				DT_CENTER | DT_VCENTER | DT_WORDBREAK);

			lprc = SizeDown(rc_Msg_1, 1);
			DrawLineBox(LayDC, lprc, Msg.MsgColor, LusColor::White, MsgBlank, DL_FRAME);

			if (sel_Msg == 1)
			{
				lprc = SizeDown(lprc, 4);
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.AlphaFormat = 0;
				bf.SourceConstantAlpha = ALPHA_SELECT;
				HBITMAP blendLay = CreateCompatibleBitmap(LayDC, 10, 10);
				HDC blendDC = CreateCompatibleDC(LayDC);
				SelectObject(blendDC, blendLay);
				SetDCBrushColor(blendDC, LusColor::White);
				RECT rc_for_blend = { 0, 0, 10, 10 };
				FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

				AlphaBlend(LayDC, lprc.left, lprc.top,
					lprc.right - lprc.left, lprc.bottom - lprc.top, blendDC, 0, 0, 10, 10, bf);

				DeleteDC(blendDC);
				DeleteObject(blendLay);

			}

			DrawText(LayDC, Msg.InBoxMsg_1, -1, &lprc,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			if (Msg.bUseMsg_2)
			{
				lprc = SizeDown(rc_Msg_2, 1);
				DrawLineBox(LayDC, lprc, Msg.MsgColor, LusColor::White, MsgBlank, DL_FRAME);
				if (sel_Msg == 2)
				{
					lprc = SizeDown(lprc, 4);
					BLENDFUNCTION bf;
					bf.BlendOp = AC_SRC_OVER;
					bf.BlendFlags = 0;
					bf.AlphaFormat = 0;
					bf.SourceConstantAlpha = ALPHA_SELECT;
					HBITMAP blendLay = CreateCompatibleBitmap(LayDC, 10, 10);
					HDC blendDC = CreateCompatibleDC(LayDC);
					SelectObject(blendDC, blendLay);
					SetDCBrushColor(blendDC, LusColor::White);
					RECT rc_for_blend = { 0, 0, 10, 10 };
					FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

					AlphaBlend(LayDC, lprc.left, lprc.top,
						lprc.right - lprc.left, lprc.bottom - lprc.top, blendDC, 0, 0, 10, 10, bf);

					DeleteDC(blendDC);
					DeleteObject(blendLay);

				}
				DrawText(LayDC, Msg.InBoxMsg_2, -1, &lprc,
					DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			
			{
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.AlphaFormat = 0;
				bf.SourceConstantAlpha = iMsgAlpha;

				AlphaBlend(hdc, 0, 0, wndRect.right, wndRect.bottom, LayDC, 0, 0, wndRect.right, wndRect.bottom, bf);

			}
			ReleaseDC(hMsg,hdc);
		}

			break;
		}
		break;
	case WM_LBUTTONDOWN:

		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);
		IsSel = false;
		sel_Msg = 0;
		// 따로 함수로 빼야함...
		if (PtInRect(&rc_Msg_1, Mp))
		{
			sel_Msg = 1;
			IsSel = true;
			InvalidateRect(hMsg, NULL, false);
			break;
		}
		else if (PtInRect(&rc_Msg_2, Mp))
		{
			sel_Msg = 2;
			IsSel = true;
			InvalidateRect(hMsg, NULL, false);
			break;
		}

		flag_for_Drag = true;

		ReleaseCapture();
		SetCapture(hMsg);

		GetCursorPos(&Mp);
		pt_for_drag = old_pt = Mp;

		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);

		old_pt = old_pt - Mp;

		break;

	case WM_MOUSEMOVE:
		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);

		if (flag_for_Drag)
		{
			GetCursorPos(&Mp);
			old_pt.x = old_pt.x + Mp.x - pt_for_drag.x;
			old_pt.y = old_pt.y + Mp.y - pt_for_drag.y;

			SetWindowPos(hMsg, 0,
				old_pt.x, old_pt.y,
				0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOZORDER);

			pt_for_drag = Mp;
			sel_Msg = 0;
			break;
		}

		if (IsSel)
			if (PtInRect(&rc_Msg_1, Mp))
				sel_Msg = 1;
			else if (PtInRect(&rc_Msg_2, Mp))
				sel_Msg = 2;
			else
				sel_Msg = 0;

		InvalidateRect(hMsg, NULL, false);
		break;


	case WM_LBUTTONUP:

		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);
		flag_for_Drag = false;
		ReleaseCapture();
		if (!IsSel) break;
		IsSel = false;
		sel_Msg = 0;

		InvalidateRect(hMsg, NULL, false);

		if (PtInRect(&rc_Msg_1, Mp))
		{
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			(Msg.Msg_1_Func)(Msg.thisClass);
			break;
		}
		else if (Msg.bUseMsg_2 && PtInRect(&rc_Msg_2, Mp))
		{
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			Msg.Msg_2_Func(Msg.thisClass);
			break;
		}

		break;

	case WM_RBUTTONDOWN:

		if (Msg.bUseMsg_2)
		{
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			Msg.Msg_2_Func(Msg.thisClass);
			break;
		}
		else
		{
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			Msg.Msg_1_Func(Msg.thisClass);
			break;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE:
		case VK_RETURN:
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			(Msg.Msg_1_Func)(Msg.thisClass);
			break;
		case VK_ESCAPE:
			if (Msg.bUseMsg_2)
			{
				SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
				SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
				EnableWindow(Msg.hWnd, true);
				Flag_LusMsgBox = false;
				Msg.Msg_2_Func(Msg.thisClass);
			}
			else
			{
				SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
				SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
					SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
				EnableWindow(Msg.hWnd, true);
				Flag_LusMsgBox = false;
				(Msg.Msg_1_Func)(Msg.thisClass);
			}
			break;
		}
		break;

	case WM_PAINT:
	{
		hdc = BeginPaint(hMsg, &ps);
		HDC LayDC = CreateCompatibleDC(hdc);
		HBITMAP Lay = CreateCompatibleBitmap(hdc, wndRect.right, wndRect.bottom);
		SelectObject(LayDC, Lay);
		HFONT mainFont = CreateFont((Msg.FontSize == -1) ? DEFAULT_FontSize : Msg.FontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
		SelectObject(LayDC, mainFont);
		SetTextColor(LayDC, Msg.TextColor);
		SelectObject(LayDC, (HPEN)GetStockObject(WHITE_PEN));
		SetBkMode(LayDC, TRANSPARENT);
		RECT lprc;

		TEXTMETRIC tm;
		GetTextMetrics(LayDC, &tm);
		int fontSize = tm.tmHeight;

		SetDCBrushColor(LayDC, Msg.MsgColor);
		FillRect(LayDC, &wndRect, (HBRUSH)GetStockObject(DC_BRUSH));

		lprc = SizeDown(wndRect, MsgBlank);
		FrameRect(LayDC, &lprc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		lprc = { MsgBlank + 2, MsgBlank + 2,
			Msg.MsgSize.right - MsgBlank - 2, lprc.top + fontSize + 2 };

		DrawText(LayDC, Msg.Title, -1, &lprc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		MoveToEx(LayDC, MsgBlank + 1, lprc.bottom, NULL);
		LineTo(LayDC, Msg.MsgSize.right - MsgBlank - 1, lprc.bottom);

		lprc = { MsgBlank + 2, lprc.bottom + 2,
			Msg.MsgSize.right - MsgBlank - 2,
			Msg.MsgSize.bottom - ButtonHeight - 2 };

		DrawText(LayDC, Msg.Msg, -1, &lprc,
			DT_CENTER | DT_VCENTER | DT_WORDBREAK);

		lprc = SizeDown(rc_Msg_1, 1);
		DrawLineBox(LayDC, lprc, Msg.MsgColor, LusColor::White, MsgBlank, DL_FRAME);

		if (sel_Msg == 1)
		{
			lprc = SizeDown(lprc, 4);
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.AlphaFormat = 0;
			bf.SourceConstantAlpha = ALPHA_SELECT;
			HBITMAP blendLay = CreateCompatibleBitmap(LayDC, 10, 10);
			HDC blendDC = CreateCompatibleDC(LayDC);
			SelectObject(blendDC, blendLay);
			SetDCBrushColor(blendDC, LusColor::White);
			RECT rc_for_blend = { 0, 0, 10, 10 };
			FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

			AlphaBlend(LayDC, lprc.left, lprc.top,
				lprc.right - lprc.left, lprc.bottom - lprc.top, blendDC, 0, 0, 10, 10, bf);

			DeleteDC(blendDC);
			DeleteObject(blendLay);

		}

		DrawText(LayDC, Msg.InBoxMsg_1, -1, &lprc,
			DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		if (Msg.bUseMsg_2)
		{
			lprc = SizeDown(rc_Msg_2, 1);
			DrawLineBox(LayDC, lprc, Msg.MsgColor, LusColor::White, MsgBlank, DL_FRAME);
			if (sel_Msg == 2)
			{
				lprc = SizeDown(lprc, 4);
				BLENDFUNCTION bf;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				bf.AlphaFormat = 0;
				bf.SourceConstantAlpha = ALPHA_SELECT;
				HBITMAP blendLay = CreateCompatibleBitmap(LayDC, 10, 10);
				HDC blendDC = CreateCompatibleDC(LayDC);
				SelectObject(blendDC, blendLay);
				SetDCBrushColor(blendDC, LusColor::White);
				RECT rc_for_blend = { 0, 0, 10, 10 };
				FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

				AlphaBlend(LayDC, lprc.left, lprc.top,
					lprc.right - lprc.left, lprc.bottom - lprc.top, blendDC, 0, 0, 10, 10, bf);

				DeleteDC(blendDC);
				DeleteObject(blendLay);

			}
			DrawText(LayDC, Msg.InBoxMsg_2, -1, &lprc,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}


		BitBlt(hdc, 0, 0, wndRect.right, wndRect.bottom, LayDC, 0, 0, SRCCOPY);

		DeleteObject(mainFont);
		DeleteObject(Lay);
		DeleteDC(LayDC);
		EndPaint(hMsg, &ps);
	}
	break;


	case WM_DESTROY:
		break;

	default:
		return DefWindowProc(hMsg, uMsg, wParam, lParam);
	}
	return 0;
}

void LusMsgBox(HWND hWnd, LusMsg Msg, HINSTANCE g_hInst, int alpha_message)
{
	static WNDCLASSEX wc;
	static bool Flag_first_Register = false;

	int	msgWidth = 300;
	int	msgHeight = 200;

	RECT wndRect;
	GetWindowRect(hWnd, &wndRect);

	POINT wndcenter = { wndRect.right / 2, wndRect.bottom / 2 };


	if (hLMg)
	{
		DestroyWindow(hLMg);
		hLMg = nullptr;
	}

	LusMsg SendMsg = Msg;

	if (!SendMsg.bUseMsg_2)
		SendMsg.InBoxMsg_2 = TEXT("");

	int MsgLen = max(
		lstrlen(SendMsg.Msg) + 2 * MsgBlank + 4,
		lstrlen(SendMsg.InBoxMsg_1) + lstrlen(SendMsg.InBoxMsg_2) + 3 * MsgBlank + 6
		);

	if (sideCheck(0, MsgLen, 100))
	{
		msgWidth = MsgWidth;
		msgHeight = 100;
	}
	else if (sideCheck(100, MsgLen, 200))
	{
		msgWidth = 250;
		msgHeight = 125;
	}
	else if (sideCheck(200, MsgLen, 1000))
	{
		msgWidth = 300;
		msgHeight = 200;
	}



	SendMsg.MsgSize.right = max(SendMsg.MsgSize.right, msgWidth);

	//	SendMsg.mainDC ;
	//	SendMsg.MsgSize;
	//	SendMsg.MsgStartPos;
	//	SendMsg.Title;
	//	SendMsg.Msg;
	//	SendMsg.Font;
	//	SendMsg.InBoxMsgNum;
	//	SendMsg.InBoxMsg_1;
	//	SendMsg.InBoxMsg_2;
	//	SendMsg.TextColor;

	// Msg Height Init
	{

		HDC hdc, LayDC;
		hdc = GetDC(NULL);
		LayDC = CreateCompatibleDC(hdc);
		HBITMAP Lay = CreateCompatibleBitmap(LayDC, 1, 1);
		SelectObject(LayDC, Lay);
		HFONT mainFont = CreateFont((SendMsg.FontSize == -1) ? DEFAULT_FontSize : SendMsg.FontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
		SelectObject(LayDC, mainFont);

		RECT lprc = {
			MsgBlank + 2,
			0,
			SendMsg.MsgSize.right - MsgBlank - 2,
			1000 };

		TEXTMETRIC tm;
		GetTextMetrics(LayDC, &tm);
		int fontSize = tm.tmHeight;
		int AdjHeight =
			DrawText(LayDC, SendMsg.Msg, -1, &lprc, DT_LEFT | DT_WORDBREAK) +
			(MsgBlank * 2 + fontSize + ButtonHeight + 8);


		DrawTextEx(LayDC, SendMsg.Msg, -1, &lprc,
			DT_CALCRECT, nullptr);

		if (abs(lprc.right + 4 + MsgBlank * 4 - msgWidth) < 46)
		{
			SendMsg.MsgSize.right = max(msgWidth, lprc.right + 4 + MsgBlank * 4);

			AdjHeight =
				lprc.bottom + (MsgBlank * 2 + fontSize + ButtonHeight + 8);
			SendMsg.MsgSize.bottom = max(msgHeight, AdjHeight);
		}
		else
			SendMsg.MsgSize.bottom = max(msgHeight, AdjHeight);

		//		lprc = SendMsg.MsgSize;

		DeleteObject(mainFont);
		ReleaseDC(NULL, hdc);
		ReleaseDC(NULL, LayDC);
		DeleteObject(Lay);
	}

	if (!Flag_first_Register)
	{
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = LusMsgProc;
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszClassName = TEXT("LusMsgClass");

		RegisterClassEx(&wc);
	}


	RECT getClientSize;
	GetWindowRect(SendMsg.hWnd, &getClientSize);

	hLMg = CreateWindow(wc.lpszClassName, NULL, WS_POPUP | WS_VISIBLE,
		getClientSize.left + SendMsg.MsgStartPos.x - SendMsg.MsgSize.right / 2,
		getClientSize.top + SendMsg.MsgStartPos.y - SendMsg.MsgSize.bottom / 2,
		SendMsg.MsgSize.right, SendMsg.MsgSize.bottom, hWnd, NULL, g_hInst, NULL);

	SetWindowLong(hLMg, GWL_EXSTYLE,
		GetWindowLong(hLMg, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hLMg, 0, alpha_message, LWA_ALPHA);
	iMsgAlpha = alpha_message;
	Lus_MsgStorage = SendMsg;
	SendMessage(hLMg, WM_GETVALUE, wGETMSG, 0);
	EnableWindow(SendMsg.hWnd, false);

}

///////////////////////////////////////////////
//		InputBox Proc

LRESULT CALLBACK LusInputMsgProc(HWND hMsg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	// Msg
	static LusInputMsg Msg;
	const static LusInputMsg nullMsg;
	static RECT rc_Msg_1, rc_Msg_2;
	static int sel_Msg = 0;
	static bool IsSel = false;
	static SIZE	Csize;
	static RECT rc_Insert;
	// Draw Vaule
	PAINTSTRUCT ps;
	HDC hdc;

	// calc Value
	RECT wndRect;

	// Move Pointer
	static POINT Mp, old_pt, pt_for_drag;
	static bool flag_for_Drag;

	GetWindowRect(hMsg, &wndRect);
	POINT pt_for_wndFix = { -wndRect.left, -wndRect.top };

	wndRect = wndRect + pt_for_wndFix;

	switch (uMsg)
	{
	case WM_CREATE:
		CreateCaret(hMsg, NULL, 1, 15);
		flag_for_Drag = false;
		Msg = nullMsg;
		sel_Msg = 0;
		Mp = old_pt = pt_for_drag = { 0, 0 };
		IsSel = false;
		Flag_LusMsgBox = true;
		break;

	case WM_GETVALUE:
		switch (wParam)
		{
		case wGETMSG:
			Msg = Lus_InputBoxStorage;
			Lus_InputBoxStorage = nullMsg;
			HDC hdc = GetDC(Msg.hWnd);
			HFONT mainFont = CreateFont((Msg.FontSize == -1) ? DEFAULT_FontSize : Msg.FontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
			SelectObject(hdc, mainFont);
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			int fontSize = tm.tmHeight;

			RECT font_rc = { 0, 0, 1000, 1000 };
			DrawText(hdc, (lstrlen(Msg.YesBox) > lstrlen(Msg.NoBox)) ? Msg.YesBox : Msg.NoBox, -1,
				&font_rc, DT_CALCRECT);


			rc_Msg_1 = {
				Msg.MsgSize.right - (MsgBlank + 4) - font_rc.right - 16,
				MsgBlank + fontSize + 6,
				Msg.MsgSize.right - (MsgBlank + 2),
				MsgBlank + fontSize + 6 + ButtonHeight
			};
			rc_Msg_2 = {
				Msg.MsgSize.right - (MsgBlank + 4) - font_rc.right - 16,
				rc_Msg_1.bottom + 2,
				Msg.MsgSize.right - (MsgBlank + 2),
				rc_Msg_1.bottom + 2 + ButtonHeight
			};

			DeleteObject(mainFont);
			ReleaseDC(Msg.hWnd, hdc);

			rc_Insert = { MsgBlank + 4, Msg.MsgSize.bottom - ButtonHeight,
				Msg.MsgSize.right - MsgBlank - 4, Msg.MsgSize.bottom - (MsgBlank + 4) };
			break;
		}
		break;
	case WM_LBUTTONDOWN:

		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);
		IsSel = false;
		sel_Msg = 0;


		if (PtInRect(&rc_Insert, Mp))
		{
			SetCaretPos(Csize.cx + rc_Insert.left + 5, rc_Insert.top + Csize.cy / 2);
			ShowCaret(hMsg);
			break;
		}
		else
			HideCaret(hMsg);

		// 따로 함수로 빼야함...
		if (PtInRect(&rc_Msg_1, Mp))
		{
			sel_Msg = 1;
			IsSel = true;
			InvalidateRect(hMsg, NULL, false);
			break;
		}
		else if (PtInRect(&rc_Msg_2, Mp))
		{
			sel_Msg = 2;
			IsSel = true;
			InvalidateRect(hMsg, NULL, false);
			break;
		}

		flag_for_Drag = true;
		SetCapture(hMsg);

		GetCursorPos(&Mp);
		pt_for_drag = old_pt = Mp;

		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);

		old_pt = old_pt - Mp;

		break;

	case WM_MOUSEMOVE:
		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);

		if (flag_for_Drag)
		{
			GetCursorPos(&Mp);
			old_pt.x = old_pt.x + Mp.x - pt_for_drag.x;
			old_pt.y = old_pt.y + Mp.y - pt_for_drag.y;

			SetWindowPos(hMsg, 0,
				old_pt.x, old_pt.y,
				0, 0, SWP_NOSIZE | SWP_NOREDRAW | SWP_NOZORDER);

			pt_for_drag = Mp;
			sel_Msg = 0;
			break;
		}

		if (IsSel == true)
			if (PtInRect(&rc_Msg_1, Mp))
				sel_Msg = 1;
			else if (PtInRect(&rc_Msg_2, Mp))
				sel_Msg = 2;
			else
				sel_Msg = 0;

			InvalidateRect(hMsg, NULL, false);
			break;


	case WM_LBUTTONUP:

		Mp.x = LOWORD(lParam);
		Mp.y = HIWORD(lParam);
		flag_for_Drag = false;
		ReleaseCapture();
		if (!IsSel) break;
		sel_Msg = 0;
		IsSel = false;


		InvalidateRect(hMsg, NULL, false);

		if (PtInRect(&rc_Msg_1, Mp))
		{
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			(Msg.resultFunc)(Msg.thisClass, Msg.BoxValue);
			break;
		}
		else if (PtInRect(&rc_Msg_2, Mp))
		{
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			Msg.undoFunc(Msg.thisClass);
			break;
		}
		break;

	case WM_RBUTTONDOWN:

		SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
		SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
		EnableWindow(Msg.hWnd, true);
		Flag_LusMsgBox = false;
		Msg.undoFunc(Msg.thisClass);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_RETURN:
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			(Msg.resultFunc)(Msg.thisClass, Msg.BoxValue);
			break;
		case VK_ESCAPE:
			SetWindowPos(Msg.hWnd, HWND_TOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			SetWindowPos(Msg.hWnd, HWND_DESKTOP, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOREDRAW);
			EnableWindow(Msg.hWnd, true);
			Flag_LusMsgBox = false;
			Msg.undoFunc(Msg.thisClass);
			break;
		}
		break;
	case WM_CHAR:
		int len_BoxValue;
		len_BoxValue = lstrlen(Msg.BoxValue);
		if (len_BoxValue < Msg.InputMax && (
			sideCheck(48, wParam, 58) || wParam > 65 || wParam == 46))
		{
			if (Msg.IsOnlyUseNum && wParam > 65) break;
			Msg.BoxValue[len_BoxValue] = wParam;//입력받은값은wParam에있습니다.
			Msg.BoxValue[len_BoxValue + 1] = 0;

			SendMessage(hMsg, WM_PAINT, wParam, lParam);
			SetCaretPos(Csize.cx + rc_Insert.left + 5, rc_Insert.top + Csize.cy / 2);
			ShowCaret(hMsg);
			InvalidateRect(hMsg, NULL, false);
			break;
		}
		if (wParam == 8)
		{
			if (len_BoxValue > 0)
				Msg.BoxValue[len_BoxValue - 1] = NULL;
			SendMessage(hMsg, WM_PAINT, wParam, lParam);
			SetCaretPos(Csize.cx + rc_Insert.left + 5, rc_Insert.top + Csize.cy / 2);
			ShowCaret(hMsg);
			InvalidateRect(hMsg, NULL, false);
		}
		break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hMsg, &ps);
		HDC LayDC = CreateCompatibleDC(hdc);
		HBITMAP Lay = CreateCompatibleBitmap(hdc, wndRect.right, wndRect.bottom);
		SelectObject(LayDC, Lay);
		HFONT mainFont = CreateFont((Msg.FontSize == -1) ? DEFAULT_FontSize : Msg.FontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
		SelectObject(LayDC, mainFont);
		SetTextColor(LayDC, Msg.TextColor);
		SelectObject(LayDC, (HPEN)GetStockObject(WHITE_PEN));
		SetBkMode(LayDC, TRANSPARENT);
		RECT lprc;

		TEXTMETRIC tm;
		GetTextMetrics(LayDC, &tm);
		int fontSize = tm.tmHeight;

		SetDCBrushColor(LayDC, Msg.MsgColor);
		FillRect(LayDC, &wndRect, (HBRUSH)GetStockObject(DC_BRUSH));

		lprc = SizeDown(wndRect, MsgBlank);
		FrameRect(LayDC, &lprc, (HBRUSH)GetStockObject(WHITE_BRUSH));

		lprc = { MsgBlank + 2, MsgBlank + 2,
			Msg.MsgSize.right - MsgBlank - 2, lprc.top + fontSize + 2 };

		DrawText(LayDC, Msg.Title, -1, &lprc, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		MoveToEx(LayDC, MsgBlank + 1, lprc.bottom, NULL);
		LineTo(LayDC, Msg.MsgSize.right - MsgBlank - 1, lprc.bottom);

		lprc = { MsgBlank + 2, lprc.bottom + 2,
			rc_Msg_1.left - 2,
			Msg.MsgSize.bottom - ButtonHeight - 2 };

		DrawText(LayDC, Msg.Msg, -1, &lprc,
			DT_CENTER | DT_VCENTER | DT_WORDBREAK);

		lprc = SizeDown(rc_Msg_1, 1);
		DrawLineBox(LayDC, lprc, Msg.MsgColor, LusColor::White, 1, DL_FRAME);

		if (sel_Msg == 1)
		{
			lprc = SizeDown(lprc, 4);
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.AlphaFormat = 0;
			bf.SourceConstantAlpha = ALPHA_SELECT;
			HBITMAP blendLay = CreateCompatibleBitmap(LayDC, 10, 10);
			HDC blendDC = CreateCompatibleDC(LayDC);
			SelectObject(blendDC, blendLay);
			SetDCBrushColor(blendDC, LusColor::White);
			RECT rc_for_blend = { 0, 0, 10, 10 };
			FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

			AlphaBlend(LayDC, lprc.left, lprc.top,
				lprc.right - lprc.left, lprc.bottom - lprc.top, blendDC, 0, 0, 10, 10, bf);

			DeleteDC(blendDC);
			DeleteObject(blendLay);

		}

		DrawText(LayDC, Msg.YesBox, -1, &lprc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		lprc = SizeDown(rc_Msg_2, 1);
		DrawLineBox(LayDC, lprc, Msg.MsgColor, LusColor::White, 1, DL_FRAME);

		if (sel_Msg == 2)
		{
			lprc = SizeDown(lprc, 4);
			BLENDFUNCTION bf;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			bf.AlphaFormat = 0;
			bf.SourceConstantAlpha = ALPHA_SELECT;
			HBITMAP blendLay = CreateCompatibleBitmap(LayDC, 10, 10);
			HDC blendDC = CreateCompatibleDC(LayDC);
			SelectObject(blendDC, blendLay);
			SetDCBrushColor(blendDC, LusColor::White);
			RECT rc_for_blend = { 0, 0, 10, 10 };
			FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

			AlphaBlend(LayDC, lprc.left, lprc.top,
				lprc.right - lprc.left, lprc.bottom - lprc.top, blendDC, 0, 0, 10, 10, bf);

			DeleteDC(blendDC);
			DeleteObject(blendLay);
		}

		DrawText(LayDC, Msg.NoBox, -1, &lprc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		FillRect(LayDC, &rc_Insert, (HBRUSH)GetStockObject(WHITE_BRUSH));
		GetTextExtentPoint32(LayDC, Msg.BoxValue, lstrlen(Msg.BoxValue), &Csize);
		lprc = rc_Insert + MakePoint(5, 0);
		SetTextColor(LayDC, Msg.MsgColor);
		DrawText(LayDC, Msg.BoxValue, -1, &lprc, DT_VCENTER | DT_SINGLELINE);

		BitBlt(hdc, 0, 0, wndRect.right, wndRect.bottom, LayDC, 0, 0, SRCCOPY);

		DeleteObject(mainFont);
		DeleteObject(Lay);
		DeleteDC(LayDC);
		EndPaint(hMsg, &ps);
	}
	break;


	case WM_DESTROY:
		break;

	default:
		return DefWindowProc(hMsg, uMsg, wParam, lParam);
	}
	return 0;
}

void LusInputMsgBox(HWND hWnd, LusInputMsg Msg, HINSTANCE g_hInst, int alpha_message)
{
	static WNDCLASSEX wc;
	static bool Flag_first_Register = false;


	int	msgWidth = 100;
	int	msgHeight = 100;

	RECT wndRect;
	GetWindowRect(hWnd, &wndRect);

	POINT wndcenter = { wndRect.right / 2, wndRect.bottom / 2 };


	if (hLMg)
	{
		DestroyWindow(hLMg);
		hLMg = nullptr;
	}

	LusInputMsg SendMsg = Msg;
	SendMsg.MsgSize.right = max(SendMsg.MsgSize.right, msgWidth);

	// Msg Height Init
	{

		HDC hdc, LayDC;
		hdc = GetDC(NULL);
		LayDC = CreateCompatibleDC(hdc);
		HBITMAP Lay = CreateCompatibleBitmap(LayDC, 1, 1);
		SelectObject(LayDC, Lay);
		HFONT mainFont = CreateFont((SendMsg.FontSize == -1) ? DEFAULT_FontSize : SendMsg.FontSize, 0, 0, 0, 700, FALSE, FALSE, 0,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial"));
		SelectObject(LayDC, mainFont);

		RECT font_rc = { 0, 0, 1000, 1000 };

		DrawText(hdc, (lstrlen(Msg.YesBox) > lstrlen(Msg.NoBox)) ? Msg.YesBox : Msg.NoBox, -1,
			&font_rc, DT_CALCRECT);
		font_rc.right += 16;
		RECT lprc = {
			MsgBlank + 2,
			0,
			max(100, SendMsg.MsgSize.right - (MsgBlank + 6) - font_rc.right),
			1000 };

		TEXTMETRIC tm;
		GetTextMetrics(LayDC, &tm);
		int fontSize = tm.tmHeight;
		int AdjHeight =
			max(MsgBlank + fontSize + 14 + ButtonHeight, DrawText(LayDC, SendMsg.Msg, -1, &lprc, DT_LEFT | DT_WORDBREAK)) + 
			(MsgBlank * 2 + fontSize + ButtonHeight + 8);
		//

		DrawTextEx(LayDC, SendMsg.Msg, -1, &lprc, DT_CALCRECT, nullptr);

		SendMsg.MsgSize.bottom = max(msgHeight, AdjHeight);
		SendMsg.MsgSize.right = max(SendMsg.MsgSize.right + MsgBlank + 2, 2 * MsgBlank + 4 + SendMsg.InputSize);
		//		lprc = SendMsg.MsgSize;

		DeleteObject(mainFont);
		ReleaseDC(NULL, hdc);
		ReleaseDC(NULL, LayDC);
		DeleteObject(Lay);
	}

	if (!Flag_first_Register)
	{
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = LusInputMsgProc;
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszClassName = TEXT("LusInputClass");

		RegisterClassEx(&wc);
	}



	RECT getClientSize;
	GetWindowRect(SendMsg.hWnd, &getClientSize);

	hLMg = CreateWindow(wc.lpszClassName, NULL, WS_POPUP | WS_VISIBLE,
		getClientSize.left + SendMsg.MsgStartPos.x - SendMsg.MsgSize.right / 2,
		getClientSize.top + SendMsg.MsgStartPos.y - SendMsg.MsgSize.bottom / 2,
		SendMsg.MsgSize.right, SendMsg.MsgSize.bottom, hWnd, NULL, g_hInst, NULL);

	SetWindowLong(hLMg, GWL_EXSTYLE,
		GetWindowLong(hLMg, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hLMg, 0, alpha_message, LWA_ALPHA);

	Lus_InputBoxStorage = SendMsg;
	SendMessage(hLMg, WM_GETVALUE, wGETMSG, 0);
	EnableWindow(SendMsg.hWnd, false);

}


void SendMessageToLusMsg_ReDraw()
{
	LusMsgProc(hLMg, WM_GETVALUE, wGETRENEWALSCREEN, 0);
}
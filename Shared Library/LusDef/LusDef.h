#pragma once

/*
*			LusMsg 사용하기 : 
*
			LusMsg Msg;
			Msg.hWnd = m_pd2dRenderTarget->GetHwnd();
			Msg.Title = TEXT("Title";
			Msg.Msg = TEXT("only use test";
			Msg.MsgStartPos = MakePoint(CenterPoint);
			Msg.InBoxMsg_1 = TEXT("Test";
			Msg.thisClass = this;
			Msg.Msg_1_Func = [](void* Class){
				close_LusMsgProc();
				//	static_cast<CScene*>(Class);
			};

			LusMsgBox(Msg.hWnd, Msg, hInst, 175);
*
*
*/

#pragma comment(lib,"msimg32")
#pragma warning(disable:4244)
#include <iostream>
#include <string>

using namespace std;

#define WM_GETVALUE 0x401
#define wGETMSG		(WPARAM)0x001
#define wGETINITFLAG	(WPARAM)0x002
#define wGETRENEWALSCREEN (WPARAM)0x003
#define sideCheck(_min, _val, _max)	( ( ( (_min) <= (_val) ) && ( (_val) < (_max) ) ) ? true : false)

#define ALPHA_SELECT	140

// FontSize Part
#define DEFAULT_FontSize 15
#define TITLE_FontSize 80
#define LARGE_FontSize 25

#define DEFAULT_MSGWIDTH 170
#define DEFAULT_MSGHEIGHT 180

// DrawLine
#define DL_FILL true
#define DL_FRAME false

#define MsgBlank 5
#define MsgWidth 160
#define ButtonHeight 35

// Color Box

typedef enum LusColor{
	AliceBlue = 0xF0F8FF,
	AntiqueWhite = 0xFAEBD7,
	Aqua = 0x00FFFF,
	Aquamarine = 0x7FFFD4,
	Azure = 0xF0FFFF,
	Beige = 0xF5F5DC,
	Bisque = 0xFFE4C4,
	Black = 0x000000,
	BlanchedAlmond = 0xFFEBCD,
	Blue = 0x0000FF,
	BlueViolet = 0x8A2BE2,
	Brown = 0xA52A2A,
	BurlyWood = 0xDEB887,
	CadetBlue = 0x5F9EA0,
	Chartreuse = 0x7FFF00,
	Chocolate = 0xD2691E,
	Coral = 0xFF7F50,
	CornflowerBlue = 0x6495ED,
	Cornsilk = 0xFFF8DC,
	Crimson = 0xDC143C,
	Cyan = 0x00FFFF,
	DarkBlue = 0x00008B,
	DarkCyan = 0x008B8B,
	DarkGoldenrod = 0xB8860B,
	DarkGray = 0xA9A9A9,
	DarkGreen = 0x006400,
	DarkKhaki = 0xBDB76B,
	DarkMagenta = 0x8B008B,
	DarkOliveGreen = 0x556B2F,
	DarkOrange = 0xFF8C00,
	DarkOrchid = 0x9932CC,
	DarkRed = 0x8B0000,
	DarkSalmon = 0xE9967A,
	DarkSeaGreen = 0x8FBC8F,
	DarkSlateBlue = 0x483D8B,
	DarkSlateGray = 0x2F4F4F,
	DarkTurquoise = 0x00CED1,
	DarkViolet = 0x9400D3,
	DeepPink = 0xFF1493,
	DeepSkyBlue = 0x00BFFF,
	DimGray = 0x696969,
	DodgerBlue = 0x1E90FF,
	Firebrick = 0xB22222,
	FloralWhite = 0xFFFAF0,
	ForestGreen = 0x228B22,
	Fuchsia = 0xFF00FF,
	Gainsboro = 0xDCDCDC,
	GhostWhite = 0xF8F8FF,
	Gold = 0xFFD700,
	Goldenrod = 0xDAA520,
	Gray = 0x808080,
	Green = 0x008000,
	GreenYellow = 0xADFF2F,
	Honeydew = 0xF0FFF0,
	HotPink = 0xFF69B4,
	IndianRed = 0xCD5C5C,
	Indigo = 0x4B0082,
	Ivory = 0xFFFFF0,
	Khaki = 0xF0E68C,
	Lavender = 0xE6E6FA,
	LavenderBlush = 0xFFF0F5,
	LawnGreen = 0x7CFC00,
	LemonChiffon = 0xFFFACD,
	LightBlue = 0xADD8E6,
	LightCoral = 0xF08080,
	LightCyan = 0xE0FFFF,
	LightGoldenrodYellow = 0xFAFAD2,
	LightGreen = 0x90EE90,
	LightGray = 0xD3D3D3,
	LightPink = 0xFFB6C1,
	LightSalmon = 0xFFA07A,
	LightSeaGreen = 0x20B2AA,
	LightSkyBlue = 0x87CEFA,
	LightSlateGray = 0x778899,
	LightSteelBlue = 0xB0C4DE,
	LightYellow = 0xFFFFE0,
	Lime = 0x00FF00,
	LimeGreen = 0x32CD32,
	Linen = 0xFAF0E6,
	Magenta = 0xFF00FF,
	Maroon = 0x800000,
	MediumAquamarine = 0x66CDAA,
	MediumBlue = 0x0000CD,
	MediumOrchid = 0xBA55D3,
	MediumPurple = 0x9370DB,
	MediumSeaGreen = 0x3CB371,
	MediumSlateBlue = 0x7B68EE,
	MediumSpringGreen = 0x00FA9A,
	MediumTurquoise = 0x48D1CC,
	MediumVioletRed = 0xC71585,
	MidnightBlue = 0x191970,
	MintCream = 0xF5FFFA,
	MistyRose = 0xFFE4E1,
	Moccasin = 0xFFE4B5,
	NavajoWhite = 0xFFDEAD,
	Navy = 0x000080,
	OldLace = 0xFDF5E6,
	Olive = 0x808000,
	OliveDrab = 0x6B8E23,
	Orange = 0xFFA500,
	OrangeRed = 0xFF4500,
	Orchid = 0xDA70D6,
	PaleGoldenrod = 0xEEE8AA,
	PaleGreen = 0x98FB98,
	PaleTurquoise = 0xAFEEEE,
	PaleVioletRed = 0xDB7093,
	PapayaWhip = 0xFFEFD5,
	PeachPuff = 0xFFDAB9,
	Peru = 0xCD853F,
	Pink = 0xFFC0CB,
	Plum = 0xDDA0DD,
	PowderBlue = 0xB0E0E6,
	Purple = 0x800080,
	Red = 0xFF0000,
	RosyBrown = 0xBC8F8F,
	RoyalBlue = 0x4169E1,
	SaddleBrown = 0x8B4513,
	Salmon = 0xFA8072,
	SandyBrown = 0xF4A460,
	SeaGreen = 0x2E8B57,
	SeaShell = 0xFFF5EE,
	Sienna = 0xA0522D,
	Silver = 0xC0C0C0,
	SkyBlue = 0x87CEEB,
	SlateBlue = 0x6A5ACD,
	SlateGray = 0x708090,
	Snow = 0xFFFAFA,
	SpringGreen = 0x00FF7F,
	SteelBlue = 0x4682B4,
	Tan = 0xD2B48C,
	Teal = 0x008080,
	Thistle = 0xD8BFD8,
	Tomato = 0xFF6347,
	Turquoise = 0x40E0D0,
	Violet = 0xEE82EE,
	Wheat = 0xF5DEB3,
	White = 0xFFFFFF,
	WhiteSmoke = 0xF5F5F5,
	Yellow = 0xFFFF00,
	YellowGreen = 0x9ACD32,
	MurkyGray = 0x5F5F5F,
}LusColor;

/*
*	재설계합니다.
*	Message를 보내는 클래스입니다. ValueBox와 연계가 가능하면 좋겠지만.
*
*
*
*
*
*/
/*
class CLusMsg{
protected:
	HWND			m_hWnd;
	HINSTANCE		m_hInst;

	POINT			m_ptMsgWorldStart;
	RECT			m_rcMsg;

	wstring			m_strTitle;
	wstring			m_strMsg;

	COLORREF		m_clrMsgColor;
	COLORREF		m_clrTextColor;

	void			*thisClass;
	int				m_iAlphaLevel;
private:
	
	wstring			m_strBt1;
	wstring			m_strBt2;

	bool			m_bUseBt2;
	
	int				m_iFontSize;

	void(*Msg_1_Func)(void *Class);
	void(*Msg_2_Func)(void *Class);

public:	// value

public:	// function

	void Initialize(HWND hWnd, HINSTANCE hInst, bool bUseBt2, int FontSize,
		COLORREF TextColor = LusColor::White, COLORREF MsgBkColor = LusColor::Gray,
		POINT ptStart = MakePoint(-1, -1), RECT rcSize = MakeRect(0, 0, 0, 0))
	{
		m_hWnd = hWnd;
		m_hInst = hInst;

		//  기본적으로 가운데로 잡습니다. 추후 수정됩니다.
		if (ptStart == MakePoint(-1, -1))
		{
			RECT rcDesktop;
			GetWindowRect(hWnd, &rcDesktop);
			m_ptMsgWorldStart = MakePoint(
				(rcDesktop.right - rcDesktop.left) * 0.5,
				(rcDesktop.bottom - rcDesktop.top) * 0.5);
		}
		else 
			m_ptMsgWorldStart = ptStart;

		m_rcMsg = rcSize;
		m_iFontSize = FontSize;
		m_bUseBt2 = bUseBt2;

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


		if (m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}

		LusMsg SendMsg = Msg;

		if (!SendMsg.bUseMsg_2)
			SendMsg.InBoxMsg_2 = TEXT("";

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
				DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, TEXT("Arial");
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

		m_hWnd = CreateWindow(wc.lpszClassName, NULL, WS_POPUP | WS_VISIBLE,
			getClientSize.left + SendMsg.MsgStartPos.x - SendMsg.MsgSize.right / 2,
			getClientSize.top + SendMsg.MsgStartPos.y - SendMsg.MsgSize.bottom / 2,
			SendMsg.MsgSize.right, SendMsg.MsgSize.bottom, hWnd, NULL, g_hInst, NULL);

		SetWindowLong(m_hWnd, GWL_EXSTYLE,
			GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(m_hWnd, 0, alpha_message, LWA_ALPHA);
		iMsgAlpha = alpha_message;
		Lus_MsgStorage = SendMsg;
		SendMessage(m_hWnd, WM_GETVALUE, wGETMSG, 0);
		EnableWindow(SendMsg.hWnd, false);

	}
};

*/





struct tag_LusMsg
{
	//[필수] hWnd : 현재 윈도우 핸들
	HWND			hWnd;
	//[필수] MsgStartPos : 메세지박스의 "중심" 위치입니다.
	POINT			MsgStartPos;
	//[선택] MsgSize : 메세지박스의 크기입니다.
	//　　　 　　　　　내용이 많을 경우 자동 조정됩니다.
	//　　　 　　　　　생략해도 정상적으로 메세지박스가 생성됩니다.
	RECT			MsgSize;
	//[필수] Title : 메세지박스의 캡션 바에 표시될 문장입니다.
	TCHAR			*Title;
	//[필수] Msg : 메세지박스의 내용입니다.
	TCHAR			*Msg;
	//[필수] InBoxMsg_1 : 메세지박스의 첫번째 선택지입니다.필수입니다.
	TCHAR			*InBoxMsg_1;
	//[선택] InBoxMsg_2 : 메세지박스의 두번째 선택지입니다. 만약 선택지를 하나만 쓰면 생략해도 됩니다.
	TCHAR			*InBoxMsg_2;
	//[선택] bUseMsg_2 : 두번째 선택지를 사용할지를 결정합니다. 기본은 쓰지 않습니다.
	bool			bUseMsg_2;
	//[선택] TextColor : 표시될 텍스트 구문의 색상입니다.기본은 흰색입니다.
	COLORREF		TextColor;
	//[선택] MsgColor : 표시될 텍스트 구문의 색상입니다. 기본은 흰색입니다.
	COLORREF		MsgColor;
	//[선택] FontSize : 폰트 사이즈입니다.생략이 가능합니다.
	int				FontSize;
	//[선택] thisClass : 메세지박스가 호출된 값을 반환받는 "클래스"입니다.
	void			*thisClass;
	//[필수] Msg_1_Func : 첫번째 선택지를 누를 때 호출되는 함수입니다.
	void			(*Msg_1_Func)(void *Class);
	//[선택] Msg_2_Func : 두번째 선택지를 누를 때 호출되는 함수입니다.
	void			(*Msg_2_Func)(void *Class);
	tag_LusMsg()	{
		hWnd = NULL; 
		MsgSize = { 0, 0, DEFAULT_MSGWIDTH, DEFAULT_MSGHEIGHT };
		bUseMsg_2 = false;
		MsgStartPos = { 0, 0 };
		FontSize = -1; 
		thisClass = NULL;
		MsgColor = LusColor::MurkyGray;
		TextColor = LusColor::White;
	}
};

struct tag_LusInputMsg
{
	//[필수] hWnd : 현재 윈도우 핸들
	HWND				hWnd;
	//[필수] MsgStartPos : 메세지박스의 "중심" 위치입니다.
	POINT				MsgStartPos;
	//[선택] MsgSize : 메세지박스의 크기입니다.
	//내용이 많을 경우 자동 조정됩니다.
	//생략해도 정상적으로 메세지박스가 생성됩니다.
	RECT				MsgSize;
	//[필수] Title : 메세지박스의 캡션 바에 표시될 문장입니다.
	TCHAR				*Title;
	//[필수] Msg : 메세지박스의 내용입니다.
	TCHAR				*Msg;
	//[선택] YesBox : 값 입력을 승인하는 선택지입니다.기본은 "확인"입니다.
	TCHAR				*YesBox;
	//[선택] NoBox : 값 입력을 취소하는 선택지입니다.기본은 "취소"입니다.
	TCHAR				*NoBox;
	//[필수] BoxValue : 입력받은 값을 저장하는 문자열입니다.
	TCHAR				BoxValue[256];
	//[선택] TextColor : 표시될 텍스트 구문의 색상입니다.기본은 흰색입니다.
	COLORREF			TextColor;
	//[선택] MsgColor : 표시될 텍스트 구문의 색상입니다.기본은 흰색입니다.
	COLORREF			MsgColor;
	//[선택] FontSize : 폰트 사이즈입니다.생략이 가능합니다.
	int					FontSize;
	//[필수] InputSize : 값 입력 박스의 너비입니다.
	int					InputSize;
	//[필수] InputMax : 입력받는 값의 최대 개수입니다.
	//"LusInputMsg"는 InputMax가 12 이상이어야 입력할 수 있습니다.
	int					InputMax;
	//[선택] IsOnlyUseNum : 숫자만 입력할 수 있는지를 체크합니다. 기본은 문자도 입력가능(false)입니다.
	bool				IsOnlyUseNum;
	//[선택] thisClass : 메세지박스가 호출된 값을 반환받는 "클래스"입니다.
	void				*thisClass;
	//[필수] resultFunc : 값을 입력했을 때 호출되는 함수입니다.
	void				(*resultFunc)(void *Class, TCHAR *output);
	//[필수] undoFunc : 취소했을 때 호출되는 함수입니다.
	void				(*undoFunc)(void *Class);
	tag_LusInputMsg(){
		hWnd = NULL;
		MsgSize = { 0, 0, DEFAULT_MSGWIDTH, DEFAULT_MSGHEIGHT };
		MsgStartPos = { 0, 0 };
		FontSize = -1;
		thisClass = NULL;
		IsOnlyUseNum = false;
		YesBox = TEXT("확인");
		NoBox = TEXT("취소");
		BoxValue[0] = '\0';
		InputMax = 256;
		MsgColor = LusColor::MurkyGray;
		TextColor = LusColor::White;
	}
};

/*
*	메세지박스의 속성을 설정할 수 있습니다.
*	[필수] hWnd : 현재 윈도우 핸들
*	[필수] MsgStartPos : 메세지박스의 "중심" 위치입니다.
*	[선택] MsgSize : 메세지박스의 크기입니다.
*	　　　 　　　　　내용이 많을 경우 자동 조정됩니다.
*	　　　 　　　　　생략해도 정상적으로 메세지박스가 생성됩니다.
*	[필수] Title : 메세지박스의 캡션 바에 표시될 문장입니다.
*	[필수] Msg : 메세지박스의 내용입니다.
*	[필수] InBoxMsg_1 : 메세지박스의 첫번째 선택지입니다. 필수입니다.
*	[선택] InBoxMsg_2 : 메세지박스의 두번째 선택지입니다.
*	　　　 　　　　　　 만약 선택지를 하나만 쓰면 생략해도 됩니다.
*	[선택] bUseMsg_2 : 두번째 선택지를 사용할지를 결정합니다. 기본은 쓰지 않습니다.
*	[선택] TextColor : 표시될 텍스트 구문의 색상입니다. 기본은 흰색입니다.
*	[선택] MsgColor : 표시될 텍스트 구문의 색상입니다. 기본은 흰색입니다.
*	[선택] FontSize : 폰트 사이즈입니다. 생략이 가능합니다.
*	[선택] thisClass : 메세지박스가 호출된 값을 반환받는 "클래스"입니다.
*	[필수] Msg_1_Func : 첫번째 선택지를 누를 때 호출되는 함수입니다.
*	[선택] Msg_2_Func : 두번째 선택지를 누를 때 호출되는 함수입니다.
*/
typedef tag_LusMsg LusMsg;

/*
*	메세지박스를 생성하는 함수입니다.
*	LusMsgProc와 LusInputMsgBox는는 프로그램 당 한 번에 하나만 생성시킬 수 있으며
*	MessageBox와 다른 로직으로 동작합니다.
*	하지만 MessageBox처럼 이 메세지를 무시할 수 없습니다.
*
*	hWnd : 현재 윈도우 핸들
*	Msg : 설정한 메세지박스 속성
*	g_hInst : 현재 프로그램의 인스턴스 핸들
*	alpha_message : 메세지박스의 투명도입니다. 0 ~ 255 사이의 값.
*/
void LusMsgBox(HWND hWnd, LusMsg Msg, HINSTANCE g_hInst, int alpha_message);

/*
*	메세지박스의 속성을 설정할 수 있습니다.
*	[필수] hWnd : 현재 윈도우 핸들
*	[필수] MsgStartPos : 메세지박스의 "중심" 위치입니다.
*	[선택] MsgSize : 메세지박스의 크기입니다.
*	　　　 　　　　　내용이 많을 경우 자동 조정됩니다.
*	　　　 　　　　　생략해도 정상적으로 메세지박스가 생성됩니다.
*	[필수] Title : 메세지박스의 캡션 바에 표시될 문장입니다.
*	[필수] Msg : 메세지박스의 내용입니다.
*	[선택] YesBox : 값 입력을 승인하는 선택지입니다. 기본은 "확인"입니다.
*	[선택] NoBox : 값 입력을 취소하는 선택지입니다. 기본은 "취소"입니다.
*	[필수] BoxValue : 입력받은 값을 저장하는 문자열입니다.
*	[선택] TextColor : 표시될 텍스트 구문의 색상입니다. 기본은 흰색입니다.
*	[선택] MsgColor : 표시될 텍스트 구문의 색상입니다. 기본은 흰색입니다.
*	[선택] FontSize : 폰트 사이즈입니다. 생략이 가능합니다.
*	[필수] InputSize : 값 입력 박스의 너비입니다.
*	[필수] InputMax : 입력받는 값의 최대 개수입니다.
*	　　　　　　　　　"LusInputMsg"는 InputMax가 12 이상이어야 입력할 수 있습니다.
*	[선택] IsOnlyUseNum : 숫자만 입력할 수 있는지를 체크합니다. 기본은 문자도 입력가능(false)입니다.
*	[선택] thisClass : 메세지박스가 호출된 값을 반환받는 "클래스"입니다.
*	[필수] resultFunc : 값을 입력했을 때 호출되는 함수입니다.
*	[필수] undoFunc : 취소했을 때 호출되는 함수입니다.
*/
typedef tag_LusInputMsg LusInputMsg;

/*
*	값을 넣을 수 있는 메세지박스를 생성하는 함수입니다.
*	LusMsgProc와 LusInputMsgBox는 프로그램 당 한 번에 하나만 생성시킬 수 있습니다.
*	MessageBox처럼 이 메세지를 무시할 수 없습니다.
*
*	hWnd : 현재 윈도우 핸들
*	Msg : 설정한 메세지박스 속성
*	g_hInst : 현재 프로그램의 인스턴스 핸들
*	alpha_message : 메세지박스의 투명도입니다. 0 ~ 255 사이의 값.
*/
void LusInputMsgBox(HWND hWnd, LusInputMsg Msg, HINSTANCE g_hInst, int alpha_message);

POINT operator+(POINT a, const POINT b);
POINT operator-(POINT a, const POINT b);
POINT operator*(float b, POINT a);
bool operator==(POINT p1, POINT p2);
bool operator!=(POINT p1, POINT p2);
RECT operator+(RECT rc, POINT pt);
RECT operator+(POINT pt, RECT rc);
RECT operator+(RECT rc_1, RECT rc_2);
RECT operator-(RECT rc_1, RECT rc_2);

void close_LusMsgProc();
void SendMessageToLusMsg_ReDraw();

void DrawLineBox(HDC mainDC, RECT rc, COLORREF main_RGB, COLORREF Line_RGB, int LineLen, bool Type);

void FixValue(LPWSTR DestMsg, int fixnum);
inline void ColorBlt(HDC hDC, const RECT &rc, COLORREF color, int alphaValue)
{
	HDC blendDC = CreateCompatibleDC(hDC);
	HBITMAP blendLay = CreateCompatibleBitmap(hDC, 10, 10);

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.AlphaFormat = 0;
	bf.SourceConstantAlpha = alphaValue;

	SelectObject(blendDC, blendLay);
	SetDCBrushColor(blendDC, color);
	RECT rc_for_blend = { 0, 0, 10, 10 };
	FillRect(blendDC, &rc_for_blend, (HBRUSH)GetStockObject(DC_BRUSH));

	AlphaBlend(hDC, rc.left, rc.top,
		rc.right - rc.left, rc.bottom - rc.top, blendDC, 0, 0, 10, 10, bf);

	DeleteDC(blendDC);
	DeleteObject(blendLay);
}

template<typename Pt = D2D_POINT_2F> inline
POINT MakePoint(Pt pt) { POINT retpt; retpt.x = static_cast<LONG>(pt.x); retpt.y = static_cast<LONG>(pt.y); return retpt; }
template<typename _Ty = LONG> inline
POINT MakePoint(_Ty x, _Ty y) { POINT pt; pt.x = static_cast<LONG>(x); pt.y = static_cast<LONG>(y); return pt; }
template<typename Rect = D2D_RECT_F> inline
RECT MakeRect(Rect rc)
{
	RECT retrc;
	retrc.left = static_cast<LONG>(rc.left);
	retrc.top = static_cast<LONG>(rc.top);
	retrc.right = static_cast<LONG>(rc.right);
	retrc.bottom = static_cast<LONG>(rc.bottom);
	return retrc;
}
template<typename _Ty = LONG> inline
RECT MakeRect(_Ty left, _Ty top, _Ty right, _Ty bottom) 
{
	RECT rc;
	rc.left = static_cast<LONG>(left);
	rc.top = static_cast<LONG>(top);
	rc.right = static_cast<LONG>(right);
	rc.bottom = static_cast<LONG>(bottom);
	return rc; 
}

// template function List

template<typename Val> inline
Val sideStore(Val min, Val value, Val max)
{
	Val return_val = value;
	if (min > value) return_val = min;
	if (value > max) return_val = max;
	return return_val;
}

template<typename Val> inline
Val pow(Val under, unsigned int power = 2)
{
	Val rVal = 1;
	while (power > 0) rVal *= under;

	return rVal;
}

template<typename Rect = RECT> inline
Rect SizeDown(Rect rc, float len)
{
	len = min(rc.right / 2, len);
	rc.left		+= len;
	rc.top		+= len;
	rc.right	-= len;
	rc.bottom	-= len;
	return rc;
}

template <typename Val, typename Pt> inline
Val Length(Pt StartPoint, Pt EndPoint){ return sqrt(pow(StartPoint.x - EndPoint.x) + pow(StartPoint.y - EndPoint.y)); }

extern TCHAR Lus_strMsg[4096];
extern TCHAR Lus_strMsg_1[128];
extern TCHAR Lus_strMsg_2[128];
#pragma once

/*
*			LusMsg ����ϱ� : 
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
*	�缳���մϴ�.
*	Message�� ������ Ŭ�����Դϴ�. ValueBox�� ���谡 �����ϸ� ��������.
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

		//  �⺻������ ����� ����ϴ�. ���� �����˴ϴ�.
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
	//[�ʼ�] hWnd : ���� ������ �ڵ�
	HWND			hWnd;
	//[�ʼ�] MsgStartPos : �޼����ڽ��� "�߽�" ��ġ�Դϴ�.
	POINT			MsgStartPos;
	//[����] MsgSize : �޼����ڽ��� ũ���Դϴ�.
	//������ ���������������� ���� ��� �ڵ� �����˴ϴ�.
	//������ ���������������ص� ���������� �޼����ڽ��� �����˴ϴ�.
	RECT			MsgSize;
	//[�ʼ�] Title : �޼����ڽ��� ĸ�� �ٿ� ǥ�õ� �����Դϴ�.
	TCHAR			*Title;
	//[�ʼ�] Msg : �޼����ڽ��� �����Դϴ�.
	TCHAR			*Msg;
	//[�ʼ�] InBoxMsg_1 : �޼����ڽ��� ù��° �������Դϴ�.�ʼ��Դϴ�.
	TCHAR			*InBoxMsg_1;
	//[����] InBoxMsg_2 : �޼����ڽ��� �ι�° �������Դϴ�. ���� �������� �ϳ��� ���� �����ص� �˴ϴ�.
	TCHAR			*InBoxMsg_2;
	//[����] bUseMsg_2 : �ι�° �������� ��������� �����մϴ�. �⺻�� ���� �ʽ��ϴ�.
	bool			bUseMsg_2;
	//[����] TextColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�.�⺻�� ����Դϴ�.
	COLORREF		TextColor;
	//[����] MsgColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�. �⺻�� ����Դϴ�.
	COLORREF		MsgColor;
	//[����] FontSize : ��Ʈ �������Դϴ�.������ �����մϴ�.
	int				FontSize;
	//[����] thisClass : �޼����ڽ��� ȣ��� ���� ��ȯ�޴� "Ŭ����"�Դϴ�.
	void			*thisClass;
	//[�ʼ�] Msg_1_Func : ù��° �������� ���� �� ȣ��Ǵ� �Լ��Դϴ�.
	void			(*Msg_1_Func)(void *Class);
	//[����] Msg_2_Func : �ι�° �������� ���� �� ȣ��Ǵ� �Լ��Դϴ�.
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
	//[�ʼ�] hWnd : ���� ������ �ڵ�
	HWND				hWnd;
	//[�ʼ�] MsgStartPos : �޼����ڽ��� "�߽�" ��ġ�Դϴ�.
	POINT				MsgStartPos;
	//[����] MsgSize : �޼����ڽ��� ũ���Դϴ�.
	//������ ���� ��� �ڵ� �����˴ϴ�.
	//�����ص� ���������� �޼����ڽ��� �����˴ϴ�.
	RECT				MsgSize;
	//[�ʼ�] Title : �޼����ڽ��� ĸ�� �ٿ� ǥ�õ� �����Դϴ�.
	TCHAR				*Title;
	//[�ʼ�] Msg : �޼����ڽ��� �����Դϴ�.
	TCHAR				*Msg;
	//[����] YesBox : �� �Է��� �����ϴ� �������Դϴ�.�⺻�� "Ȯ��"�Դϴ�.
	TCHAR				*YesBox;
	//[����] NoBox : �� �Է��� ����ϴ� �������Դϴ�.�⺻�� "���"�Դϴ�.
	TCHAR				*NoBox;
	//[�ʼ�] BoxValue : �Է¹��� ���� �����ϴ� ���ڿ��Դϴ�.
	TCHAR				BoxValue[256];
	//[����] TextColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�.�⺻�� ����Դϴ�.
	COLORREF			TextColor;
	//[����] MsgColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�.�⺻�� ����Դϴ�.
	COLORREF			MsgColor;
	//[����] FontSize : ��Ʈ �������Դϴ�.������ �����մϴ�.
	int					FontSize;
	//[�ʼ�] InputSize : �� �Է� �ڽ��� �ʺ��Դϴ�.
	int					InputSize;
	//[�ʼ�] InputMax : �Է¹޴� ���� �ִ� �����Դϴ�.
	//"LusInputMsg"�� InputMax�� 12 �̻��̾�� �Է��� �� �ֽ��ϴ�.
	int					InputMax;
	//[����] IsOnlyUseNum : ���ڸ� �Է��� �� �ִ����� üũ�մϴ�. �⺻�� ���ڵ� �Է°���(false)�Դϴ�.
	bool				IsOnlyUseNum;
	//[����] thisClass : �޼����ڽ��� ȣ��� ���� ��ȯ�޴� "Ŭ����"�Դϴ�.
	void				*thisClass;
	//[�ʼ�] resultFunc : ���� �Է����� �� ȣ��Ǵ� �Լ��Դϴ�.
	void				(*resultFunc)(void *Class, TCHAR *output);
	//[�ʼ�] undoFunc : ������� �� ȣ��Ǵ� �Լ��Դϴ�.
	void				(*undoFunc)(void *Class);
	tag_LusInputMsg(){
		hWnd = NULL;
		MsgSize = { 0, 0, DEFAULT_MSGWIDTH, DEFAULT_MSGHEIGHT };
		MsgStartPos = { 0, 0 };
		FontSize = -1;
		thisClass = NULL;
		IsOnlyUseNum = false;
		YesBox = TEXT("Ȯ��");
		NoBox = TEXT("���");
		BoxValue[0] = '\0';
		InputMax = 256;
		MsgColor = LusColor::MurkyGray;
		TextColor = LusColor::White;
	}
};

/*
*	�޼����ڽ��� �Ӽ��� ������ �� �ֽ��ϴ�.
*	[�ʼ�] hWnd : ���� ������ �ڵ�
*	[�ʼ�] MsgStartPos : �޼����ڽ��� "�߽�" ��ġ�Դϴ�.
*	[����] MsgSize : �޼����ڽ��� ũ���Դϴ�.
*	������ ���������������� ���� ��� �ڵ� �����˴ϴ�.
*	������ ���������������ص� ���������� �޼����ڽ��� �����˴ϴ�.
*	[�ʼ�] Title : �޼����ڽ��� ĸ�� �ٿ� ǥ�õ� �����Դϴ�.
*	[�ʼ�] Msg : �޼����ڽ��� �����Դϴ�.
*	[�ʼ�] InBoxMsg_1 : �޼����ڽ��� ù��° �������Դϴ�. �ʼ��Դϴ�.
*	[����] InBoxMsg_2 : �޼����ڽ��� �ι�° �������Դϴ�.
*	������ ������������ ���� �������� �ϳ��� ���� �����ص� �˴ϴ�.
*	[����] bUseMsg_2 : �ι�° �������� ��������� �����մϴ�. �⺻�� ���� �ʽ��ϴ�.
*	[����] TextColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�. �⺻�� ����Դϴ�.
*	[����] MsgColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�. �⺻�� ����Դϴ�.
*	[����] FontSize : ��Ʈ �������Դϴ�. ������ �����մϴ�.
*	[����] thisClass : �޼����ڽ��� ȣ��� ���� ��ȯ�޴� "Ŭ����"�Դϴ�.
*	[�ʼ�] Msg_1_Func : ù��° �������� ���� �� ȣ��Ǵ� �Լ��Դϴ�.
*	[����] Msg_2_Func : �ι�° �������� ���� �� ȣ��Ǵ� �Լ��Դϴ�.
*/
typedef tag_LusMsg LusMsg;

/*
*	�޼����ڽ��� �����ϴ� �Լ��Դϴ�.
*	LusMsgProc�� LusInputMsgBox�´� ���α׷� �� �� ���� �ϳ��� ������ų �� ������
*	MessageBox�� �ٸ� �������� �����մϴ�.
*	������ MessageBoxó�� �� �޼����� ������ �� �����ϴ�.
*
*	hWnd : ���� ������ �ڵ�
*	Msg : ������ �޼����ڽ� �Ӽ�
*	g_hInst : ���� ���α׷��� �ν��Ͻ� �ڵ�
*	alpha_message : �޼����ڽ��� �����Դϴ�. 0 ~ 255 ������ ��.
*/
void LusMsgBox(HWND hWnd, LusMsg Msg, HINSTANCE g_hInst, int alpha_message);

/*
*	�޼����ڽ��� �Ӽ��� ������ �� �ֽ��ϴ�.
*	[�ʼ�] hWnd : ���� ������ �ڵ�
*	[�ʼ�] MsgStartPos : �޼����ڽ��� "�߽�" ��ġ�Դϴ�.
*	[����] MsgSize : �޼����ڽ��� ũ���Դϴ�.
*	������ ���������������� ���� ��� �ڵ� �����˴ϴ�.
*	������ ���������������ص� ���������� �޼����ڽ��� �����˴ϴ�.
*	[�ʼ�] Title : �޼����ڽ��� ĸ�� �ٿ� ǥ�õ� �����Դϴ�.
*	[�ʼ�] Msg : �޼����ڽ��� �����Դϴ�.
*	[����] YesBox : �� �Է��� �����ϴ� �������Դϴ�. �⺻�� "Ȯ��"�Դϴ�.
*	[����] NoBox : �� �Է��� ����ϴ� �������Դϴ�. �⺻�� "���"�Դϴ�.
*	[�ʼ�] BoxValue : �Է¹��� ���� �����ϴ� ���ڿ��Դϴ�.
*	[����] TextColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�. �⺻�� ����Դϴ�.
*	[����] MsgColor : ǥ�õ� �ؽ�Ʈ ������ �����Դϴ�. �⺻�� ����Դϴ�.
*	[����] FontSize : ��Ʈ �������Դϴ�. ������ �����մϴ�.
*	[�ʼ�] InputSize : �� �Է� �ڽ��� �ʺ��Դϴ�.
*	[�ʼ�] InputMax : �Է¹޴� ���� �ִ� �����Դϴ�.
*	������������������"LusInputMsg"�� InputMax�� 12 �̻��̾�� �Է��� �� �ֽ��ϴ�.
*	[����] IsOnlyUseNum : ���ڸ� �Է��� �� �ִ����� üũ�մϴ�. �⺻�� ���ڵ� �Է°���(false)�Դϴ�.
*	[����] thisClass : �޼����ڽ��� ȣ��� ���� ��ȯ�޴� "Ŭ����"�Դϴ�.
*	[�ʼ�] resultFunc : ���� �Է����� �� ȣ��Ǵ� �Լ��Դϴ�.
*	[�ʼ�] undoFunc : ������� �� ȣ��Ǵ� �Լ��Դϴ�.
*/
typedef tag_LusInputMsg LusInputMsg;

/*
*	���� ���� �� �ִ� �޼����ڽ��� �����ϴ� �Լ��Դϴ�.
*	LusMsgProc�� LusInputMsgBox�� ���α׷� �� �� ���� �ϳ��� ������ų �� �ֽ��ϴ�.
*	MessageBoxó�� �� �޼����� ������ �� �����ϴ�.
*
*	hWnd : ���� ������ �ڵ�
*	Msg : ������ �޼����ڽ� �Ӽ�
*	g_hInst : ���� ���α׷��� �ν��Ͻ� �ڵ�
*	alpha_message : �޼����ڽ��� �����Դϴ�. 0 ~ 255 ������ ��.
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
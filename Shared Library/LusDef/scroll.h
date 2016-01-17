#pragma once

//	사용하실 스크롤 클래스를 정의하세요.
#define scroll	scroll_Win32

//---------------------------------------------------------------
//	basic_scroll을 상속한 자식클래스를 이용해서 프로그래밍하세요.
//---------------------------------------------------------------

#pragma region	 default setting part

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#pragma endregion

/*
* 스크롤을 사용하기 위한 기반클래스입니다.
* 스크롤에 대한 기본적인 코드가 제공됩니다.
* 수직, 수평 스크롤 방식을 선택해서 사용할 수 있습니다.
* 초기화(initialize)를 한 뒤 사용해야 합니다.
* 이 클래스를 이용한 스크롤 클래스는 마우스 클릭, 그리기 함수를 제공해야 합니다.
*/
class basic_scroll{

public:

	/*
	* 스크롤의 타입입니다.
	* SCR_H : 수평방향으로 생성되는 스크롤입니다.
	* SCR_V : 수직방향으로 생성되는 스크롤입니다.
	*/
	typedef enum{ SCR_H, SCR_V }SCRType;

protected:
	//	현재 스크롤의 타입입니다.
	SCRType	m_scType;

	//	현재 스크롤의 길이입니다.
	float	m_flScroll;
	//	현재 스크롤에서 움직일 수 있는 막대의 길이입니다.
	float	m_flThumb;
	//	현재 스크롤에서 움직일 수 있는 막대의 중심의 위치입니다.
	float	m_fnowScrollpos;

	//	스크롤할 대상 데이터의 길이입니다.
	float	m_flSrc;
	//	대상 데이터가 보여지는 화면의 길이입니다.
	float	m_flSrcView;

	//	스크롤의 너비입니다.
	float	m_flThickness;

	//	스크롤의 잡은 마지막 위치입니다.
	float	m_fLastGrapPosition;
	bool	m_bGrap;

	//	스크롤이 바깥으로 삐치지 않도록 하는 함수입니다.
	float Lus_onlyuse_SideStore(float min, float value, float max){ if (min > value) value = min; if (value > max) value = max; return value; }
public:

	basic_scroll()
	{
		m_bGrap = false;
		m_fLastGrapPosition = 0.f;
		m_fnowScrollpos = 0;
	}

	bool IsNeedView() { return (m_flSrc > 0); }

	/*
	*	초기화 함수입니다.
	*	SCRType : 스크롤 타입입니다. scroll::SCRType::SCR_H 와 같은 방식으로 작성됩니다.
	*	ScrLength : 스크롤할 원본 데이터 길이입니다.
	*	ScrViewLength : 화면에 보이는 데이터 길이입니다.
	*	ScrollLength : 스크롤바의 길이입니다.
	*	ThumbLength( = 0.f) : 스크롤 바의 이동시킬 수 있는 막대 길이(1/2)입니다. 0을 입력하면 자동으로 계산합니다.
	※ ThumbLength의 길이를 자동으로 조절할 수 있는 조건 : ScrollLength > ThumbLength * 2
	*	thickness : 스크롤의 굵기입니다.
	*/
	virtual void initialize(SCRType type, float ScrLength, float ScrViewLength, float ScrollLength,
		float ThumbLength = 0.f, float thickness = 5.0f)
	{
		m_bGrap = false;
		m_fLastGrapPosition = 0.f;
		m_fnowScrollpos = 0;

		m_scType = type;
		
		m_flSrc = ScrLength;
		m_flSrcView = ScrViewLength;
		if (ThumbLength != 0)
			m_flThumb = ThumbLength;
		else
		{
		//	m_flThumb = max(5, ScrollLength * m_flSrcView / m_flSrc);
		//	(길이) * (표시) / ((실제 길이 + 표시) * 2)
			m_flThumb = ScrViewLength * ScrollLength / ((ScrViewLength + ScrLength) * 2);
		}
		m_flScroll = ScrollLength - 2 * m_flThumb;

		m_flThickness = thickness;
	}

	/*
	* 마우스로 Thumb(스크롤 바의 이동시킬 수 있는 막대) 영역을 눌렀을 때 호출합니다. 현재 마우스가 누른 위치를 기억합니다.
	* 수평과 수직을 동시에 제공하지 않으므로 호출시에 사용할 좌표값을 지정해야 합니다.
	* 해당 함수는 오버라이딩이 가능합니다.
	*/
	virtual void SetGrip(float pos) { if (!IsNeedView()) return; m_fLastGrapPosition = pos; m_bGrap = true; }

	//	마우스를 놓았을 때 호출합니다.
	void ReleaseGrip(){ if (!IsNeedView()) return; m_bGrap = false; }

	/*
	* SetGrip을 호출하고 마우스를 드래그할 때 호출합니다.
	* Thumb의 위치를 마우스가 이동한 만큼 변경합니다. 이 위치는 스크롤 바를 튀어나갈 수 없습니다.
	* 만약 SetGrip을 호출하지 않았을 경우 false값을 반환하며, 기본적으로 true를 반환합니다.
	*/
	virtual bool Drag(float pos)
	{
		if (!IsNeedView()) return false;
		if (!m_bGrap) return false;
		float moveLen = pos - m_fLastGrapPosition;
		m_fnowScrollpos = Lus_onlyuse_SideStore(0, m_fnowScrollpos + moveLen, m_flScroll);
		m_fLastGrapPosition = pos;
		return true;
	}

	//	Thumb의 위치에서 임의의 길이만큼 이동합니다. Thumb를 이용하지 않고 스크롤 바를 움직일 때 사용합니다.
	void AdjustnowScrollPosition(float len)
	{
		m_fLastGrapPosition = 0;
		m_bGrap = false;
		m_fnowScrollpos = Lus_onlyuse_SideStore(0, m_fnowScrollpos + len, m_flScroll);
	}
	//	Thumb의 위치를 임의로 변경합니다. Thumb를 이용하지 않고 스크롤 바를 움직일 때 사용합니다.
	void SetnowScrollPosition(float len)
	{
		m_fLastGrapPosition = 0;
		m_bGrap = false;
		m_fnowScrollpos = Lus_onlyuse_SideStore(0, len * m_flScroll / m_flSrc, m_flScroll);
	}

	//	현재 스크롤이 가리키는 실제 데이터의 길이를 반환합니다.
	virtual float GetNowSrcPosition(){ return m_flSrc * m_fnowScrollpos / m_flScroll; }

};

//---------------------------------------------------------------

//	이 구역 내부에 필요한 헤더를 정의합니다.
#pragma region	 user setting part

#pragma comment(lib,"msimg32")
#include <windows.h>

#pragma endregion

/*
* basic_scroll을 상속받은 Win32API에서 사용이 가능한 scroll입니다.
* OnProcessingMouseMessage를 이용하여 스크롤을 이용할 때 필요한 마우스입력을 간단히 사용할 수 있도록 했습니다.
* 스크롤을 사용할 시점에 initialize를 한 뒤 그 이후 OnProcessingMouseMessage를 호출해주고, WM_PAINT에서 Draw 함수를 호출해 주는 것으로 사용할 수 있습니다.
* 스크롤을 이용하여 변경할 데이터는 GetNowSrcPosition을 이용해 값을 읽을 수 있습니다.
* 아래 클래스를 참고하여 다른 그래픽라이브러리에서 동작하는 스크롤 클래스를 만들 수 있습니다.
*/
class scroll_Win32 : public basic_scroll{

protected:
	//	스크롤의 투명도입니다.
	int		m_iOpacity;
	//	스크롤 영역입니다. 이 영역대로 그립니다.
	RECT	m_rcScroll;
	//	Thumb의 영역입니다. 이 영역대로 그립니다.
	RECT	m_rcThumb;
	//	현재 마우스가 눌렸는지를 반환합니다.
	bool	m_bOnClick;
	//	스크롤이 그려지는 전역 좌표입니다. 이 좌표를 원점으로 하여 스크롤이 그려집니다.
	POINT	m_ptWorldPosition;

public:

	/*
	* 초기화 함수입니다.
	*	SCRType : 스크롤 타입입니다. scroll::SCRType::SCR_H 와 같은 방식으로 작성됩니다.
	*	ScrLength : 스크롤할 원본 데이터 길이입니다.
	*	ScrViewLength : 화면에 보이는 데이터 길이입니다.
	*	ScrollLength : 스크롤바의 길이입니다.
	*	worldPosition : 스크롤이 그려질 영역의 전역 시작 좌표입니다.
	*	Opacity : 스크롤의 투명도입니다. 0부터 255까지의 정수입니다.
	*	ThumbLength( = 0.f) : 스크롤 바의 이동시킬 수 있는 막대 길이(1/2)입니다. 0을 입력하면 자동으로 계산합니다.
	※ ThumbLength의 길이를 자동으로 조절할 수 있는 조건 : ScrollLength > ThumbLength * 2
	*	thickness : 스크롤의 굵기입니다.
	*/
	virtual void initialize(SCRType type, float ScrLength, float ScrViewLength, float ScrollLength, POINT WorldPosition,
		float Opacity = 255.0f, float ThumbLength = 0.f, float thickness = 5.0f)
	{
		basic_scroll::initialize(type, ScrLength, ScrViewLength, ScrollLength, ThumbLength, thickness);
		m_ptWorldPosition = WorldPosition;
		m_iOpacity = Opacity;
		RenewalThumbRect();
		if (m_scType == SCRType::SCR_H)
			m_rcScroll = MakeRect<float>(
			m_ptWorldPosition.x,
			m_ptWorldPosition.y,
			m_ptWorldPosition.x + m_flScroll + 2 * m_flThumb,
			m_ptWorldPosition.y + m_flThickness );
		else
			m_rcScroll = MakeRect<float>(
			m_ptWorldPosition.x,
			m_ptWorldPosition.y,
			m_ptWorldPosition.x + m_flThickness,
			m_ptWorldPosition.y + m_flScroll + 2 * m_flThumb );

	}

	/*
	* 마우스가 눌려졌을 때의 상황을 처리합니다.
	*/
	bool TouchCheck(POINT pt, float ScrollmoveLength = 10.f)
	{
		float pos = ((m_scType == SCRType::SCR_H) ? pt.x - m_ptWorldPosition.x : pt.y - m_ptWorldPosition.y);
		if (PtInRect(&m_rcThumb, pt))
		{
			SetGrip(pos);
			m_bOnClick = true;
		}
		else if (PtInRect(&m_rcScroll, pt))
		{
			if (pos > m_flThumb + m_fnowScrollpos)
				SetnowScrollPosition(ScrollmoveLength);
			else
				SetnowScrollPosition(-ScrollmoveLength);
			RenewalThumbRect();
			m_bOnClick = true;
		}
		else return false;
		return true;
	}

	/*
	* SetGrip을 호출하고 마우스를 드래그할 때 호출합니다.
	* Thumb의 위치를 마우스가 이동한 만큼 변경합니다. 이 위치는 스크롤 바를 튀어나갈 수 없습니다.
	* 만약 SetGrip을 호출하지 않았을 경우 false값을 반환하며, 기본적으로 true를 반환합니다.
	* (추가) 드래그 후 Thumb를 갱신합니다.
	*/
	virtual bool Drag(float pos)
	{
		bool bReturn = basic_scroll::Drag(pos);
		RenewalThumbRect();
		return bReturn;
	}

	/*
	* Thumb의 위치를 갱신합니다.
	*/
	void RenewalThumbRect()
	{
		if (m_scType == SCRType::SCR_H)
			m_rcThumb = MakeRect<float>(
			m_ptWorldPosition.x + m_fnowScrollpos,
			m_ptWorldPosition.y - 1,
			m_ptWorldPosition.x + 2 * m_flThumb + m_fnowScrollpos,
			m_ptWorldPosition.y + m_flThickness + 1 );
		else
			m_rcThumb = MakeRect<float>(
			m_ptWorldPosition.x - 1,
			m_ptWorldPosition.y + m_fnowScrollpos,
			m_ptWorldPosition.x + m_flThickness + 1,
			m_ptWorldPosition.y + 2 * m_flThumb + m_fnowScrollpos );
	}

	/*
	* 마우스를 이용한 스크롤의 사용이 편리하게 만들었습니다.
	* 마우스 클릭을 통한 Thumb의 이동, 스크롤바를 클릭하여 Thumb의 이동이 구현돼 있습니다.
	* true를 리턴하여 프로세스를 더 실행할지의 여부를 결정할 수 있습니다.
	*/
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (!IsNeedView()) return false;
		POINT pt = { LOWORD(lParam), HIWORD(lParam) };
		switch (uMsg)
		{
		case WM_LBUTTONDOWN:

			m_bOnClick = false;
			return TouchCheck(pt);
			break;
		case WM_MOUSEMOVE:
			if (m_bOnClick)
			{
				if (!Drag(((m_scType == SCRType::SCR_H) ? pt.x - m_ptWorldPosition.x : pt.y - m_ptWorldPosition.y)))
					if (PtInRect(&m_rcScroll, pt))
					{
						if (((m_scType == SCRType::SCR_H) ? pt.x - m_ptWorldPosition.x : pt.y - m_ptWorldPosition.y) > m_flThumb + m_fnowScrollpos)
							SetnowScrollPosition(10.f);
						else
							SetnowScrollPosition(-10.f);
						RenewalThumbRect();
					}
			}
			break;
		case WM_LBUTTONUP:
			ReleaseGrip();
			m_bOnClick = false;
			return true;
		}
		return false;
	}

	/*
	* 스크롤을 그려줍니다.
	* 모서리가 둥근 사각형이 기본형이며, 내부적으로 변경하여 사용하실 수 있습니다.
	* 투명값을 이용하여 반투명한 스크롤을 구현할 수 있습니다.
	*/
	virtual void Draw(HDC hDC, RECT DrawAreaSize, COLORREF color = RGB(0, 0, 0))
	{
		if (!IsNeedView()) return;

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.AlphaFormat = 0;
		HDC LayDC = CreateCompatibleDC(hDC);
		HBITMAP Lay = CreateCompatibleBitmap(hDC, DrawAreaSize.right, DrawAreaSize.bottom);
		SelectObject(LayDC, Lay);

		bf.SourceConstantAlpha = m_iOpacity;
		BitBlt(LayDC, 0, 0, DrawAreaSize.right, DrawAreaSize.bottom, hDC, 0, 0, SRCCOPY);

		SetDCBrushColor(LayDC, RGB(255, 255, 255));
		SelectObject(LayDC, (HBRUSH)GetStockObject(DC_BRUSH));

		RoundRect(LayDC, m_rcScroll.left, m_rcScroll.top, m_rcScroll.right, m_rcScroll.bottom, m_flThickness * 0.5f, m_flThickness * 0.5f);

		SetDCPenColor(LayDC, color);
		SetDCBrushColor(LayDC, color);
		SelectObject(LayDC, (HBRUSH)GetStockObject(DC_BRUSH));
		SelectObject(LayDC, (HPEN)GetStockObject(DC_PEN));
		RoundRect(LayDC, m_rcThumb.left, m_rcThumb.top, m_rcThumb.right, m_rcThumb.bottom, m_flThickness * 0.5f, m_flThickness * 0.5f);

		AlphaBlend(hDC, DrawAreaSize.left, DrawAreaSize.top, DrawAreaSize.right - DrawAreaSize.left, DrawAreaSize.bottom - DrawAreaSize.top,
			LayDC, DrawAreaSize.left, DrawAreaSize.top, DrawAreaSize.right - DrawAreaSize.left, DrawAreaSize.bottom - DrawAreaSize.top, bf);

		DeleteDC(LayDC);
		DeleteObject(Lay);
	}

};

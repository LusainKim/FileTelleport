// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <iostream>
#include <chrono>
#include <xutility>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <memory>
#include <list>
#include <fstream>
#include <map>
#include <deque>

#include "LusDef.h"
#include "scroll.h"

#include "FileCopy.h"
#include "ServerMain.h"

#pragma comment(lib, "imm32")

using namespace std;

#define CLIENT_WIDTH 400
#define CLIENT_HEIGHT 500

#define Win32TitleName _T("LusDrive[Server]")

#define voidcasting(_Typename, info) *static_cast<_Typename*>(info)
#define voidpointcasting(_Typename, info) reinterpret_cast<_Typename*>(info)

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

//────────────────────────
//				Font Define
//────────────────────────

#define FontSet_Arial 0,0,0,700,FALSE,FALSE,0,\
			DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,\
			DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, TEXT("Arial")

#define FontSet_Arial_B 0,0,0,1000,FALSE,FALSE,0,\
			DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS,\
			DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, TEXT("Arial")

#define FontSet_Arial_L 0,0,0,1000,FALSE,FALSE,0,\
			DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,CLIP_DEFAULT_PRECIS,\
			DEFAULT_QUALITY,DEFAULT_PITCH | FF_SWISS, TEXT("Arial")

//=============================

#define RandomColor (rand()%128 + 128)*256*256 + (rand()%128 + 128)*256 + (rand()%128 + 128)

extern HINSTANCE	hInst;
extern HWND			ghWndMain;

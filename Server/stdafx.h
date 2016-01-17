// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:
#include <windows.h>

// C ��Ÿ�� ��� �����Դϴ�.
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

// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.

//������������������������������������������������
//				Font Define
//������������������������������������������������

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

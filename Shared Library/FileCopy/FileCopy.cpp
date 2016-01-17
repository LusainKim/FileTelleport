
#include <windows.h>
#include <atlImage.h>
#include <iostream>
#include <string>
#include <vector>
#include "FileCopy.h"

using namespace std;

namespace {

	HANDLE consoleHandler = GetStdHandle(STD_OUTPUT_HANDLE);

};

int gotoxy(int x, int y)
{
	if (consoleHandler == INVALID_HANDLE_VALUE)
		return 0;
	COORD coords = { static_cast<short>(x), static_cast<short>(y) };
	SetConsoleCursorPosition(consoleHandler, coords);
	return 1;
}

int wherex()
{
	if (consoleHandler == INVALID_HANDLE_VALUE)
		return 0;
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);
	return screenInfo.dwCursorPosition.X;
}

int wherey()
{
	if (consoleHandler == INVALID_HANDLE_VALUE)
		return 0;
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);
	return screenInfo.dwCursorPosition.Y;
}

int bufferMaxX()
{
	if (consoleHandler == INVALID_HANDLE_VALUE)
		return 0;
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);
	return screenInfo.dwMaximumWindowSize.X;
}

int bufferMaxY()
{
	if (consoleHandler == INVALID_HANDLE_VALUE)
		return 0;
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;
	GetConsoleScreenBufferInfo(consoleHandler, &screenInfo);
	return screenInfo.dwMaximumWindowSize.Y;
}
#ifdef UNICODE
wstring FormatData(size_t data)
{
	int byteLevel = 0;
	double d = (double)data;
	wstring str;
	while (d / 1024 > 1)
	{
		d /= 1024;
		byteLevel++;
	}
	switch (byteLevel)
	{
	case 1:	// kb
		str = _Floating_to_wstring(TEXT("%.1f"), d) + TEXT(" KB");
		break;
	case 2:	// mb				  
		str = _Floating_to_wstring(TEXT("%.1f"), d) + TEXT(" MB");
		break;							
	case 3:	// gb					
		str = _Floating_to_wstring(TEXT("%.1f"), d) + TEXT(" GB");
		break;
	default:
		str = FixValue(data) + ((data > 1) ? TEXT(" bytes") : TEXT(" byte"));
		break;
	}
	return str;
}

wstring FormatTime(double times)
{
	int timeLevel = 0;

	wstring str;

	if (times > 60.0)
	{
		times /= 60.0;
		timeLevel++;
	}

	if (times > 60.0)
	{
		times /= 60.0;
		timeLevel++;
	}

	if (times > 24.0)
	{
		times /= 24.0;
		timeLevel++;
	}

	switch (timeLevel)
	{
	case 1:	// kb
		str = _Floating_to_wstring(TEXT("%.1f"), times) + TEXT(" 분"		);
		break;					   
	case 2:	// mb				   
		str = _Floating_to_wstring(TEXT("%.1f"), times) + TEXT(" 시간"	);
		break;					   
	case 3:	// gb				   
		str = _Floating_to_wstring(TEXT("%.1f"), times) + TEXT(" 일"		);
		break;					  
	default:					  
		str = _Floating_to_wstring(TEXT("%.1f"), times) + TEXT(" 초"		);
		break;
	}
	return str;
}
#else
string FormatData(size_t data)
{
	int byteLevel = 0;
	double d = (double)data;
	string str;
	while (d / 1024 > 1)
	{
		d /= 1024;
		byteLevel++;
	}
	switch (byteLevel)
	{
	case 1:	// kb
		str = _Floating_to_string("%.1f", d) + " KB";
		break;
	case 2:	// mb		
		str = _Floating_to_string("%.1f", d) + " MB";
		break;
	case 3:	// gb		
		str = _Floating_to_string("%.1f", d) + " GB";
		break;
	default:
		str = FixValue(data) + ((data > 1) ? " bytes" : " byte");
		break;
	}
	return str;
}

string FormatTime(double times)
{
	int timeLevel = 0;

	string str;

	if (times > 60.0)
	{
		times /= 60.0;
		timeLevel++;
	}

	if (times > 60.0)
	{
		times /= 60.0;
		timeLevel++;
	}

	if (times > 24.0)
	{
		times /= 24.0;
		timeLevel++;
	}

	switch (timeLevel)
	{
	case 1:	// kb
		str = _Floating_to_string("%.1f", times) + " 분";
		break;
	case 2:	// mb		
		str = _Floating_to_string("%.1f", times) + " 시간";
		break;
	case 3:	// gb		
		str = _Floating_to_string("%.1f", times) + " 일";
		break;
	default:
		str = _Floating_to_string("%.1f", times) + " 초";
		break;
	}
	return str;
}

#endif

#ifdef UNICODE
void GetFiles(std::vector<std::wstring> &vList, std::wstring sPath, bool bAllDirectories)
{
	std::wstring sTmp = sPath + std::wstring(TEXT("\\*.*"));

	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(sTmp.c_str(), &fd);
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if (fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				if (bAllDirectories)
				{
					if (fd.cFileName[0] != TEXT('.'))
					{
						sTmp =
							sPath +
							std::wstring(TEXT("/")) +
							std::wstring(fd.cFileName);
						GetFiles(vList, sTmp, bAllDirectories);
					}
				}
			}
			else if (lstrlen(PathFindExtension(fd.cFileName)) > 2)
			{
				sTmp =
				//	sPath +
				//	wstring(TEXT("/")) +
					std::wstring(fd.cFileName);
				vList.push_back(sTmp);
			}

		} while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}
}
#endif
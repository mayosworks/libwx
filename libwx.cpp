//=============================================================================
// Auxiliary library for Windows API (C++) 
//                                                     Copyright (c) 2005 MAYO.
//=============================================================================
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

//=============================================================================
// アプリケーションエントリポイント
//=============================================================================
int WINAPI WinMain(HINSTANCE /*inst*/, HINSTANCE /*prev*/, LPSTR /*cmdLine*/, int /*cmdShow*/)
{
	MessageBox(HWND_DESKTOP, TEXT("libwx"), TEXT("About"), MB_OK | MB_ICONINFORMATION);
	return 0;
}

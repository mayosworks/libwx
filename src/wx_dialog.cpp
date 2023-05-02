//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2008 MAYO.
//=============================================================================

//#define WIN32_LEAN_AND_MEAN

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include "wx_dialog.h"

namespace wx {
namespace {

// 変数
HWND	g_hwnd = HWND_DESKTOP;
PCTSTR	g_title = TEXT("AppName");

} //namespace

//-----------------------------------------------------------------------------
// 親となるウィンドウを指定
//-----------------------------------------------------------------------------
void Dialog::SetParent(HWND hwnd)
{
	g_hwnd = hwnd;
}

//-----------------------------------------------------------------------------
// ウィンドウタイトルを指定
//-----------------------------------------------------------------------------
void Dialog::SetTitle(PCTSTR title)
{
	g_title = title;
}

//-----------------------------------------------------------------------------
// 情報・イベントメッセージを表示
//-----------------------------------------------------------------------------
void Dialog::Event(PCTSTR msg, ...)
{
	TCHAR buf[1024];
	va_list args;

	buf[WX_NUMBEROF(buf) - 1] = TEXT('\0');
	va_start(args, msg);
	wvsprintf(buf, msg, args);
	va_end(args);

	MessageBox(g_hwnd, buf, g_title, MB_ICONINFORMATION);
}

//-----------------------------------------------------------------------------
// 警告メッセージを表示
//-----------------------------------------------------------------------------
void Dialog::Alert(PCTSTR msg, ...)
{
	TCHAR buf[1024];
	va_list args;

	buf[WX_NUMBEROF(buf) - 1] = '\0';
	va_start(args, msg);
	wvsprintf(buf, msg, args);
	va_end(args);

	MessageBox(g_hwnd, buf, g_title, MB_ICONWARNING);
}

//-----------------------------------------------------------------------------
// エラーメッセージを表示
//-----------------------------------------------------------------------------
void Dialog::Error(PCTSTR msg, ...)
{
	TCHAR buf[1024];
	va_list args;

	buf[WX_NUMBEROF(buf) - 1] = '\0';
	va_start(args, msg);
	wvsprintf(buf, msg, args);
	va_end(args);

	MessageBox(g_hwnd, buf, g_title, MB_ICONERROR);
}

//-----------------------------------------------------------------------------
// ファイルを開くダイアログを表示
//-----------------------------------------------------------------------------
bool Dialog::OpenFile(PTSTR path, PCTSTR filter, PCTSTR title)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize	= sizeof(ofn);
	ofn.Flags		= OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
	ofn.hwndOwner	= g_hwnd;
	ofn.lpstrFilter	= filter;
	ofn.lpstrFile	= path;
	ofn.nMaxFile	= MAX_PATH;
	ofn.lpstrTitle	= title? title : g_title;

	if (!GetOpenFileName(&ofn)) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// ファイルの保存ダイアログを表示
//-----------------------------------------------------------------------------
bool Dialog::SaveFile(PTSTR path, PCTSTR filter, PCTSTR title)
{
	OPENFILENAME ofn;

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize	= sizeof(ofn);
	ofn.Flags		= OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.hwndOwner	= g_hwnd;
	ofn.lpstrFilter	= filter;
	ofn.lpstrFile	= path;
	ofn.nMaxFile	= MAX_PATH;
	ofn.lpstrTitle	= title? title : g_title;

	if (!GetSaveFileName(&ofn)) {
		return false;
	}

	return false;
}

//-----------------------------------------------------------------------------
// カラー選択ダイアログを表示
//-----------------------------------------------------------------------------
bool Dialog::SelectColor(COLORREF& color)
{
	CHOOSECOLOR cc;
	COLORREF lst[] =
	{
		0xFFFFFF, 0xC0C0C0, 0x808080, 0x404040,		// グレー系の色
		0xFF0000, 0xFF8080, 0xFFFF00, 0xFFFF80,		// 青系の色
		0x0000FF, 0x8080FF, 0xFF00FF, 0xFF80FF,		// 赤系の色
		0x00FF00, 0x80FF80, 0x00FFFF, 0x80FFFF,		// 緑系の色
	};

	ZeroMemory(&cc, sizeof(cc));

	cc.lStructSize	= sizeof(cc);
	cc.hwndOwner	= g_hwnd;
	cc.Flags		= CC_FULLOPEN | CC_RGBINIT;
	cc.rgbResult	= color;
	cc.lpCustColors	= lst;

	if (!ChooseColor(&cc)) {
		return false;
	}

	color = cc.rgbResult;
	return true;
}

} //namespace wx

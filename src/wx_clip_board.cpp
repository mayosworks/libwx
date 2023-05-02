//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_misc.h"
#include "wx_clip_board.h"

namespace wx {

//-----------------------------------------------------------------------------
// クリップボードを開く
//-----------------------------------------------------------------------------
bool ClipBoard::Open(HWND owner)
{
	if (!OpenClipboard(owner)) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// クリップボードを閉じる
//-----------------------------------------------------------------------------
void ClipBoard::Close()
{
	CloseClipboard();
}

//-----------------------------------------------------------------------------
// クリップボードを空にする
//-----------------------------------------------------------------------------
bool ClipBoard::Empty()
{
	if (!EmptyClipboard()) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// クリップボードにデータを貼り付ける
//-----------------------------------------------------------------------------
bool ClipBoard::SetData(UINT format, void* data, UINT size)
{
	HGLOBAL hmem = GlobalAlloc(GHND, size);
	if (!hmem) {
		return false;
	}

	void* buffer = GlobalLock(hmem);
	if (!buffer) {
		GlobalFree(hmem);
		return false;
	}

	CopyMemory(buffer, data, size);
	GlobalUnlock(hmem);

	HANDLE handle = SetClipboardData(format, hmem);
	if (!handle) {
		GlobalFree(hmem);
		return false;
	}

	return true;
}

} //namespace wx

//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_console.h"

namespace wx {

//-----------------------------------------------------------------------------
// デフォルトコンストラクタ
//-----------------------------------------------------------------------------
Console::Console()
	: m_console(INVALID_HANDLE_VALUE)
	, m_visible(true)
{
	m_console = GetStdHandle(STD_OUTPUT_HANDLE);
}

//-----------------------------------------------------------------------------
// ハンドル指定コンストラクタ
//-----------------------------------------------------------------------------
Console::Console(HANDLE console)
	: m_console(console)
	, m_visible(true)
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Console::~Console()
{
}

//-----------------------------------------------------------------------------
// コンソールをクリアする（書き込みアクセスできる場合のみ）
//-----------------------------------------------------------------------------
void Console::Clear()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	DWORD written = 0;
	COORD pos = { 0, 0 };

	if (GetConsoleScreenBufferInfo(m_console, &info)) {
		FillConsoleOutputCharacter(m_console, TEXT(' '),
			info.dwSize.X * info.dwSize.Y, pos, &written);
	}
}


//-----------------------------------------------------------------------------
// カーソルを配置する
//-----------------------------------------------------------------------------
void Console::Locate(int x, int y)
{
	COORD pos = { static_cast<short>(x), static_cast<short>(y) };

	SetConsoleCursorPosition(m_console, pos);
}

//-----------------------------------------------------------------------------
// 文字色を設定する
//-----------------------------------------------------------------------------
void Console::SetColor(WORD color)
{
	SetConsoleTextAttribute(m_console, color);
}

//-----------------------------------------------------------------------------
// カーソルの表示を設定する
//-----------------------------------------------------------------------------
void Console::ShowCursor(bool show)
{
	CONSOLE_CURSOR_INFO info;

	GetConsoleCursorInfo(m_console, &info);
	info.bVisible = show;

	SetConsoleCursorInfo(m_console, &info);
	m_visible = show;
}

//-----------------------------------------------------------------------------
// カーソルの表示状態を取得する
//-----------------------------------------------------------------------------
bool Console::IsCursorVisible() const
{
	return m_visible;
}

} //namespace wx

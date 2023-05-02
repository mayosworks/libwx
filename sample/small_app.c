//=============================================================================
// Small application entry point sample.
//                                                     Copyright (c) 2008 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

//-----------------------------------------------------------------------------
// C/C++とWin32APIを用いたプログラムで、アプリの実行ファイルサイズを
// 小さくするためのライブラリです。Cランタイムライブラリが使えません。
// ※この関数をエントリポイントに設定すること。
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//! @brief	WinMainCRTStartup() の代替え実装、Cランタイム等を使わないようにする
//-----------------------------------------------------------------------------
void __cdecl WinMainCRTStartup(void)
{
	char* command_line;
	STARTUPINFO startup_info;
	int show_window;
	int result = 0;

	command_line = GetCommandLineA();

	if (*command_line == '"') {
		do {
			++command_line;
		} while (*command_line && (*command_line != '"'));

		if (*command_line == '"') {
			++command_line;
		}
	}
	else {
		while (' ' < *command_line) {
			++command_line;
		}
	}

	while (*command_line && (*command_line <= ' ')) {
		++command_line;
	}

	startup_info.dwFlags = 0;
	GetStartupInfo(&startup_info);
	show_window = (startup_info.dwFlags & STARTF_USESHOWWINDOW)? startup_info.wShowWindow : SW_SHOWDEFAULT;

	// WinMainを呼び出す
	result = WinMain(GetModuleHandle(NULL), NULL, command_line, show_window);

	ExitProcess(result);
}


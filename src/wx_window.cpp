//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_misc.h"
#include "wx_window.h"

namespace wx {

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Window::Window()
	: m_style(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU)
	, m_ex_style(0)
	, m_hwnd(NULL)
	, m_width(1280)
	, m_height(720)
	, m_listener(NULL)
{
	ZeroMemory(&m_wndclass, sizeof(m_wndclass));

	// デフォルトウィンドウクラス設定
	m_wndclass.cbSize			= sizeof(m_wndclass);
	m_wndclass.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_OWNDC;
	m_wndclass.lpszClassName	= TEXT("wxWindow");
	m_wndclass.hbrBackground	= reinterpret_cast<HBRUSH>(COLOR_BACKGROUND + 1);
	m_wndclass.lpfnWndProc		= WndProc;
	m_wndclass.hInstance		= GetModuleHandle(NULL);
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
Window::~Window()
{
}

//-----------------------------------------------------------------------------
// ウィンドウリスナー設定
//-----------------------------------------------------------------------------
void Window::SetListener(IWindowListener* listener)
{
	m_listener = listener;
}

//-----------------------------------------------------------------------------
// ウィンドウクラスを保持するインスタンス設定
//-----------------------------------------------------------------------------
void Window::SetInstance(HINSTANCE instance)
{
	m_wndclass.hInstance = instance;
}

//-----------------------------------------------------------------------------
// ウィンドウクラスカーソル設定
//-----------------------------------------------------------------------------
void Window::SetClassCursor(HCURSOR cursor)
{
	m_wndclass.hCursor = cursor;
}

//-----------------------------------------------------------------------------
// ウィンドウクラスアイコン（大）設定
//-----------------------------------------------------------------------------
void Window::SetClassIconL(HICON icon)
{
	m_wndclass.hIcon = icon;
}

//-----------------------------------------------------------------------------
// ウィンドウクラスアイコン（小）設定
//-----------------------------------------------------------------------------
void Window::SetClassIconS(HICON icon)
{
	m_wndclass.hIconSm = icon;
}

//-----------------------------------------------------------------------------
// ウィンドウクラスブラシ設定
//-----------------------------------------------------------------------------
void Window::SetClassBrush(HBRUSH brush)
{
	m_wndclass.hbrBackground = brush;
}

//-----------------------------------------------------------------------------
// ウィンドウタイトル設定（ウィンドウ生成後のみ設定可）
//-----------------------------------------------------------------------------
void Window::SetWindowTitle(PCTSTR title)
{
	SetWindowText(m_hwnd, title);
}

//-----------------------------------------------------------------------------
// クライアント領域のサイズ設定
//-----------------------------------------------------------------------------
void Window::SetClientSize(int width, int height)
{
	RECT rt;

	m_width  = width;
	m_height = height;

	SetRect(&rt, 0, 0, width, height);
	AdjustWindowRect(&rt, m_style, FALSE);
	//AdjustWindowRectEx(&rt, WS_OVERLAPPEDWINDOW, FALSE, 0);

	if (m_hwnd) {
		SetWindowPos(m_hwnd, HWND_TOP, 0, 0, rt.right - rt.left, rt.bottom - rt.top, SWP_NOMOVE | SWP_NOZORDER);
	}
}

//-----------------------------------------------------------------------------
// クライアント領域の幅取得
//-----------------------------------------------------------------------------
int Window::GetClientWidth() const
{
	return m_width;
}

//-----------------------------------------------------------------------------
// クライアント領域の高さ取得
//-----------------------------------------------------------------------------
int Window::GetClientHeight() const
{
	return m_height;
}

//-----------------------------------------------------------------------------
// ウィンドウスタイル設定
//-----------------------------------------------------------------------------
void Window::SetWindowStyle(DWORD style)
{
	m_style = style;
}

//-----------------------------------------------------------------------------
// 拡張ウィンドウスタイル設定
//-----------------------------------------------------------------------------
void Window::SetWindowStyleEx(DWORD ex_style)
{
	m_ex_style = ex_style;
}

//-----------------------------------------------------------------------------
// ウィンドウスタイル取得
//-----------------------------------------------------------------------------
DWORD Window::GetWindowStyle() const
{
	return m_style;
}

//-----------------------------------------------------------------------------
// 拡張ウィンドウスタイル取得
//-----------------------------------------------------------------------------
DWORD Window::GetWindowStyleEx() const
{
	return m_ex_style;
}

//-----------------------------------------------------------------------------
// ウィンドウ生成
//-----------------------------------------------------------------------------
bool Window::Create(PCTSTR classname)
{
	if (classname) {
		m_wndclass.lpszClassName = classname;
	}

	if (!RegisterClassEx(&m_wndclass)) {
		return false;
	}

	RECT rt;

	SetRect(&rt, 0, 0, m_width, m_height);
	AdjustWindowRect(&rt, m_style, FALSE);

	m_hwnd = CreateWindow(m_wndclass.lpszClassName, m_wndclass.lpszClassName,
		m_style, CW_USEDEFAULT, CW_USEDEFAULT, rt.right - rt.left,
		rt.bottom - rt.top, NULL, NULL, m_wndclass.hInstance, m_listener);
	if (!m_hwnd) {
		return false;
	}

	ShowWindow(m_hwnd, SW_SHOWNORMAL);
	UpdateWindow(m_hwnd);
	return true;
}

//-----------------------------------------------------------------------------
// ウィンドウ破棄
//-----------------------------------------------------------------------------
void Window::Destroy()
{
	if (m_hwnd) {
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;
	}
}

//-----------------------------------------------------------------------------
// ウィンドウハンドル取得
//-----------------------------------------------------------------------------
HWND Window::GetHandle() const
{
	return m_hwnd;
}

//-----------------------------------------------------------------------------
// ウィンドウ共通ウィンドウプロシージャー
//-----------------------------------------------------------------------------
LRESULT Window::WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	// ウィンドウのユーザーデータエリアにリスナーを保持させる
	if (WM_CREATE == msg) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
		LONG_PTR lptr = reinterpret_cast<LONG_PTR>(cs->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, static_cast<long>(lptr));
	}

	LONG_PTR userdata = GetWindowLongPtr(hwnd, GWLP_USERDATA);

	if (WM_DESTROY == msg) {
		SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
	}

	// メッセージ処理
	IWindowListener* listener = reinterpret_cast<IWindowListener*>(userdata);
	if (listener) {
		return listener->RecvMessage(hwnd, msg, wp, lp);
	}

	return DefWindowProc(hwnd, msg, wp, lp);
}

} //namespace wx

//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2008 MAYO.
//=============================================================================

#include <windows.h>
#include "wx_coded_string.h"

namespace wx {
namespace {

// 定数定義
const wchar_t*	WSZ_EMPTY = L"\0";
const char*		STR_EMPTY =  "\0";

// コードページの実際の値
const UINT CP_VALUE[cp::NUM] = 
{
	CP_ACP,
	CP_UTF8,
};

//-----------------------------------------------------------------------------
// Ansi -> Wide 文字変換
//-----------------------------------------------------------------------------
const wchar_t* AnsiToWide(const char* ansi, UINT code, int& retlen)
{
	int len = MultiByteToWideChar(code, 0, ansi, -1, NULL, 0);
	if (len == 0) {
		return WSZ_EMPTY;
	}

	wchar_t* buf = new wchar_t[len + 1];
	if (!buf) {
		return NULL;
	}

	if (!MultiByteToWideChar(code, 0, ansi, -1, buf, len)) {
		delete [] buf;
		return NULL;
	}

	buf[len] = L'\0';
	retlen = len;
	return buf;
}


//-----------------------------------------------------------------------------
// Wide -> Ansi 文字変換
//-----------------------------------------------------------------------------
const char* WideToAnsi(const wchar_t* wide, UINT code, int& retlen)
{
	int len = WideCharToMultiByte(code, 0, wide, -1, NULL, 0, NULL, NULL);
	if (len == 0) {
		return STR_EMPTY;
	}

	char* buf = new char[len + 1];
	if (!buf) {
		return NULL;
	}

	if (!WideCharToMultiByte(code, 0, wide, -1, buf, len, NULL, NULL)) {
		delete [] buf;
		return NULL;
	}

	buf[len] = '\0';
	retlen = len;
	return buf;
}


//-----------------------------------------------------------------------------
// Wide文字解放
//-----------------------------------------------------------------------------
void FreeWide(const wchar_t* str)
{
	if (str && str != WSZ_EMPTY) {
		delete [] str;
	}
}


//-----------------------------------------------------------------------------
// Ansi文字解放
//-----------------------------------------------------------------------------
void FreeAnsi(const char* str)
{
	if (str && str != STR_EMPTY) {
		delete [] str;
	}
}

} //namespace


//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SJISString::SJISString(const char* str)
	: m_string(NULL)
	, m_length(0)
{
	int len = 0;
	const wchar_t* wide = AnsiToWide(str, CP_UTF8, len);

	m_string = WideToAnsi(wide, CP_ACP, m_length);
	FreeWide(wide);
}


//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
SJISString::SJISString(const wchar_t* str)
	: m_string(NULL)
	, m_length(0)
{
	m_string = WideToAnsi(str, CP_ACP, m_length);
}


//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
SJISString::~SJISString()
{
	FreeAnsi(m_string);
	m_string = NULL;
	m_length = 0;
}



//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
UTF8String::UTF8String(const char* str)
	: m_string(NULL)
	, m_length(0)
{
	int len = 0;
	const wchar_t* wide = AnsiToWide(str, CP_ACP, len);

	m_string = WideToAnsi(wide, CP_UTF8, m_length);
	FreeWide(wide);
}


//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
UTF8String::UTF8String(const wchar_t* str)
	: m_string(NULL)
	, m_length(0)
{
	m_string = WideToAnsi(str, CP_UTF8, m_length);
}


//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
UTF8String::~UTF8String()
{
	FreeAnsi(m_string);
	m_string = NULL;
	m_length = 0;
}



//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
WideString::WideString(const char* str, cp::Value code)
	: m_string(NULL)
	, m_length(0)
{
	if (code < cp::NUM) {
		m_string = AnsiToWide(str, CP_VALUE[code], m_length);
	}
}


//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
WideString::~WideString()
{
	FreeWide(m_string);
	m_string = NULL;
	m_length = 0;
}

} //namespace wx

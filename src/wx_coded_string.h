﻿//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2008 MAYO.
//=============================================================================

#pragma once

#include <windows.h>

namespace wx {

//-----------------------------------------------------------------------------
// char型コードページ定義
//-----------------------------------------------------------------------------
namespace cp {
enum Value
{
	SJIS,	///< Shift_JIS
	UTF8,	///< UTF-8

	NUM,
};

} //namespace cp

//-----------------------------------------------------------------------------
//! @class	SJISString
//! @brief	Shift_JIS文字列
//-----------------------------------------------------------------------------
class SJISString
{
public:
	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//!
	//! @param	str			UTF-8文字列
	//-------------------------------------------------------------------------
	SJISString(const char* str);

	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//!
	//! @param	str			Unicode文字列
	//-------------------------------------------------------------------------
	SJISString(const wchar_t* str);

	//-------------------------------------------------------------------------
	//! @brief	デストラクタ
	//-------------------------------------------------------------------------
	~SJISString();

	//-------------------------------------------------------------------------
	//! @brief	文字列が有効かどうかを取得する
	//!
	//! @retval	true	有効
	//! @retval	false	無効
	//-------------------------------------------------------------------------
	bool IsValid() const
	{
		return (m_string != NULL);
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列を取得する
	//!
	//! @return	文字列
	//-------------------------------------------------------------------------
	operator const char* () const
	{
		return m_string;
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列を取得する
	//!
	//! @return	文字列
	//-------------------------------------------------------------------------
	const char* Get() const
	{
		return m_string;
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列の長さを取得する
	//!
	//! @return	文字列の長さ
	//-------------------------------------------------------------------------
	int Length() const
	{
		return m_length;
	}

private:
	const char*	m_string;
	int			m_length;
};


//-----------------------------------------------------------------------------
//! @class	UTF8String
//! @brief	UTF-8文字列
//-----------------------------------------------------------------------------
class UTF8String
{
public:
	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//!
	//! @param	str			Shift_JIS文字列
	//-------------------------------------------------------------------------
	UTF8String(const char* str);

	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//!
	//! @param	str			Unicode文字列
	//-------------------------------------------------------------------------
	UTF8String(const wchar_t* str);

	//-------------------------------------------------------------------------
	//! @brief	デストラクタ
	//-------------------------------------------------------------------------
	~UTF8String();

	//-------------------------------------------------------------------------
	//! @brief	文字列が有効かどうかを取得する
	//!
	//! @retval	true	有効
	//! @retval	false	無効
	//-------------------------------------------------------------------------
	bool IsValid() const
	{
		return (m_string != NULL);
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列を取得する
	//!
	//! @return	文字列
	//-------------------------------------------------------------------------
	operator const char* () const
	{
		return m_string;
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列を取得する
	//!
	//! @return	文字列
	//-------------------------------------------------------------------------
	const char* Get() const
	{
		return m_string;
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列の長さを取得する
	//!
	//! @return	文字列の長さ
	//-------------------------------------------------------------------------
	int Length() const
	{
		return m_length;
	}

private:
	const char*	m_string;
	int			m_length;
};


//-----------------------------------------------------------------------------
//! @class	WideString
//! @brief	ワイド(Unicode)文字列
//-----------------------------------------------------------------------------
class WideString
{
public:
	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//!
	//! @param	str			UTF-8 or Shift_JIS文字列
	//! @param	code		strのコードページ
	//-------------------------------------------------------------------------
	WideString(const char* str, cp::Value code);

	//-------------------------------------------------------------------------
	//! @brief	デストラクタ
	//-------------------------------------------------------------------------
	~WideString();

	//-------------------------------------------------------------------------
	//! @brief	文字列が有効かどうかを取得する
	//!
	//! @retval	true	有効
	//! @retval	false	無効
	//-------------------------------------------------------------------------
	bool IsValid() const
	{
		return (m_string != NULL);
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列を取得する
	//!
	//! @return	文字列
	//-------------------------------------------------------------------------
	operator const wchar_t* () const
	{
		return m_string;
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列を取得する
	//!
	//! @return	文字列
	//-------------------------------------------------------------------------
	const wchar_t* Get() const
	{
		return m_string;
	}

	//-------------------------------------------------------------------------
	//! @brief	文字列の長さを取得する
	//!
	//! @return	文字列の長さ
	//-------------------------------------------------------------------------
	int Length() const
	{
		return m_length;
	}

private:
	const wchar_t*	m_string;
	int				m_length;
};

} //namespace wx

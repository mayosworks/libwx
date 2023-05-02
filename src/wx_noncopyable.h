﻿//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#pragma once

namespace wx {

//-----------------------------------------------------------------------------
//! @class	NonCopyable
//! @brief	コピーを防止したいクラスで、このクラスをprotected継承してください
//-----------------------------------------------------------------------------
template <class T>
class NonCopyable
{
protected:
	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//-------------------------------------------------------------------------
	NonCopyable(){}

	
	//-------------------------------------------------------------------------
	//! @brief	デストラクタ
	//-------------------------------------------------------------------------
	~NonCopyable(){}


private:
	// コピーコンストラクタを無効化
	NonCopyable(const NonCopyable&);
	// 代入演算子を無効化
	T& operator = (const T&);
};

} //namespace wx
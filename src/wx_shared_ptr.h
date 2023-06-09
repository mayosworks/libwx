﻿//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#pragma once

namespace wx {

//-----------------------------------------------------------------------------
//! @class	SharedPtr
//! @brief	所有権をもつポインタ
//-----------------------------------------------------------------------------
template <class T>
class SharedPtr
{
public:
	//-------------------------------------------------------------------------
	//! @brief	デフォルトコンストラクタ
	//-------------------------------------------------------------------------
	SharedPtr()
		: m_object(NULL)
		, m_count(NULL)
	{
	}

	//-------------------------------------------------------------------------
	//! @brief	コンストラクタ
	//!
	//! @param	object	保持するオブジェクト
	//-------------------------------------------------------------------------
	SharedPtr(T* object)
		: m_object(object)
		, m_count(new int(1))
	{
	}

	//-------------------------------------------------------------------------
	//! @brief	コピーコンストラクタ
	//!
	//! @param	object	コピーするオブジェクト
	//-------------------------------------------------------------------------
	SharedPtr(SharedPtr<T>& object)
	{
		*this = object;
	}

	//-------------------------------------------------------------------------
	//! @brief	デストラクタ
	//-------------------------------------------------------------------------
	~SharedPtr()
	{
		if (m_count) {
			if (--(*m_count) == 0) {
				delete m_object;
				delete m_count;
			}
		}
	}

	//-------------------------------------------------------------------------
	//! @brief	代入演算子
	//!
	//! @param	object	代入するオブジェクト
	//-------------------------------------------------------------------------
	void operator = (SharedPtr<T>& object)
	{
		m_object = object.m_object;
		m_count  = object.m_count;
		++(*m_count);
	}

	//-------------------------------------------------------------------------
	//! @brief	リファレンス取得
	//!
	//! @return	オブジェクトへのリファレンス
	//-------------------------------------------------------------------------
	operator T&()
	{ 
		return *m_object;
	}

	//-------------------------------------------------------------------------
	//! @brief	ポインタ取得
	//!
	//! @return	オブジェクトへのポインタ
	//-------------------------------------------------------------------------
	operator T*()
	{
		return  m_object;
	}

	//-------------------------------------------------------------------------
	//! @brief	アロー演算子
	//!
	//! @return	オブジェクトへのポインタ
	//-------------------------------------------------------------------------
	T* operator ->()
	{
		return  m_object;
	}

	//-------------------------------------------------------------------------
	//! @brief	ポインタ取得
	//!
	//! @return	オブジェクトへのポインタ
	//-------------------------------------------------------------------------
	T* Get()
	{
		return  m_object;
	}

private:
	T*		m_object;
	int*	m_count;
};

} //namespace wx

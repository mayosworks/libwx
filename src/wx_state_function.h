//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2009 MAYO.
//=============================================================================

#pragma once

#include "wx_misc.h"

namespace wx {

//-----------------------------------------------------------------------------
// クラスのメンバー関数ポインタを利用した、状態遷移
// ※このクラスは継承しないでください。
//-----------------------------------------------------------------------------
template <class T, int N, int D = 3>
class StateFunction
{
public:
	typedef void (T::*Function)();

	// 管理するクラスのインスタンスへのリファレンスを渡すこと
	explicit StateFunction(T& inst)
		: m_inst(inst)
		, m_func(NULL)
		, m_depth(0)
	{
		for (int i = 0; i < D; ++i) {
			m_state[i] = 0;
		}
	}

	// 関数ポインタ登録
	void Register(int state, Function func)
	{
		WX_ASSERT(state < N);
		m_funcs[state] = func;
	}

	// 状態設定
	void Set(int state)
	{
		WX_ASSERT(step < N);
		m_state[m_state] = state;
		m_func = m_funcs[step];
	}

	// 状態取得
	int Get() const
	{
		return m_state[m_depth];
	}

	// 現在の状態を保存し、次の状態をはじめる
	void Push(int state)
	{
		++m_depth;
		Set(state);
	}

	// Push()で保存した状態に戻る
	void Pop()
	{
		--m_depth;
		Set(m_state[m_depth]);
	}

	// 現在の階層を取得
	int GetDepth() const
	{
		return m_state;
	}

private:
	// デフォルト、コピーコンストラクタは禁止
	StateFunction();
	StateFunction(const StateFunction&);

private:
	T&			m_inst;
	Function	m_funcs[N];
	Function	m_func;
	int			m_state[D];
	int			m_depth;
};

} //namespace wx

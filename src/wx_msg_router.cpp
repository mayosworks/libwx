//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_misc.h"
#include "wx_msg_router.h"

namespace wx {

//-----------------------------------------------------------------------------
// 定義
//-----------------------------------------------------------------------------
IMsgListener* MsgRouter::s_root = NULL;

//-----------------------------------------------------------------------------
// リスナー追加
//-----------------------------------------------------------------------------
void MsgRouter::AddListener(IMsgListener* listener)
{
	// 先頭に追加
	if (listener) {
		listener->m_link_listener = s_root;
		s_root = listener;
	}
}

//-----------------------------------------------------------------------------
// リスナー削除
//-----------------------------------------------------------------------------
void MsgRouter::RemoveListener(IMsgListener* listener)
{
	IMsgListener* node = s_root;
	IMsgListener* prev = s_root;
	while (node != listener) {
		prev = node;
		node = node->m_link_listener;
	}

	// ノードを見つけたら、単方向リンクリストから外す
	if (node == listener) {
		prev->m_link_listener = node->m_link_listener;
		listener->m_link_listener = NULL;
	}
}

//-----------------------------------------------------------------------------
// メッセージを配信する
//-----------------------------------------------------------------------------
bool MsgRouter::Dispatch(MSG& msg)
{
	for (IMsgListener* node = s_root; node; node = node->m_link_listener) {
		if (node->ProcessMessage(msg)) {
			return true;
		}
	}

	return false;
}

} //namespace wx

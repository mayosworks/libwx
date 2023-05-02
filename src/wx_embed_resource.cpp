//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_embed_resource.h"

namespace wx {

//-----------------------------------------------------------------------------
// デフォルトコンストラクタ
//-----------------------------------------------------------------------------
EmbedResource::EmbedResource()
	: m_module(NULL)
	, m_loaded(false)
	, m_rsrc(NULL)
	, m_hmem(NULL)
	, m_data(NULL)
{
}

//-----------------------------------------------------------------------------
// モジュールハンドル指定コンストラクタ
//-----------------------------------------------------------------------------
EmbedResource::EmbedResource(HMODULE module)
	: m_module(module)
	, m_loaded(false)
	, m_rsrc(NULL)
	, m_hmem(NULL)
	, m_data(NULL)
{
}

//-----------------------------------------------------------------------------
// モジュールファイル名指定コンストラクタ
//-----------------------------------------------------------------------------
EmbedResource::EmbedResource(PCTSTR exe_name)
	: m_module(NULL)
	, m_loaded(false)
	, m_rsrc(NULL)
	, m_hmem(NULL)
	, m_data(NULL)
{
	m_module = LoadLibraryEx(exe_name, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if (m_module) {
		m_loaded = true;
	}
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
EmbedResource::~EmbedResource()
{
	Free();

	if (m_loaded) {
		FreeLibrary(m_module);
	}

	m_module = NULL;
}

//-----------------------------------------------------------------------------
// リソースIDとタイプ名をしてリソースを読み込む
//-----------------------------------------------------------------------------
bool EmbedResource::Load(UINT res_id, PCTSTR res_type)
{
	return Load(MAKEINTRESOURCE(res_id), res_type);
}

//-----------------------------------------------------------------------------
// リソース名とタイプ名をしてリソースを読み込む
//-----------------------------------------------------------------------------
bool EmbedResource::Load(PCTSTR res_name, PCTSTR res_type)
{
	Free();

	m_rsrc = FindResource(m_module, res_name, res_type);
	if (!m_rsrc) {
		return false;
	}

	m_hmem = LoadResource(m_module, m_rsrc);
	if (!m_hmem) {
		m_rsrc = NULL;
		return false;
	}

	m_data = LockResource(m_hmem);
	if (!m_data) {
		Free();
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------
// リソースを解放する
//-----------------------------------------------------------------------------
void EmbedResource::Free()
{
	if (m_hmem) {
		FreeResource(m_hmem);
	}

	m_rsrc = NULL;
	m_hmem = NULL;
	m_data = NULL;
}

//-----------------------------------------------------------------------------
// リソースデータを取得
//-----------------------------------------------------------------------------
void* EmbedResource::GetData() const
{
	return m_data;
}

//-----------------------------------------------------------------------------
// リソースデータのサイズを取得
//-----------------------------------------------------------------------------
UINT EmbedResource::GetSize() const
{
	return SizeofResource(m_module, m_rsrc);
}

} //namespace wx

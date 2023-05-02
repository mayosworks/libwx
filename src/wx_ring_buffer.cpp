//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2006 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_misc.h"
#include "wx_ring_buffer.h"

namespace wx {
namespace {
// 定義
const int RETRY_CNT =  10;
const int WAIT_TIME = 200;

} //namespace

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
RingBuffer::RingBuffer()
	: m_rend(NULL)
	, m_wend(NULL)
	, m_retry(false)
	, m_spos(NULL)
	, m_epos(NULL)
	, m_rpos(NULL)
	, m_wpos(NULL)
	, m_free(0)
	, m_used(0)
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
RingBuffer::~RingBuffer()
{
}

//-----------------------------------------------------------------------------
// リングバッファ初期化
//-----------------------------------------------------------------------------
bool RingBuffer::Initialize(UINT size, bool retry)
{
	if (!m_buf.Initialize(size)) {
		return false;
	}

	m_rend = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_wend = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_rend || !m_wend) {
		Finalize();
		return false;
	}

	m_spos  = static_cast<BYTE*>(m_buf.GetData());
	m_epos  = m_spos + size;
	m_retry = retry;

	Clear();
	return true;
}

//-----------------------------------------------------------------------------
// リングバッファ破棄
//-----------------------------------------------------------------------------
void RingBuffer::Finalize()
{
	m_buf.Finalize();

	if (m_rend) {
		CloseHandle(m_rend);
	}

	if (m_wend) {
		CloseHandle(m_wend);
	}

	m_rend  = NULL;
	m_wend  = NULL;
	m_retry = false;
	m_spos =  NULL;
	m_epos = NULL;
	m_rpos = NULL;
	m_wpos = NULL;
}

//-----------------------------------------------------------------------------
// バッファをクリアする
//-----------------------------------------------------------------------------
void RingBuffer::Clear()
{
	m_buf.Clear();

	ResetEvent(m_rend);
	ResetEvent(m_wend);

	m_rpos = m_spos;
	m_wpos = m_spos;
	m_free = m_buf.GetSize();
	m_used = 0;
}

//-----------------------------------------------------------------------------
// データを読み取る
//-----------------------------------------------------------------------------
UINT RingBuffer::Read(void* buffer, UINT size)
{
	WX_NULL_ASSERT(buffer);

	if (!buffer || !size) {
		return 0;
	}

	if (WaitForSingleObject(m_wend, 0) == WAIT_OBJECT_0) {
		return 0;
	}

	BYTE* bufptr = static_cast<BYTE*>(buffer);
	UINT  readed = 0;

	for (int i = 0; i < RETRY_CNT; ++i) {
		m_lock.Acquire();

		if (size < m_used) {
			readed += ReadData(bufptr, size);
			m_lock.Release();
			return readed;
		}

		bool retry = m_retry;
		UINT used  = m_used;

		readed += ReadData(bufptr, used);
		m_lock.Release();

		bufptr += used;
		size   -= used;

		if (!retry) {
			return readed;
		}

		if (WaitForSingleObject(m_rend, 0) == WAIT_OBJECT_0 || WaitForSingleObject(m_wend, 0) == WAIT_OBJECT_0) {
			return readed;
		}

		Sleep(WAIT_TIME);
	}

	return readed;
}

//-----------------------------------------------------------------------------
// データを書き込む
//-----------------------------------------------------------------------------
UINT RingBuffer::Write(void* buffer, UINT size)
{
	WX_NULL_ASSERT(buffer);

	if (!buffer || !size) {
		return 0;
	}

	if (WaitForSingleObject(m_rend, 0) == WAIT_OBJECT_0) {
		return 0;
	}

	BYTE* bufptr  = static_cast<BYTE*>(buffer);
	UINT  written = 0;

	for (int i = 0; i < RETRY_CNT; ++i) {
		m_lock.Acquire();

		if (size < m_free) {
			written += WriteData(bufptr, size);
			m_lock.Release();
			return written;
		}

		bool retry = m_retry;
		UINT free = m_free;

		written += WriteData(bufptr, free);
		m_lock.Release();

		bufptr += free;
		size   -= free;

		if (!retry) {
			return written;
		}

		if (WaitForSingleObject(m_rend, 0) == WAIT_OBJECT_0 || WaitForSingleObject(m_wend, 0) == WAIT_OBJECT_0) {
			return written;
		}

		Sleep(WAIT_TIME);
	}

	return written;
}

//-----------------------------------------------------------------------------
// 読み取り終了シグナルをセット
//-----------------------------------------------------------------------------
void RingBuffer::ReadEnd()
{
	SetEvent(m_rend);
}

//-----------------------------------------------------------------------------
// 書き込み終了シグナルをセット
//-----------------------------------------------------------------------------
void RingBuffer::WriteEnd()
{
	SetEvent(m_wend);
}

//-----------------------------------------------------------------------------
// バッファサイズ取得
//-----------------------------------------------------------------------------
UINT RingBuffer::GetBufferSize() const
{
	return m_buf.GetSize();
}

//-----------------------------------------------------------------------------
// バッファの空きサイズを取得
//-----------------------------------------------------------------------------
UINT RingBuffer::GetFreeSize() const
{
	ALock lock(m_lock);
	return m_free;
}

//-----------------------------------------------------------------------------
// バッファの使用サイズを取得
//-----------------------------------------------------------------------------
UINT RingBuffer::GetUsedSize() const
{
	ALock lock(m_lock);
	return m_used;
}

//-----------------------------------------------------------------------------
// データ読み取り
//-----------------------------------------------------------------------------
UINT RingBuffer::ReadData(BYTE* data, UINT size)
{
	if (m_rpos + size > m_epos) {
		size_t size1 = m_epos - m_rpos;
		size_t size2 = size   - size1;

		CopyMemory(data,         m_rpos, size1);
		CopyMemory(data + size1, m_spos, size2);

		m_rpos = m_spos + size2;
	}
	else {
		CopyMemory(data, m_rpos, size);
		m_rpos += size;
	}

	m_free += size;
	m_used -= size;
	return size;
}

//-----------------------------------------------------------------------------
// データ書き込み
//-----------------------------------------------------------------------------
UINT RingBuffer::WriteData(BYTE* data, UINT size)
{
	if (m_wpos + size > m_epos) {
		size_t size1 = m_epos - m_wpos;
		size_t size2 = size   - size1;

		CopyMemory(m_wpos, data,         size1);
		CopyMemory(m_spos, data + size1, size2);

		m_wpos = m_spos + size2;
	}
	else {
		CopyMemory(m_wpos, data, size);
		m_wpos += size;
	}

	m_free -= size;
	m_used += size;
	return size;
}

} //namespace wx

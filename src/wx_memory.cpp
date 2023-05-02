//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

// コンパイラの警告抑制
#pragma warning(disable: 4200)	//構造体または共用体中にサイズが 0 の配列があります。

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <crtdbg.h>
#include <map>
#include "wx_memory.h"

namespace wx {
namespace {

// 定義
const int TAG_OFFSET = sizeof(void*);	// タグを使う際のオフセット

HANDLE g_heap = NULL;	// ヒープ
DWORD  g_opt = 0;		// 確保オプション

// デバッグ用
#if WX_ENABLE_MEMCHK

// デバッグ時に使用するメモリタグ
struct MemTag
{
	UINT	frame;	// 確保したフレーム数
	char	tag[];	// タグ文字列
};

#endif //WX_ENABLE_MEMCHK

} //namespace


//-----------------------------------------------------------------------------
// 指定ヒープからメモリを確保する
//-----------------------------------------------------------------------------
static void* AllocFromHeap(HANDLE heap, UINT opt, size_t size)
{
#if WX_ENABLE_MEMCHK
	void* mem = HeapAlloc(heap, opt, size);
	if(mem) {
		*static_cast<UINT*>(mem) = 0;
		return AddOffset(mem, TAG_OFFSET);
	}

	return NULL;
#else //WX_ENABLE_MEMCHK
	return HeapAlloc(heap, opt, size);
#endif //WX_ENABLE_MEMCHK
}

//-----------------------------------------------------------------------------
// 指定ヒープのメモリを解放する
//-----------------------------------------------------------------------------
static void FreeToHeap(HANDLE heap, void* mem, UINT opt)
{
#if WX_ENABLE_MEMCHK
	if(mem) {
		void* ptr = AddOffset(mem, -TAG_OFFSET);
		void* mem_tag = reinterpret_cast<void*>(*static_cast<UINT*>(ptr));
		if(mem_tag) {
			Memory::SysFree(mem_tag);
		}

		HeapFree(heap, opt, ptr);
	}
#else //WX_ENABLE_MEMCHK
	HeapFree(heap, opt, mem);
#endif //WX_ENABLE_MEMCHK
}

//-----------------------------------------------------------------------------
// ヒープに対し、LFHを設定する
//-----------------------------------------------------------------------------
static bool SetLFH(HANDLE heap)
{
	ULONG enable_lfh = 2;

	BOOL result = HeapSetInformation(heap,
		HeapCompatibilityInformation, &enable_lfh, sizeof(enable_lfh));

	return (result != FALSE);
}


//-----------------------------------------------------------------------------
// ヒープの情報を取得する
//-----------------------------------------------------------------------------
static void GetHeapStatus(HANDLE heap, MemoryStatus& status)
{
	ZeroMemory(&status, sizeof(status));

	PROCESS_HEAP_ENTRY entry;
	DWORD max_size = 0;

	ZeroMemory(&entry, sizeof(entry));

	HeapLock(heap);
	while (HeapWalk(heap, &entry)) {
		if (entry.wFlags & PROCESS_HEAP_REGION) {
			// コミットページ領域をヒープサイズに換算
			status.total_size += entry.Region.dwCommittedSize;
			status.total_size += entry.Region.dwUnCommittedSize;
		}
		else if (entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE) {
		}
		else if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
			status.used_size += entry.cbData;
			++status.block_num;

			// リージョンインデックスがある場合は、その領域のサイズを確認する
			if (entry.iRegionIndex > 0) {
				MEMORY_BASIC_INFORMATION m;
				ZeroMemory(&m, sizeof(m));
				VirtualQuery(entry.lpData, &m, sizeof(m));

				status.total_size += static_cast<UINT>(m.RegionSize);
			}
		}
		else {
			status.free_size += entry.cbData;

			// 最大空きブロックサイズを取得
			max_size = Max(max_size, entry.cbData);
		}
	}

	HeapUnlock(heap);

	status.max_size = max_size;
}

//-----------------------------------------------------------------------------
// ヒープのメモリをログ出力へダンプする
//-----------------------------------------------------------------------------
static void DumpHeap(HANDLE heap, const char* heap_name)
{
#if 0
	PROCESS_HEAP_ENTRY entry;
	ZeroMemory(&entry, sizeof(entry));

	WX_DEBUG_LOG("Heap[%s] dump start\n", heap_name);
	WX_DEBUG_LOG("--------------------------------------------------\n");

	HeapLock(heap);
	while (HeapWalk(heap, &entry)) {
		if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
#if WX_ENABLE_MEMCHK
			// 先頭はメモリタグになっているはず
			MemTag* mem_tag = reinterpret_cast<MemTag*>(*static_cast<UINT*>(entry.lpData));
			if (mem_tag && !IsBadReadPtr(mem_tag, sizeof(mem_tag) + 1)) {
				WX_DEBUG_LOG("%p, %10d, %d, %s\n", entry.lpData, entry.cbData, mem_tag->frame, mem_tag->tag);
			} else {
				WX_DEBUG_LOG("%p, %10d\n", entry.lpData, entry.cbData);
			}

#else
			// MEMCHKが定義されていないなら、普通に出力
			WX_DEBUG_LOG("%p, %10d\n", entry.lpData, entry.cbData);

#endif //WX_ENABLE_MEMCHK
		}
	}

	HeapUnlock(heap);

	WX_DEBUG_LOG("--------------------------------------------------\n");
#else
	WX_UNUSED(heap);
	WX_UNUSED(heap_name);
#endif
}

//-----------------------------------------------------------------------------
// メモリ管理を初期化する
//-----------------------------------------------------------------------------
bool Memory::Initialize(UINT size, UINT flags)
{
	WX_ASSERT(!g_heap, "ヒープは初期化済みです。");

	// CRTヒープにLFHを設定する
	SetLFH(reinterpret_cast<HANDLE>(_get_heap_handle()));

	DWORD opt = (flags & MEM_NOSER)? HEAP_NO_SERIALIZE : 0;
	HANDLE heap = HeapCreate(opt, size, size);
	if (!heap) {
		return false;
	}

	SetLFH(heap);

	g_heap = heap;

	// 確保オプション設定
	g_opt = 0;
	g_opt |= (flags & MEM_CLEAR)? HEAP_ZERO_MEMORY : 0;
	g_opt |= (flags & MEM_NOSER)? HEAP_NO_SERIALIZE : 0;

#ifdef _DBEUG
	int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	_CrtSetDbgFlag(tmpFlag | _CRTDBG_LEAK_CHECK_DF);
#endif //_DBEUG

	return true;
}

//-----------------------------------------------------------------------------
// メモリ管理を終了する
//-----------------------------------------------------------------------------
void Memory::Finalize()
{
	Dump();

	if (g_heap) {
		HeapDestroy(g_heap);
		g_heap = NULL;
	}
}

//-----------------------------------------------------------------------------
// 通常メモリ確保
//-----------------------------------------------------------------------------
void* Memory::Alloc(size_t size)
{
	return AllocFromHeap(g_heap, g_opt, size);
}

//-----------------------------------------------------------------------------
// Allocで確保したメモリの解放
//-----------------------------------------------------------------------------
void Memory::Free(void* mem)
{
	FreeToHeap(g_heap, mem, g_opt);
}

//-----------------------------------------------------------------------------
// メモリにタグ付けする
//-----------------------------------------------------------------------------
void Memory::SetTag(void* mem, const char* tag)
{
#if WX_ENABLE_MEMCHK
	if (!mem) {
		return;
	}

	size_t tlen = strlen(tag) + 1;
	MemTag* mem_tag = static_cast<MemTag*>(SysAlloc(sizeof(mem_tag) + tlen));
	if (!mem_tag) {
		return;
	}

	mem_tag->frame = FrameTimer::GetFrameCount();
	memcpy(mem_tag->tag, tag, tlen);

	void* addr = AddOffset(mem, -TAG_OFFSET);
	void* old_tag = reinterpret_cast<void*>(*static_cast<UINT*>(addr));
	// すでに設定してあったら、それを消す
	if (old_tag) {
		SysFree(old_tag);
	}

	*static_cast<UINT*>(addr) = reinterpret_cast<UINT>(mem_tag);

#else
	WX_UNUSED(mem);
	WX_UNUSED(tag);
#endif //WX_ENABLE_MEMCHK
}

//-----------------------------------------------------------------------------
// メモリ状態を取得する
//-----------------------------------------------------------------------------
void Memory::GetStatus(MemoryStatus& status)
{
	GetHeapStatus(g_heap, status);
}

//-----------------------------------------------------------------------------
// システム全体の空きメモリサイズを取得
//-----------------------------------------------------------------------------
UINT Memory::GetGlobalFreeSize()
{
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);

	GlobalMemoryStatusEx(&ms);
	return static_cast<UINT>(ms.ullAvailPhys);
}

//-----------------------------------------------------------------------------
// 確保されているメモリをダンプする
//-----------------------------------------------------------------------------
void Memory::Dump()
{
	DumpHeap(g_heap, "ヒープ");

#ifdef _DEBUG
	_CrtDumpMemoryLeaks();
#endif //_DEBUG
}

//-----------------------------------------------------------------------------
// システムメモリ確保
//-----------------------------------------------------------------------------
void* Memory::SysAlloc(size_t size)
{
	return HeapAlloc(GetProcessHeap(), 0, size);
}

//-----------------------------------------------------------------------------
// SysAllocで確保したメモリの解放
//-----------------------------------------------------------------------------
void Memory::SysFree(void* mem)
{
	HeapFree(GetProcessHeap(), 0, mem);
}

} //namespace gm


//-----------------------------------------------------------------------------
// グローバル new
//-----------------------------------------------------------------------------
void* operator new(size_t size)
{
	return wx::Memory::Alloc(size);
}

//-----------------------------------------------------------------------------
// グローバル new []
//-----------------------------------------------------------------------------
void* operator new [](size_t size)
{
	return wx::Memory::Alloc(size);
}

//-----------------------------------------------------------------------------
// グローバル delete
//-----------------------------------------------------------------------------
void operator delete(void* mem)
{
	wx::Memory::Free(mem);
}

//-----------------------------------------------------------------------------
// グローバル delete []
//-----------------------------------------------------------------------------
void operator delete[](void* mem)
{
	wx::Memory::Free(mem);
}

#if WX_ENABLE_MEMCHK

//-----------------------------------------------------------------------------
// デバッグ版 new
//-----------------------------------------------------------------------------
void* operator new(size_t size, const char* file, int line)
{
	char buf[MAX_PATH];
	_snprintf(buf, sizeof(buf) - 1, "%s:%d", file, line);

	void* mem = wx::Memory::Alloc(size);
	wx::Memory::SetTag(mem, buf);
	return mem;
}

//-----------------------------------------------------------------------------
// デバッグ版 new []
//-----------------------------------------------------------------------------
void* operator new [](size_t size, const char* file, int line)
{
	char buf[MAX_PATH];
	_snprintf(buf, sizeof(buf) - 1, "%s:%d", file, line);

	void* mem = wx::Memory::Alloc(size);
	wx::Memory::SetTag(mem, buf);
	return mem;
}

//-----------------------------------------------------------------------------
// デバッグ版 delete
//-----------------------------------------------------------------------------
void operator delete(void* mem, const char* file, int line)
{
	WX_UNUSED(file);
	WX_UNUSED(line);

	wx::Memory::Free(mem);
}

//-----------------------------------------------------------------------------
// デバッグ版 delete []
//-----------------------------------------------------------------------------
void operator delete [](void* mem, const char* file, int line)
{
	WX_UNUSED(file);
	WX_UNUSED(line);

	wx::Memory::Free(mem);
}

#endif //WX_ENABLE_MEMCHK

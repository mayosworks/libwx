//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_misc.h"
#include "wx_bin_rw.h"

namespace wx {

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
BinReader::BinReader()
	: m_file(INVALID_HANDLE_VALUE)
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
BinReader::~BinReader()
{
	Close();
}

//-----------------------------------------------------------------------------
// ファイルを読み取り用に開く
//-----------------------------------------------------------------------------
bool BinReader::Open(PCTSTR file_path)
{
	WX_NULL_ASSERT(file_path);

	// オブジェクトの再利用は禁止
	if (!file_path || m_file != INVALID_HANDLE_VALUE) {
		return false;
	}

	m_file = CreateFile(file_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_file == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// ファイルを閉じる
//-----------------------------------------------------------------------------
void BinReader::Close()
{
	if (m_file != INVALID_HANDLE_VALUE) {
		CloseHandle(m_file);
	}

	m_file = INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------
// ファイルから読み取る
//-----------------------------------------------------------------------------
DWORD BinReader::Read(void* buffer, DWORD size)
{
	if (m_file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	DWORD readed_size = 0;

	if (!ReadFile(m_file, buffer, size, &readed_size, NULL)) {
		return 0;
	}

	return readed_size;
}

//-----------------------------------------------------------------------------
// ファイルポインタを設定する
//-----------------------------------------------------------------------------
DWORD BinReader::SetPointer(long new_pointer, DWORD move_method)
{
	if (m_file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	return SetFilePointer(m_file, new_pointer, NULL, move_method);
}

//-----------------------------------------------------------------------------
// ファイルサイズを取得する
//-----------------------------------------------------------------------------
DWORD BinReader::GetSize() const
{
	if (m_file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	return GetFileSize(m_file, NULL);
}

//-----------------------------------------------------------------------------
// まだ読み取ってないサイズを取得する
//-----------------------------------------------------------------------------
DWORD BinReader::GetLeftSize() const
{
	if (m_file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	return (GetSize() - SetFilePointer(m_file, 0, NULL, FILE_CURRENT));
}


//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
BinWriter::BinWriter()
	: m_file(INVALID_HANDLE_VALUE)
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
BinWriter::~BinWriter()
{
	Close();
}

//-----------------------------------------------------------------------------
// ファイルを書き込み用に開く
//-----------------------------------------------------------------------------
bool BinWriter::Open(PCTSTR file_path)
{
	// オブジェクトの再利用は禁止
	if (!file_path || m_file != INVALID_HANDLE_VALUE) {
		return false;
	}

	m_file = CreateFile(file_path, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_file == INVALID_HANDLE_VALUE) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// ファイルを閉じる
//-----------------------------------------------------------------------------
void BinWriter::Close()
{
	if (m_file != INVALID_HANDLE_VALUE) {
		CloseHandle(m_file);
	}

	m_file = INVALID_HANDLE_VALUE;
}

//-----------------------------------------------------------------------------
// ファイルに書き込む
//-----------------------------------------------------------------------------
DWORD BinWriter::Write(const void* data, DWORD size)
{
	if (m_file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	DWORD written_size = 0;

	if (!WriteFile(m_file, data, size, &written_size, NULL)) {
		return 0;
	}

	return written_size;
}

//-----------------------------------------------------------------------------
// ファイルポインタを設定する
//-----------------------------------------------------------------------------
DWORD BinWriter::SetPointer(long new_pointer, DWORD move_method)
{
	if (m_file == INVALID_HANDLE_VALUE) {
		return 0;
	}

	return SetFilePointer(m_file, new_pointer, NULL, move_method);
}

} //namespace wx

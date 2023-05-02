//=============================================================================
// Auxiliary library for Windows API (C++)
//                                                     Copyright (c) 2007 MAYO.
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "wx_critical_section.h"

namespace wx {

class CriticalSection::Impl
{
public:
	Impl();
	~Impl();

	void Acquire();
	void Release();

private:
	CRITICAL_SECTION	m_cs;
};

CriticalSection::Impl::Impl()
{
	InitializeCriticalSection(&m_cs);
}

CriticalSection::Impl::~Impl()
{
	DeleteCriticalSection(&m_cs);
}

void CriticalSection::Impl::Acquire()
{
	EnterCriticalSection(&m_cs);
}

void CriticalSection::Impl::Release()
{
	LeaveCriticalSection(&m_cs);
}


CriticalSection::CriticalSection()
	: m_impl(new Impl())
{
}

CriticalSection::~CriticalSection()
{
	delete m_impl;
	m_impl = NULL;
}

void CriticalSection::Acquire() const
{
	const_cast<Impl*>(m_impl)->Acquire();
}

void CriticalSection::Release() const
{
	const_cast<Impl*>(m_impl)->Release();
}

} //namespace wx

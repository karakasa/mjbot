#include "stdafx.h"
#include "PlatformAdapter.h"
// This is the Windows Version of PlatformAdapter

void MemoryPool::initialize(size_t size, size_t maximumSize)
{
	handle = HeapCreate(0, size, maximumSize);
}

void MemoryPool::deinitialize()
{
	HeapDestroy(handle);
}

void* MemoryPool::allocate(size_t size)
{
	return HeapAlloc(handle, 0, size);
}

void MemoryPool::free(void* memblock)
{
	HeapFree(handle, 0, memblock);
}

void CriticalSection::create()
{
	token = new CRITICAL_SECTION;
	InitializeCriticalSection((CRITICAL_SECTION*)token);
}
void CriticalSection::release()
{
	CRITICAL_SECTION* iToken = (CRITICAL_SECTION*)token;
	DeleteCriticalSection(iToken);
	delete iToken;
}
void CriticalSection::enter()
{
	EnterCriticalSection((CRITICAL_SECTION*)token);
}
void CriticalSection::leave()
{
	LeaveCriticalSection((CRITICAL_SECTION*)token);
}
void CriticalSection::waitForComplete()
{
	enter();
	leave();
}
#include "stdafx.h"

namespace mma
{
	HANDLE heap;
	void init()
	{
		heap = HeapCreate(0, 1048576, 67108864);
	}
	void deinit()
	{
		HeapDestroy(heap);
	}
	void* alloc(int size)
	{
		return HeapAlloc(heap, 0, size);
	}
	bool free(void* ptr)
	{
		return (bool)HeapFree(heap, 0, ptr);
	}
}

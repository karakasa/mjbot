#pragma once

namespace mma
{
	void init();
	void deinit();
	void* alloc(int size);
	bool free(void* ptr);
}

#pragma once

// 用于屏蔽平台差异，方便迁移到不同平台

class MemoryPool
{
private:
public:
	void* handle;
	void initialize(size_t size, size_t maximumSize = 0);
	void deinitialize();
	void* allocate(size_t size);
	void free(void* memblock);
};

class CriticalSection
{
private:
public:
	volatile void* token;
	void create();
	void release();
	void enter();
	void leave();
	void waitForComplete();
};
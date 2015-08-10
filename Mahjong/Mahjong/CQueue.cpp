#include "stdafx.h"
#include "MemoryLeakMonitor.h"
#include "CQueue.h"

	template <typename T>
	void CQueue<T>::init(int init, int start)
	{
		size = init; //初始大小
		base = new T[size];
		if (!base) {
			size = 0;
			return;
		};//存储分配失败
		MemoryLeakMonitor::addMonitor(base, sizeof(T)*size, "CQUEUE_INIT");
		pos = 0;
		startId = start;
	}
	template <typename T>
	CQueue<T>::~CQueue()
	{
		if (base)
			delete[] base;
		MemoryLeakMonitor::removeMonitor(base);
	}
	template <typename T>
	void CQueue<T>::push_back(const T& data)
	{
		if (pos == size)
		{
			pos = 0;
		}
		base[pos] = data;
		pos++;
		startId++;
	}
	template <typename T>
	void CQueue<T>::get(const int id, T* container)
	{
		int sPos = id - startId + pos;
		while (sPos < 0)
			sPos += size;
		sPos %= size;
		*container = base[sPos];
	}
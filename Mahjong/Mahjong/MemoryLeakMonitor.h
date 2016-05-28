#pragma once

// 主动的跟踪动态内存的使用情况，仅在 VS 配置为“Debug”时才会开启。

namespace MemoryLeakMonitor
{
	// 添加跟踪项。应在申请一块动态内存后调用。
	// pointer : 跟踪的指针起始点
	// size : 动态内存大小
	// desc : 该指针指向的对象的功能说明
	// 返回值 : 无
	void addMonitor(void* pointer, int size, const char* desc);

	// 删除跟踪项。应该释放一块动态内存后调用。
	// pointer : 跟踪的指针起始点
	// 返回值 : 无
	void removeMonitor(void* pointer);

	// 显示跟踪状态。跟踪状态会直接输出到 std:cerr
	// 返回值 : 0 为所有跟踪项均被成功回收。-1 为存在未能成功回收的项目。
	int gc();
}
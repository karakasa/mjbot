#pragma once

// 实现了事件调度

#include "SLWNetwork.h"
#include "SimpleAI.h"
#include "MemoryLeakMonitor.h"
#include "ModernMemoryAllocator.h"
#include "PlatformAdapter.h"

class eb
{
private:
	std::default_random_engine* e1;
	int result;
	int queueDepth = 0;
	client shu[4];
	DWORD threadId;

	std::queue<ebRequest> msgQueue;
	std::queue<ebEvent> evtQueue;

	bool working = true;
	CriticalSection evt;
	
	HANDLE sema;

	int clientType[4] = { 0 };
	EventBusUser* clientHandle[4];
	MatchingUser* currentMatch;
	bool customRemoteFuncEnabled = false;
	LPVOID crfParam;

	void shuffle_internal();
	void roll_internal();
	void shuffle();

public:
	// 初始化 EventBus
	// 返回值: 无
	void init();

	// 释放 EventBus
	// 返回值: 无
	void deinit();

	// 清空消息队列（不建议直接调用）
	// 返回值: 无
	void clear();

	// 发送/广播消息（不建议直接调用）
	// id : 客户 ID，从 0 - 3
	// 其他参数参考 aiMessage 中接口定义
	// 返回值: 无
	void send(int id, unsigned char msgType, unsigned int par1, unsigned int par2);
	void send(int id, unsigned char msgType, unsigned int par1);
	void send(int id, unsigned char msgType);
	void send(int id, unsigned char msgType, unsigned int par1, void* payload, int lpayload);
	void broadcast(unsigned char msgType, unsigned int par1, void* payload, int lpayload);
	void broadcast(unsigned char msgType, unsigned int par1, unsigned int par2);
	void broadcast(unsigned char msgType, unsigned int par1);
	void broadcast(unsigned char msgType);

	// 收到远程返回值，并加入返回值处理队列。由外部调用。
	// 典型的情况包括，远程发来的数据，界面事件等
	// hClient : 客户句柄
	// response : 返回值
	// 返回值: 无
	void receiveEvent(EventBusUser* hClient, int response);

	// 向客户发送一条消息（除特殊情况外，不建议直接调用）
	// rq : 消息
	// 返回值: 无
	void completeRequest(const ebRequest& rq);

	// 开始游戏，设置完游戏模式后要开始才会开始。该函数会立即返回。
	// 返回值: 成功与否
	bool run();

	// 设置当前 EventBus 处理的赛事（内部函数）
	// proceedMath : 当前的赛事，实现了 MatchingUser::receiveEvent 函数，具体实现请参考 match::receiveEvent
	void setMatching(MatchingUser* proceedMatch);

	// 等待当前游戏结束后返回。
	// 返回值 : 真为正常返回，假为当前没有游戏在进行。
	bool waitUntilEnd();

	// 设置转发/接收事件的客户
	// id : 客户序号
	// client : 对应的实现了 EventBusUser 接口的类
	void assignClient(int id, EventBusUser* client);

	// 内部工作线程，不要直接调用。
	DWORD workingThreadInternal();

	void roll();
	void finish();
};

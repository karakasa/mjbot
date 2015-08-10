#pragma once

// 实现了事件调度

#include "SLWNetwork.h"
#include "SimpleAI.h"
#include "MemoryLeakMonitor.h"
#include "ModernMemoryAllocator.h"

class eb
{
private:
	std::default_random_engine* e1;
	int result;
	SimpleAI ai[4];
	int queueDepth = 0;
	client shu[4];
	slw net;
	DWORD threadId;

	std::queue<ebRequest> msgQueue;
	std::queue<ebEvent> evtQueue;

	bool working = true;
	CRITICAL_SECTION evt;
	evtDealer evtFunc;
	LPVOID evtParam;
	HANDLE sema;

	int clientType[4] = { 0 };
	int clientHandle[4] = { 0 };

	bool customRemoteFuncEnabled = false;
	crf customRemoteFunc = NULL;
	LPVOID crfParam;

	void shuffle_internal();
	void roll_internal();
	void roll();
	void shuffle();
	void finish();

public:
	// 初始化 EventBus
	// 返回值: 无
	void init();

	// 释放 EventBus
	// 返回值: 无
	void deinit();

	// 设置为调试模式。将 EventBus 绑定到四个 AI
	// 返回值: 无
	void debug();

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
	// hClient : 客户句柄
	// response : 返回值
	// 返回值: 无
	void receiveEvent(int hClient, int response);

	// 向客户发送一条消息（除特殊情况外，不建议直接调用）
	// rq : 消息
	// 返回值: 无
	void completeRequest(const ebRequest& rq);

	// 设置为单人游戏模式
	// uiFunc : 玩家消息处理过程。接口参 aiMessage
	// 返回值: 无
	void startSinglePlayer(aiFunc uiFunc);

	// 设置为多人游戏模式。不足 4 人的部分会由 ai 补齐。
	// uiFunc : 本地玩家消息处理过程。接口参 aiMessage。如果为 NULL，则没有本地玩家
	// playerSocket : 一个数组，存储玩家句柄
	// playerCount : 数组的长度
	// 返回值: 无
	void startMultiPlayer(void* uiFunc, int* playerSocket, int playerCount);

	// 开始游戏，设置完游戏模式后要开始才会开始。该函数会立即返回。
	// evtFoo : 一个 evtDealer 类型的函数。参考 match::receiveEvent
	// evtP : 要传递给该函数的第一个参数
	// 返回值: 成功与否
	bool run(void* evtFoo, LPVOID evtP);

	// 开始游戏，设置完游戏模式后要开始才会开始。该函数会等待到游戏结束才返回。
	// evtFoo : 一个 evtDealer 类型的函数。参考 match::receiveEvent
	// evtP : 要传递给该函数的第一个参数
	// 返回值: 成功与否
	void runAwait(void* evtFoo, LPVOID evtP);

	// 等待当前游戏结束后返回。
	// 返回值 : 真为正常返回，假为当前没有游戏在进行。
	bool waitUntilEnd();

	// 设置自定义多人游戏处理函数。不设置时，默认为 slw::server_send
	// scrf : 一个 evtDealer 类型的函数。设置为 NULL 时即取消。
	// param : 额外需要传递的参数。
	void eb::setCustomRemoteFunc(void* scrf, LPVOID param);

	// 内部工作线程，不要直接调用。
	DWORD WINAPI workingThreadInternal();
};

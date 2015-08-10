#pragma once

#include "SLWNetwork.h"
#include "SimpleAI.h"
#include "MemoryLeakMonitor.h"
#include "ModernMemoryAllocator.h"

class eb
{
public:
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
	HANDLE sema;

	int clientType[4] = { 0 };
	int clientHandle[4] = { 0 };

	DWORD WINAPI workingThreadInternal();
	void send(int id, unsigned char msgType, unsigned int par1, unsigned int par2);
	void send(int id, unsigned char msgType, unsigned int par1);
	void send(int id, unsigned char msgType);
	void send(int id, unsigned char msgType, unsigned int par1, void* payload, int lpayload);
	void broadcast(unsigned char msgType, unsigned int par1, void* payload, int lpayload);
	void broadcast(unsigned char msgType, unsigned int par1, unsigned int par2);
	void broadcast(unsigned char msgType, unsigned int par1);
	void broadcast(unsigned char msgType);
	void roll_internal();
	void roll();
	void shuffle_internal();
	void shuffle();
	void clear();
	void init();
	void deinit();
	void receiveEvent(int hClient, int response);
	void completeRequest(const ebRequest rq);
	void finish();
	void debug();
	void startSinglePlayer(aiFunc uiFunc);
	void startMultiPlayer(void* uiFunc, int* playerSocket, int playerCount);
	bool run(void* evtFoo);
	void runAwait(void* evtFoo);
	bool waitUntilEnd();
};

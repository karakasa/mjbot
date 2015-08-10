#include "stdafx.h"

#include "SLWNetwork.h"
#include "SimpleAI.h"
#include "MemoryLeakMonitor.h"
#include "ModernMemoryAllocator.h"

#include "EventBus.h"

DWORD WINAPI workingThread(LPVOID param);

	DWORD WINAPI eb::workingThreadInternal()
	{
		while (working)
		{
			WaitForSingleObject(sema, INFINITE);
			EnterCriticalSection(&evt);
			while (!evtQueue.empty())
			{
				ebEvent eEvt = evtQueue.front();
				evtQueue.pop();
				evtFunc(evtParam, eEvt.id, eEvt.response);
				while (!msgQueue.empty())
				{
					ebRequest eq;
					eq = msgQueue.front();
					if (!msgQueue.empty())
						msgQueue.pop();
					completeRequest(eq);
				}
				//Sleep(30);
			}
			LeaveCriticalSection(&evt);
		}
		//TerminateProcess((HANDLE)-1, 0);
		return 0;
	}

	void eb::send(int id, unsigned char msgType, unsigned int par1, unsigned int par2)
	{
		ebRequest ebr;
		ebr.id = id;
		ebr.msgType = msgType;
		ebr.par1 = par1;
		ebr.par2 = par2;
		ebr.payload = NULL;
		msgQueue.push(ebr);
		queueDepth++;
		return;
	}
	void eb::send(int id, unsigned char msgType, unsigned int par1)
	{
		send(id, msgType, par1, 0);
	}
	void eb::send(int id, unsigned char msgType)
	{
		send(id, msgType, 0, 0);
	}
	void eb::send(int id, unsigned char msgType, unsigned int par1, void* payload, int lpayload)
	{
		queueDepth++;
		ebRequest ebr;
		ebr.id = id;
		ebr.msgType = msgType;
		ebr.par1 = par1;
		ebr.par2 = 0;
		char* p;
		p = (char*)mma::alloc(lpayload);
		MemoryLeakMonitor::addMonitor(p, lpayload, "EB_SEND");
		memcpy(p, payload, lpayload);
		ebr.payload = (void*)p;
		ebr.lpayload = lpayload;
		msgQueue.push(ebr);
		return;
	}
	void eb::broadcast(unsigned char msgType, unsigned int par1, void* payload, int lpayload)
	{
		send(0, msgType, par1, payload, lpayload);
		send(1, msgType, par1, payload, lpayload);
		send(2, msgType, par1, payload, lpayload);
		send(3, msgType, par1, payload, lpayload);
		return;
	}
	void eb::broadcast(unsigned char msgType, unsigned int par1, unsigned int par2)
	{
		send(0, msgType, par1, par2);
		send(1, msgType, par1, par2);
		send(2, msgType, par1, par2);
		send(3, msgType, par1, par2);
		return;
	}
	void eb::broadcast(unsigned char msgType, unsigned int par1)
	{
		broadcast(msgType, par1, 0);
	}
	void eb::broadcast(unsigned char msgType)
	{
		broadcast(msgType, 0, 0);
	}
	void eb::roll_internal()
	{
		int m = clientType[0], n = clientHandle[0];
		clientType[0] = clientType[1];
		clientType[1] = clientType[2];
		clientType[2] = clientType[3];
		clientHandle[0] = clientHandle[1];
		clientHandle[1] = clientHandle[2];
		clientHandle[2] = clientHandle[3];
		clientType[3] = m;
		clientHandle[3] = n;
	}
	void eb::roll()
	{
		send(-1, 1);
	}
	void eb::shuffle_internal()
	{
		srand((unsigned int)time(0));
		for (int i = 0; i<4; i++)
		{
			shu[i].clientHandle = clientHandle[i];
			shu[i].clientType = clientType[i];
		}
		std::random_shuffle(shu + 0, shu + 4, [this](int i) {return (*e1)() % i; });
		for (int i = 0; i<4; i++)
		{
			clientHandle[i] = shu[i].clientHandle;
			clientType[i] = shu[i].clientType;
		}
	}
	void eb::shuffle()
	{
		send(-1, 2);
	}
	void eb::clear()
	{
		EnterCriticalSection(&evt);
		evtQueue = {};
		msgQueue = {};
		LeaveCriticalSection(&evt);
	}
	void eb::init()
	{
		e1 = new std::default_random_engine((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
		mma::init();
		working = true;
		InitializeCriticalSection(&evt);
		sema = CreateSemaphore(NULL, 0, 64, NULL);
		memset(clientType, 0, sizeof clientType);
		memset(clientHandle, 0, sizeof clientHandle);
	}
	void eb::deinit()
	{
		EnterCriticalSection(&evt);
		LeaveCriticalSection(&evt); //空的一对许可区请求，完成当前许可区操作
		DeleteCriticalSection(&evt);
		CloseHandle(sema);
		delete e1;
		mma::deinit();
	}
	void eb::receiveEvent(int hClient, int response)
	{
		EnterCriticalSection(&evt);
		int cId = -1;
		for (int i = 0; i<4; i++)
			if (hClient == clientHandle[i])
				cId = i;
		ebEvent eEvt;
		eEvt.id = cId;
		eEvt.response = response;
		evtQueue.push(eEvt);
		LeaveCriticalSection(&evt);
		ReleaseSemaphore(sema, 1, NULL);
	}
	void eb::completeRequest(const ebRequest& rq)
	{
		queueDepth--;
		if (rq.id == -1)
		{
			switch (rq.msgType)
			{
			case 1:
				roll_internal();
				break;
			case 2:
				shuffle_internal();
				break;
			case 3:
				working = false;
				break;
			}
			return;
		}
		bool hasReturn = false;
		switch (clientType[rq.id])
		{
		case ct::local_ai:
			if (rq.payload == NULL)
				result = ai[clientHandle[rq.id]].aiMessage(rq.msgType, rq.par1, rq.par2, &hasReturn);
			else
				result = ai[clientHandle[rq.id]].aiMessage(rq.msgType, rq.par1, (int)(rq.payload), &hasReturn);
			if (hasReturn)
			{
				ebEvent eEvt;
				eEvt.id = rq.id;
				eEvt.response = result;
				evtQueue.push(eEvt);
			}
			break;
		case ct::remote:
			if(customRemoteFuncEnabled)
			{
				if (rq.payload == NULL)
					customRemoteFunc(crfParam, clientHandle[rq.id], rq.msgType, rq.par1, rq.par2, 0);
				else
					customRemoteFunc(crfParam, clientHandle[rq.id], rq.msgType, rq.par1, (int)rq.payload, rq.lpayload);
				break;
			}
			else {
				if (rq.payload == NULL)
					net.server_send(clientHandle[rq.id], rq.msgType, rq.par1, rq.par2);
				else
					net.server_send(clientHandle[rq.id], rq.msgType, rq.par1, 0, rq.payload, rq.lpayload);
				break;
			}
		case ct::local_user:
			if (rq.payload == NULL)
				((aiFunc)clientHandle[rq.id])(rq.msgType, rq.par1, rq.par2);
			else
				((aiFunc)clientHandle[rq.id])(rq.msgType, rq.par1, (int)rq.payload);
			break;
		}
		if (rq.payload != NULL)
		{
			char* pu;
			pu = (char*)rq.payload;
			mma::free(pu);
			MemoryLeakMonitor::removeMonitor(pu);
		}
	}
	void eb::finish()
	{
		send(-1, 3);
	}
	
	void eb::debug()
	{
		clientType[0] = ct::local_ai;
		clientType[1] = ct::local_ai;
		clientType[2] = ct::local_ai;
		clientType[3] = ct::local_ai;
		clientHandle[0] = 0;
		clientHandle[1] = 1;
		clientHandle[2] = 2;
		clientHandle[3] = 3;
	}
	void eb::startSinglePlayer(aiFunc uiFunc)
	{
		working = true;
		clientType[0] = ct::local_ai;
		clientType[1] = ct::local_ai;
		clientType[2] = ct::local_ai;
		clientType[3] = ct::local_user;
		clientHandle[0] = 0;
		clientHandle[1] = 1;
		clientHandle[2] = 2;
		clientHandle[3] = (int)uiFunc;
		shuffle_internal();
	}
	void eb::startMultiPlayer(void* uiFunc, int* playerSocket, int playerCount)
	{
		working = true;
		int index;
		if (uiFunc == NULL)
			index = 0;
		else {
			index = 1;
			clientType[0] = ct::local_user;
			clientHandle[0] = (int)uiFunc;
		}
		for (int i = 0; i<playerCount; i++)
		{
			clientType[index] = ct::remote;
			clientHandle[index] = (int)playerSocket[i];
			index++;
		}
		for (int i = 0; i<3 - playerCount + ((uiFunc == NULL)?1:0); i++)
		{
			clientType[index] = ct::local_ai;
			clientHandle[index] = i;
			index++;
		}
		shuffle_internal();
	}

	bool eb::run(void* evtFoo, LPVOID evtP)
	{
		working = true;
		evtFunc = (evtDealer)evtFoo;
		evtParam = evtP;
		HANDLE hThread = CreateThread(NULL, 0, &workingThread, this, 0, &threadId);
		if (hThread == 0)
			return false;
		CloseHandle(hThread);
		return true;
	}

	void eb::runAwait(void* evtFoo, LPVOID evtP)
	{
		working = true;
		evtFunc = (evtDealer)evtFoo;
		evtParam = evtP;
		workingThreadInternal();
	}

	bool eb::waitUntilEnd()
	{
		HANDLE hThread = OpenThread(SYNCHRONIZE, FALSE, threadId);
		if (hThread == 0)
			return false;
		if (WaitForSingleObject(hThread, INFINITE) == 0xFFFFFFFF)
		{
			CloseHandle(hThread);
			return false;
		}
		CloseHandle(hThread);
		return true;
	}

	void eb::setCustomRemoteFunc(void* scrf, LPVOID param)
	{
		crfParam = param;
		if(scrf == NULL)
		{
			customRemoteFuncEnabled = false;
			customRemoteFunc = NULL;
		}
		else {
			customRemoteFuncEnabled = true;
			customRemoteFunc = (crf)scrf;
		}
	}

DWORD WINAPI workingThread(LPVOID param)
{
	return ((eb*)param)->workingThreadInternal();
}
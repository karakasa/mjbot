#include "stdafx.h"

#include "SLWNetwork.h"
#include "SimpleAI.h"
#include "MemoryLeakMonitor.h"
#include "ModernMemoryAllocator.h"
#include "EventBus.h"

DWORD __stdcall workingThread(LPVOID param);

void eb::assignClient(int id, EventBusUser* client)
{
	clientType[id] = ct::local_user;
	clientHandle[id] = client;
}

DWORD eb::workingThreadInternal()
	{
		while (working)
		{
			WaitForSingleObject(sema, INFINITE);
			evt.enter();
			while (!evtQueue.empty())
			{
				ebEvent eEvt = evtQueue.front();
				evtQueue.pop();
				currentMatch -> receiveEvent(eEvt.id, eEvt.response);
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
			evt.leave();
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
		int m = clientType[0];
		EventBusUser* n = clientHandle[0];
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
		evt.enter();
		evtQueue = {};
		msgQueue = {};
		evt.leave();
	}
	void eb::init()
	{
		e1 = new std::default_random_engine((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
		mma::init();
		working = true;
		evt.create();
		sema = CreateSemaphore(NULL, 0, 64, NULL);
		memset(clientType, 0, sizeof clientType);
		memset(clientHandle, 0, sizeof clientHandle);
	}
	void eb::deinit()
	{
		evt.waitForComplete();
		evt.release();
		CloseHandle(sema);
		delete e1;
		mma::deinit();
	}
	void eb::receiveEvent(EventBusUser* hClient, int response)
	{
		evt.enter();
		int cId = -1;
		for (int i = 0; i<4; i++)
			if (hClient == clientHandle[i])
				cId = i;
		ebEvent eEvt;
		eEvt.id = cId;
		eEvt.response = response;
		evtQueue.push(eEvt);
		evt.leave();
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
		case ct::local_user:
			result = clientHandle[rq.id]->aiMessage(rq.msgType, rq.par1, rq.par2, &hasReturn, rq.payload);
			if (hasReturn)
			{
				ebEvent eEvt;
				eEvt.id = rq.id;
				eEvt.response = result;
				evtQueue.push(eEvt);
			}
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
	
	bool eb::run()
	{
		working = true;
		shuffle_internal();
		HANDLE hThread = CreateThread(NULL, 0, &workingThread, this, 0, &threadId);
		if (hThread == 0)
			return false;
		CloseHandle(hThread);
		return true;
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

	void eb::setMatching(MatchingUser* proceedMatch)
	{
		currentMatch = proceedMatch;
	}

DWORD __stdcall workingThread(LPVOID param)
{
	return ((eb*)param)->workingThreadInternal();
}
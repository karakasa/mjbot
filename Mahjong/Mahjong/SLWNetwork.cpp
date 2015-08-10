#include "stdafx.h"

#include "PublicFunction.h"
#include "CQueue.h"
#include "SLWNetwork.h"

/*
namespace slw
{
	char server_ip[128] = { 0 };
	bool managedLock = true;
	bool responsive = false;

	CQueue<Packet> clist;
	int cid = 1;
	unsigned int lastServerId = 0;

	
	WSAData wsaData;
	SOCKET sServer;
	SOCKET sClient;
	CRITICAL_SECTION clientFinish;
	CRITICAL_SECTION serverFinish;
	CRITICAL_SECTION clientEvent;
	CRITICAL_SECTION serverEvent;

	//参数1: 套接字句柄，参数2: 消息类型，参数3: 消息参数1，参数4: 消息参数1，参数5，Payload 指针，参数6，Payload 长度
	typedef bool(WINAPI *evtServer)(int, int, int, int, void*, int);

	//参数1: 消息类型，参数2: 消息参数1，参数3: 消息参数1，参数4，Payload 指针，参数5，Payload 长度
	typedef bool(WINAPI *evtClient)(int, int, int, void*, int);
	evtServer serverFunc = NULL;
	evtClient clientFunc = NULL;
	bool serverFinished = false, clientFinished = false;

	bool client_send(unsigned char uType, int p1, int p2, void* pl, int lpl);
	bool client_send(unsigned char uType, int p1, int p2);
	bool client_send_ctl(unsigned char type, int p1);
	bool server_send(SOCKET client, unsigned char uType, int p1, int p2, void* pl, int lpl);
	bool server_send(SOCKET client, unsigned char uType, int p1, int p2);
	bool server_send_ctl(SOCKET client, unsigned char type, int p1);

	void registerEvent(bool type, void* func, bool managed = TRUE)
	{
		managedLock = managed;
		if (type)
			serverFunc = (evtServer)func;
		else
			clientFunc = (evtClient)func;
	}

	bool initialize()
	{

		InitializeCriticalSection(&clientFinish);
		InitializeCriticalSection(&serverFinish);
		InitializeCriticalSection(&clientEvent);
		InitializeCriticalSection(&serverEvent);
		return 0 == (int)WSAStartup(MAKEWORD(2, 0), &wsaData);
	}
	bool deinitialize()
	{
		DeleteCriticalSection(&clientFinish);
		DeleteCriticalSection(&serverFinish);
		DeleteCriticalSection(&clientEvent);
		DeleteCriticalSection(&serverEvent);
		return 0 == (int)WSACleanup();
	}
	unsigned int WINAPI answer_thread(LPVOID param)
	{
		SOCKET client = (SOCKET)param;
		int nZero = 0;
		setsockopt(client, SOL_SOCKET, SO_SNDBUF, (const char*)&nZero, sizeof(int));
		if (managedLock)
			EnterCriticalSection(&serverEvent);
		//Deal with  Msg Here
		if (serverFunc != NULL)
			if (serverFunc((int)client, -2, 0, 0, NULL, 0) == false) //客户进入
			{
				closesocket(client);
				return 0;
			}
		if (managedLock)
			LeaveCriticalSection(&serverEvent);
		int bytesRecv, bytesRecvCur;
		unsigned char recvbuf[2048];
		bool breakflag = false;
		while (1)
		{
			memset(recvbuf, 0, sizeof(recvbuf));
			bytesRecv = 0;
			while (1)
			{
				EnterCriticalSection(&serverFinish);
				if (serverFinished)
				{
					breakflag = true;
					break;
				}
				LeaveCriticalSection(&serverFinish);
				//Receiving Data
				if (bytesRecv >= 2048)
				{
					printf("Data flooded\n");
					breakflag = true;
					break;
				}
				bytesRecvCur = recv(client, (char*)(recvbuf + bytesRecv), 2048 - bytesRecv, 0);
				if (bytesRecvCur == 0)
				{
					breakflag = true;
					break;
				}
				if (bytesRecvCur == SOCKET_ERROR)
				{
					int errcode = WSAGetLastError();
					if (errcode != WSAEMSGSIZE)
					{
						printf("ERR: %d\n", errcode);
						breakflag = true;
						break;
					}
				}
				bytesRecv += bytesRecvCur;
				Packet pmsg;
				ControlPacket pctlmsg;
				while (bytesRecv != 0) {
					switch (recvbuf[0])
					{
					case 0:
						for (int i = 0; i<bytesRecv - 1; i++)
							recvbuf[i] = recvbuf[i + 1];
						bytesRecv--;
						break;
					case 0xFF:
						for (int i = 0; i<bytesRecv - 1; i++)
							recvbuf[i] = recvbuf[i + 1];
						bytesRecv--;
						if ((pctlmsg.packetLength = recvbuf[0]) >= 10)
						{
							pctlmsg.crc = *((unsigned int*)(recvbuf + 1));
							pctlmsg.type = *((unsigned char*)(recvbuf + 5));
							pctlmsg.param1 = *((unsigned int*)(recvbuf + 6));
							printf("CTL MSG: %d,%d,%d,%d\n", (int)pctlmsg.packetLength, pctlmsg.crc, (int)pctlmsg.type, pctlmsg.param1);
							if (responsive)
								switch (pctlmsg.type)
								{
								case 0xFD:
									for (unsigned int pc = LOWORD(pctlmsg.param1); pc <= HIWORD(pctlmsg.param1); pc++)
									{
										Packet p;
										clist.get(pc, &p);
										if (p.id != pc)
										{
											server_send_ctl(client, 0xF0, -1); //failed to recover the communication
											continue;
										}
										else {
											server_send(client, p.uType, p.p1, p.p2); //restore
										}
									}
									break;
								}
							//Deal with ctl msg here
							if (pctlmsg.type == 3) //bye
							{
								breakflag = true;
								break;
							}
						}
						for (int i = 0; i<bytesRecv - pctlmsg.packetLength; i++)
							recvbuf[i] = recvbuf[i + pctlmsg.packetLength];
						bytesRecv -= pctlmsg.packetLength;
						break;
					default:
							if ((pmsg.packetLength = recvbuf[0]) >= 19)
							{
								pmsg.crc = *((unsigned int*)(recvbuf + 1));
								pmsg.id = *((unsigned int*)(recvbuf + 5));
								pmsg.uType = *((unsigned char*)(recvbuf + 9));
								pmsg.p1 = *((unsigned int *)(recvbuf + 10));
								pmsg.p2 = *((unsigned int *)(recvbuf + 14));
								pmsg.payloadLength = *((unsigned char *)(recvbuf + 18));
								unsigned char* payload = NULL;
								if (pmsg.packetLength > 19 && pmsg.payloadLength + 19 == pmsg.packetLength)
								{
									payload = new unsigned char[pmsg.payloadLength];
									memcpy(payload, recvbuf + 19, pmsg.packetLength);
								}
								printf("MSG: %d,%d,%d,%d,%d,%d,%d\n", (int)pmsg.packetLength, pmsg.crc, pmsg.id, (int)pmsg.uType, pmsg.p1, pmsg.p2, (int)pmsg.payloadLength);
								if (managedLock)
									EnterCriticalSection(&serverEvent);
								//Deal with  Msg Here
								if (serverFunc != NULL)
									serverFunc((int)client, pmsg.uType, pmsg.p1, pmsg.p2, payload, pmsg.payloadLength);
								if (managedLock)
									LeaveCriticalSection(&serverEvent);
								if (payload != NULL)
									delete[] payload;
							}
						for (int i = 0; i<bytesRecv - pmsg.packetLength; i++)
							recvbuf[i] = recvbuf[i + pmsg.packetLength];
						bytesRecv -= pmsg.packetLength;
						break;

					}
				}
			}
			if (breakflag) break;
			if (managedLock)
				EnterCriticalSection(&serverEvent);
			//Deal with  Msg Here
			if (serverFunc != NULL)
				serverFunc((int)client, -1, 0, 0, NULL, 0); //客户离开
			if (managedLock)
				LeaveCriticalSection(&serverEvent);
		}
		closesocket(client);
		return 0;
	}
	unsigned int WINAPI listen_thread(LPVOID param)
	{
		int port = (int)param;
		sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sServer == (u_int)SOCKET_ERROR)
			return 1;
		printf("Socket initialized.\n");
		sockaddr_in myaddr;
		memset(&myaddr, 0, sizeof myaddr);
		myaddr.sin_family = AF_INET;
		myaddr.sin_port = htons((unsigned short)port);
		myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(sServer, (sockaddr*)&myaddr, sizeof(myaddr)) == SOCKET_ERROR)
		{
			closesocket(sServer);
			return 1;
		}
		printf("Binded.\n");
		if (listen(sServer, SOMAXCONN) == SOCKET_ERROR)
		{
			closesocket(sServer);
			return 1;
		}
		printf("Listening.\n");
		int note = 0;
		SOCKET sAccept;
		while ((sAccept = accept(sServer, NULL, NULL)))
		{
			printf("Client connected\n");
			note++;
			if (note>100)
			{
				printf("too many new connections \n.");
				TerminateProcess((HANDLE)-1, -1);
			}
			HANDLE hThread = CreateThread(NULL, 0, &answer_thread, (void*)sAccept, 0, NULL);
			CloseHandle(hThread);
		}
		printf("Done\n");
		return 0;
	}
	unsigned int WINAPI client_thread(LPVOID param)
	{
		int bytesRecv, bytesRecvCur;
		unsigned char recvbuf[2048];
		bool breakflag = false;
		while (1)
		{
			memset(recvbuf, 0, sizeof(recvbuf));
			bytesRecv = 0;
			while (1)
			{
				EnterCriticalSection(&clientFinish);
				if (clientFinished)
				{
					breakflag = true;
					break;
				}
				LeaveCriticalSection(&clientFinish);
				//Receiving Data
				if (bytesRecv >= 2048)
				{
					printf("Data flooded\n");
					breakflag = true;
					break;
				}
				bytesRecvCur = recv(sClient, (char*)(recvbuf + bytesRecv), 2048 - bytesRecv, 0);
				if (bytesRecvCur == 0)
				{
					breakflag = true;
					break;
				}
				bytesRecv += bytesRecvCur;
				if (bytesRecv == SOCKET_ERROR)
				{
					int errcode = WSAGetLastError();
					if (errcode != WSAEMSGSIZE)
					{
						printf("ERR: %d\n", errcode);
						breakflag = true;
						break;
					}
				}
				Packet pmsg;
				ControlPacket pctlmsg;
				switch (recvbuf[0])
				{
				case 0:
					for (int i = 0; i<bytesRecv - 1; i++)
						recvbuf[i] = recvbuf[i + 1];
					bytesRecv--;
					break;
				case 0xFF:
					for (int i = 0; i<bytesRecv - 1; i++)
						recvbuf[i] = recvbuf[i + 1];
					bytesRecv--;
					if ((pctlmsg.packetLength = recvbuf[0]) >= 10)
					{
						pctlmsg.crc = *((unsigned int*)(recvbuf + 1));
						pctlmsg.type = *((unsigned char*)(recvbuf + 5));
						pctlmsg.param1 = *((unsigned int*)(recvbuf + 6));
						printf("CTL MSG: %d,%d,%d,%d\n", (int)pctlmsg.packetLength, pctlmsg.crc, (int)pctlmsg.type, pctlmsg.param1);
						//Deal with ctl msg here

					}
					for (int i = 0; i<bytesRecv - pctlmsg.packetLength; i++)
						recvbuf[i] = recvbuf[i + pctlmsg.packetLength];
					bytesRecv -= pctlmsg.packetLength;
					break;
				default:
					if ((pmsg.packetLength = recvbuf[0]) >= 19)
					{
						pmsg.crc = *((unsigned int*)(recvbuf + 1));
						pmsg.id = *((unsigned int*)(recvbuf + 5));
						pmsg.uType = *((unsigned char*)(recvbuf + 9));
						pmsg.p1 = *((unsigned int *)(recvbuf + 10));
						pmsg.p2 = *((unsigned int *)(recvbuf + 14));
						if (pmsg.id != lastServerId + 1 && responsive)
						{
							client_send_ctl(0xFD, (pmsg.id) << 16 | (lastServerId + 1)); //packet losses
							continue;
						}
						else
							lastServerId = pmsg.id;
						pmsg.payloadLength = *((unsigned char *)(recvbuf + 18));
						unsigned char* payload = NULL;
						if (pmsg.packetLength > 19 && pmsg.payloadLength + 19 == pmsg.packetLength)
						{
							payload = new unsigned char[pmsg.payloadLength];
							memcpy(payload, recvbuf + 19, pmsg.packetLength);
						}
						printf("MSG: %d,%d,%d,%d,%d,%d,%d\n", (int)pmsg.packetLength, pmsg.crc, pmsg.id, (int)pmsg.uType, pmsg.p1, pmsg.p2, (int)pmsg.payloadLength);
						if (managedLock)
							EnterCriticalSection(&clientEvent);
						//Deal with  Msg Here
						if (clientFunc != NULL)
							clientFunc(pmsg.uType, pmsg.p1, pmsg.p2, payload, pmsg.payloadLength);
						if (managedLock)
							LeaveCriticalSection(&clientEvent);
						if (payload != NULL)
							delete[] payload;

					}
					for (int i = 0; i<bytesRecv - pmsg.packetLength; i++)
						recvbuf[i] = recvbuf[i + pmsg.packetLength];
					bytesRecv -= pmsg.packetLength;
					break;

				}
			}
			if (breakflag) break;
		}
		//closesocket(sClient);
		return 0;
	}

	bool server_start(int port)
	{
		cid = 1;
		clist.~CQueue();
		clist.init(256, 1);
		serverFinished = false;
		HANDLE hThread = CreateThread(NULL, 0, &listen_thread, (void*)port, 0, NULL);
		if (!hThread)
			return false;
		CloseHandle(hThread);
		return true;
	}

	bool client_start(const char* si, int port)
	{
		lastServerId = 0;
		cid = 1;
		clist.~CQueue();
		clist.init(256, 1);
		clientFinished = false;
		strcpy(server_ip, si);
		sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sClient == (u_int)SOCKET_ERROR)
			return false;
		sockaddr_in myaddr;
		memset(&myaddr, 0, sizeof myaddr);
		myaddr.sin_family = AF_INET;
		myaddr.sin_port = htons((unsigned short)port);
		myaddr.sin_addr.s_addr = inet_addr(server_ip);
		if ((sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == (u_int)SOCKET_ERROR) {
			return false;
		}
		if (connect(sClient, (sockaddr*)&myaddr, sizeof(sockaddr)) == SOCKET_ERROR) {
			return false;
		}
		int nZero = 0;
		setsockopt(sClient, SOL_SOCKET, SO_SNDBUF, (const char*)&nZero, sizeof(int));
		printf("Client initialized.\n");
		HANDLE hThread = CreateThread(NULL, 0, &client_thread, (void*)port, 0, NULL);
		if (!hThread)
			return false;
		CloseHandle(hThread);
		return true;
	}

	bool client_send(unsigned char uType, int p1, int p2, void* pl, int lpl)
	{
		if (lpl>230)
			return false;
		char* senddata = new char[19 + lpl];
		bool result;
		senddata[0] = (char)(19 + lpl);
		*(unsigned int*)(senddata + 1) = 0; //crc
		*(unsigned int*)(senddata + 5) = 0; //id
		*(unsigned char*)(senddata + 9) = uType;
		*(unsigned int*)(senddata + 10) = p1;
		*(unsigned int*)(senddata + 14) = p2;
		*(unsigned char*)(senddata + 18) = lpl;
		if (lpl != 0) memcpy(senddata + 19, pl, lpl);
		result = send(sClient, senddata, 19 + lpl, 0) == lpl + 19;
		delete[] senddata;
		return result;
	}

	bool client_send(unsigned char uType, int p1, int p2)
	{
		return client_send(uType, p1, p2, NULL, 0);
	}

	bool client_send_ctl(unsigned char type, int p1)
	{
		char* senddata = new char[11];
		bool result;
		senddata[0] = 0xFF;
		senddata[1] = 10;
		*(unsigned int*)(senddata + 2) = 0; //crc
		*(unsigned char*)(senddata + 6) = type;
		*(unsigned int*)(senddata + 7) = p1;
		result = send(sClient, senddata, 11, 0) == 11;
		delete[] senddata;
		return result;
	}

	bool server_send(SOCKET client, unsigned char uType, int p1, int p2, void* pl, int lpl)
	{
		if (lpl>230)
			return false;
		char* senddata = new char[19 + lpl];
		bool result;
		senddata[0] = (char)(19 + lpl);
		*(unsigned int*)(senddata + 1) = 0; //crc
		*(unsigned int*)(senddata + 5) = cid++; //id
		*(unsigned char*)(senddata + 9) = uType;
		*(unsigned int*)(senddata + 10) = p1;
		*(unsigned int*)(senddata + 14) = p2;
		*(unsigned char*)(senddata + 18) = lpl;
		if (responsive)
		{
			Packet p;
			p.id = cid;
			p.p1 = p1;
			p.p2 = p2;
			p.uType = uType;
			clist.push_back(p);
		}
		if (lpl != 0) memcpy(senddata + 19, pl, lpl);
		result = send(client, senddata, 19 + lpl, 0) == lpl + 19;
		delete[] senddata;
		return result;
	}

	bool server_send(SOCKET client, unsigned char uType, int p1, int p2)
	{
		return server_send(client, uType, p1, p2, NULL, 0);
	}

	bool server_send_ctl(SOCKET client, unsigned char type, int p1)
	{
		char* senddata = new char[11];
		bool result;
		senddata[0] = 0xFF;
		senddata[1] = 10;
		*(unsigned int*)(senddata + 2) = 0; //crc
		*(unsigned char*)(senddata + 6) = type;
		*(unsigned int*)(senddata + 7) = p1;
		result = send(client, senddata, 11, 0) == 11;
		delete[] senddata;
		return result;
	}

	void finishClient()
	{
		client_send_ctl(3, 0);
		EnterCriticalSection(&clientFinish);
		clientFinished = true;
		LeaveCriticalSection(&clientFinish);
	}

	void finishServer()
	{
		EnterCriticalSection(&serverFinish);
		serverFinished = true;
		LeaveCriticalSection(&serverFinish);
	}

}
*/

	void slw::registerEvent(bool type, void* func, bool managed = TRUE)
	{
		/*
		managedLock = managed;
		if (type)
			serverFunc = (evtServer)func;
		else
			clientFunc = (evtClient)func;
		*/

	}

	bool slw::initialize()
	{
		return true;
	}
	bool slw::deinitialize()
	{
		return true;
	}
	bool slw::server_start(int port)
	{
		return false;
	}

	bool slw::client_start(const char* si, int port)
	{
		return false;
	}

	bool slw::client_send(unsigned char uType, int p1, int p2, void* pl, int lpl)
	{
		return false;
	}

	bool slw::client_send(unsigned char uType, int p1, int p2)
	{
		return client_send(uType, p1, p2, NULL, 0);
	}

	bool slw::client_send_ctl(unsigned char type, int p1)
	{
		return false;
	}

	bool slw::server_send(SOCKET client, unsigned char uType, int p1, int p2, void* pl, int lpl)
	{
		return false;
	}

	bool slw::server_send(SOCKET client, unsigned char uType, int p1, int p2)
	{
		return server_send(client, uType, p1, p2, NULL, 0);
	}

	bool slw::server_send_ctl(SOCKET client, unsigned char type, int p1)
	{
		return false;
	}

	void slw::finishClient()
	{
	}

	void slw::finishServer()
	{
	}


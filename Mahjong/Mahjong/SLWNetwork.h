#pragma once

// 网络中间层。
// 现在实现有若干问题，需要重写。

#include "stdafx.h"
class slw
{
public:
	bool client_send(unsigned char uType, int p1, int p2, void* pl, int lpl);
	bool client_send(unsigned char uType, int p1, int p2);
	bool client_send_ctl(unsigned char type, int p1);
	bool server_send(SOCKET client, unsigned char uType, int p1, int p2, void* pl, int lpl);
	bool server_send(SOCKET client, unsigned char uType, int p1, int p2);
	bool server_send_ctl(SOCKET client, unsigned char type, int p1);
	void registerEvent(bool type, void* func, bool managed);
	bool initialize();
	bool deinitialize();
	bool server_start(int port);
	bool client_start(const char* si, int port);
	void finishClient();
	void finishServer();

};

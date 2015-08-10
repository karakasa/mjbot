#pragma once

// 赛程控制，具体的使用方法请参考例程。

#include "PublicFunction.h"
#include "EventBus.h"
#include "YamaControl.h"
#include "SyanTen.h"
#include "TenpaiAkariJudge.h"

#define make_p1_start(x,y,z) (x)&((y)<<8)&((z)<<16)

class match
{
private:
	bool running = false;
	int changbang = 0, riichibang = 0;
	int syan_id = 1; //1= EAST 1
	int status = 0; //参文档
	int oya = 0, cpos = 0;
	bool toufuu = false;
	pai tepai[4][13], current;
	char paidata[13];
	int waitPending = 4;
	int tepaicnt[4] = { 13,13,13,13 };
	int fulucnt[4] = { 0,0,0,0 };
	mentsu fulu[4][4];
	int clientStatus[4] = { -1,-1,-1,-1 };
	unsigned int clientResponse[4] = { 0,0,0,0 };
	int clientSyanTen[4] = { 9,9,9,9 };
	char akarijyouhuun;
	bool norelease[4] = { false,false,false,false };
	int akariflags[4] = { 0,0,0,0 };
	const char akarijihuun[4] = { 'D','N','X','B' };
	bool furiten[4] = { false,false,false,false };
	int clientRiichi[4] = { 0,0,0,0 };
	bool clientIhatsu[4] = { false,false,false,false };
	bool waitForIncomingEvent = false;
	int whoIsWaiting = 0;
	int waitingType = 0;
	int waitingPai = 0;
	bool clientHaidei = false, clientHoudei = false, clientRinsyou = false, clientTyankan = false;
	pai paiout;
	int score[4] = { 0 };
	bool lianzhuang = false;
	bool juststart = true;

	int waitCnt = 0;



public:
	int roundPoint(int rp);
	void waitEvent();
	void specialTurn(unsigned int response);
	void scoreChange();
	void finishMatch();
	void minEventFinish();
	void minEvent(int clientId, unsigned int response);
	void nextTurn_state2(unsigned int response);
	void nextTurn_state1();
	void nextMatch();
	void startMatchMid();
	void startMatchMid2();
	void startMatch();
	void __stdcall receiveEvent(int clientId, unsigned int response);

	Yama yama;
	Syanten st;
	taj tajcore;
	eb evcore;
};
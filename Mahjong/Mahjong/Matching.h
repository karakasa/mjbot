#pragma once

// 赛程控制，具体的使用方法请参考例程。

#include "PublicFunction.h"
#include "EventBus.h"
#include "YamaControl.h"
#include "SyanTen.h"
#include "TenpaiAkariJudge.h"

#define make_p1_start(x,y,z) (x)&((y)<<8)&((z)<<16)

class match : public MatchingUser
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
	pai clientTenpai[4][13];
	int clientTenpaiCnt[4];
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
	pai paiKawa[4][32];
	int paiKawaCnt[4];

	int waitCnt = 0;

	// 将牌置入牌河，并计算振听状态等（本函数不负责荣和可能性的计算，主要是处理打牌者自己对打出的牌振听，和见逃振听的情况）
	void pushbackKawa(int pos, pai& paiout);

	// 设置听牌状态，在每次手牌发生变化（不包含摸的那张牌）后调用，计算当前的向听数，如果听牌的话，计算听牌
	void stTenpaiStatus(int pos);

	// 向上取整到整百
	int roundPoint(int rp);

	// 等待所有玩家响应；每有一位玩家响应，调用本过程；所有玩家响应后，将开始下一局
	void waitEvent();

	// 将最新的分数情况广播给客户端，并处理被飞的问题。应该在每一次分数发生变化后调用（包括立直等）
	void scoreChange();

	// 客户端返回了特殊回合的动作，特殊回合为类似于吃、碰的，不需要摸牌即打牌的过程。
	void specialTurn(unsigned int response);

	// 在全部局数结束或有人被飞后，结束游戏；要强行终止游戏，请使用 eb::abort() 函数。
	void finishMatch();

	// 收到无人阻止在某一鸣牌操作成立后（没有被和、抢杠等），执行的函数，会具体的执行鸣牌操作
	void minEventFinish();

	// 在向客户端发出“有鸣牌可能性”的消息，处理客户端回应的函数
	void minEvent(int clientId, unsigned int response);

	// 需要处理客户端回应（上一家打的牌）时的发牌过程
	void nextTurn_state2(unsigned int response);

	// 不需要处理客户端回应时的发牌过程（比如每局第一张牌，上家已经鸣完了）
	void nextTurn_state1();

	// 准备一局的游戏的必要资讯，并发第一张牌
	void nextMatch();

	// 在一局结束后调用，发出等待信息；等待四家都开始游戏
	void startMatchMid();

	// 在上面一个过程都返回后调用，主要是清除上一盘的数据；之后会自动调用 nextMatch
	void startMatchMid2();

	// 启动一个半庄（一个半庄）。
	void startMatch();

public:
	
	// 从 EventBus 接收到消息，具体消息类型请参考源代码
	void receiveEvent(int clientId, unsigned int response);

	match();
	~match();

	Yama yama;
	Syanten st;
	taj tajcore;
	eb evcore;
	eb* core = &evcore;
};
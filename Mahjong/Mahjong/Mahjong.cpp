// Mahjong.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PublicFunction.h"
#include "SyanTen.h"
#include "TenpaiAkariJudge.h"
#include "YamaControl.h"
#include "MemoryLeakMonitor.h"
#include "EventBus.h"
#include "Matching.h"

// Unit Test
#include "YPBasicToolBox.h"

using namespace std;

int main()
{
	Syanten engineSyanten;
	taj engineTaj;
	string paiStr;
	pai pais[20];
	int paiSafeLen = 16;

	bool simu = false;

	utility::prepareSystemYakuProvider();

	while (true)
	{
		paiSafeLen = 18;
		cin >> paiStr;
		if (paiStr == "exit")
			break;
		if (paiStr == "simu")
		{
			simu = true;
			break;
		}
		int paiLen = convertPaiStringPrepare(paiStr);
		if(paiLen <= 0 || paiLen > 14)
		{
			cerr << "非法的输入。" << endl;
		}
		else {
			switch (paiLen % 3)
			{
				case 1:
					sort(pais, pais + paiLen, paiSort);
					if(convertPaiString(paiStr, pais, &paiSafeLen))
					{
						int k = engineSyanten.calculateSyanten(pais, paiLen);
						if (k > 0)
						{
							cout << k << " 向听。" << endl;
						}
						else {
							cout << "听牌：";
							unordered_set<pai> jrs;
							jrs = std::move(engineTaj.tenpaiDetect(pais, paiLen));
							for (auto& p : jrs)
								cout << (int)(p.fig) << p.type << " ";
							cout << endl;
						}
					}
					break;
				case 2:
				{
					bool failed = false;
					judgeRequest jrq;
					judgeResult jrs;
					jrq.mode = 1;
					convertPaiString(paiStr, pais, &paiSafeLen);
					jrq.tgtpai = pais[paiLen - 1];
					jrq.fulucnt = 4 - (paiLen - 2) / 3;
					string fulud;
					int fuludCnt = 4;
					pai fulu[4];
					jrq.paicnt = paiLen - 1;
					copy(pais, pais + paiLen - 1, jrq.pais);
					for (int i = 0; i < jrq.fulucnt; i++)
					{
						cin >> fulud;
						fuludCnt = convertPaiStringPrepare(fulud);
						if (fuludCnt != 3 && fuludCnt != 4)
						{
							cout << "副露 " << i << " 不合法。" << endl;
							failed = true;
							break;
						}
						int fuludCntReserved = fuludCnt;
						convertPaiString(fulud, fulu, &fuludCnt);
						if (fuludCntReserved == 4)
						{
							cout << "暗杠与否？(y/n)";
							char p;
							cin >> p;
							jrq.fulus[i].type = (p == 'y') ? mentsu_KEZ_KANG_S : mentsu_KEZ_KANG_A;
							jrq.fulus[i].start = jrq.fulus[i].middle = jrq.fulus[i].last = fulu[0];
							cout << endl;
						}
						else {
							if (fulu[0] == fulu[1])
							{
								jrq.fulus[i].type = mentsu_KEZ_PON_A;
								jrq.fulus[i].start = jrq.fulus[i].middle = jrq.fulus[i].last = fulu[0];
							}
							else {
								jrq.fulus[i].type = mentsu_SHUNZ_CHI_A;
								jrq.fulus[i].start = fulu[0];
								jrq.fulus[i].middle = fulu[1];
								jrq.fulus[i].last = fulu[2];
							}
						}
					}
					if (failed && jrq.fulucnt != 0)
					{
						break;
					}
					cout << "荣和与否？(y/n)";
					char p;
					cin >> p;
					cout << endl;
					jrq.akariStatus = (p == 'y') ? RON : TSUMO;
					yakuTable yt = engineTaj.yakuDetect(jrq);
					cout << yt.yakutotal << " 翻 " << yt.huutotal << " 符 " << yt.basicpt << " 基本点" << endl;
					char w[255];
					memset(w, 0, sizeof(w));
					for (auto& yk : yt.yakus)
					{
						engineTaj.currentProvider->queryName(yk.yakuid, yk.yakusubid, w, 250);
						cout << w << " " << yk.pt << " 翻" << endl;
					}
					break;
				}
				default:
					cerr << "非法的输入。" << endl;
					break;
			}
		}
	}

	if (simu)
	{
		int p = GetTickCount();
		SimpleAI sai[4];
		match* testRun = new match();
		for (int i = 0; i < 4; i++)
			testRun->core->assignClient(i, &(sai[i]));
		testRun->core->run();
		testRun->core->receiveEvent(NULL, 1);
		testRun->core->waitUntilEnd();
		delete testRun;
		p = GetTickCount() - p;
		cout << endl << p << endl;
		cin >> paiSafeLen;
	}

    return MemoryLeakMonitor::gc();
}


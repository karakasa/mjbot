#include "stdafx.h"

#include "PublicFunction.h"
#include "EventBus.h"
#include "YamaControl.h"
#include "SyanTen.h"
#include "TenpaiAkariJudge.h"
#include "Matching.h"

#define make_p1_start(x,y,z) (x)&((y)<<8)&((z)<<16)


void match::stTenpaiStatus(int pos)
{
	std::sort(tepai[pos] + 0, tepai[pos] + tepaicnt[pos], paiSort);
	clientSyanTen[pos] = st.calculateSyanten(tepai[pos], tepaicnt[pos]);
	if (clientSyanTen[pos] == 0)
	{
		judgeRequest jr;
		judgeResult jres;

		jr.mode = 0;
		jr.paicnt = tepaicnt[pos];
		std::copy(tepai[pos], tepai[pos] + tepaicnt[pos], jr.pais);

		tajcore.tenpaiDetect(&jr, &jres);

		clientTenpaiCnt[pos] = jres.cnt;
		std::copy(jres.t + 0, jres.t + jres.cnt, clientTenpai[pos] + 0);
	}
}

void match::pushbackKawa(int pos, pai& paiout)
{
	if (!clientRiichi[pos])
		furiten[pos] = false;
	paiKawa[pos][paiKawaCnt[pos]++] = paiout;
	if (clientSyanTen[pos] == 0)
	{
		bool loopFlag = false;

		for (int i = 0; i < clientTenpaiCnt[pos]; i++)
		{
			for (int j = 0; j < paiKawaCnt[pos] - 1; j++)
				if (compare_pai(paiKawa[pos][j], clientTenpai[cpos][i]))
				{
					loopFlag = true;
					break;
				}
			if (loopFlag)
				break;
		}

		if (loopFlag)
			furiten[pos] = true;
	}

	for (int i = 0; i < 4;i++)
		if (i != pos)
		{
			if(clientSyanTen[i] == 0)
			{
				bool fnd = false;
				for (int j = 0; j < clientTenpaiCnt[i]; j++)
					if (compare_pai(paiKawa[pos][j], clientTenpai[i][j]))
					{
						fnd = true;
						break;
					}
				if (fnd)
				{
					furiten[i] = true;
					continue;
				}
			}	
		}
}

	int match::roundPoint(int rp)
	{
		if (rp % 100 == 0)
			return rp;
		return (rp / 100 + 1) * 100;
	}

	void match::waitEvent()
	{
		waitPending--;
		if (waitPending == 0)
		{
			startMatchMid2();
		}

	}
	void match::specialTurn(unsigned int response)
	{
		bool flag = false;
		int paiIndex;
		for (int i = 0; i<tepaicnt[cpos]; i++)
			if ((unsigned int)retrieveID2(tepai[cpos][i]) == response)
			{
				paiIndex = i;
				paiout = tepai[cpos][i];
				flag = true;
				break;
			}
		if (flag)
		{
			for (int i = paiIndex + 1; i<tepaicnt[cpos]; i++)
				tepai[cpos][i - 1] = tepai[cpos][i];
			tepaicnt[cpos] --;
			stTenpaiStatus(cpos);
			for (int i = 0; i<4; i++)
				if (cpos != i)
					evcore.send(i, ai::othersTurnFinished, response, 1);
			pushbackKawa(cpos, paiout);
		}
		norelease[cpos] = false;
		if (true)
		{
			//判断可否被鸣
			//荣和、吃、碰、大明杠判定
			//因为minChance可能发生多线程调用，所以利用CriticalSection进行互斥，在可靠的网络条件下 可以采用等待所有minChance返回，也可以先到先得
			int nxtpos = (cpos + 1) % 4;
			int minRequest[4] = { 0,0,0,0 };
			if (!(waitingType == 2 && waitForIncomingEvent))
			{
				if (paiout.type == 'M' || paiout.type == 'S' || paiout.type == 'P')
				{
					bool fnd1 = false, fnd2 = false;
					if (paiout.fig != 1 && paiout.fig != 9)
					{
						int i;
						for (i = 0; i<tepaicnt[nxtpos]; i++)
							if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig - 1)
							{
								fnd1 = true; break;
							}
						if (fnd1)
							for (i = i + 1; i<tepaicnt[nxtpos]; i++)
								if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig + 1)
								{
									fnd2 = true; break;
								}
						if (fnd2)
							minRequest[nxtpos] |= 2;
					}
					if (paiout.fig != 8 && paiout.fig != 9 && !fnd2)
					{
						int i;
						for (i = 0; i<tepaicnt[nxtpos]; i++)
							if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig + 1)
							{
								fnd1 = true; break;
							}
						if (fnd1)
							for (i = i + 1; i<tepaicnt[nxtpos]; i++)
								if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig + 2)
								{
									fnd2 = true; break;
								}
						if (fnd2)
							minRequest[nxtpos] |= 2;
					}
					if (paiout.fig != 1 && paiout.fig != 2 && !fnd2)
					{
						int i;
						for (i = 0; i<tepaicnt[nxtpos]; i++)
							if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig - 2)
							{
								fnd1 = true; break;
							}
						if (fnd1)
							for (i = i + 1; i<tepaicnt[nxtpos]; i++)
								if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig - 1)
								{
									fnd2 = true; break;
								}
						if (fnd2)
							minRequest[nxtpos] |= 2;
					}
				}
				bool fnd3 = false;
				for (int i = 0; i<4; i++)
				{
					if (i != cpos)
					{
						for (int j = 0; j<tepaicnt[i] - 1; j++)
							if (tepai[i][j].type == paiout.type && tepai[i][j].fig == paiout.fig && tepai[i][j + 1].type == paiout.type && tepai[i][j + 1].fig == paiout.fig)
							{
								minRequest[i] |= 4;
								fnd3 = true;
								break;
							}
					}
					if (fnd3) break;
				}
				fnd3 = false;
				for (int i = 0; i<4; i++)
				{
					if (i != cpos)
					{
						for (int j = 0; j<tepaicnt[i] - 2; j++)
							if (tepai[i][j].type == paiout.type && tepai[i][j].fig == paiout.fig && tepai[i][j + 1].type == paiout.type && tepai[i][j + 1].fig == paiout.fig && tepai[i][j + 2].type == paiout.type && tepai[i][j + 2].fig == paiout.fig)
							{
								minRequest[i] |= 8;
								fnd3 = true;
								break;
							}
					}
					if (fnd3) break;
				}
			}
			for (int i = 0; i<4; i++)
				if (i != cpos && clientSyanTen[i] == 0 && !furiten[i])
				{
					judgeRequest jreq;
					judgeResult jres;
					jreq.mode = 1;
					jreq.paicnt = tepaicnt[i];
					for (int pc = 0; pc<tepaicnt[i]; pc++)
						jreq.pais[pc] = tepai[i][pc];
					jreq.tgtpai = paiout;
					jreq.fulucnt = fulucnt[i];
					for (int j = 0; j<jreq.fulucnt; j++)
						jreq.fulus[j] = fulu[i][j];
					jreq.akari_status = RON;
					jreq.doracnt = 0;
					jreq.norelease = norelease[i];
					clientHoudei = yama.get_remaining() == 0;
					clientHaidei = false;
					jreq.flags = clientRiichi[i] | (clientIhatsu[i] ? 4 : 0) | (clientHaidei ? 8 : 0) | (clientHoudei ? 16 : 0) | (clientRinsyou ? 32 : 0) | (clientTyankan ? 64 : 0);
					jreq.jyouhuun = akarijyouhuun;
					jreq.jihuun = akarijihuun[i];
					//其他过程还未处理norelease 场风 flags

					pai doras;
					for (int p = 0; p < yama.kancount + 1; p++)
					{
						doraNext(yama.yama[130 - 2 * p], &doras);
						for (int pp = 0; pp < jreq.paicnt; pp++)
							if (doras == jreq.pais[pp])
								jreq.doracnt++;
						for (int pw = 0; pw < jreq.fulucnt; pw++)
						{
							if (doras == jreq.fulus[pw].start) {
								if (isKangz(&(jreq.fulus[pw])))
								{
									jreq.doracnt += 4;
								}
								else
								{
									if (isKez(&(jreq.fulus[pw])))
										jreq.doracnt += 4;
								}
							}
							if (!isKangz(&(jreq.fulus[pw])) && !isKez(&(jreq.fulus[pw])))
								if (jreq.fulus[pw].start == doras || jreq.fulus[pw].middle == doras || jreq.fulus[pw].last == doras)
									jreq.doracnt++;
						}
					}
					if (clientRiichi[i])
						for (int p = 0; p < yama.kancount + 1; p++)
						{
							doraNext(yama.yama[129 - 2 * p], &doras);
							for (int pp = 0; pp < jreq.paicnt; pp++)
								if (doras == jreq.pais[pp])
									jreq.doracnt++;
						}

					tajcore.tenpai_detect(&jreq, &jres);
					if (jres.yakucnt != 0)
						minRequest[i] |= 1;
				}
			if (minRequest[0] || minRequest[1] || minRequest[2] || minRequest[3])
			{
				status = 5; //waiting for minChance response;
				for (int i = 0; i<4; i++)
				{
					clientResponse[i] = 0xFFFFFFFF;
					clientStatus[i] = -1;
					if (minRequest[i] != 0)
					{
						clientStatus[i] = 1;
						evcore.send(i, ai::minChance, minRequest[i], 0);
					}
				}
				return;
			}
		}
		clientTyankan = false, clientRinsyou = false;
		cpos = (cpos + 1) % 4;
		nextTurn_state1();
	}
	void match::scoreChange()
	{
		evcore.broadcast(ai::scoreChange, 0, score, sizeof(int) * 4);
		for (int i = 0; i<3; i++)
			if (score[i]<0)
			{
				running = false;
				evcore.broadcast(ai::gameFinish, -2);
				evcore.broadcast(ai::deinit);
				evcore.finish();
				return;
			}
	}
	void match::finishMatch()
	{
		ai::TENPAI tp;
		memset(&tp, 0, sizeof(tp));
		for (int i = 0; i<4; i++)
			if ((tp.p[i] = clientSyanTen[i] == 0) == 0)
				tp.tepaicnt[i] = tepaicnt[i];
		changbang++;
		lianzhuang = clientSyanTen[0] == 0;
		memcpy(tp.tepai0, tepai, sizeof(pai)*tepaicnt[0]);
		memcpy(tp.tepai1, tepai, sizeof(pai)*tepaicnt[1]);
		memcpy(tp.tepai2, tepai, sizeof(pai)*tepaicnt[2]);
		memcpy(tp.tepai3, tepai, sizeof(pai)*tepaicnt[3]);
		if (!(clientSyanTen[0] == 0 && clientSyanTen[1] == 0 && clientSyanTen[2] == 0 && clientSyanTen[3] == 0))
			if (!(clientSyanTen[0] != 0 && clientSyanTen[1] != 0 && clientSyanTen[2] != 0 && clientSyanTen[3] != 0))
			{
				int ten = 0;
				int ten_id[4] = { -1,-1,-1,-1 };
				for (int i = 0; i<4; i++)
					if (clientSyanTen[i] == 0)
						ten_id[ten++] = i;
				switch (ten)
				{
				case 1:
					score[0] -= 1000;
					score[1] -= 1000;
					score[2] -= 1000;
					score[3] -= 1000;
					score[ten_id[0]] += 4000;
				case 2:
					score[0] -= 1500;
					score[1] -= 1500;
					score[2] -= 1500;
					score[3] -= 1500;
					score[ten_id[0]] += 3000;
					score[ten_id[1]] += 3000;
					break;
				case 3:
					score[0] -= 3000;
					score[1] -= 3000;
					score[2] -= 3000;
					score[3] -= 3000;
					score[ten_id[0]] += 4000;
					score[ten_id[1]] += 4000;
					score[ten_id[2]] += 4000;
					break;
				case 4:
					break;
				}
				scoreChange();
			}
		evcore.broadcast(ai::finishNaga, 4, &tp, sizeof(tp));
		status = 6;
		waitPending = 4;
		evcore.broadcast(ai::wait);
	}
	void match::minEventFinish()
	{
		status = 3;
		int ncpos = cpos;
		bool happened = false;
		if ((clientResponse[0] >> 28) == 0 || (clientResponse[1] >> 28) == 0 || (clientResponse[2] >> 28) == 0 || (clientResponse[3] >> 28) == 0)
		{
			int ronrenshu = 0, index = 0;
			for (int i = 0; i<4; i++)
				if (clientResponse[i] >> 28 == 0)
					ronrenshu++;
			ai::AKARI* akariresult;
			akariresult = new ai::AKARI[ronrenshu];
			if (clientResponse[0] >> 28 == 0)
			{
				lianzhuang = true;
				changbang++;
			}
			else {
				lianzhuang = false;
			}
			bool uraRevealed = false;
			for (int i = 0; i<4; i++)
			{
				if (clientResponse[i] >> 28 == 0)
				{
					judgeRequest jreq;
					judgeResult jres;
					jreq.mode = 1;
					jreq.paicnt = tepaicnt[i];
					for (int pc = 0; pc<tepaicnt[i]; pc++)
						jreq.pais[pc] = tepai[i][pc];
					jreq.tgtpai = paiout;
					jreq.fulucnt = fulucnt[i];
					for (int j = 0; j<jreq.fulucnt; j++)
						jreq.fulus[j] = fulu[i][j];
					jreq.akari_status = RON;
					jreq.doracnt = 0;

					pai doras;
					for (int p = 0; p < yama.kancount + 1; p++)
					{
						doraNext(yama.yama[130 - 2 * p], &doras);
						for (int pp = 0; pp < jreq.paicnt; pp++)
							if (doras == jreq.pais[pp])
								jreq.doracnt++;
						for (int pw = 0; pw < jreq.fulucnt; pw++)
						{
							if(doras == jreq.fulus[pw].start){
								if (isKangz(&(jreq.fulus[pw])))
								{
									jreq.doracnt += 4;
								}
								else
								{
									if (isKez(&(jreq.fulus[pw])))
										jreq.doracnt += 4;
								}
							}
							if (!isKangz(&(jreq.fulus[pw])) && !isKez(&(jreq.fulus[pw])))
								if (jreq.fulus[pw].start == doras || jreq.fulus[pw].middle == doras || jreq.fulus[pw].last == doras)
									jreq.doracnt++;
						}
					}
					if (clientRiichi[i])
						for (int p = 0; p < yama.kancount + 1; p++)
						{
							doraNext(yama.yama[129 - 2 * p], &doras);
							for (int pp = 0; pp < jreq.paicnt; pp++)
								if (doras == jreq.pais[pp])
									jreq.doracnt++;
						}


					jreq.norelease = norelease[i];
					clientHoudei = yama.get_remaining() == 0;
					clientHaidei = false;
					jreq.flags = clientRiichi[i] | (clientIhatsu[i] ? 4 : 0) | (clientHaidei ? 8 : 0) | (clientHoudei ? 16 : 0) | (clientRinsyou ? 32 : 0) | (clientTyankan ? 64 : 0);
					jreq.jyouhuun = akarijyouhuun;
					jreq.jihuun = akarijihuun[i];
					//其他过程还未处理norelease 场风 flags
					
					tajcore.tenpai_detect(&jreq, &jres);

					akariresult[index].from = cpos;
					akariresult[index].to = i;
					akariresult[index].pai = retrieveID2(paiout);
					akariresult[index].huu = jres.huutotal;
					akariresult[index].fan = jres.yakutotal;
					akariresult[index].pt = roundPoint(jres.basicpt * ((i == 0) ? 6 : 4));
					akariresult[index].reserved = 0;
					akariresult[index].yakucnt = jres.yakucnt;
					for (int j = 0; j<jres.yakucnt; j++)
					{
						akariresult[index].yaku_id[j] = (unsigned char)jres.yakuid[j];
						akariresult[index].yaku_fan[j] = (unsigned char)jres.pt[j];
					}
					if (index == 0)
					{
						score[i] += roundPoint(jres.basicpt * ((i == 0) ? 6 : 4)) + changbang * 300 + 1000 * riichibang; //棒子归亲开始的第一个人
						score[cpos] -= roundPoint((jres.basicpt * ((i == 0) ? 6 : 4)) + changbang * 300);
						riichibang = 0;
					}
					else {
						score[i] += roundPoint(jres.basicpt * ((i == 0) ? 6 : 4));
						score[cpos] -= roundPoint(jres.basicpt * ((i == 0) ? 6 : 4));
					}
					if (!lianzhuang)
						changbang = 0;
					index++;

					if (!uraRevealed && clientRiichi[cpos])
					{
						uraRevealed = true;
						unsigned char *uras;
						uras = new unsigned char[yama.kancount + 1];
						for (int p = 0; p<yama.kancount + 1; p++)
							uras[p] = retrieveID2(yama.yama[129 - 2 * p]);
						evcore.broadcast(ai::newUra, yama.kancount + 1, uras, sizeof(unsigned char)*(yama.kancount + 1));
						delete[] uras;
					}
					unsigned char* ppai;
					ppai = new unsigned char[tepaicnt[i]];
					for (int m = 0; m<tepaicnt[i]; m++)
						ppai[m] = retrieveID2(tepai[i][m]);
					evcore.broadcast(ai::othersTepai, tepaicnt[i] << 8 | i, ppai, sizeof(unsigned char)*(tepaicnt[i]));
					delete[] ppai;

				}
			}
			evcore.broadcast(ai::finish, 1, akariresult, sizeof(ai::AKARI));
			scoreChange();
			if (running)
				startMatchMid();
			delete[] akariresult;
			return;
		}
		else if ((clientResponse[0] >> 28) == 2 || (clientResponse[1] >> 28) == 2 || (clientResponse[2] >> 28) == 2 || (clientResponse[3] >> 28) == 2)
		{
			for (int i = 0; i<4; i++)
				if (clientResponse[i] >> 28 == 2)
				{
					//碰成立
					int j = -1, k = -1;
					for (j = 0; j<tepaicnt[i]; j++)
						if ((unsigned int)retrieveID2(tepai[i][j]) == (clientResponse[i] & 0xFF))
							break;
					if (j == -1)
						break;
					for (k = 0; k<tepaicnt[i]; k++)
						if (k != j)
							if ((unsigned int)retrieveID2(tepai[i][k]) == (clientResponse[i] & 0xFF00) >> 8)
								break;
					if (j>k)
					{
						int m = k;
						k = j;
						j = m;
					}
					fulu[i][fulucnt[i]].type = mentsu_KEZ_PON_A + getRelativePosition(i, cpos);
					fulu[i][fulucnt[i]].start = tepai[i][j];
					fulu[i][fulucnt[i]].middle = tepai[i][k];
					fulu[i][fulucnt[i]].last = paiout;
					for (int m = k + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;
					for (int m = j + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;
					for (int m = 0; m<4; m++)
						evcore.send(m, ai::minDone, i | (cpos << 2) | (ai::min::pon << 4) | (retrieveID2(paiout) << 8), retrieveID2(fulu[i][fulucnt[i]].start) | (retrieveID2(fulu[i][fulucnt[i]].middle) << 8) | (retrieveID2(fulu[i][fulucnt[i]].last) << 16));
					fulucnt[i]++;
					ncpos = i;
					for (int m = 0; m<4; m++)
						if (ncpos != m)
							evcore.send(m, ai::othersTurn, ncpos, 2);
					if (!clientRiichi[ncpos])
						furiten[ncpos] = false;

					stTenpaiStatus(cpos);

					status = 4;
					happened = true;
					cpos = ncpos;
					evcore.send(ncpos, ai::yourTurnSpecial);
					norelease[0] = false;
					norelease[1] = false;
					norelease[2] = false;
					norelease[3] = false;
					break;
				}
		}
		else if ((clientResponse[0] >> 28) == 3 || (clientResponse[1] >> 28) == 3 || (clientResponse[2] >> 28) == 3 || (clientResponse[3] >> 28) == 3)
		{
			for (int i = 0; i<4; i++)
				if (clientResponse[i] >> 28 == 2)
				{
					//碰成立
					int j = -1, k = -1, l = -1;
					for (j = 0; j<tepaicnt[i]; j++)
						if ((unsigned int)retrieveID2(tepai[i][j]) == (clientResponse[i] & 0xFF))
							break;
					if (j == -1)
						break;
					for (k = 0; k<tepaicnt[i]; k++)
						if (k != j)
							if ((unsigned int)retrieveID2(tepai[i][k]) == (clientResponse[i] & 0xFF00) >> 8)
								break;
					if (k == -1)
						break;
					for (l = 0; l<tepaicnt[i]; l++)
						if (l != j && l != k)
							if ((unsigned int)retrieveID2(tepai[i][l]) == (clientResponse[i] & 0xFF0000) >> 16)
								break;
					if (j>k)
					{
						int m = k;
						k = j;
						j = m;
					}
										
					fulu[i][fulucnt[i]].type = mentsu_KEZ_KANG_A + getRelativePosition(i, cpos);
					fulu[i][fulucnt[i]].start = tepai[i][j];
					fulu[i][fulucnt[i]].middle = tepai[i][k];
					fulu[i][fulucnt[i]].last = paiout;
					for (int m = l + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;
					for (int m = k + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;
					for (int m = j + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;

					stTenpaiStatus(cpos);

					for (int m = 0; m<4; m++)
						evcore.send(m, ai::minDone, i | (cpos << 2) | (ai::min::kang << 4) | (retrieveID2(paiout) << 8), retrieveID2(fulu[i][fulucnt[i]].start) | (retrieveID2(fulu[i][fulucnt[i]].middle) << 8) | (retrieveID2(fulu[i][fulucnt[i]].last) << 16) | (retrieveID2(fulu[i][fulucnt[i]].last) << 24));
					fulucnt[i]++;
					ncpos = i;
					for (int m = 0; m<4; m++)
						if (ncpos != m)
							evcore.send(m, ai::othersTurn, ncpos, 1);
					if (!clientRiichi[ncpos])
						furiten[ncpos] = false;
					status = 1;
					happened = true;
					yama.kang(&current);
					evcore.send(ncpos, ai::yourTurn, retrieveID2(current));
					norelease[0] = false;
					norelease[1] = false;
					norelease[2] = false;
					norelease[3] = false;
					break;
				}
		}
		else if ((clientResponse[0] >> 28) == 1 || (clientResponse[1] >> 28) == 1 || (clientResponse[2] >> 28) == 1 || (clientResponse[3] >> 28) == 1)
		{
			for (int i = 0; i<4; i++)
				if (clientResponse[i] >> 28 == 2)
				{
					//吃成立
					int j = -1, k = -1;
					for (j = 0; j<tepaicnt[i]; j++)
						if ((unsigned int)retrieveID2(tepai[i][j]) == (clientResponse[i] & 0xFF))
							break;
					if (j == -1)
						break;
					for (k = 0; k<tepaicnt[i]; k++)
						if (k != j)
							if ((unsigned int)retrieveID2(tepai[i][k]) == (clientResponse[i] & 0xFF00) >> 8)
								break;
					if (j>k)
					{
						int m = k;
						k = j;
						j = m;
					}
					fulu[i][fulucnt[i]].type = mentsu_SHUNZ_CHI_C;
					if (paiout.fig<tepai[i][j].fig && tepai[i][j].fig < tepai[i][k].fig)
					{
						fulu[i][fulucnt[i]].start = paiout;
						fulu[i][fulucnt[i]].middle = tepai[i][j];
						fulu[i][fulucnt[i]].last = tepai[i][k];
					}
					else if (tepai[i][j].fig<paiout.fig && paiout.fig < tepai[i][k].fig)
					{
						fulu[i][fulucnt[i]].start = tepai[i][j];
						fulu[i][fulucnt[i]].middle = paiout;
						fulu[i][fulucnt[i]].last = tepai[i][k];
					}
					else {
						fulu[i][fulucnt[i]].start = tepai[i][j];
						fulu[i][fulucnt[i]].middle = tepai[i][k];
						fulu[i][fulucnt[i]].last = paiout;
					}
					for (int m = k + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;
					for (int m = j + 1; m<tepaicnt[i]; m++)
						tepai[i][m - 1] = tepai[i][m];
					tepaicnt[i]--;

					stTenpaiStatus(cpos);

					for (int m = 0; m<4; m++)
						evcore.send(m, ai::minDone, i | (cpos << 2) | (ai::min::pon << 4) | (retrieveID2(paiout) << 8), retrieveID2(fulu[i][fulucnt[i]].start) | (retrieveID2(fulu[i][fulucnt[i]].middle) << 8) | (retrieveID2(fulu[i][fulucnt[i]].last) << 16));
					fulucnt[i]++;
					ncpos = i;
					for (int m = 0; m<4; m++)
						if (ncpos != m)
							evcore.send(m, ai::othersTurn, ncpos, 2);
					if (!clientRiichi[ncpos])
						furiten[ncpos] = false;
					status = 4;
					happened = true;
					cpos = ncpos;
					evcore.send(ncpos, ai::yourTurnSpecial);
					norelease[0] = false;
					norelease[1] = false;
					norelease[2] = false;
					norelease[3] = false;
					break;
				}
		}
		if (waitForIncomingEvent)
		{
			waitForIncomingEvent = false;
			switch (waitingType)
			{
			case 1: //立直
				if (happened)
					clientIhatsu[cpos] = false;
				for (int j = 0; j<4; j++)
					if (j != cpos)
						evcore.send(j, ai::minDone, (ai::min::riichi << 4) | cpos, 0);
				clientRiichi[cpos] = norelease[cpos] ? 2 : 1;
				score[cpos] -= 1000;
				scoreChange();
				riichibang++;
				break;
			case 2: //加杠
				for (int i = 0; i<fulucnt[cpos]; i++)
				{
					if (isKez(&(fulu[cpos][i])) && retrieveID3(fulu[cpos][i].start) == waitingPai)
					{
						fulu[cpos][i].type += 3;
						for (int j = 0; j<4; j++)
							if (j != cpos)
							{
								evcore.send(j, ai::minDone, (ai::min::kangj << 4) | cpos | (waitingPai << 8), waitingPai | (waitingPai << 8) | (waitingPai << 16) | (waitingPai << 24));
								evcore.send(j, ai::othersTurn, cpos, 1);
							}
						if (!clientRiichi[cpos])
							furiten[cpos] = false;
						yama.kang(&current);
						status = 1;
						evcore.send(cpos, ai::yourTurn, retrieveID2(current));
						return;
						break;
					}
				}
				break;
			}
		}
		if (happened)
			cpos = ncpos;
		else
		{
			clientTyankan = false, clientRinsyou = false;
			cpos = (cpos + 1) % 4;
			nextTurn_state1();
		}
	}
	void match::minEvent(int clientId, unsigned int response)
	{
		if (clientStatus[clientId] == 1)
		{
			clientResponse[clientId] = response;
			clientStatus[clientId] = 0;
			bool flag = false;
			for (int i = 0; i<4; i++)
				if (clientStatus[i] == 1)
				{
					flag = true;
					break;
				}
			if (!flag)
				minEventFinish();
		}
	}
	void match::nextTurn_state2(unsigned int response)
	{
		bool flag = false;
		status = 2;
		waitForIncomingEvent = false;
		bool minJudge = true;
		paiout.type = 'L';
		switch (response)
		{
		case 0xFFFFFFFF://模切
			paiout = current;
			stTenpaiStatus(cpos);
			for (int i = 0; i<4; i++)
				if (cpos != i)
					evcore.send(i, ai::othersTurnFinished, retrieveID2(current), 0);
			pushbackKawa(cpos, paiout);
			break;
		case 0xFE000000://自摸
			if (clientSyanTen[cpos] == 0)
			{
				if (cpos == 0)
				{
					lianzhuang = true;
					changbang++;
				}
				else
					lianzhuang = false;
				judgeRequest jreq;
				judgeResult jres;
				jreq.mode = 1;
				jreq.paicnt = tepaicnt[cpos];
				for (int pc = 0; pc<tepaicnt[cpos]; pc++)
					jreq.pais[pc] = tepai[cpos][pc];
				jreq.tgtpai = paiout;
				jreq.fulucnt = fulucnt[cpos];
				for (int j = 0; j<jreq.fulucnt; j++)
					jreq.fulus[j] = fulu[cpos][j];
				jreq.akari_status = TSUMO;

				//dora issue

				jreq.doracnt = 0;
				pai doras;
				for (int p = 0; p < yama.kancount + 1; p++)
				{
					doraNext(yama.yama[130 - 2 * p], &doras);
					for (int pp = 0; pp < jreq.paicnt; pp++)
						if (doras == jreq.pais[pp])
							jreq.doracnt++;
					for (int pw = 0; pw < jreq.fulucnt; pw++)
					{
						if (doras == jreq.fulus[pw].start) {
							if (isKangz(&(jreq.fulus[pw])))
							{
								jreq.doracnt += 4;
							}
							else
							{
								if (isKez(&(jreq.fulus[pw])))
									jreq.doracnt += 4;
							}
						}
						if (!isKangz(&(jreq.fulus[pw])) && !isKez(&(jreq.fulus[pw])))
							if (jreq.fulus[pw].start == doras || jreq.fulus[pw].middle == doras || jreq.fulus[pw].last == doras)
								jreq.doracnt++;
					}
				}
				if (clientRiichi[cpos])
					for (int p = 0; p < yama.kancount + 1; p++)
					{
						doraNext(yama.yama[129 - 2 * p], &doras);
						for (int pp = 0; pp < jreq.paicnt; pp++)
							if (doras == jreq.pais[pp])
								jreq.doracnt++;
					}
				
				jreq.norelease = norelease[cpos];
				clientHoudei = false;
				clientHaidei = yama.get_remaining() == 0;
				jreq.flags = clientRiichi[cpos] | (clientIhatsu[cpos] ? 4 : 0) | (clientHaidei ? 8 : 0) | (clientHoudei ? 16 : 0) | (clientRinsyou ? 32 : 0) | (clientTyankan ? 64 : 0);
				jreq.jyouhuun = akarijyouhuun;
				jreq.jihuun = akarijihuun[cpos];
				//其他过程还未处理norelease 场风 flags
				tajcore.tenpai_detect(&jreq, &jres);
				if (jres.yakucnt != 0)
				{
					ai::AKARI akariresult[1];
					akariresult[0].from = cpos;
					akariresult[0].to = cpos;
					akariresult[0].pai = retrieveID2(current);
					akariresult[0].huu = jres.huutotal;
					akariresult[0].fan = jres.yakutotal;
					akariresult[0].pt = jres.basicpt;
					akariresult[0].reserved = 0;
					akariresult[0].yakucnt = jres.yakucnt;
					for (int i = 0; i<jres.yakucnt; i++)
					{
						akariresult[0].yaku_id[i] = (unsigned char)jres.yakuid[i];
						akariresult[0].yaku_fan[i] = (unsigned char)jres.pt[i];
					}
					unsigned char* ppai;
					ppai = new unsigned char[tepaicnt[cpos]];
					for (int m = 0; m<tepaicnt[cpos]; m++)
						ppai[m] = retrieveID2(tepai[cpos][m]);
					if (clientRiichi[cpos])
					{
						unsigned char* uras;
						uras = new unsigned char[yama.kancount + 1];
						for (int p = 0; p<yama.kancount + 1; p++)
							uras[p] = retrieveID2(yama.yama[129 - 2 * p]);
						evcore.broadcast(ai::newUra, yama.kancount + 1, uras, sizeof(unsigned char)*(yama.kancount + 1));
						delete[] uras;
					}
					evcore.broadcast(ai::othersTepai, tepaicnt[cpos] << 8 | cpos, ppai, sizeof(unsigned char)*(tepaicnt[cpos]));
					evcore.broadcast(ai::finish, 1, akariresult, sizeof(ai::AKARI));
					if (cpos == 0)
					{
						score[cpos] += roundPoint(6 * jres.basicpt) + 300 * changbang + 1000 * riichibang;
						score[1] -= roundPoint(2 * jres.basicpt + changbang * 100);
						score[2] -= roundPoint(2 * jres.basicpt + changbang * 100);
						score[3] -= roundPoint(2 * jres.basicpt + changbang * 100);
						riichibang = 0;
					}
					else {
						score[cpos] += roundPoint(4 * jres.basicpt) + 300 * changbang + 1000 * riichibang;
						score[0] -= roundPoint(2 * jres.basicpt + changbang * 100);
						for (int i = 1; i<4; i++)
							if (i != cpos)
								score[i] -= roundPoint(jres.basicpt + changbang * 100);
						riichibang = 0;
					}
					scoreChange();
					if (running)
						startMatchMid();
					delete[] ppai;
					return;
				}
			}

			break;
		case 0xFE010000://九种九牌
			break;
		case 0xFE020000://拔北
			break;
		default:
			if ((response & 0xFF000000) == 0xFD000000)
			{
				int paiid;
				switch ((response & 0xFFFF00) >> 8)
				{
				case 0://暗杠
					paiid = response & 0xFF;
					if (retrieveID3(current) == paiid)
					{
						for (int i = 0; i<tepaicnt[cpos] - 3; i++)
						{
							if (retrieveID3(tepai[cpos][i]) == paiid)
								if (retrieveID3(tepai[cpos][i]) == retrieveID3(tepai[cpos][i + 1]))
									if (retrieveID3(tepai[cpos][i + 1]) == retrieveID3(tepai[cpos][i + 2]))
									{
										fulu[cpos][fulucnt[cpos]].type = mentsu_KEZ_KANG_S;
										fulu[cpos][fulucnt[cpos]].start = tepai[cpos][i];
										fulu[cpos][fulucnt[cpos]].middle = tepai[cpos][i + 1];
										fulu[cpos][fulucnt[cpos]].last = tepai[cpos][i + 2];
										for (int j = i + 3; j<tepaicnt[cpos]; j++)
											tepai[cpos][j - 3] = tepai[cpos][j];
										fulucnt[cpos]++;
										tepaicnt[cpos] -= 3;
										for (int j = 0; j<4; j++)
											if (j != cpos)
											{
												evcore.send(j, ai::minDone, (ai::min::kangs << 4) | cpos | (paiid << 8), paiid | (paiid << 8) | (paiid << 16) | (paiid << 24));
												evcore.send(j, ai::othersTurn, cpos, 1);
											}
										if (!clientRiichi[cpos])
											furiten[cpos] = false;
										yama.kang(&current);
										status = 1;
										clientRinsyou = true;
										memset(norelease, 0, sizeof(norelease));
										evcore.send(cpos, ai::yourTurn, retrieveID2(current));
										return;
									}
						}
					}
					else {
						for (int i = 0; i<tepaicnt[cpos] - 4; i++)
						{
							if (retrieveID3(tepai[cpos][i]) == paiid)
								if (retrieveID3(tepai[cpos][i]) == retrieveID3(tepai[cpos][i + 1]))
									if (retrieveID3(tepai[cpos][i + 1]) == retrieveID3(tepai[cpos][i + 2]))
										if (retrieveID3(tepai[cpos][i + 2]) == retrieveID3(tepai[cpos][i + 3]))
										{
											fulu[cpos][fulucnt[cpos]].type = mentsu_KEZ_KANG_S;
											fulu[cpos][fulucnt[cpos]].start = tepai[cpos][i];
											fulu[cpos][fulucnt[cpos]].middle = tepai[cpos][i + 1];
											fulu[cpos][fulucnt[cpos]].last = tepai[cpos][i + 2];
											for (int j = i + 4; j<tepaicnt[cpos]; j++)
												tepai[cpos][j - 4] = tepai[cpos][j];
											fulucnt[cpos]++;
											tepaicnt[cpos] -= 4;
											tepai[cpos][tepaicnt[cpos]] = current;
											tepaicnt[cpos]++;
											stTenpaiStatus(cpos);

											for (int j = 0; j<4; j++)
												if (j != cpos)
												{
													evcore.send(j, ai::minDone, (ai::min::kangs << 4) | cpos | (paiid << 8), paiid | (paiid << 8) | (paiid << 16) | (paiid << 24));
													evcore.send(j, ai::othersTurn, cpos, 1);
												}
											if (!clientRiichi[cpos])
												furiten[cpos] = false;
											yama.kang(&current);
											status = 1;
											clientRinsyou = true;
											evcore.send(cpos, ai::yourTurn, retrieveID2(current));
											return;
										}
						}
					}
					break;
				case 1://加杠
					paiid = response & 0xFF;
					waitForIncomingEvent = true;
					whoIsWaiting = cpos;
					waitingType = 2;
					waitingPai = paiid;
					if (retrieveID3(current) == paiid)
					{
						paiout = current;
						clientTyankan = true;
						for (int i = 0; i<4; i++)
							if (cpos != i)
								evcore.send(i, ai::othersTurnFinished, paiid, 4);
					}
					else {
						for (int i = 0; i<tepaicnt[cpos]; i++)
							if (retrieveID3(tepai[cpos][i]) == paiid)
							{
								paiout = tepai[cpos][i];
								tepai[cpos][i] = current;

								stTenpaiStatus(cpos);

								flag = true;
								clientTyankan = true;
								for (int i = 0; i<4; i++)
									if (cpos != i)
										evcore.send(i, ai::othersTurnFinished, paiid, 5);
								break;
							}
						if (!flag)
						{
							waitForIncomingEvent = false;
							waitingType = 0;
						}
					}
					break;
				case 2://手切立直
					paiid = response & 0xFF;
					waitForIncomingEvent = true;
					whoIsWaiting = cpos;
					waitingType = 1;
					for (int i = 0; i<tepaicnt[cpos]; i++)
						if (retrieveID2(tepai[cpos][i]) == paiid)
						{
							paiout = tepai[cpos][i];
							tepai[cpos][i] = current;
							flag = true;
							break;
						}
					if (flag)
					{
						stTenpaiStatus(cpos);

						for (int i = 0; i<4; i++)
							if (cpos != i)
								evcore.send(i, ai::othersTurnFinished, paiid, 3);
						pushbackKawa(cpos, paiout);
					}
					break;
				case 3://模切立直
					paiid = response & 0xFF;
					waitForIncomingEvent = true;
					whoIsWaiting = cpos;
					waitingType = 1;
					paiout = current;
					for (int i = 0; i<4; i++)
						if (cpos != i)
							evcore.send(i, ai::othersTurnFinished, paiid, 2);
					pushbackKawa(cpos, paiout);
					break;
				}
			}
			else {
				//正常打牌
				for (int i = 0; i<tepaicnt[cpos]; i++)
					if (retrieveID2(tepai[cpos][i]) == (unsigned char)response)
					{
						paiout = tepai[cpos][i];
						tepai[cpos][i] = current;
						flag = true;
						break;
					}
				if (flag)
				{
					stTenpaiStatus(cpos);

					for (int i = 0; i<4; i++)
						if (cpos != i)
							evcore.send(i, ai::othersTurnFinished, response, 1);
					pushbackKawa(cpos, paiout);
				}
			}
		}
		if (paiout.type == 'L') //failsafe
		{
			paiout = current;
			for (int i = 0; i<4; i++)
				if (cpos != i)
					evcore.send(i, ai::othersTurnFinished, retrieveID2(current), 0);
			pushbackKawa(cpos, paiout);
		}
		norelease[cpos] = 0;
		int minRequest[4] = { 0,0,0,0 };
		if (minJudge)
		{
			//判断可否被鸣
			//荣和、吃、碰、大明杠判定
			//因为minChance可能发生多线程调用，所以利用CriticalSection进行互斥，在可靠的网络条件下 可以采用等待所有minChance返回，也可以先到先得
			int nxtpos = (cpos + 1) % 4;
			if (!(waitForIncomingEvent && waitingType == 2))	
			{
				if (paiout.type == 'M' || paiout.type == 'S' || paiout.type == 'P')
				{
					bool fnd1 = false, fnd2 = false;
					if (paiout.fig != 1 && paiout.fig != 9)
					{
						int i;
						for (i = 0; i<tepaicnt[nxtpos]; i++)
							if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig - 1)
							{
								fnd1 = true; break;
							}
						if (fnd1)
							for (i = i + 1; i<tepaicnt[nxtpos]; i++)
								if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig + 1)
								{
									fnd2 = true; break;
								}
						if (fnd2)
							minRequest[nxtpos] |= 2;
					}
					if (paiout.fig != 8 && paiout.fig != 9 && !fnd2)
					{
						int i;
						for (i = 0; i<tepaicnt[nxtpos]; i++)
							if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig + 1)
							{
								fnd1 = true; break;
							}
						if (fnd1)
							for (i = i + 1; i<tepaicnt[nxtpos]; i++)
								if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig + 2)
								{
									fnd2 = true; break;
								}
						if (fnd2)
							minRequest[nxtpos] |= 2;
					}
					if (paiout.fig != 1 && paiout.fig != 2 && !fnd2)
					{
						int i;
						for (i = 0; i<tepaicnt[nxtpos]; i++)
							if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig - 2)
							{
								fnd1 = true; break;
							}
						if (fnd1)
							for (i = i + 1; i<tepaicnt[nxtpos]; i++)
								if (tepai[nxtpos][i].type == paiout.type && tepai[nxtpos][i].fig == paiout.fig - 1)
								{
									fnd2 = true; break;
								}
						if (fnd2)
							minRequest[nxtpos] |= 2;
					}
				}
				bool fnd3 = false;
				for (int i = 0; i<4; i++)
				{
					if (i != cpos)
					{
						for (int j = 0; j<tepaicnt[i] - 1; j++)
							if (tepai[i][j].type == paiout.type && tepai[i][j].fig == paiout.fig && tepai[i][j + 1].type == paiout.type && tepai[i][j + 1].fig == paiout.fig)
							{
								minRequest[i] |= 4;
								fnd3 = true;
								break;
							}
					}
					if (fnd3) break;
				}
				fnd3 = false;
				for (int i = 0; i<4; i++)
				{
					if (i != cpos)
					{
						for (int j = 0; j<tepaicnt[i] - 2; j++)
							if (tepai[i][j].type == paiout.type && tepai[i][j].fig == paiout.fig && tepai[i][j + 1].type == paiout.type && tepai[i][j + 1].fig == paiout.fig && tepai[i][j + 2].type == paiout.type && tepai[i][j + 2].fig == paiout.fig)
							{
								minRequest[i] |= 8;
								fnd3 = true;
								break;
							}
					}
					if (fnd3) break;
				}
			}
			for (int i = 0; i<4; i++)
				if (i != cpos && clientSyanTen[i] == 0 && !furiten[i])
				{
					judgeRequest jreq;
					judgeResult jres;
					jreq.mode = 1;
					jreq.paicnt = tepaicnt[i];
					for (int pc = 0; pc<tepaicnt[i]; pc++)
						jreq.pais[pc] = tepai[i][pc];
					jreq.tgtpai = paiout;
					jreq.fulucnt = fulucnt[i];
					for (int j = 0; j<jreq.fulucnt; j++)
						jreq.fulus[j] = fulu[i][j];
					jreq.akari_status = RON;
					jreq.doracnt = 0;
					jreq.norelease = norelease[i];
					clientHoudei = yama.get_remaining() == 0;
					clientHaidei = false;
					jreq.flags = clientRiichi[i] | (clientIhatsu[i] ? 4 : 0) | (clientHaidei ? 8 : 0) | (clientHoudei ? 16 : 0) | (clientRinsyou ? 32 : 0) | (clientTyankan ? 64 : 0);
					jreq.jyouhuun = akarijyouhuun;
					jreq.jihuun = akarijihuun[i];
					//其他过程还未处理norelease 场风 flags
					tajcore.tenpai_detect(&jreq, &jres);
					if (jres.yakucnt != 0)
						minRequest[i] |= 1;
				}
			for (int pc = 0; pc<4; pc++)
				if (clientRiichi[pc] != 0)
					minRequest[pc] &= 1;
			if (minRequest[0] || minRequest[1] || minRequest[2] || minRequest[3])
			{
				status = 5; //waiting for minChance response
				for (int i = 0; i<4; i++)
				{
					clientResponse[i] = 0xFFFFFFFF;
					clientStatus[i] = -1;
					if (minRequest[i] != 0)
					{
						clientStatus[i] = 1;
						evcore.send(i, ai::minChance, minRequest[i], 0);
					}
				}
				return;
			}
		}
		if (waitForIncomingEvent && waitingType == 2 && !minRequest[0] && !minRequest[1] && !minRequest[2] && !minRequest[3])
		{
			//加杠实质上直接成立
			waitForIncomingEvent = false;
			waitingType = 0;
			for (int i = 0; i<fulucnt[cpos]; i++)
			{
				if (isKez(&(fulu[cpos][i])) && retrieveID3(fulu[cpos][i].start) == waitingPai)
				{
					fulu[cpos][i].type += 3;
					for (int j = 0; j<4; j++)
						if (j != cpos)
						{
							evcore.send(j, ai::minDone, (ai::min::kangj << 4) | cpos | (waitingPai << 8), waitingPai | (waitingPai << 8) | (waitingPai << 16) | (waitingPai << 24));
							evcore.send(j, ai::othersTurn, cpos, 1);
						}
					if (!clientRiichi[cpos])
						furiten[cpos] = false;
					yama.kang(&current);
					status = 1;
					evcore.send(cpos, ai::yourTurn, retrieveID2(current));
					return;
					break;
				}
			}
		}
		if (waitForIncomingEvent && waitingType == 1 && !minRequest[0] && !minRequest[1] && !minRequest[2] && !minRequest[3])
		{
			//立直实质上直接成立
			waitForIncomingEvent = false;
			waitingType = 0;
			for (int j = 0; j<4; j++)
				if (j != cpos)
					evcore.send(j, ai::minDone, (ai::min::riichi << 4) | cpos, 0);
			clientRiichi[cpos] = norelease[cpos] ? 2 : 1;
			score[cpos] -= 1000;
			riichibang++;
			scoreChange();
		}
		clientTyankan = false, clientRinsyou = false;
		cpos = (cpos + 1) % 4;
		nextTurn_state1();
	}
	void match::nextTurn_state1()
	{
		if (yama.get_remaining() == 14)
		{
			finishMatch();
			return;
		}
		for (int i = 0; i<4; i++)
			if (cpos != i)
				evcore.send(i, ai::othersTurn, cpos, 0);
		yama.next(&current);
		status = 1;
		if (!clientRiichi[cpos])
			furiten[cpos] = false;
		evcore.send(cpos, ai::yourTurn, retrieveID2(current));
	}
	void match::nextMatch()
	{
		evcore.send(0, ai::start, 0, make_p1_start(changbang, riichibang, syan_id));
		evcore.send(1, ai::start, 1, make_p1_start(changbang, riichibang, syan_id));
		evcore.send(2, ai::start, 2, make_p1_start(changbang, riichibang, syan_id));
		evcore.send(3, ai::start, 3, make_p1_start(changbang, riichibang, syan_id));
		yama.init();
		for (int i = 0; i<4; i++)
		{
			yama.next(&(tepai[i][0]));
			yama.next(&(tepai[i][1]));
			yama.next(&(tepai[i][2]));
			yama.next(&(tepai[i][3]));
		}
		for (int i = 0; i<4; i++)
		{
			yama.next(&(tepai[i][4]));
			yama.next(&(tepai[i][5]));
			yama.next(&(tepai[i][6]));
			yama.next(&(tepai[i][7]));
		}
		for (int i = 0; i<4; i++)
		{
			yama.next(&(tepai[i][8]));
			yama.next(&(tepai[i][9]));
			yama.next(&(tepai[i][10]));
			yama.next(&(tepai[i][11]));
		}
		for (int i = 0; i<4; i++)
		{
			yama.next(&(tepai[i][12]));

			stTenpaiStatus(i);

			for (int j = 0; j<13; j++)
				paidata[j] = retrieveID2(tepai[i][j]);
			evcore.send(i, ai::initalizeTehai, 13, paidata, sizeof(char) * 13);
		}
		evcore.broadcast(ai::newDora, 1, retrieveID2(yama.yama[130]));
		nextTurn_state1();
	}
	void match::startMatchMid()
	{
		status = 6;
		waitPending = 4;
		evcore.broadcast(ai::wait);
	}
	void match::startMatchMid2()
	{
		if (syan_id == 7)
		{
			status = 8;
			evcore.broadcast(ai::gameFinish, -1);
			evcore.broadcast(ai::deinit);
			printf("finish");
			yama.finalize();
			evcore.finish();
			return;
		}
		cpos = 0;
		if (juststart)
			juststart = false;
		else if (!lianzhuang)
		{
			evcore.roll();
			syan_id++;
		}
		akarijyouhuun = syan_id<4 ? 'D' : 'N';
		status = 0;
		tepaicnt[0] = 13;
		tepaicnt[1] = 13;
		tepaicnt[2] = 13;
		tepaicnt[3] = 13;

		fulucnt[0] = 0;
		fulucnt[1] = 0;
		fulucnt[2] = 0;
		fulucnt[3] = 0;
		clientStatus[0] = -1;
		clientStatus[1] = -1;
		clientStatus[2] = -1;
		clientStatus[3] = -1;
		clientRiichi[0] = 0;
		clientRiichi[1] = 0;
		clientRiichi[2] = 0;
		clientRiichi[3] = 0;
		furiten[0] = false;
		furiten[1] = false;
		furiten[2] = false;
		furiten[3] = false;
		norelease[0] = true;
		norelease[1] = true;
		norelease[2] = true;
		norelease[3] = true;
		clientSyanTen[0] = 9;
		clientSyanTen[1] = 9;
		clientSyanTen[2] = 9;
		clientSyanTen[3] = 9;
		clientIhatsu[0] = false;
		clientIhatsu[1] = false;
		clientIhatsu[2] = false;
		clientIhatsu[3] = false;
		paiKawaCnt[0] = 0;
		paiKawaCnt[1] = 0;
		paiKawaCnt[2] = 0;
		paiKawaCnt[3] = 0;
		clientTenpaiCnt[0] = 0;
		clientTenpaiCnt[1] = 0;
		clientTenpaiCnt[2] = 0;
		clientTenpaiCnt[3] = 0;
		nextMatch();
	}
	void match::startMatch()
	{
		cpos = 0;
		lianzhuang = false;
		changbang = 0; riichibang = 0;
		syan_id = 0;
		score[0] = 25000;
		score[1] = 25000;
		score[2] = 25000;
		score[3] = 25000;
		juststart = true;
		evcore.broadcast(ai::init);
		startMatchMid2();
	}
	void match::receiveEvent(int clientId, unsigned int response)
	{
		if (clientId == -1)
		{
			switch (response)
			{
			case 1: //启动一局游戏
				running = true;
				startMatch();
				break;
			}
			return;
		}
		if (!running)
			return;
		switch (status)
		{
		case 1:
			nextTurn_state2(response);
			break;
		case 4:
			specialTurn(response);
			break;
		case 5:
			minEvent(clientId, response);
			break;
		case 6:
			waitEvent();
			break;
		case 8:
			//游戏已经结束
			return;
		default:
			printf("UNDEFINED STATUS %d\n", status);
			break;
		}
	}

match::match()
{
	this->core->init();
	this->core->setMatching(this);
}

match::~match()
{
	this->core->deinit();
}
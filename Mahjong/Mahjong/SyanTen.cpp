#include "stdafx.h"

#include "PublicFunction.h"
#include "SyanTen.h"

#define MAX_SYANTEN 99
#define ERR_SYANTEN_INTERNAL -2
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

	int Syanten::calculateKokushiSyanten(const pai* tpai, const int paicnt)
	{
		if (paicnt != 13)
			return MAX_SYANTEN;
		int maicnt[14], daiji = false, typecnt = 0;
		memset(maicnt, 0, sizeof(maicnt));
		for (int i = 0; i<paicnt; i++)
			++maicnt[getYaotyuuId(tpai[i])];
		for (int i = 0; i<13; i++)
		{
			if (maicnt[i] != 0)
				typecnt++;
			if (maicnt[i] >= 2)
				daiji = true;
		}
		if (daiji)
			return 12 - typecnt;
		return 13 - typecnt;
	}
	int Syanten::calculateChiitoitsu(const pai* tpai, const int paicnt)
	{
		if (paicnt != 13)
			return MAX_SYANTEN;
		int daiji = 0, j, k, sange = 0, sige = 0;
		for (int i = 0; i<paicnt - 1; i++)
		{
			if (compare_pai(tpai[i], tpai[i + 1]))
			{
				daiji++;
				k = i;
				for (j = i++; i<paicnt - 1 && compare_pai(tpai[j], tpai[i]); i++);
				if (i - k == 3)
					sange++;
				if (i - k == 4)
					sige++;
				i--;
			}
		}
		int freepai = paicnt - 2 * (daiji - sange - sige) - 3 * sange - 4 * sige; // 自由牌（单张牌的数量）
		int freepaiNeed = sange + sige * 2;                                       // 为了消解龙七对，所需的自由牌的数量
		if (freepaiNeed > freepai)                                                // 这个方法可能是对的
			return 6 - daiji + freepaiNeed - freepai;
		return 6 - daiji;
	}
	int Syanten::normalCalculate3(pai* tpai, const int paicnt, int currentMenchi)
	{
		if (paicnt < 0)
			throw ERR_SYANTEN_INTERNAL;
		if (paicnt == 0)
			return 0;
		bool shunzAvail;
		int npaicnt = paicnt, remainingMenchi = currentMenchi, cSyanTen = 0;
		do
		{
			shunzAvail = false;
			for (int i = 0; i<npaicnt - 2; i++)
				for (int j = i + 1; j<npaicnt - 1; j++)
				{
					if (tpai[i].type != tpai[j].type)
						break;
					if (tpai[i].fig + 1 == tpai[j].fig)
					{
						for (int k = j + 1; k<npaicnt; k++)
						{
							if (tpai[i].type != tpai[k].type)
								break;
							if (tpai[j].fig + 1 == tpai[k].fig)
							{
								for (int q = i + 1; q<j; q++)
									tpai[q - 1] = tpai[q];
								for (int q = j + 1; q<k; q++)
									tpai[q - 2] = tpai[q];
								for (int q = k + 1; q<npaicnt; q++)
									tpai[q - 3] = tpai[q];
								npaicnt -= 3;
								shunzAvail = true;
								remainingMenchi--;
								if (remainingMenchi <= 0)
									return 0;
								goto outInWhile1;
								break;
							}
						}
						break;
					}
				}
		outInWhile1:;
		} while (shunzAvail);
		do
		{
			shunzAvail = false;
			for (int i = 0; i<npaicnt - 1; i++)
				for (int j = i + 1; j<npaicnt; j++)
				{
					if (tpai[i].type != tpai[j].type)
						break;
					if ((tpai[i].fig + 1 == tpai[j].fig) || (tpai[i].fig + 2 == tpai[j].fig))
					{
						for (int q = i + 1; q<j; q++)
							tpai[q - 1] = tpai[q];
						for (int q = j + 1; q<npaicnt; q++)
							tpai[q - 2] = tpai[q];
						npaicnt -= 2;
						shunzAvail = true;
						remainingMenchi--;
						cSyanTen++;
						if (remainingMenchi <= 0)
							return cSyanTen;
						goto outInWhile2;
						break;
					}
				}
		outInWhile2:;
		} while (shunzAvail);
		return cSyanTen + 2 * remainingMenchi;
	}
	int Syanten::normalCalculate2(const pai* tpai, const int paicnt, int currentMenchi)
	{
		//assert(false);
		if (paicnt < 0)
			throw ERR_SYANTEN_INTERNAL;
		if (currentMenchi == 0)
			return 0;
		int syanTen = MAX_SYANTEN, cSyanTen;
		int kez_pos[6] = { 0,0,0,0,0,0 }, kez_status[6] = { 0,0,0,0,0,0 }, kez_cnt = 0;
		bool kez_type[6] = { false,false,false,false,false,false };
		for (int i = 0; i<paicnt - 1; i++)
			if (compare_pai(tpai[i], tpai[i + 1]))
			{
				kez_pos[kez_cnt] = i;
				kez_type[kez_cnt] = (i<paicnt - 2 && compare_pai(tpai[i], tpai[i + 2]));
				kez_status[kez_cnt++] = 1;
				for (int j = i++; i<paicnt - 1 && compare_pai(tpai[j], tpai[i]); i++);
				i--;
			}
		for (int k1 = 0; k1 <= kez_status[0]; k1++)
			for (int k2 = 0; k2 <= kez_status[1]; k2++)
				for (int k3 = 0; k3 <= kez_status[2]; k3++)
					for (int k4 = 0; k4 <= kez_status[3]; k4++)
						for (int k5 = 0; k5 <= kez_status[4]; k5++)
							for (int k6 = 0; k6 <= kez_status[5]; k6++)
							{
								cSyanTen = 0;
								int cpos = 0, npaicnt = paicnt, remainingMenchi = currentMenchi, dupBeginPos = 0;

								if (k1)
								{
									for (int i = dupBeginPos; i<kez_pos[0]; i++)
										tpai2st1[cpos++] = tpai[i];
									if (kez_type[0])
									{
										npaicnt -= 3;
										dupBeginPos = kez_pos[0] + 3;
									}
									else
									{
										npaicnt -= 2;
										dupBeginPos = kez_pos[0] + 2;
										cSyanTen++;
									}
									remainingMenchi--;
								}
								if (remainingMenchi == 0)
								{
									syanTen = min(syanTen, cSyanTen);
									continue;
								}
								if (cSyanTen >= SyantenMaxRecur)
								{
									continue;
								}
								if (k2)
								{
									for (int i = dupBeginPos; i<kez_pos[1]; i++)
										tpai2st1[cpos++] = tpai[i];
									if (kez_type[1])
									{
										npaicnt -= 3;
										dupBeginPos = kez_pos[1] + 3;
									}
									else
									{
										npaicnt -= 2;
										dupBeginPos = kez_pos[1] + 2;
										cSyanTen++;
									}
									remainingMenchi--;
								}
								if (remainingMenchi == 0)
								{
									syanTen = min(syanTen, cSyanTen);
									continue;
								}
								if (cSyanTen >= SyantenMaxRecur)
								{
									continue;
								}
								if (k3)
								{
									for (int i = dupBeginPos; i<kez_pos[2]; i++)
										tpai2st1[cpos++] = tpai[i];
									if (kez_type[2])
									{
										npaicnt -= 3;
										dupBeginPos = kez_pos[2] + 3;
									}
									else
									{
										npaicnt -= 2;
										dupBeginPos = kez_pos[1] + 2;
										cSyanTen++;
									}
									remainingMenchi--;
								}
								if (remainingMenchi == 0)
								{
									syanTen = min(syanTen, cSyanTen);
									continue;
								}
								if (cSyanTen >= SyantenMaxRecur)
								{
									continue;
								}
								if (k4)
								{
									for (int i = dupBeginPos; i<kez_pos[3]; i++)
										tpai2st1[cpos++] = tpai[i];
									if (kez_type[3])
									{
										npaicnt -= 3;
										dupBeginPos = kez_pos[3] + 3;
									}
									else
									{
										npaicnt -= 2;
										dupBeginPos = kez_pos[3] + 2;
										cSyanTen++;
									}
									remainingMenchi--;
								}
								if (remainingMenchi == 0)
								{
									syanTen = min(syanTen, cSyanTen);
									continue;
								}
								if (cSyanTen >= SyantenMaxRecur)
								{
									continue;
								}
								if (k5)
								{
									for (int i = dupBeginPos; i<kez_pos[4]; i++)
										tpai2st1[cpos++] = tpai[i];
									if (kez_type[4])
									{
										npaicnt -= 3;
										dupBeginPos = kez_pos[4] + 3;
									}
									else
									{
										npaicnt -= 2;
										dupBeginPos = kez_pos[4] + 2;
										cSyanTen++;
									}
									remainingMenchi--;
								}
								if (remainingMenchi == 0)
								{
									syanTen = min(syanTen, cSyanTen);
									continue;
								}
								if (cSyanTen >= SyantenMaxRecur)
								{
									continue;
								}
								if (k6)
								{
									for (int i = dupBeginPos; i<kez_pos[5]; i++)
										tpai2st1[cpos++] = tpai[i];
									if (kez_type[5])
									{
										npaicnt -= 3;
										dupBeginPos = kez_pos[5] + 3;
									}
									else
									{
										npaicnt -= 2;
										dupBeginPos = kez_pos[5] + 2;
										cSyanTen++;
									}
									remainingMenchi--;
								}
								if (remainingMenchi == 0)
								{
									syanTen = min(syanTen, cSyanTen);
									continue;
								}
								if (cSyanTen >= SyantenMaxRecur)
								{
									continue;
								}
								for (int i = dupBeginPos; i<paicnt; i++)
									tpai2st1[cpos++] = tpai[i];
								cSyanTen += normalCalculate3(tpai2st1, npaicnt, remainingMenchi);
								syanTen = min(syanTen, cSyanTen);
								if (syanTen == 0) return 0;
							}
		return syanTen;
	}
	int Syanten::normalCalculate(const pai* tpai, const int paicnt)
	{
		if (paicnt < 0)
			throw ERR_SYANTEN_INTERNAL;
		int syanten = MAX_SYANTEN;
		int csyanten;
		for (int i = 0; i<paicnt; i++)
		{
			if (i + 1<paicnt && compare_pai(tpai[i], tpai[i + 1]))
			{
				int j;
				for (j = 0; j<i; j++)
					tpai2st2[j] = tpai[j];
				for (j = i + 2; j<paicnt; j++)
					tpai2st2[j - 2] = tpai[j];
				csyanten = normalCalculate2(tpai2st2, paicnt - 2, (paicnt - 1) / 3);
				syanten = min(syanten, csyanten);
				for (j = i++; i<paicnt&&compare_pai(tpai[j], tpai[i]); i++);
				i--;
			}
			else
			{
				int j;
				for (j = 0; j<i; j++)
					tpai2st2[j] = tpai[j];
				for (j = i + 1; j<paicnt; j++)
					tpai2st2[j - 1] = tpai[j];
				csyanten = 1 + normalCalculate2(tpai2st2, paicnt - 1, (paicnt - 1) / 3);
				syanten = min(syanten, csyanten);
			}
		}
		return syanten;
	}

	int Syanten::calculateSyanten(const pai* tpai, const int paicnt, bool normalonly) //参数本身已经是有序的了
	{
		int kokushi, chiitoi;
		if (!normalonly)
		{
			kokushi = calculateKokushiSyanten(tpai, paicnt);
			chiitoi = calculateChiitoitsu(tpai, paicnt);
			if (kokushi == 0 || chiitoi == 0)
				return 0;
			if (kokushi == 1 || chiitoi == 1) //不可能介于0和1向听之间
					return 1;
			int pshi = normalCalculate(tpai, paicnt) - 1;
			return min(min(kokushi, chiitoi), pshi);
			}
		return normalCalculate(tpai, paicnt) - 1;
	}

	bool Syanten::specialized_tenpai_detect(pai* pais, int paicnt, judgeResult* resultEx)
	{
			int ycnt[14];
			memset(ycnt, 0, sizeof(ycnt));
			for (int i = 0; i<13; i++)
				ycnt[getYaotyuuId((pais)[i])]++;
			if (ycnt[0] == 1 && ycnt[1] == 1 && ycnt[2] == 1 && ycnt[3] == 1 && ycnt[4] == 1 && ycnt[5] == 1 && ycnt[6] == 1 && ycnt[7] == 1 && ycnt[8] == 1 && ycnt[9] == 1 && ycnt[10] == 1 && ycnt[11] == 1 && ycnt[12] == 1)
			{
				resultEx->cnt = 13;
				retrievePai3(&((resultEx->t)[0]), 1);
				retrievePai3(&((resultEx->t)[1]), 9);
				retrievePai3(&((resultEx->t)[2]), 10);
				retrievePai3(&((resultEx->t)[3]), 18);
				retrievePai3(&((resultEx->t)[4]), 19);
				retrievePai3(&((resultEx->t)[5]), 27);
				retrievePai3(&((resultEx->t)[6]), 28);
				retrievePai3(&((resultEx->t)[7]), 29);
				retrievePai3(&((resultEx->t)[8]), 30);
				retrievePai3(&((resultEx->t)[9]), 31);
				retrievePai3(&((resultEx->t)[10]), 32);
				retrievePai3(&((resultEx->t)[11]), 33);
				retrievePai3(&((resultEx->t)[12]), 34);
				return true;
			}
			int y0 = 0, y1 = 0, y2 = 0, yid = 0;
			for (int i = 0; i<13; i++)
				switch (ycnt[i])
				{
				case 0:
					y0++;
					yid = i;
					break;
				case 1:
					y1++;
					break;
				case 2:
					y2++;
					break;
				default:
					return false;
				}
			if (y0 == 1 && y2 == 1 && y1 == 11)
			{
				resultEx->cnt = 1;
				retrievePai3(&((resultEx->t)[0]), yaotrans[yid]);
				return true;
			}
			return false;
	}

	bool Syanten::kouritsuDetect(pai* tpai, const int paicnt, judgeResult* jres)
	{
		if (paicnt == 13)
		{
			if (Syanten::specialized_tenpai_detect(tpai, 13, jres))
				return true;
		}
		jres->cnt = 0;
		int cst = Syanten::calculateSyanten(tpai, paicnt);
		pai* tpai2 = new pai[paicnt+1];
		pai nt;
		nt.trait = 0;
		nt.type = 'M';
		for (int i = 1; i <= 9; i++)
		{
			nt.fig = i;
			std::copy(tpai, tpai + paicnt, tpai2);
			tpai2[paicnt] = nt;
			std::sort(tpai2, tpai2 + paicnt + 1, paiSort);
			if (Syanten::calculateSyanten(tpai2, paicnt + 1) < cst)
				jres->t[(jres->cnt)++] = nt;
		}
		nt.type = 'S';
		for (int i = 1; i <= 9; i++)
		{
			nt.fig = i;
			std::copy(tpai, tpai + paicnt, tpai2);
			tpai2[paicnt] = nt;
			std::sort(tpai2, tpai2 + paicnt + 1, paiSort);
			if (Syanten::calculateSyanten(tpai2, paicnt + 1) < cst)
				jres->t[(jres->cnt)++] = nt;
		}
		nt.type = 'P';
		for (int i = 1; i <= 9; i++)
		{
			nt.fig = i;
			std::copy(tpai, tpai + paicnt, tpai2);
			tpai2[paicnt] = nt;
			std::sort(tpai2, tpai2 + paicnt + 1, paiSort);
			if (Syanten::calculateSyanten(tpai2, paicnt + 1) < cst)
				jres->t[(jres->cnt)++] = nt;
		}
		nt.fig = 1;
		for (int i = 0; i <= 6; i++)
		{
			nt.type = funpai[i];
			std::copy(tpai, tpai + paicnt, tpai2);
			tpai2[paicnt] = nt;
			std::sort(tpai2, tpai2 + paicnt + 1, paiSort);
			if (Syanten::calculateSyanten(tpai2, paicnt + 1) < cst)
				jres->t[(jres->cnt)++] = nt;
		}
		delete[] tpai2;
		return true;
	}

	bool Syanten::kouritsuDetect(judgeRequest* jreq, judgeResult* jres)
	{
		if (jreq->mode != 0)
			return false;
		return Syanten::kouritsuDetect(jreq->pais, jreq->paicnt, jres);
	}
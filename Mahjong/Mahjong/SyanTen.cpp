#include "stdafx.h"

#include "PublicFunction.h"
#include "Kokushimusou.h"

#define MAX_SYANTEN 99
#define ERR_SYANTEN_INTERNAL -2
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

class Syanten
{
private:
	pai tpai2st1[20];
	pai tpai2st2[20];
	int SyantenMaxRecur = 5;
	int calculateKokushiSyanten(const pai* tpai, const int paicnt)
	{
		if (paicnt != 13)
			return MAX_SYANTEN;
		int maicnt[14], daiji = false, typecnt = 0;
		memset(maicnt, 0, sizeof(maicnt));
		for (int i = 0; i<paicnt; i++)
			++maicnt[get_yaotyuu_id(tpai[i])];
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
	int calculateChiitoitsu(const pai* tpai, const int paicnt)
	{
		if (paicnt != 13)
			return MAX_SYANTEN;
		int daiji = 0, j;
		for (int i = 0; i<paicnt - 1; i++)
		{
			if (compare_pai(tpai[i], tpai[i + 1]))
			{
				daiji++;
				for (j = i++; i<paicnt - 1 && compare_pai(tpai[j], tpai[i]); i++);
				i--;
			}
		}
		return 6 - daiji;
	}
	int normalCalculate3(pai* tpai, const int paicnt, int currentMenchi)
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
	int normalCalculate2(const pai* tpai, const int paicnt, int currentMenchi)
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
	int normalCalculate(const pai* tpai, const int paicnt)
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

	public:
	int calculateSyanten(const pai* tpai, const int paicnt, bool normalonly = false) //参数本身已经是有序的了
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
};

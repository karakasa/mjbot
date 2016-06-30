#include "stdafx.h"

#include "PublicFunction.h"
#include "MemoryLeakMonitor.h"
#include "YakuProvider.h"
#include "TenpaiAkariJudge.h"
#include "YPBasic.h"

#define bit(x,y) (((x)&(y))==(y))

#define JUDGE_PATTERN(x) if ((x) % 3 == 2) { if (janto_available) {return;}	else {(x) -= 2; janto_available = true; }}
#define TRY_AVAILABLE(basecond,statement,lowlimit,uplimit) \
                         tepai[tepos].basecond; \
                         for (int i = (lowlimit); i <= (uplimit); i++) \
                         { \
	                         solutionAvailable = false; \
	                         tepai[tepos].statement; \
	                         currentTenpai = tepai[tepos]; \
	                         if (paicnt[retrieveID(tepai[tepos])] == 4) continue; \
	                         mentsuDecideJanto(tepai, tepos + 1); \
                         }

YakuProvider taj::systemProvider;

void taj::addCombination()
{
	paiCombination pc;
	pc.janto[0] = janto_now[0];
	pc.janto[1] = janto_now[1];
	std::copy(mtsGlobal, mtsGlobal + mtsCnt, pc.mts);
	pComb.push_back(std::move(pc));
}

void taj::solutionFound()
{
	effeci_cnt++;
	if (calculate_yaku)
		addCombination();
	else
		tenpai.insert(currentTenpai);
	solutionAvailable = true;
}

void taj::mentsuDecideMentsu(pai* cpai, int cpcount)
{
	if (cpcount == 0)
	{
		solutionFound();
		return;
	}
	pai lastpai1, lastpai2;
	if (compare_pai(cpai[0], cpai[1]))
		if (compare_pai(cpai[0], cpai[2]))
		{
			if (compare_pai(cpai[0], cpai[3]))
			{
				int akari_pos = -1;
				for (int i = 0; i<3; i++)
					if (is_akari(cpai[i]))
					{
						akari_pos = i;
						break;
					}
				if (akari_pos != -1)
				{
					lastpai1 = cpai[akari_pos];
					cpai[akari_pos] = cpai[3];
					cpai[3] = lastpai1;
				}
			}

			mtsGlobal[mtsCnt] = MENTSU(mentsu_KEZ, cpai[0], cpai[1], cpai[2]);
			mtsCnt++;
			if (cpcount == 3)
			{
				solutionFound();
			}
			else {
				std::copy(cpai + 3, cpai + cpcount, middleStepPais[mtsCnt - 1]);
				mentsuDecideMentsu(middleStepPais[mtsCnt - 1], cpcount - 3);
			}
			mtsCnt--;
			if (tenpaiMode && solutionAvailable)
				return;
		}
	lastpai1.fig = lastpai2.fig = lastpai1.trait = lastpai2.trait = lastpai1.type = lastpai2.type = 0;
	for (int i = 1; i<cpcount - 1; i++)
	{
		if (cpai[i].type == cpai[0].type)
			if (cpai[i].fig == cpai[0].fig + 1)
			{
				if (!compare_pai_thesame(lastpai1, cpai[i]))
				{
					lastpai1 = cpai[i];
					lastpai2.trait = lastpai2.type = lastpai2.fig = 0;
					for (int p = i + 1; p<cpcount; p++)
					{
						if (cpai[p].type == cpai[0].type)
							if (cpai[p].fig == cpai[0].fig + 2)
							{
								if (!compare_pai_thesame(lastpai2, cpai[p]))
								{
									lastpai2 = cpai[p];
									mtsGlobal[mtsCnt] = MENTSU(mentsu_SHUNZ, cpai[0], cpai[i], cpai[p]);
									mtsCnt++;

									if (cpcount == 3)
									{
										solutionFound();
									}
									else
									{
										int tpos = 0;
										for (int j = 1; j < i; j++)
										{
											middleStepPais[mtsCnt - 1][tpos] = cpai[j];
											tpos++;
										}
										for (int j = i + 1; j < p; j++)
										{
											middleStepPais[mtsCnt - 1][tpos] = cpai[j];
											tpos++;
										}
										for (int j = p + 1; j < cpcount; j++)
										{
											middleStepPais[mtsCnt - 1][tpos] = cpai[j];
											tpos++;
										}
										mentsuDecideMentsu(middleStepPais[mtsCnt - 1], cpcount - 3);
									}
									mtsCnt--;
									if (tenpaiMode && solutionAvailable)
										return;
								}
							}
						if (cpai[p].type != cpai[0].type)
							break;
					}
				}
			}
		if (cpai[i].type != cpai[0].type)
			return;
	}
}

void taj::mentsuDecideJanto(const pai* opai2, int cpcount)
{
	int count_m = 0, count_s = 0, count_p = 0, count_z = 0;
	bool janto_available = false;

	MLM_ARRAY_NEW("taj::mtsDecideJanto::opai", opai, pai, cpcount);

	std::copy(opai2, opai2 + cpcount, opai);
	std::sort(opai, opai + cpcount, paiSort);

	if (tenpaiMode)
	{
		if (currentProvider->judgeYakuExtended(opai, cpcount, NULL))
		{
			solutionFound();
			MLM_ARRAY_END(opai);
			return;
		}
	}

	std::for_each(opai, opai + cpcount, [&](const pai& p) {
		switch (p.type)
		{
		case 'M':
			count_m++;
			break;
		case 'S':
			count_s++;
			break;
		case 'P':
			count_p++;
			break;
		default:
			count_z++;
			break;
		}
	});

	JUDGE_PATTERN(count_m);
	JUDGE_PATTERN(count_s);
	JUDGE_PATTERN(count_p);
	JUDGE_PATTERN(count_z);

	if ((count_m % 3 != 0) || (count_s % 3 != 0) || (count_p % 3 != 0) || (count_z % 3 != 0))
	{
		MLM_ARRAY_END(opai);
		return;
	}

	mtsCnt = 0;

	if (cpcount == 2)
	{
		if (compare_pai(opai[0], opai[1]))
		{
			janto_now[0] = opai[0];
			janto_now[1] = opai[1];
			solutionFound();
		}
		MLM_ARRAY_END(opai);
		return;
	}

	pai lastvalid;
	lastvalid.fig = 0;
	lastvalid.type = 'Q';
	lastvalid.trait = 0;

	MLM_ARRAY_NEW("taj::mtsDecideJanto::ws", ws, pai, cpcount);

	for (int i = 0; i<cpcount - 1; i++)
	{
		if (compare_pai(opai[i], opai[i + 1]))
			if (!compare_pai_thesame(opai[i], lastvalid))
			{
				janto_now[0] = opai[i];
				janto_now[1] = opai[i + 1];
				lastvalid = opai[i];
				mtsCnt = 0;

				std::copy(opai, opai + i, ws);
				std::copy(opai + i + 2, opai + cpcount, ws + i);
				mentsuDecideMentsu(ws, cpcount - 2);
			}
	}

	MLM_ARRAY_END(ws);
	MLM_ARRAY_END(opai);
}

yakuTable taj::yakuDetect(const judgeRequest& rpai, bool* result)
{
	yakuTable empty;

	solutionAvailable = false;
	tenpaiMode = false;

	judgeRequestSimple jreq;
	pai tepai[14], expandTepai[14];

	currentProvider->clearExtendData();
	currentProvider->setExtendData("dora", rpai.doracnt);

	yTables.clear();
	pComb.clear();
	effeci_cnt = 0;

	jreq.akariStatus = rpai.akariStatus;
	jreq.doracnt = rpai.doracnt;
	jreq.flags = rpai.flags;
	jreq.jihuun = rpai.jihuun;
	jreq.jyouhuun = rpai.jyouhuun;
	jreq.norelease = rpai.norelease;
	calculate_yaku = true;

	std::copy(rpai.pais, rpai.pais + rpai.paicnt, tepai);
	tepai[rpai.paicnt] = rpai.tgtpai;
	tepai[rpai.paicnt].trait |= TRAIT_AKARIPAI;

	std::copy(tepai, tepai + rpai.paicnt + 1, expandTepai);
	int nid = rpai.paicnt + 1;
	std::for_each(rpai.fulus, rpai.fulus + rpai.fulucnt, [&](const mentsu& mt) {
		expandTepai[nid++] = mt.start;
		expandTepai[nid++] = mt.middle;
		expandTepai[nid++] = mt.last;
	});

	std::sort(tepai, tepai + rpai.paicnt + 1, paiSort);
	std::sort(expandTepai, expandTepai + nid, paiSort);

	yakuTable currentYaku;

	currentProvider->setSpecialCase(jreq);
	currentProvider->judgeYakuExtended(expandTepai, nid, &currentYaku);

	if (currentYaku.basicpt != 0)
		yTables.push_back(currentYaku);

	mentsuDecideJanto(tepai, rpai.paicnt + 1);

	std::sort(pComb.begin(), pComb.end());
	pComb.erase(std::unique(pComb.begin(), pComb.end()), pComb.end());

	mentsu mts[4];
	std::copy(rpai.fulus, rpai.fulus + rpai.fulucnt, mts + (4 - rpai.fulucnt));

	for (auto& pc : pComb)
	{
		resetYakuTable2(currentYaku);
		std::copy(pc.mts, pc.mts + (4 - rpai.fulucnt), mts);
		currentProvider->judgeYaku(expandTepai, nid, mts, 4, pc.janto, 2, &currentYaku);
		if (currentYaku.basicpt != 0)
			yTables.push_back(currentYaku);
	}

	resetYakuTable2(currentYaku);
	if (yTables.size() != 0)
	{
		std::sort(yTables.begin(), yTables.end());
		currentYaku = yTables[0];
	}

	if (result != NULL)
		*result = currentYaku.yakutotal != 0;

	return currentYaku;
}

std::unordered_set<pai> taj::tenpaiDetect(const pai* pais, const int tpaicnt, bool* result)
{
	tenpaiMode = true;
	calculate_yaku = false;

	int paicnt[34] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	pai tepai[14];

	effeci_cnt = 0;
	calculate_yaku = false;
	mtsCnt = 0;
	tenpai.clear();

	std::for_each(pais, pais + tpaicnt, [&](auto& p) {paicnt[retrieveID(p)]++; });
	std::copy(pais, pais + tpaicnt, tepai);
	int tepos = tpaicnt;
	tepai[tepos].trait = TRAIT_AKARIPAI;

	currentProvider->clearExtendData();

	TRY_AVAILABLE(type = 'M', fig  = i        , 1, 9);
	TRY_AVAILABLE(type = 'S', fig  = i        , 1, 9);
	TRY_AVAILABLE(type = 'P', fig  = i        , 1, 9);
	TRY_AVAILABLE(fig  =  1 , type = funpai[i], 0, 6);

	if (result != NULL)
		*result = tenpai.size() != 0;

	return tenpai;
}

bool operator<(const paiCombination & a, const paiCombination & b)
{
	if (!comparePaiSame(a.janto[0], b.janto[0]))
		return a.janto[0] < b.janto[0];
	if (!comparePaiSame(a.janto[1], b.janto[1]))
		return a.janto[1] < b.janto[1];
	if (!compareMentsuSame(a.mts[0], b.mts[0]))
		return a.mts[0] < b.mts[0];
	if (!compareMentsuSame(a.mts[1], b.mts[1]))
		return a.mts[1] < b.mts[1];
	if (!compareMentsuSame(a.mts[2], b.mts[2]))
		return a.mts[2] < b.mts[2];
	if (!compareMentsuSame(a.mts[3], b.mts[3]))
		return a.mts[3] < b.mts[3];
	return false;
}

bool operator==(const paiCombination & a, const paiCombination & b)
{
	return comparePaiSame(a.janto[0], b.janto[0]) && comparePaiSame(a.janto[1], b.janto[1]) && \
		compareMentsuSame(a.mts[0], b.mts[0]) && compareMentsuSame(a.mts[1], b.mts[1]) && \
		compareMentsuSame(a.mts[2], b.mts[2]) && compareMentsuSame(a.mts[3], b.mts[3]);
}

bool operator<(const yakuTable & a, const yakuTable & b)
{
	return a.basicpt > b.basicpt;
}

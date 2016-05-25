#include "stdafx.h"

#include "PublicFunction.h"
#include "SimpleAI.h"



int SimpleAI::calcJinZhang(const pai* tepai, const int tepaicnt, const int nSyanten)
{
	pai* tepai2 = new pai[tepaicnt];
	int cJinZhang = 0;
	for (int i = 1; i <= 34; i++)
	{
			pai l;
			retrievePai3(&l, i);
			bool effect = false;
			for (int k = 0; k<tepaicnt; k++)
			{
				if (compare_pai(tepai[k], l))
					continue;
				for (int j = 0; j<k; j++)
					tepai2[j] = tepai[j];
				for (int j = k + 1; j<tepaicnt; j++)
					tepai2[j] = tepai[j];
				tepai2[k] = l;
				std::sort(tepai2 + 0, tepai2 + tepaicnt, paiSort);
				int cSyanTen = st.calculateSyanten(tepai2, tepaicnt);
				if (cSyanTen < nSyanten)
				{
					effect = true;
					break;
				}
			}
			if (effect)
			{
				cJinZhang += 4;
				cJinZhang -= uCanSee[i];
				for (int j = 0; j<tepaicnt; j++)
					if (compare_pai(tepai[j], l))
						cJinZhang--;
			}
		}
		delete[] tepai2;
		return cJinZhang;
}

	
void SimpleAI::outputDebug(char p)
{
	printf("dbg%c %d\n", p, ++dbg);
}
	
int SimpleAI::toID3(int p)
{
	switch (p)
	{
		case 35:
			return 5;
		case 36:
			return 14;
		case 37:
			return 23;
		default:
			return p;
	}
}


int SimpleAI::aiMessage(unsigned char msgType, int par1, int par2, bool* hasReturn, void* payload)
{
	unsigned char* p;
	int mSyanten = 9, paiId, paiIndex = 0;

	switch (msgType)
	{
	case ai::init:
		break;
	case ai::deinit:
		printf("\n%d TICK: %d in %d\n", cid, tickCount, cnt);
		break;
	case ai::start:
		xunshu = 0;
		jihuun = ji[par1];
		jyouhuun = ((par2 & 0xffff0000) >> 16 < 4) ? 'D' : 'N';
		pcnt = 13;
		if (ciid == -1)
			ciid = par1;
		cid = par1;
		riichi = false;
		memset(uCanSee, 0, sizeof(uCanSee));
		break;
	case ai::initalizeTehai:
		p = (unsigned char*)payload;
		for (int i = 0; i<13; i++)
		{
			retrievePai3(&tepai[i], p[i]);
			if (tepai[i].fig == 0)
				printf("error\n");
		}

		std::sort(tepai + 0, tepai + 13, paiSort);
		break;
	case ai::newDora:
		uCanSee[toID3(par2)] ++;
		break;
	case ai::newUra:
		break;
	case ai::othersTurn:
		break;
	case ai::othersTurnFinished:
		retrievePai3(&lastpai, toID3(par1));
		uCanSee[toID3(par1)] ++;
		break;
	case ai::yourTurn:
	{
		*hasReturn = true;
		//  return 0xFFFFFFFF;
		xunshu++;
		pai p;
		int jinzhang_t = 0;
		retrievePai3(&p, par1);
		if (p.fig == 0)
			printf("error\n");
		nSyanten = st.calculateSyanten(tepai, pcnt);
		if (nSyanten == 0)
		{
			for (int i = 0; i<pcnt; i++)
				tepai2[i] = tepai[i];
			tepai2[pcnt] = p;
			std::sort(tepai2 + 0, tepai2 + pcnt + 1, paiSort);
			if (st.calculateSyanten(tepai2, pcnt + 1) == -1)
			{
				*hasReturn = true;
				return(0xFE000000);
			}
			*hasReturn = true;
			return(0xFFFFFFFF);
		}
		if (riichi)
		{
			*hasReturn = true;
			return 0xFFFFFFFF;
		}
		for (int i = 0; i<pcnt; i++)
		{
			//outputDebug('a');
			for (int j = 0; j<i; j++)
				tepai2[j] = tepai[j];
			for (int j = i + 1; j<pcnt; j++)
				tepai2[j] = tepai[j];
			tepai2[i] = p;
			std::sort(tepai2 + 0, tepai2 + pcnt, paiSort);
			int cSyanTen = st.calculateSyanten(tepai2, pcnt);
			pSyanten[i] = cSyanTen;
			if (cSyanTen < nSyanten && cSyanTen <= mSyanten)
			{
				if (cSyanTen == mSyanten || jinzhang_t == 0)
				{
					int jinzhang_t2 = calcJinZhang(tepai2, pcnt, cSyanTen);
					if (jinzhang_t2 > jinzhang_t)
					{
						mSyanten = cSyanTen;
						jinzhang_t = jinzhang_t2;
						paiId = retrieveID2(p);
						paiIndex = i;
					}
				}
				if (cSyanTen < mSyanten)
				{
					mSyanten = cSyanTen;
					paiId = retrieveID2(p);
					paiIndex = i;
				}
			}
		}
		if (nSyanten <= mSyanten)
		{
			if (true)
			{
				int maxJinzhang = calcJinZhang(tepai, pcnt, nSyanten);
				int jinzhangId = -1;
				for (int i = 0; i<pcnt; i++)
				{
					if (compare_pai(tepai[i], p))
						continue;
					for (int j = 0; j<i; j++)
						tepai2[j] = tepai[j];
					for (int j = i + 1; j<pcnt; j++)
						tepai2[j] = tepai[j];
					tepai2[i] = p;
					std::sort(tepai2 + 0, tepai2 + pcnt, paiSort);
					if (pSyanten[i] == nSyanten)
					{
						int jinzhang_c = calcJinZhang(tepai2, pcnt, pSyanten[i]);
						if (jinzhang_c > maxJinzhang)
						{
							jinzhangId = i;
							maxJinzhang = jinzhang_c;
						}
					}
				}
				if (jinzhangId != -1)
				{
					int c = retrieveID2(tepai[jinzhangId]);
					tepai[jinzhangId] = p;
					std::sort(tepai + 0, tepai + pcnt, paiSort);
					*hasReturn = true;
					return c;
				}
			}
			*hasReturn = true;
			return(0xFFFFFFFF);
		}
		else
		{
			paiId = retrieveID2(tepai[paiIndex]);
			tepai[paiIndex] = p;
			std::sort(tepai + 0, tepai + pcnt, paiSort);
			if (mSyanten == 0 && pcnt == 13)
			{
				printf("%d : %d: RIICHI!\n", ciid, cid);
				riichi = true;
				*hasReturn = true;
				return 0xfd000200 | paiId;
			}
			*hasReturn = true;
			return paiId;
		}
	}
	break;
	case ai::yourTurnSpecial:
	{
		xunshu++;
		int jinzhang = 0, lowestSyanten = 9, paiIndex = 0;
		for (int i = 0; i<pcnt; i++)
		{
			for (int j = 0; j<i; j++)
				tepai2[j] = tepai[j];
			for (int j = i + 1; j<pcnt; j++)
				tepai2[j - 1] = tepai[j];
			int cSyanten = st.calculateSyanten(tepai2, pcnt - 1);
			int jinzhang_t = calcJinZhang(tepai2, pcnt - 1, cSyanten);
			if ((jinzhang_t > jinzhang && cSyanten <= lowestSyanten) || cSyanten < lowestSyanten)
			{
				jinzhang = jinzhang_t;
				lowestSyanten = cSyanten;
				paiIndex = i;
			}
		}
		int paiId = retrieveID2(tepai[paiIndex]);
		for (int i = paiIndex + 1; i<pcnt; i++)
			tepai[i - 1] = tepai[i];
		pcnt--;
		*hasReturn = true;
		return paiId;
	}
	break;
	case ai::minChance:
		if ((par1 & 1) == 1)
		{
			*hasReturn = true;
			return(0x01000000);
		}
		else {
			if ((par1 & 4) >> 2)
			{
				if (lastpai.type == jyouhuun || lastpai.type == jihuun || lastpai.type == 'W' || lastpai.type == 'F' || lastpai.type == 'Z')
				{
					int cnt = 0;
					for (int i = 0; i<pcnt; i++)
						if (lastpai.type == tepai[i].type)
							cnt++;
					if (cnt == 2)
					{
						*hasReturn = true;
						return 0x20000000 | retrieveID2(lastpai) | retrieveID2(lastpai) << 8;
					}
				}
			}
			*hasReturn = true;
			return(0xFF000000);
		}
		break;
	case ai::minDone:
		if ((par1 & 3) == cid)
		{
			for (int i = 0; i<pcnt - 1; i++)
				if (lastpai.type == tepai[i].type)
				{
					for (int j = i + 2; j<pcnt; j++)
						tepai[j - 2] = tepai[j];
					pcnt -= 2;
					break;
				}
			return 0;
		}
		switch ((par1 & 0xf0) >> 4)
		{
		case ai::min::kang:
			uCanSee[toID3(par2 & 0xff)] += 3;
			break;
		case ai::min::kangs:
			uCanSee[toID3(par2 & 0xff)] += 4;
			break;
		case ai::min::kangj:
			uCanSee[toID3(par2 & 0xff)] += 1;
			break;
		case ai::min::pon:
			uCanSee[toID3(par2 & 0xff)] += 2;
			break;
		case ai::min::chi:
			uCanSee[toID3((par1 & 0xff00) >> 8)] --;
			uCanSee[toID3(par2 & 0xff)] ++;
			uCanSee[toID3((par2 & 0xff00) >> 8)] ++;
			uCanSee[toID3((par2 & 0xff0000) >> 16)] ++;
			break;
		default:
			break;
		}

		break;
	case ai::finish:
	{
		ai::AKARI* hele = (ai::AKARI*)payload;
		for (int j = 0; j<par1; j++)
		{
			if (hele[j].to == (unsigned int)cid)
			{
				printf((hele[j].from == hele[j].to) ? "%d %d: tsumo! " : "%d %d: akari! ", ciid, cid);
				for (int i = 0; i<pcnt; i++)
				{
					if (tepai[i].fig == 0)
						printf("error\n");
					printf("%d%c ", (int)tepai[i].fig, tepai[i].type);
				}
				pai m;
				retrievePai3(&m, hele[j].pai);
				printf(": %d%c", (int)m.fig, m.type);
				printf("\n");
				for (int i = 0; i<hele[j].yakucnt; i++)
				{
					printf("%s %d\n", yakuname[hele[j].yaku_id[i]], hele[j].yaku_fan[i]);
				}
				printf("%d ·­ %d ·û %d µã\n", hele[j].fan, hele[j].huu, hele[j].pt);
			}
			if (hele[j].from == (unsigned int)cid && hele[j].from != hele[j].to)
			{
				printf("%d %d: i give out a ron...\n", ciid, cid);
			}
		}
	}
	break;
	case ai::finishNaga:
	{
		printf("%d: ", cid);
		for (int i = 0; i<pcnt; i++)
		{
			if (tepai[i].fig == 0)
				printf("error\n");
			printf("%d%c ", (int)tepai[i].fig, tepai[i].type);
		}
		printf("\n");
	}
	break;
	case ai::finishNagaSpecial:
		break;
	case ai::scoreChange:
	{
		int* sc = (int*)payload;
		printf("%d: new point: %d\n", cid, sc[cid]);
	}
	break;
	case ai::wait:
		*hasReturn = true;
		return 0;
		break;
	case ai::gameFinish:
		break;
	case ai::othersTepai:
		break;
	}
	return 0;
}

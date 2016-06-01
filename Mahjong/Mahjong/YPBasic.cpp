#include "stdafx.h"
#include "PublicFunction.h"
#include "YakuProvider.h"
#include "YPBasic.h"

#define TOKEN_7TOITSU 7

bool judgeChiitoitsu(const pai* ppai, bool* nostrict)
{
	if (ppai[0] == ppai[1] && ppai[2] == ppai[3] && ppai[4] == ppai[5] && ppai[6] == ppai[7] && \
		ppai[8] == ppai[9] && ppai[10] == ppai[11] && ppai[12] == ppai[13])
	{
		if (nostrict != NULL)
			*nostrict = true;
		if (ppai[0] != ppai[2] && ppai[2] != ppai[4] && ppai[4] != ppai[6] && \
			ppai[6] != ppai[8] && ppai[8] != ppai[10] && ppai[10] != ppai[12])
			return true;
	}
	return false;
}

// 基本颜色类

YAKU_DEFINE(YakuBasicCYM, yakuType::colorLike, yakuTrait::yakumanLike)

YAKU_SUBNAME_BEGIN(YakuBasicCYM)
YAKU_SUBNAME(0, "字一色")
YAKU_SUBNAME(1, "国士无双")
YAKU_SUBNAME(2, "国士无双十三面")
YAKU_SUBNAME(3, "九莲宝灯")
YAKU_SUBNAME(4, "纯正九莲宝灯")

YAKU_SUBNAME(5, "天和")
YAKU_SUBNAME(6, "地和")
YAKU_SUBNAME(7, "人和")
YAKU_SUBNAME_END()

YAKU_JUDGE_BEGIN_C(YakuBasicCYM)
{
	SUBYAKU("天地人和")
	{
		if (YAKU_CURRENT.norelease)
		{
			if (TSUMO == YAKU_CURRENT.akariStatus)
			{
				YAKU_ADD_IF(YAKU_CURRENT.jihuun == 'D', 5, 13);
				YAKU_ADD_IF(YAKU_CURRENT.jihuun != 'D', 6, 13);
			}
			else {
				YAKU_ADD(7, 13);
			}
		}
	}

	SUBYAKU("字一色")
	{
		if (std::any_of(pais, pais + paicnt, isJi))
			YAKU_ADD(0, 13);
	}
	SUBYAKU("国士无双(十三面)")
	{
		if (YAKU_MENZEN && paicnt == 14)
		{
			int cnt1[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			int twoId = -1;
			bool valid = true;
			std::for_each(pais, pais + paicnt, [&](auto& p) {
				valid &= isYaotyuu(p);
				if (isPaiAkari(p))
					twoId = getYaotyuuId(p);
				else
					cnt1[getYaotyuuId(p)]++;
			});
			if (valid)
			{
				auto singlePai = std::count(cnt1, cnt1 + 13, 1);
				if (singlePai == 13)
				{
					YAKU_ADD(2, YAKU_REDUCED(26));
				}
				else if (singlePai == 11)
				{
					cnt1[twoId]++;
					auto it = std::find(cnt1, cnt1 + 13, 2);
					if (it != cnt1 + 13)
					{
						(*it)--;
						if (13 == std::count(cnt1, cnt1 + 13, 1))
							YAKU_ADD(1, 13);
					}
				}
			}
		}
	}
	SUBYAKU("(纯正)九莲宝灯")
	{
		if (YAKU_MENZEN && paicnt == 14)
		{

			int num_cnt[10] = { 0,0,0,0,0,0,0,0,0,0 };
			int akari = 0;
			bool valid = true;
			std::for_each(pais, pais + paicnt, [&](auto& p) {
				valid &= (p.type == pais[0].type);
				if (isPaiAkari(p))
					akari = p.fig;
				else
					num_cnt[p.fig]++; 
			});
			if (valid)
			{
				if (num_cnt[1] == 3 && num_cnt[9] == 3 && std::count(num_cnt + 2, num_cnt + 8, 1) == 7)
				{
					YAKU_ADD(4, YAKU_REDUCED(26));
				}
				else {
					bool kyuurenboudo = false;
					num_cnt[akari] ++;
					for (int i = 1; i <= 9; i++)
					{
						num_cnt[i]--;
						if (num_cnt[1] == 3 &&
							num_cnt[2] == 1 &&
							num_cnt[3] == 1 &&
							num_cnt[4] == 1 &&
							num_cnt[5] == 1 &&
							num_cnt[6] == 1 &&
							num_cnt[7] == 1 &&
							num_cnt[8] == 1 &&
							num_cnt[9] == 3)
						{
							kyuurenboudo = true;
							break;
						}
						num_cnt[i]++;
					}
					if (kyuurenboudo)
						YAKU_ADD(3, 13);
				}
			}
		}
	}
}
YAKU_JUDGE_END()

YAKU_DEFINE(YakuBasicC, yakuType::colorLike, 0)

YAKU_SUBNAME_BEGIN(YakuBasicC)
YAKU_SUBNAME(0, "断幺九")
YAKU_SUBNAME(1, "混一色")
YAKU_SUBNAME(2, "清一色")
YAKU_SUBNAME(3, "七对子")
YAKU_SUBNAME(4, "混老头")

YAKU_SUBNAME(5, "立直")
YAKU_SUBNAME(6, "W立直")
YAKU_SUBNAME(7, "一发")
YAKU_SUBNAME(8, "门前自摸")
YAKU_SUBNAME(9, "岭上开花")
YAKU_SUBNAME(10, "抢杠")
YAKU_SUBNAME(11, "海底摸月")
YAKU_SUBNAME(12, "河底捞鱼")
YAKU_SUBNAME_END()

YAKU_JUDGE_BEGIN_C(YakuBasicC)
{
	//一组逻辑值，flags&1 立直 flags&2 W立直 flags&4 一发 flags&8 海底 flags&16 河底 flags&32 岭上 flags&64 抢杠
	SUBYAKU("场面役")
	{
		YAKU_ADD_IF((YAKU_CURRENT.flags & 1) != 0, 5, 1);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 2) != 0, 6, 2);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 4) != 0, 7, 1);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 1) != 0 && YAKU_CURRENT.akariStatus == TSUMO, 8, 1);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 32) != 0, 9, 1);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 64) != 0, 10, 1);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 8) != 0, 11, 1);
		YAKU_ADD_IF((YAKU_CURRENT.flags & 16) != 0, 12, 1);
	}

	SUBYAKU("断幺九")
		YAKU_ADD_IF(!std::any_of(pais, pais + paicnt, isYaotyuu), 0, 1);
	SUBYAKU("染手")
	{
		char type = '\0';
		bool noji = false, valid = true;
		for (int i = 0; i < paicnt; i++)
		{
			if (isJi(pais[i]))
				noji = true;
			else
			{
				if (type == '\0')
				{
					type = pais[i].type;
				}
				else {
					if (type != pais[i].type)
					{
						valid = false;
						break;
					}
				}
			}
		}
		if (valid && type != '\0')
		{
			if (noji)
				YAKU_ADD(2, YAKU_REDUCED(6));
			else
				YAKU_ADD(1, YAKU_REDUCED(3));
		}
	}
	SUBYAKU("七对子")
	{
		if (paicnt == 14)
			if (judgeChiitoitsu(pais))
				YAKU_ADD_PEND(3, 2, TOKEN_7TOITSU);
	}
	SUBYAKU("混老头")
		YAKU_ADD_IF( std::all_of(pais, pais + paicnt, isYaotyuu), 4, 2);
//	YAKU_ADD_IF
}
YAKU_JUDGE_END()

YAKU_DEFINE(YakuBasicM, yakuType::mentsuLike, 0)

YAKU_SUBNAME_BEGIN(YakuBasicM)
YAKU_SUBNAME(0, "三元牌")
YAKU_SUBNAME(1, "小三元")
YAKU_SUBNAME(2, "平和")
YAKU_SUBNAME(3, "一气通贯")
YAKU_SUBNAME(4, "一杯口")
YAKU_SUBNAME(5, "二杯口")
YAKU_SUBNAME(6, "混全带幺九")
YAKU_SUBNAME(7, "纯全带幺九")
YAKU_SUBNAME(8, "风牌")
YAKU_SUBNAME(9, "三色同顺")
YAKU_SUBNAME(10, "三色同刻")
YAKU_SUBNAME(11, "对对和")
YAKU_SUBNAME(12, "三暗刻")
YAKU_SUBNAME(13, "三杠子")
YAKU_SUBNAME_END()

YAKU_JUDGE_BEGIN_M(YakuBasicM)
{
	SUBYAKU("风牌")
	{
		int fpYaku = 0;
		std::for_each(mentsus, mentsus + mentsucnt, [&](auto& mt) {
			if (isKez2(mt))
			{
				if (mt.start.type == YAKU_CURRENT.jyouhuun)
					fpYaku++;
				if (mt.start.type == YAKU_CURRENT.jihuun)
					fpYaku++;
			}
		});
		YAKU_ADD_IF(fpYaku != 0, 8, fpYaku);
	}
	SUBYAKU("役牌、小三元")
	{
		auto yp = std::count_if(mentsus, mentsus + mentsucnt, [](auto& mt) {return isSanyuan(mt.start); });
		YAKU_ADD_IF(yp > 0, 0, (int)yp);
		YAKU_ADD_IF(yp == 2 && isSanyuan(janto[0]), 1, 2);
	}
	SUBYAKU("平和")
		YAKU_ADD_IF(YAKU_MENZEN && isNotYakuPai(janto[0]) && !isPaiAkari(janto[0]) && !isPaiAkari(janto[1]) && \
			        std::none_of(mentsus, mentsus + mentsucnt, [&](auto& mt) -> bool {
		                if(mt.type != mentsu_SHUNZ)
							return true;
						if (isMentsuAkari(mt))
						{
							if (isPaiAkari(mt.middle))
								return true;
							if (isPaiAkari(mt.start) && mt.start.fig == 7)
								return true;
							if (isPaiAkari(mt.last) && mt.last.fig == 3)
								return true;
						}
						return false;
	                }), 2, 1);
	SUBYAKU("一气")
	{
		static auto isItsuu = [](const mentsu& mc1, const mentsu& mc2, const mentsu& mc3) -> bool {
			if (isShunz2(mc1) && isShunz2(mc2) && isShunz2(mc3))
				if (mc1.start.type == mc2.start.type == mc3.start.type)
					if (mc1.start.fig == 1 && mc2.start.fig == 4 && mc3.start.fig == 7)
						return true;
			return false;
		};
		YAKU_ADD_IF(YAKU_SELECT3(isItsuu), 3, YAKU_REDUCED(2));
	}
	SUBYAKU("一二杯口")
	{
		if (YAKU_MENZEN && mentsucnt == 4)
		{
			if ((mentsus[0] == mentsus[1] && mentsus[2] == mentsus[3] && mentsus[0].type == mentsu_SHUNZ && mentsus[2].type == mentsu_SHUNZ) || \
				(mentsus[0] == mentsus[2] && mentsus[1] == mentsus[3] && mentsus[0].type == mentsu_SHUNZ && mentsus[1].type == mentsu_SHUNZ))
			{
				YAKU_SUPPRESS(TOKEN_7TOITSU);
				YAKU_ADD(5, 3);
			}
			else {
				if ((mentsus[0] == mentsus[1] && mentsus[0].type == mentsu_SHUNZ) || \
					(mentsus[1] == mentsus[2] && mentsus[1].type == mentsu_SHUNZ) || \
					(mentsus[2] == mentsus[3] && mentsus[2].type == mentsu_SHUNZ))
					YAKU_ADD(4, 1);
			}
		}
	}
	SUBYAKU("带幺九")
	{
		bool hasJi = isJi(janto[0])      || std::any_of(mentsus, mentsus + mentsucnt, [](const mentsu& mt) {return isJi(mt.start); });
		bool isYao = isYaotyuu(janto[0]) || std::any_of(mentsus, mentsus + mentsucnt, [](const mentsu& mt) {return (mt.start.fig == 1) || (mt.last.fig == 9); });
		YAKU_ADD_IF(isYao &  hasJi, 6, YAKU_REDUCED(2));
		YAKU_ADD_IF(isYao & !hasJi, 7, YAKU_REDUCED(3));
	}
	SUBYAKU("三色同顺/三色同刻")
	{
		static auto isSansya = [](const mentsu& mc1, const mentsu& mc2, const mentsu& mc3) -> int {
			bool sansya = false;
			if (mc1.start.type == 'M' && mc2.start.type == 'S' && mc3.start.type == 'P') sansya = true;
			if (mc1.start.type == 'M' && mc2.start.type == 'P' && mc3.start.type == 'S') sansya = true;
			if (mc1.start.type == 'S' && mc2.start.type == 'M' && mc3.start.type == 'P') sansya = true;
			if (mc1.start.type == 'S' && mc2.start.type == 'P' && mc3.start.type == 'M') sansya = true;
			if (mc1.start.type == 'P' && mc2.start.type == 'S' && mc3.start.type == 'M') sansya = true;
			if (mc1.start.type == 'P' && mc2.start.type == 'M' && mc3.start.type == 'S') sansya = true;
			if (!sansya) return 0;
			if (mc1.start.fig != mc2.start.fig || mc2.start.fig != mc3.start.fig || mc1.start.fig != mc3.start.fig) return 0;
			if (isSameType(mc1, mc2) && isSameType(mc2, mc3))
				return isShunz2(mc1) ? 1 : 2;
			return 0;
		};
		static auto dealtWithSansya = [&](const mentsu& mc1, const mentsu& mc2, const mentsu& mc3) -> bool {
			int result = isSansya(mc1, mc2, mc3);
			switch (result)
			{
			case 1:
				YAKU_ADD(9, YAKU_REDUCED(2));
				return true;
				break;
			case 2:
				YAKU_ADD(9, 2);
				return true;
				break;
			}
			return false;
		};
		YAKU_SELECT3(dealtWithSansya);
	}
	SUBYAKU("对对和")
		YAKU_ADD_IF(std::all_of(mentsus, mentsus + mentsucnt, isKez2), 11, 2);
	SUBYAKU("三暗刻")
		YAKU_ADD_IF(std::count_if(mentsus, mentsus + mentsucnt, [=](auto& mt) {return isAnke(mt); }) == 3, 13, 2);
	SUBYAKU("三杠子")
		YAKU_ADD_IF(std::count_if(mentsus, mentsus + mentsucnt, isKangz2) == 3, 13, 2);
}
YAKU_JUDGE_END()

YAKU_DEFINE(YakuBasicMYM, yakuType::mentsuLike, yakuTrait::yakumanLike)

YAKU_SUBNAME_BEGIN(YakuBasicMYM)
YAKU_SUBNAME(0, "大三元")
YAKU_SUBNAME(1, "小四喜")
YAKU_SUBNAME(2, "大四喜")
YAKU_SUBNAME(3, "绿一色")
YAKU_SUBNAME(4, "清老头")
YAKU_SUBNAME(5, "四暗刻")
YAKU_SUBNAME(6, "四暗刻单骑")
YAKU_SUBNAME(7, "四杠子")
YAKU_SUBNAME_END()

YAKU_JUDGE_BEGIN_M(YakuBasicMYM)
{
	SUBYAKU("大三元")
		YAKU_ADD_IF(3 == std::count_if(mentsus, mentsus + mentsucnt, [](auto& mt) {return isSanyuan(mt.start); }), 0, 13);
	SUBYAKU("大小四喜")
	{
		auto fengpai = std::count_if(mentsus, mentsus + mentsucnt, [](auto& mt) {return isFeng(mt.start); });
		if (fengpai == 4)
			YAKU_ADD(2, YAKU_REDUCED(26));
		else
			YAKU_ADD_IF(isFeng(janto[0]), 1, 13);
	}
	SUBYAKU("绿一色")
		YAKU_ADD_IF(isGreen(janto[0]) && std::all_of(mentsus, mentsus + mentsucnt, [](auto& mt) {
		    return isGreen(mt.start) && (isKez2(mt) || mt.start.fig == 2);
		}), 3, 13);
	SUBYAKU("清老头")
		YAKU_ADD_IF(isYaotyuu2(janto[0]) && std::all_of(mentsus, mentsus + mentsucnt, [](auto& mt) {return isKez2(mt) && isYaotyuu2(mt.start); }), 4, 13);
	SUBYAKU("四暗刻(单)")
	{
		bool danqi = isPaiAkari(janto[0]) || isPaiAkari(janto[1]);
		bool sianke = std::all_of(mentsus, mentsus + mentsucnt, [&](auto& mt) {return isAnke(mt); });
		if (sianke)
		{
			YAKU_ADD_IF(!danqi, 5, 13);
			YAKU_ADD_IF( danqi, 6, YAKU_REDUCED(26));
		}
	}
	SUBYAKU("四杠子")
		YAKU_ADD_IF(std::count_if(mentsus, mentsus + mentsucnt, isKangz2) == 4, 7, 13);
}
YAKU_JUDGE_END()

YAKU_DEFINE(YakuBasicDora, yakuType::colorLike, yakuTrait::doraLike)

YAKU_SUBNAME_BEGIN(YakuBasicDora)
YAKU_SUBNAME(0, "宝牌")
YAKU_SUBNAME(1, "北宝牌") // not used
YAKU_SUBNAME_END()

YAKU_JUDGE_BEGIN_C(YakuBasicDora)
{
	YAKU_ADD_IF(this->doraCnt != 0, 0, this->doraCnt);
}
YAKU_JUDGE_END()
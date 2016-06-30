#pragma once
#include "PublicFunction.h"
#include "YakuProvider.h"

// 判断七对子。
// ppai : 长度为 14 的排序后的 pai 数组
// nostrict : 可选，接收参数。不管是不是龙七对，只要满足有 7 组一样的两张牌，本参数即为 true。
// 返回值 : 是或否，该是和否为严格值。如果　nostrict 为 true，返回值为 false，则是龙七对的情况。
bool judgeChiitoitsu(const pai* ppai, bool* nostrict = NULL);

class SysPtProvider : public BasicPtProviderBase
{
public:
	void judgeFu(const pai* pais, int paicnt, const mentsu* mentsus, int mentsucnt, const pai* janto, int jantocnt, yakuTable* yTable, void* yakuProvider);
	bool judgePt(const pai* pais, int paicnt, const mentsu* mentsus, int mentsucnt, const pai* janto, int jantocnt, yakuTable* yTable, void* yakuProvider) override;
};

// 声明基本的判断
YAKU_CLASS_C(YakuBasicShape)
YAKU_CLASS_C(YakuBasicShapeYM)

YAKU_CLASS_C(YakuBasicC)
YAKU_CLASS_C(YakuBasicCYM)
YAKU_CLASS_M(YakuBasicM)
YAKU_CLASS_M(YakuBasicMYM)

YAKU_CLASS_C(YakuBasicDora)

YAKU_CLASS_C_DEF(YakuCustom)
void customJudge(YAKU_PARAM_C, int customValue);
YAKU_CLASS_DEF_END()
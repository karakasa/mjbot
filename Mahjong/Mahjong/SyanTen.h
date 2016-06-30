#pragma once
#include "stdafx.h"

// 向听计算

#include "PublicFunction.h"

struct koritsu
{
	pai paiOut;
	int totalNumber;
	std::vector<pai> expectedPai;
};

class Syanten
{
	private:
		pai tpai2st1[20];
		pai tpai2st2[20];
		const int SyantenMaxRecur = 5;

		// 计算国士向听
		int calculateKokushiSyanten(const pai* tpai, const int paicnt);

		// 计算七对子向听
		int calculateChiitoitsu(const pai* tpai, const int paicnt);

		// 计算搭子
		int normalCalculate3(pai* tpai, const int paicnt, int currentMenchi);

		// 消成形面子
		int normalCalculate2(const pai* tpai, const int paicnt, int currentMenchi);

		// 消雀头
		int normalCalculate(const pai* tpai, const int paicnt);

		// 计算国士听牌内容（本函数已经废弃，仅做兼容性使用，已经统一到 taj::tenpaiDetect 中）
		bool specialized_tenpai_detect(pai* pais, int paicnt, std::unordered_set<pai>& resultEx);
		

	public:

		// 计算向听。该方法比 TenpaiAkariJudge 的判定要快得多，在仅需要判定和牌的形状和听牌的形状时推荐使用本命令。
		// tpai : 需要有序的手牌数组。该数组可以为任意长度。3n + 1 时计算的是当前向听，3n + 2 时计算的是打掉某张牌后的最小向听，3n 时结果无意义。
		// paicnt : 数组长度	
		// normalonly : 仅计算标准型（不计算七对子和国士）
		// 返回值 : (最小)向听数，0 即为听牌，-1 即为和牌。
		int calculateSyanten(const pai* tpai, const int paicnt, bool normalonly = false);

		// 计算有效进张。本过程也可以计算听牌种类，不过效率低于 taj.tenpai_detect()
		// tpai : 手牌数组。
	    // paicnt : 数组长度。应该为 3n+1
		std::unordered_set<pai> kouritsuDetect(pai* tpai, const int paicnt);

		// 计算最优切牌。类似于天凤牌理
		// tpai : 手牌数组。
		// paicnt : 数组长度。应该为 3n+2
		std::vector<> kouritsuDetect2(pai* tpai, const int paicnt);
};

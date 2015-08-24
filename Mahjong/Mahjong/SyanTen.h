#pragma once
#include "stdafx.h"

// 向听计算

#include "PublicFunction.h"

class Syanten
{
	private:
		pai tpai2st1[20];
		pai tpai2st2[20];
		const int SyantenMaxRecur = 5;
		int calculateKokushiSyanten(const pai* tpai, const int paicnt);
		int calculateChiitoitsu(const pai* tpai, const int paicnt);
		int normalCalculate3(pai* tpai, const int paicnt, int currentMenchi);
		int normalCalculate2(const pai* tpai, const int paicnt, int currentMenchi);
		int normalCalculate(const pai* tpai, const int paicnt);
		bool specialized_tenpai_detect(pai* pais, int paicnt, judgeResult* resultEx);

	public:

		// 计算向听。当前，该函数对于七对子向听的计算还有一些问题。该方法比 TenpaiAkariJudge 的判定要快得多，在仅需要判定和牌的形状和听牌的形状时推荐使用本命令。
		// tpai : 需要有序的手牌数组。该数组可以为任意长度。3n + 1 时计算的是当前向听，3n + 2 时计算的是打掉某张牌后的最小向听，3n 时结果无意义。
		// paicnt : 数组长度
		// normalonly : 仅计算标准型（不计算七对子和国士）
		// 返回值 : (最小)向听数，0 即为听牌，-1 即为和牌。
		int calculateSyanten(const pai* tpai, const int paicnt, bool normalonly = false);

		// 计算有效进张。本过程也可以计算听牌种类，不过效率低于 taj.tenpai_detect()
		// tpai : 手牌数组。不需要有序，paicnt : 数组长度。
		// 或者 judgeRequest : 形同 tenpai_detect 的调用，mode 成员必须为 0，否则函数将失败。
		// jres : cnt 与 t 成员分别存放了有效进张的种类数，和具体有效进张
		// 返回值 : 成功与否
		bool kouritsuDetect(pai* tpai, const int paicnt, judgeResult* jres);
		bool kouritsuDetect(judgeRequest* jreq, judgeResult* jres);
};

#pragma once

// 听牌、役的判定。

#include "PublicFunction.h"
#include "YakuProvider.h"

struct paiCombination
{
	pai janto[2];
	int mtscnt = 0;
	mentsu mts[4];
};

/*struct paiSolution
{
	bool mentsuStyle = false;
	paiCombination pc;
	yakuTable yt;
};*/

bool operator< (const paiCombination& a, const paiCombination& b);
bool operator==(const paiCombination& a, const paiCombination& b);

bool operator< (const yakuTable& a, const yakuTable& b);

class taj
{
	private:

		bool tenpaiMode = false;
		bool solutionAvailable = false;

		mentsu mtsGlobal[4];
		pai middleStepPais[4][14];
		int mtsCnt = 0;
		 
		bool calculate_yaku = true;
		bool akari_status;

		std::vector<paiCombination> pComb;
		std::vector<yakuTable> yTables;
		std::unordered_set<pai> tenpai;

		pai janto_now[2];
		pai currentTenpai;

		void addCombination();
		void solutionFound();

		// 消面子
		void mentsuDecideMentsu(pai* cpai, int cpcount);
	
		// 消雀头
		void mentsuDecideJanto(const pai* opai, int cpcount);
	
	public:

		int effeci_cnt = 0;

		static YakuProvider systemProvider;

		YakuProvider* currentProvider = &systemProvider;

		// 判定役、听牌等。调用详情请参考 judgeRequest 和 judgeResult 两个数据类型的说明。
		// rpai : 请求
		// resultEx : 结果
		// 返回值 : 0 为检测成功完成。

		yakuTable yakuDetect(const judgeRequest & rpai, bool* result = NULL);
		std::unordered_set<pai> tenpaiDetect(const pai * pais, const int tpaicnt, bool* result = NULL);
};

#pragma once

// 听牌、役的判定。

#include "PublicFunction.h"

class taj
{
	private:

		bool akari_status;

		yaku_table* current = NULL;
		yaku_table* maxresult = NULL;

		int effeci_cnt = 0;
		int tepos;
		bool hule = false;
		int tenpai_mianshu = 0;
		bool sresult[34];
		bool calculate_yaku = true;
		bool targetcalculate = false;
		int paicount;
		mentsutable mentsutachi;
		bool fulu_status;
		pai janto_now[2];
		pai* tepai;
		judgeResult* resx;
		bool yakuman = false;

		//still used in Main.cpp
		int paicnt[34];

		bool norelease = false;
		bool riichi = false, ihatsu = false, wriichii = false, haidei = false, houdei = false, rinnsyo = false, tyankan = false;
		char jyouhuun = 'N', jihuun = 'N';
		bool chiitoitsu = false, pure_chiitoitsu = false;
		bool kyuurenboudo = false, pure_kyuurenboudo = false;
		bool danyo = false, muuji = false, isou = false, laotou = false, jiisou = false;
		int dora = 0, beidora = 0;
		bool ignore_tsumo_huu = false;

		bool is_annkez(const mentsu* mc);

		void free_yaku(yaku* first);

		void free_yaku_table(yaku_table* first);

		void start();
	
		void reset();
	
		void finish();
	
		void add_yaku(int yaku_id, int yaku_point);
	
		void calculate_huu();
	
		void calculate_basicpoint();
	
		bool judge_ruiisou(const mentsu* mc);
	
		bool is_daiyaojiu(const mentsu* mc);
	
		int is_sansya(const mentsu* mc1, const mentsu* mc2, const mentsu* mc3); //返回0则不是，返回1则为三色通顺，返回2则为三色同刻
	
		bool is_iitsu(const mentsu* mc1, const mentsu* mc2, const mentsu* mc3);
	
		void calculate();
	
		bool show_result(judgeResult* res);

		bool specialized_tenpai_detect(judgeRequest* rpai, judgeResult* resultEx);

		void output_tenpai(pai machi);
	
		void tenpai_detect_recur_2(pai* cpai, int cpcount);
	
		void tenpai_detect_recur(const pai* opai, int cpcount);
	
		int tenpai_detect_internal(judgeRequest* rpai, judgeResult* resultEx);
	
	public:

		// 判定役、听牌等。调用详情请参考 judgeRequest 和 judgeResult 两个数据类型的说明。
		// rpai : 请求
		// resultEx : 结果
		// 返回值 : 0 为检测成功完成。
		int tenpai_detect(const judgeRequest* rpai, judgeResult* resultEx);
	
};


// 判断七对子。
// ppai : 长度为 14 的排序后的 pai 数组
// nostrict : 可选，接收参数。不管是不是龙七对，只要满足有 7 组一样的两张牌，本参数即为 true。
// 返回值 : 是或否。
bool JudgeChiitoitsu(const pai* ppai, bool* nostrict = NULL);


// 判断特殊役满。
// ppai : 长度为 14 的排序后的 pai 数组
// 返回值 : 0 为均不是，否则为本文件头部定义的宏。
int JudgeDaisyarin(const pai* ppai);
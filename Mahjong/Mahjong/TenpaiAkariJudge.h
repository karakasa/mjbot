#pragma once

// 听牌、役的判定。

#include "PublicFunction.h"

class taj
{
	private:
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
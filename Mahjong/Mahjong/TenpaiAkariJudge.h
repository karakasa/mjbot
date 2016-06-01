#pragma once

// 听牌、役的判定。

#include "PublicFunction.h"
#include "YakuProvider.h"

class taj
{
	private:

		bool akari_status;

		yakuTable* current = NULL;
		yakuTable* maxresult = NULL;

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

		int paicnt[34];

		bool norelease = false;
		bool riichi = false, ihatsu = false, wriichii = false, haidei = false, houdei = false, rinnsyo = false, tyankan = false;
		char jyouhuun = 'N', jihuun = 'N';
		bool chiitoitsu = false, pure_chiitoitsu = false;
		bool kyuurenboudo = false, pure_kyuurenboudo = false;
		bool danyo = false, muuji = false, isou = false, laotou = false, jiisou = false;
		int dora = 0, beidora = 0;
		bool ignore_tsumo_huu = false;

		// 是否为暗刻，包括暗杠
		bool is_annkez(const mentsu* mc);

		// 释放某一种面子拆分情况下的 yaku 链表
		void free_yaku(yaku* first);

		// 释放全部面子拆分情况下的　yakuTable 链表
		void free_yaku_table(yakuTable* first);

		// 开始某种情况的计算（为了高点法而设）
		void start();
	
		// 重置模块
		void reset();
	
		// 结束某种情况的计算
		void finish();
	
		// 添加役，在已经役满或役满与一般役的情况下会有单独处理，不需要上层程序处理
		void add_yaku(int yaku_id, int yaku_point);
	
		// 计算符
		void calculate_huu();
	
		// 计算基本点（已经升到整百）
		void calculate_basicpoint();
	
		// 判断是否为绿一色面子
		bool judge_ruiisou(const mentsu* mc);
	
		// 判断是否为带幺九面子
		bool is_daiyaojiu(const mentsu* mc);
	
		// 判断三个面子是否构成三色同顺/同刻，三个面子顺序无所谓
		// 返回0则不是，返回1则为三色通顺，返回2则为三色同刻
		int is_sansya(const mentsu* mc1, const mentsu* mc2, const mentsu* mc3); 
	
		// 判断是否为一通的三个面子，顺序必须从小到大
		bool is_iitsu(const mentsu* mc1, const mentsu* mc2, const mentsu* mc3);
	
		// 主计算过程
		void calculate();
	
		//　统计最大结果，在全部 finish 后
		bool show_result(judgeResult* res);

		// 判断国士听牌/役，替代了 Syanten 类下的同名函数
		bool specialized_tenpai_detect(judgeRequest* rpai, judgeResult* resultEx);

		// 加入听牌情况
		void output_tenpai(pai machi);
	
		// 消面子
		void tenpai_detect_recur_2(pai* cpai, int cpcount);
	
		// 消雀头
		void tenpai_detect_recur(const pai* opai, int cpcount);
	
		// 主处理入口
		int tenpai_detect_internal(judgeRequest* rpai, judgeResult* resultEx);
	
	public:

		static YakuProvider systemProvider;

		YakuProvider* currentProvider = &systemProvider;

		// 判定役、听牌等。调用详情请参考 judgeRequest 和 judgeResult 两个数据类型的说明。
		// rpai : 请求
		// resultEx : 结果
		// 返回值 : 0 为检测成功完成。
		// tenpaiDetect 为别名
		int tenpai_detect(const judgeRequest* rpai, judgeResult* resultEx);
		int tenpaiDetect(const judgeRequest* rpai, judgeResult* resultEx);
	
};

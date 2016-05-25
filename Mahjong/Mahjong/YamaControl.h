#pragma once

// 牌山相关功能

#include "PublicFunction.h"

class Yama
{
	private:
		std::default_random_engine* e1 = NULL;
		const int kanpos[4] = { 128,126,124,122 };
		void initalize_internal_func(int paiCnt);

	public:
		pai* yama = NULL;
		int yamacnt = 0, cpos = 0, kancount = 0;

		Yama();

		// 获得山的指针。如无特殊需要，请不要直接修改。第一张岭上牌为 [135]
		// 返回值 : 为一 pai 的数组。
		pai* getYama();

		// 初始化牌山和随机数发生器，如果之前有牌山的话，状态会被重置。
		// 返回值 : 无
		void init();

		// 释放牌山资源。
		// 返回值 : 无
		void finalize();

		// 将某张牌定为翻开的牌，翻开的牌无法受到开挂函数的影响。
		// id : 要翻开的牌在牌山的位置
		// 返回值 : 无
		void open_pai(int id);

		// 交换牌山中的两张牌
		// id1,id2 : 要交换的牌在牌山的位置
		// 返回值 : 成功与否。如果牌不在可换范围内会失败。
		bool swap(int ID1, int ID2);

		// 获得下一张摸的牌。
		// receive : 接收要摸的下一张牌
		// 返回值 : 成功与否。如果牌山已经摸完返回否，此时 receive 的值无意义。
		bool next(pai* receive);

		// 获得排山还剩余可摸的张数。
		// 返回值 : 如上。
		int get_remaining();

		// 获得下一张所需要牌的位置。
		// type : 牌的花色
		// fig : 牌的数字
		// 返回值 : 返回该张牌在牌山中的位置，找不到则返回 -1。
		int next_swapable_pai(int type, int fig);

		// 杠。
		// receive : 获得岭上牌
		// kandora : 可选，获得新的杠宝牌
		// 返回值 : 已经四杠后则返回 false
		bool kang(pai* receive, pai* kandora = NULL);
};

#include "stdafx.h"

#include "PublicFunction.h"
#include "MemoryLeakMonitor.h"
#include "TenpaiAkariJudge.h"

#define bit(x,y) (((x)&(y))==(y))
#define notji(x) ((x).type=='M' || (x).type=='S' || (x).type=='P')
#define HUU_DETAIL true
#define YAKU_DETAIL true
#define AOTENJYOURUURU false

#ifndef DBG_SHOW_DETAIL
#define DBG_SHOW_DETAIL false
#endif
#ifndef SAFE_MODE
#define SAFE_MODE true
#endif

	bool taj::is_annkez(const mentsu* mc)
	{
		switch (mc->type)
		{
		case mentsu_KEZ:
			return akari_status || ((((mc->start.trait)&TRAIT_AKARIPAI) == 0) && (((mc->middle.trait)&TRAIT_AKARIPAI) == 0) && (((mc->last.trait)&TRAIT_AKARIPAI) == 0));
		case mentsu_KEZ_KANG_S:
			return true;
		default:
			break;
		}
		return false;
	}

	void taj::free_yaku(yaku* first)
	{
		if (first == NULL) return;
		yaku* cpos = first, *tpos;
		while (cpos->next != NULL)
			cpos = cpos->next;
		while (cpos != first)
		{
			tpos = cpos->prev;
			MemoryLeakMonitor::removeMonitor(cpos);
			delete cpos;
			cpos = tpos;
			if (cpos == NULL)
				break;
		}
		MemoryLeakMonitor::removeMonitor(first);
		delete first;
	}
	void taj::free_yaku_table(yaku_table* first)
	{
		free_yaku(first->first);
		MemoryLeakMonitor::removeMonitor(first);
		delete first;
	}
	void taj::start()
	{
		if (current != NULL)
			free_yaku_table(current);
		ignore_tsumo_huu = false;
		yakuman = false;
		current = new yaku_table;
		MemoryLeakMonitor::addMonitor(current, sizeof(yaku_table), "YAKU_TABLE CURRENT");
		current->basicpt = 0;
		current->huutotal = 0;
		current->yakutotal = 0;
		current->tail = NULL;
		current->first = NULL;
	}
	void taj::reset()
	{
		if (current != NULL)
			free_yaku_table(current);
		current = NULL;
		if (maxresult != NULL)
			free_yaku_table(maxresult);
		maxresult = NULL;
	}
	void taj::finish()
	{
		if (maxresult == NULL)
		{
			if (current->yakutotal != 0)
			{
				maxresult = current;
				current = NULL;
			}
			else
			{
				free_yaku_table(current);
				current = NULL;
			}
			return;
		}
		if (current->basicpt > maxresult->basicpt)
		{
			free_yaku_table(maxresult);
			maxresult = current;
			current = NULL;
			return;
		}
		free_yaku_table(current);
		current = NULL;
	}
	void taj::add_yaku(int yaku_id, int yaku_point)
	{
		if (yaku_point <= 0)
			return;
		if (yaku_point >= 13)
			yakuman = true;
		if (yakuman && yaku_point < 13 && !YAKU_DETAIL)
			return;
		if (yakuman && (yaku_id == 61 || yaku_id == 62) && !YAKU_DETAIL)
			return;
		if (current->yakutotal == 0)
			if (yaku_id == 61 || yaku_id == 62) //DORA不算起和翻
				return;
		yaku* yaku_tmp = new yaku;
		MemoryLeakMonitor::addMonitor(yaku_tmp, sizeof(yaku), "YAKU_TMP CURRENT");
		yaku_tmp->yakuid = yaku_id;
		yaku_tmp->pt = yaku_point;
		yaku_tmp->prev = NULL;
		yaku_tmp->next = NULL;
		if (current->first == NULL)
		{
			current->first = yaku_tmp;
			current->tail = yaku_tmp;
		}
		else
		{
			current->tail->next = yaku_tmp;
			yaku_tmp->prev = current->tail;
			current->tail = yaku_tmp;
		}
		current->yakutotal += yaku_point;
	}
	void taj::calculate_huu()
	{
		if (current->huutotal == 0)
		{
			if (taj::pure_chiitoitsu)
			{
				current->huutotal = 25;
				return;
			}
			current->huutotal = 20;
			mentsu* next = (taj::mentsutachi.first)->next;
			while (next != NULL)
			{
				if (is_akari(next->start) || is_akari(next->middle) || is_akari(next->last))
					if (next->type == mentsu_SHUNZ)
					{
						if ((((next->middle).trait)&TRAIT_AKARIPAI) == TRAIT_AKARIPAI)
							current->huutotal += 2;
						if ((((next->start).trait)&TRAIT_AKARIPAI) == TRAIT_AKARIPAI && (next->start).fig == 7)
							current->huutotal += 2;
						if ((((next->last).trait)&TRAIT_AKARIPAI) == TRAIT_AKARIPAI && (next->last).fig == 3)
							current->huutotal += 2;
					}
				bool ann = true;
				switch (next->type)
				{
				case mentsu_KEZ_PON_A:
				case mentsu_KEZ_PON_B:
				case mentsu_KEZ_PON_C:
					ann = false;
				case mentsu_KEZ:
					if (akari_status == RON)
						if (is_akari(next->start) || is_akari(next->middle) || is_akari(next->last))
							ann = false;
					if ((next->start).fig == 1 || (next->start).fig == 9)
						if (ann)
							current->huutotal += 8;
						else
							current->huutotal += 4;
					else if (ann)
						current->huutotal += 4;
					else
						current->huutotal += 2;
					break;
				case mentsu_KEZ_KANG_S:
					if ((next->start).fig == 1 || (next->start).fig == 9)
						current->huutotal += 32;
					else
						current->huutotal += 16;
					break;
				case mentsu_KEZ_KANG_A:
				case mentsu_KEZ_KANG_B:
				case mentsu_KEZ_KANG_C:
					if ((next->start).fig == 1 || (next->start).fig == 9)
						current->huutotal += 16;
					else
						current->huutotal += 8;
					break;
				default:
					break;
				}
				next = next->next;
			}
			if (taj::janto_now[0].type == jyouhuun || taj::janto_now[0].type == jihuun || taj::janto_now[0].type == 'Z' || taj::janto_now[0].type == 'W' || taj::janto_now[0].type == 'F')
				current->huutotal += 2;
			if ((taj::janto_now[0].trait&TRAIT_AKARIPAI) == TRAIT_AKARIPAI || (taj::janto_now[1].trait&TRAIT_AKARIPAI) == TRAIT_AKARIPAI)
				current->huutotal += 2;
			if (!fulu_status && akari_status == RON)
				current->huutotal += 10;
			if (!ignore_tsumo_huu && akari_status == TSUMO)
				current->huutotal += 2;
			if (current->yakutotal == 1 && current->huutotal == 20)
				current->huutotal = 30;
			if (fulu_status && akari_status == RON && current->huutotal == 20)
				current->huutotal = 30;
			if (current->huutotal % 10 != 0)
				current->huutotal = 10 * (1 + (current->huutotal) / 10);
		}
	}
	void taj::calculate_basicpoint()
	{
		if (HUU_DETAIL && current->yakutotal != 0)
			calculate_huu();
		switch (current->yakutotal)
		{
		case 0:
			current->basicpt = 0;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			if (!HUU_DETAIL)
				calculate_huu();
			current->basicpt = current->huutotal * 1 << (current->yakutotal + 2);
			if (current->basicpt >= 2000)
				current->basicpt = 2000;
			break;
		case 5:
			current->basicpt = 2000;
			break;
		case 6:
		case 7:
			current->basicpt = 3000;
			break;
		case 8:
		case 9:
		case 10:
			current->basicpt = 4000;
			break;
		case 11:
		case 12:
			current->basicpt = 6000;
			break;
		case 13:
			current->basicpt = 8000;
			break;
		default:
			if (current->yakutotal > 13)
				current->basicpt = (current->yakutotal) / 13 * 8000;
			break;
		}
	}
	bool taj::judge_ruiisou(const mentsu* mc)
	{
		if (mc->start.type == 'F')
			return true;
		if (mc->start.type != 'S')
			return false;
		if (mc->start.fig == 2)
			return true;
		switch (mc->type)
		{
		case mentsu_KEZ:
		case mentsu_KEZ_KANG_A:
		case mentsu_KEZ_KANG_B:
		case mentsu_KEZ_KANG_C:
		case mentsu_KEZ_KANG_S:
		case mentsu_KEZ_PON_A:
		case mentsu_KEZ_PON_B:
		case mentsu_KEZ_PON_C:
			switch (mc->start.fig)
			{
			case 3:
			case 4:
			case 6:
			case 8:
				return true;
			default:
				break;
			}
			break;
		default:
			break;
		}
		return false;
	}
	bool taj::is_daiyaojiu(const mentsu* mc)
	{
		return (mc->start.fig == 1) || (mc->last.fig == 9);
	}
	int taj::is_sansya(const mentsu* mc1, const mentsu* mc2, const mentsu* mc3) //返回0则不是，返回1则为三色通顺，返回2则为三色同刻
	{
		bool sansya = false;
		if (mc1->start.type == 'M' && mc2->start.type == 'S' && mc3->start.type == 'P') sansya = true;
		if (mc1->start.type == 'M' && mc2->start.type == 'P' && mc3->start.type == 'S') sansya = true;
		if (mc1->start.type == 'S' && mc2->start.type == 'M' && mc3->start.type == 'P') sansya = true;
		if (mc1->start.type == 'S' && mc2->start.type == 'P' && mc3->start.type == 'M') sansya = true;
		if (mc1->start.type == 'P' && mc2->start.type == 'S' && mc3->start.type == 'M') sansya = true;
		if (mc1->start.type == 'P' && mc2->start.type == 'M' && mc3->start.type == 'S') sansya = true;
		if (!sansya) return 0;
		if (mc1->start.fig != mc2->start.fig || mc2->start.fig != mc3->start.fig || mc1->start.fig != mc3->start.fig) return 0;
		if (isKez(mc1) && isKez(mc2) && isKez(mc3)) return 2;
		if (!isKez(mc1) && !isKez(mc2) && !isKez(mc3)) return 1;
		return 0;
	}
	bool taj::is_iitsu(const mentsu* mc1, const mentsu* mc2, const mentsu* mc3)
	{
		if (mc1->type == mc2->type && mc1->type == mc3->type && mc2->type == mc3->type)
			if (!isKez(mc1) && !isKez(mc2) && !isKez(mc3))
				if (mc1->start.fig == 1 && mc2->start.fig == 4 && mc3->start.fig == 7)
					return true;
		return false;
	}
	void taj::calculate()
	{

		//尚未处理输入的面子数不足四个的情况
		mentsu *next1, *next2, *next3, *next4, *next;
		int count_tmp = 0, sansyastatus = 0;
		bool nipai;
		bool yibeikou = false;

		if (norelease && akari_status == TSUMO && jihuun == 'D') add_yaku(50, 13); //天和
		if (norelease && akari_status == TSUMO && jihuun != 'D') add_yaku(51, 13); //地和
		if (norelease && akari_status == RON) add_yaku(52, 13); //人和

		if (jiisou)
			add_yaku(45, 13);  //字一色情况 唯一一个可以和七对子复合的役满 所以提到前面来

		next = (mentsutachi.first)->next;
		if (pure_chiitoitsu && next == NULL)
		{
			add_yaku(29, 2);    //七对子 其余役待实现
			goto finish_stage;
		}


		next2 = NULL, next3 = NULL, next4 = NULL;
		next1 = next;
		if (next != NULL) next2 = next->next; else goto finish_stage;
		if (next2 != NULL) next3 = next2->next; else goto finish_stage;
		if (next3 != NULL) next4 = next3->next; else goto finish_stage;

		//常规役满判定
		if (is_annkez(next1) && is_annkez(next2) && is_annkez(next3) && is_annkez(next4) && !fulu_status)
		{
			if ((taj::janto_now[0].trait&TRAIT_AKARIPAI) == TRAIT_AKARIPAI || (taj::janto_now[1].trait&TRAIT_AKARIPAI) == TRAIT_AKARIPAI)
				add_yaku(55, 26); //四暗单骑
			else if (akari_status == TSUMO)
				add_yaku(42, 13); //四暗
		}
		if (next->start.type == 'Z' || next2->start.type == 'Z' || next3->start.type == 'Z' || next4->start.type == 'Z')
			if (next->start.type == 'W' || next2->start.type == 'W' || next3->start.type == 'W' || next4->start.type == 'W')
				if (next->start.type == 'F' || next2->start.type == 'F' || next3->start.type == 'F' || next4->start.type == 'F')
					add_yaku(43, 13); //大三元
		if ((next->start.type == 'D' || next2->start.type == 'D' || next3->start.type == 'D' || next4->start.type == 'D') &&
			(next->start.type == 'N' || next2->start.type == 'N' || next3->start.type == 'N' || next4->start.type == 'N') &&
			(next->start.type == 'X' || next2->start.type == 'X' || next3->start.type == 'X' || next4->start.type == 'X') &&
			(next->start.type == 'B' || next2->start.type == 'B' || next3->start.type == 'B' || next4->start.type == 'B'))
			add_yaku(54, 26); //大四喜
							  //小四喜判定
		if (taj::janto_now[0].type == 'D')
			if (next->start.type == 'N' || next2->start.type == 'N' || next3->start.type == 'N' || next4->start.type == 'N')
				if (next->start.type == 'X' || next2->start.type == 'X' || next3->start.type == 'X' || next4->start.type == 'X')
					if (next->start.type == 'B' || next2->start.type == 'B' || next3->start.type == 'B' || next4->start.type == 'B')
						add_yaku(44, 13);
		if (taj::janto_now[0].type == 'N')
			if (next->start.type == 'D' || next2->start.type == 'D' || next3->start.type == 'D' || next4->start.type == 'D')
				if (next->start.type == 'X' || next2->start.type == 'X' || next3->start.type == 'X' || next4->start.type == 'X')
					if (next->start.type == 'B' || next2->start.type == 'B' || next3->start.type == 'B' || next4->start.type == 'B')
						add_yaku(44, 13);
		if (taj::janto_now[0].type == 'X')
			if (next->start.type == 'N' || next2->start.type == 'N' || next3->start.type == 'N' || next4->start.type == 'N')
				if (next->start.type == 'D' || next2->start.type == 'D' || next3->start.type == 'D' || next4->start.type == 'D')
					if (next->start.type == 'B' || next2->start.type == 'B' || next3->start.type == 'B' || next4->start.type == 'B')
						add_yaku(44, 13);
		if (taj::janto_now[0].type == 'B')
			if (next->start.type == 'N' || next2->start.type == 'N' || next3->start.type == 'N' || next4->start.type == 'N')
				if (next->start.type == 'X' || next2->start.type == 'X' || next3->start.type == 'X' || next4->start.type == 'X')
					if (next->start.type == 'D' || next2->start.type == 'D' || next3->start.type == 'D' || next4->start.type == 'D')
						add_yaku(44, 13);
		//小四喜判定结束
		//绿一色判定开始
		switch (retrieveID(taj::janto_now[0]))
		{
		case 33:
		case 10:
		case 11:
		case 12:
		case 14:
		case 16:
			// 雀头是绿的
			if (judge_ruiisou(next))
				if (judge_ruiisou(next2))
					if (judge_ruiisou(next3))
						if (judge_ruiisou(next4))
							add_yaku(46, 13);
			break;
		default:
			break;
		}
		//绿一色判定结束
		if (muuji)
			if (isKez(next))
				if (isKez(next2))
					if (isKez(next3))
						if (isKez(next4))
							if (next->start.fig == 1 || next->start.fig == 9)
								if (next2->start.fig == 1 || next2->start.fig == 9)
									if (next3->start.fig == 1 || next3->start.fig == 9)
										if (next4->start.fig == 1 || next4->start.fig == 9)
											add_yaku(47, 13); //清老头
		if (pure_kyuurenboudo)
			add_yaku(60, 26); //纯正九连宝灯
		if (kyuurenboudo)
			add_yaku(48, 13); //九连宝灯
		if (isKangz(next))
			if (isKangz(next2))
				if (isKangz(next3))
					if (isKangz(next4))
						add_yaku(49, 13); //四杠子

		if (yakuman && !YAKU_DETAIL)
			goto finish_stage;

		if (riichi && !wriichii) add_yaku(1, 1); //立直
		if (wriichii) add_yaku(36, 2); //W立直
		if (ihatsu) add_yaku(2, 1); //一发
		if (akari_status == TSUMO && !fulu_status) add_yaku(3, 1); //门前自摸
		if (rinnsyo)
		{
			add_yaku(22, 1);    //岭上开花
			ignore_tsumo_huu = true;
		}
		if (tyankan) add_yaku(23, 1); //抢杠
		if (haidei) add_yaku(24, 1); //海底
		if (houdei) add_yaku(25, 1); //河底

		if (danyo)
			add_yaku(4, 1); //断幺
		if (isou && !jiisou)
		{
			if (muuji)
				add_yaku(40, fulu_status ? 5 : 6); //清一色
			else
				add_yaku(37, fulu_status ? 2 : 3); //混一色
		}
		if (laotou && !muuji && !jiisou)
			add_yaku(34, 2); //混老头

		count_tmp = 0;
		if (isKangz(next1)) count_tmp++;
		if (isKangz(next2)) count_tmp++;
		if (isKangz(next3)) count_tmp++;
		if (isKangz(next4)) count_tmp++;
		if (count_tmp == 3)
			add_yaku(32, 2); //三杠子
		if (isKez(next1))
			if (isKez(next2))
				if (isKez(next3))
					if (isKez(next4))
						add_yaku(30, 2); //对对
		count_tmp = 0;
		if (is_annkez(next1)) count_tmp++;
		if (is_annkez(next2)) count_tmp++;
		if (is_annkez(next3)) count_tmp++;
		if (is_annkez(next4)) count_tmp++;
		if (count_tmp == 3)
			add_yaku(31, 2); //三暗刻
		if (!laotou)
			if (is_daiyaojiu(next1))
				if (is_daiyaojiu(next2))
					if (is_daiyaojiu(next3))
						if (is_daiyaojiu(next4))
							if (taj::janto_now[0].fig == 1 || taj::janto_now[0].fig == 9)
							{
								if (muuji)
									add_yaku(38, fulu_status ? 2 : 3); //纯全
								else
									add_yaku(28, fulu_status ? 1 : 2); //混全
							}
		// 小三元判定
		if (taj::janto_now[0].type == 'Z')
			if (next1->start.type == 'W' || next2->start.type == 'W' || next3->start.type == 'W' || next4->start.type == 'W')
				if (next1->start.type == 'F' || next2->start.type == 'F' || next3->start.type == 'F' || next4->start.type == 'F')
					add_yaku(35, 2);
		if (taj::janto_now[0].type == 'W')
			if (next1->start.type == 'Z' || next2->start.type == 'Z' || next3->start.type == 'Z' || next4->start.type == 'Z')
				if (next1->start.type == 'F' || next2->start.type == 'F' || next3->start.type == 'F' || next4->start.type == 'F')
					add_yaku(35, 2);
		if (taj::janto_now[0].type == 'F')
			if (next1->start.type == 'W' || next2->start.type == 'W' || next3->start.type == 'W' || next4->start.type == 'W')
				if (next1->start.type == 'Z' || next2->start.type == 'Z' || next3->start.type == 'Z' || next4->start.type == 'Z')
					add_yaku(35, 2);
		// 小三元判定终了
		// 三色判定开始
		sansyastatus = 0;
		sansyastatus = is_sansya(next1, next2, next3);
		if (sansyastatus != 0)
		{
			if (sansyastatus == 1)
				add_yaku(26, fulu_status ? 1 : 2);
			else
				add_yaku(33, 2);
		}
		else
		{
			sansyastatus = is_sansya(next1, next2, next4);
			if (sansyastatus != 0)
			{
				if (sansyastatus == 1)
					add_yaku(26, fulu_status ? 1 : 2);
				else
					add_yaku(33, 2);
			}
			else
			{
				sansyastatus = is_sansya(next1, next3, next4);
				if (sansyastatus != 0)
				{
					if (sansyastatus == 1)
						add_yaku(26, fulu_status ? 1 : 2);
					else
						add_yaku(33, 2);
				}
				else
				{
					sansyastatus = is_sansya(next2, next3, next4);
					if (sansyastatus != 0)
					{
						if (sansyastatus == 1)
							add_yaku(26, fulu_status ? 1 : 2);
						else
							add_yaku(33, 2);
					}
				}
			}
		} //三色判定终了
		  // 一气判定开始
		if (is_iitsu(next1, next2, next3))
			add_yaku(27, fulu_status ? 1 : 2);
		else if (is_iitsu(next1, next2, next4))
			add_yaku(27, fulu_status ? 1 : 2);
		else if (is_iitsu(next1, next3, next4))
			add_yaku(27, fulu_status ? 1 : 2);
		else if (is_iitsu(next2, next3, next4))
			add_yaku(27, fulu_status ? 1 : 2);
		// 一气判定终了
		nipai = false;
		if (chiitoitsu && !fulu_status)
		{
			//二杯口判断
			if (next->type == mentsu_SHUNZ && (next->next)->type == mentsu_SHUNZ && ((next->next)->next)->type == mentsu_SHUNZ && (((next->next)->next)->next)->type == mentsu_SHUNZ)
			{
				//四个暗顺是基础
				if ((next->start).type == (next2->start).type)
					if ((next3->start).type == (next4->start).type)
						if ((next->start).fig == (next2->start).fig)
							if ((next3->start).fig == (next4->start).fig)
							{
								//历尽千辛万苦
								add_yaku(39, 3); //二杯口
								nipai = true;
							}
			}
		}
		while (next != NULL)
		{
			if (!nipai) //一杯口的前提:不是二杯口
				if (next->next != NULL)
					if (next->type == mentsu_SHUNZ)
						if (next->next->type == mentsu_SHUNZ)
							if ((next->next->start).type == (next->start).type)
								if ((next->next->start).fig == (next->start).fig)
									if (!yibeikou)
									{
										add_yaku(6, 1); //一杯口
										yibeikou = true;
									}

			if (jyouhuun == (next->start).type && jihuun == (next->start).type) //役牌
			{
				switch (jyouhuun)
				{
				case 'D':
					add_yaku(18, 2);
					break;
				case 'N':
					add_yaku(19, 2);
					break;
				case 'X':
					add_yaku(20, 2);
					break;
				case 'B':
					add_yaku(21, 2);
					break;
				default:
					add_yaku(59, 2);
					break;
				}
			}
			else
			{
				if (jyouhuun == (next->start).type)
					switch (jyouhuun)
					{
					case 'D':
						add_yaku(10, 1);
						break;
					case 'N':
						add_yaku(11, 1);
						break;
					case 'X':
						add_yaku(12, 1);
						break;
					case 'B':
						add_yaku(13, 1);
						break;
					default:
						add_yaku(57, 1);
						break;
					}

				if (jihuun == (next->start).type)
					switch (jihuun)
					{
					case 'D':
						add_yaku(14, 1);
						break;
					case 'N':
						add_yaku(15, 1);
						break;
					case 'X':
						add_yaku(16, 1);
						break;
					case 'B':
						add_yaku(17, 1);
						break;
					default:
						add_yaku(58, 1);
						break;
					}
			}

			if (next->start.type == 'Z')
				add_yaku(9, 1);
			if (next->start.type == 'W')
				add_yaku(7, 1);
			if (next->start.type == 'F')
				add_yaku(8, 1);

			next = next->next;
		}
		//平和判定开始
		if (!fulu_status)
			if (taj::janto_now[0].type != 'Z' && taj::janto_now[0].type != 'W' && taj::janto_now[0].type != 'F' && taj::janto_now[0].type != jyouhuun  && taj::janto_now[0].type != jihuun)
				if (next1->type == mentsu_SHUNZ && next2->type == mentsu_SHUNZ && next3->type == mentsu_SHUNZ && next4->type == mentsu_SHUNZ)
				{
					//std::cout<<"PINGHE"<<std::endl;
					bool pin = true;
					next = next1;
					while (next != NULL)
					{
						if (is_akari(next->start) || is_akari(next->middle) || is_akari(next->last))
							if ((((next->middle).trait)&TRAIT_AKARIPAI) == TRAIT_AKARIPAI)
							{
								pin = false;
								break;
							}
						if ((((next->start).trait)&TRAIT_AKARIPAI) == TRAIT_AKARIPAI && (next->start).fig == 7)
						{
							pin = false;
							break;
						}
						if ((((next->last).trait)&TRAIT_AKARIPAI) == TRAIT_AKARIPAI && (next->last).fig == 3)
						{
							pin = false;
							break;
						}
						next = next->next;
					}
					if (is_akari(taj::janto_now[0]) || is_akari(taj::janto_now[1]))
						pin = false;
					if (pin)
					{
						add_yaku(5, 1); //平和
						ignore_tsumo_huu = true; //不计算自摸的符
					}
				}
		//平和判定结束

	finish_stage:
		if (dora)
			add_yaku(61, dora);
		if (beidora)
			add_yaku(62, beidora);
		calculate_basicpoint();
	}
	bool taj::show_result(judgeResult* res)
	{
		if (maxresult != NULL)
		{
			res->yakucnt = 0;
			res->yakutotal = 0;
			res->basicpt = maxresult->basicpt;
			res->huutotal = maxresult->huutotal;
			int i = 0;
			yaku* n2 = maxresult->first;
			while (n2 != NULL)
			{
				if (yakuman && n2->pt < 13)
				{
					n2 = n2->next;
					continue;
				}
				res->yakuid[i] = n2->yakuid;
				res->yakutotal += (res->pt[i] = n2->pt);
				n2 = n2->next;
				(res->yakucnt)++;
				i++;
			}
		}
		else
		{
			res->yakutotal = res->huutotal = res->basicpt = 0;
		}
		return maxresult != NULL;
	}



	bool taj::specialized_tenpai_detect(judgeRequest* rpai, judgeResult* resultEx)
	{
		if (rpai->paicnt == 13 && rpai->mode == 0)
		{
			int ycnt[14];
			memset(ycnt, 0, sizeof(ycnt));
			for (int i = 0; i<13; i++)
				ycnt[get_yaotyuu_id((rpai->pais)[i])]++;
			if (ycnt[0] == 1 && ycnt[1] == 1 && ycnt[2] == 1 && ycnt[3] == 1 && ycnt[4] == 1 && ycnt[5] == 1 && ycnt[6] == 1 && ycnt[7] == 1 && ycnt[8] == 1 && ycnt[9] == 1 && ycnt[10] == 1 && ycnt[11] == 1 && ycnt[12] == 1)
			{
				resultEx->cnt = 13;
				retrievePai3(&((resultEx->t)[0]), 1);
				retrievePai3(&((resultEx->t)[1]), 9);
				retrievePai3(&((resultEx->t)[2]), 10);
				retrievePai3(&((resultEx->t)[3]), 18);
				retrievePai3(&((resultEx->t)[4]), 19);
				retrievePai3(&((resultEx->t)[5]), 27);
				retrievePai3(&((resultEx->t)[6]), 28);
				retrievePai3(&((resultEx->t)[7]), 29);
				retrievePai3(&((resultEx->t)[8]), 30);
				retrievePai3(&((resultEx->t)[9]), 31);
				retrievePai3(&((resultEx->t)[10]), 32);
				retrievePai3(&((resultEx->t)[11]), 33);
				retrievePai3(&((resultEx->t)[12]), 34);
				return true;
			}
			int y0 = 0, y1 = 0, y2 = 0, yid = 0;
			for (int i = 0; i<13; i++)
				switch (ycnt[i])
				{
				case 0:
					y0++;
					yid = i;
					break;
				case 1:
					y1++;
					break;
				case 2:
					y2++;
					break;
				default:
					return false;
				}
			if (y0 == 1 && y2 == 1 && y1 == 11)
			{
				resultEx->cnt = 1;
				retrievePai3(&((resultEx->t)[0]), yaotrans[yid]);
				return true;
			}
			return false;
		}
		if (rpai->paicnt == 13 && rpai->mode == 1)
		{
			int ycnt[14];
			memset(ycnt, 0, sizeof(ycnt));
			for (int i = 0; i<13; i++)
				ycnt[get_yaotyuu_id((rpai->pais)[i])]++;
			if (ycnt[0] == 1 && ycnt[1] == 1 && ycnt[2] == 1 && ycnt[3] == 1 && ycnt[4] == 1 && ycnt[5] == 1 && ycnt[6] == 1 && ycnt[7] == 1 && ycnt[8] == 1 && ycnt[9] == 1 && ycnt[10] == 1 && ycnt[11] == 1 && ycnt[12] == 1 && get_yaotyuu_id(rpai->tgtpai) != 13)
			{
				resultEx->yakucnt = 1;
				resultEx->yakutotal = 26;
				resultEx->huutotal = 30;
				resultEx->basicpt = 16000;
				resultEx->yakuid[0] = 53;
				resultEx->pt[0] = 26;
				if (rpai->norelease)
				{
					if (rpai->akari_status == RON)
					{
						resultEx->yakucnt++;
						resultEx->yakutotal += 13;
						resultEx->basicpt += 8000;
						resultEx->yakuid[1] = 52;
						resultEx->pt[1] = 13;
					}
					else if (rpai->jihuun == 'D')
					{
						resultEx->yakucnt++;
						resultEx->yakutotal += 13;
						resultEx->basicpt += 8000;
						resultEx->yakuid[1] = 50;
						resultEx->pt[1] = 13;
					}
					else
					{
						resultEx->yakucnt++;
						resultEx->yakutotal += 13;
						resultEx->basicpt += 8000;
						resultEx->yakuid[1] = 51;
						resultEx->pt[1] = 13;
					}
				}
				return true;
			}
			ycnt[get_yaotyuu_id(rpai->tgtpai)]++;
			for (int i = 0; i<13; i++)
				if (ycnt[i] == 2)
				{
					ycnt[i]--;
					break;
				}
			if (ycnt[0] == 1 && ycnt[1] == 1 && ycnt[2] == 1 && ycnt[3] == 1 && ycnt[4] == 1 && ycnt[5] == 1 && ycnt[6] == 1 && ycnt[7] == 1 && ycnt[8] == 1 && ycnt[9] == 1 && ycnt[10] == 1 && ycnt[11] == 1 && ycnt[12] == 1)
			{
				resultEx->yakucnt = 1;
				resultEx->yakutotal = 13;
				resultEx->huutotal = 30;
				resultEx->basicpt = 8000;
				resultEx->yakuid[0] = 41;
				resultEx->pt[0] = 13;
				if (rpai->norelease)
				{
					if (rpai->akari_status == RON)
					{
						resultEx->yakucnt++;
						resultEx->yakutotal += 13;
						resultEx->basicpt += 8000;
						resultEx->yakuid[1] = 52;
						resultEx->pt[1] = 13;
					}
					else if (rpai->jihuun == 'D')
					{
						resultEx->yakucnt++;
						resultEx->yakutotal += 13;
						resultEx->basicpt += 8000;
						resultEx->yakuid[1] = 50;
						resultEx->pt[1] = 13;
					}
					else
					{
						resultEx->yakucnt++;
						resultEx->yakutotal += 13;
						resultEx->basicpt += 8000;
						resultEx->yakuid[1] = 51;
						resultEx->pt[1] = 13;
					}
				}
				return true;
			}
		}
		return false;
	}

	void taj::output_tenpai(pai machi)
	{
		int pid = retrieveID(machi);
		bool allowcalc = paicnt[pid] <= 4;
		effeci_cnt++;
		if (calculate_yaku && allowcalc)
		{
			start();
			calculate();
			finish();
		}
		hule = true;
		if (!targetcalculate && (DBG_SHOW_DETAIL || !sresult[pid]))
		{
			tenpai_mianshu++;
			if (!allowcalc)
			{
				hule = false;
			}
			else
			{
				resx->t[(resx->cnt)++] = machi;
			}
			sresult[pid] = true;
		}
	}
	void taj::tenpai_detect_recur_2(pai* cpai, int cpcount)
	{
		if (cpcount == 0)
		{
			output_tenpai(tepai[tepos]);
			return;
		}
		pai lastpai1, lastpai2;
		bool KezAvail = false;
		if (compare_pai(cpai[0], cpai[1]))
			if (compare_pai(cpai[0], cpai[2]))
			{
				if (compare_pai(cpai[0], cpai[3]))
				{
					int akari_pos = -1;
					for (int i = 0; i<3; i++)
						if (is_akari(cpai[i]))
						{
							akari_pos = i;
							break;
						}
					if (akari_pos != -1)
					{
						lastpai1 = cpai[akari_pos];
						cpai[akari_pos] = cpai[3];
						cpai[3] = lastpai1;
					}
				}
				KezAvail = true;
				if(cpcount == 3)
				{
					mentsu* tmentsu = new mentsu;
					MemoryLeakMonitor::addMonitor(tmentsu, sizeof(mentsu), "TENPAI RECUR 2 1");
					tmentsu->next = NULL;
					tmentsu->prev = mentsutachi.tail;
					tmentsu->type = mentsu_KEZ;
					tmentsu->start = cpai[0];
					tmentsu->middle = cpai[1];
					tmentsu->last = cpai[2];
					(mentsutachi.tail)->next = tmentsu;
					mentsutachi.tail = tmentsu;
					tenpai_detect_recur_2(NULL, cpcount - 3);
					mentsutachi.tail = tmentsu->prev;
					(mentsutachi.tail)->next = NULL;
					MemoryLeakMonitor::removeMonitor(tmentsu);
					delete tmentsu;
				}
				else {
					pai* tepai3 = new pai[cpcount - 3];
					MemoryLeakMonitor::addMonitor(tepai3, (cpcount - 3)*sizeof(pai), "TEPAI3 1");
					for (int i = 3; i < cpcount; i++)
						tepai3[i - 3] = cpai[i];
					mentsu* tmentsu = new mentsu;
					MemoryLeakMonitor::addMonitor(tmentsu, sizeof(mentsu), "TENPAI RECUR 2 1");
					tmentsu->next = NULL;
					tmentsu->prev = mentsutachi.tail;
					tmentsu->type = mentsu_KEZ;
					tmentsu->start = cpai[0];
					tmentsu->middle = cpai[1];
					tmentsu->last = cpai[2];
					(mentsutachi.tail)->next = tmentsu;
					mentsutachi.tail = tmentsu;
					tenpai_detect_recur_2(tepai3, cpcount - 3);
					mentsutachi.tail = tmentsu->prev;
					(mentsutachi.tail)->next = NULL;
					MemoryLeakMonitor::removeMonitor(tmentsu);
					delete tmentsu;
					MemoryLeakMonitor::removeMonitor(tepai3);
					delete[] tepai3;
				}
			}
		if (KezAvail && !SAFE_MODE)
			return;
		lastpai1.fig = lastpai2.fig = lastpai1.trait = lastpai2.trait = lastpai1.type = lastpai2.type = 0;
		for (int i = 1; i<cpcount - 1; i++)
		{
			if (cpai[i].type == cpai[0].type)
				if (cpai[i].fig == cpai[0].fig + 1)
				{
					if (!compare_pai_thesame(lastpai1, cpai[i]))
					{
						lastpai1 = cpai[i];
						lastpai2.trait = lastpai2.type = lastpai2.fig = 0;
						for (int p = i + 1; p<cpcount; p++)
						{
							if (cpai[p].type == cpai[0].type)
								if (cpai[p].fig == cpai[0].fig + 2)
								{
									if (!compare_pai_thesame(lastpai2, cpai[p]))
									{
										lastpai2 = cpai[p];
										if(cpcount == 3)
										{
											mentsu* tmentsu = new mentsu;
											MemoryLeakMonitor::addMonitor(tmentsu, sizeof(mentsu), "TEPAI2 MENTSU");
											tmentsu->next = NULL;
											tmentsu->prev = mentsutachi.tail;
											tmentsu->type = mentsu_SHUNZ;
											tmentsu->start = cpai[0];
											tmentsu->middle = cpai[i];
											tmentsu->last = cpai[p];
											(mentsutachi.tail)->next = tmentsu;
											mentsutachi.tail = tmentsu;
											tenpai_detect_recur_2(NULL, cpcount - 3);
											mentsutachi.tail = tmentsu->prev;
											(mentsutachi.tail)->next = NULL;
											MemoryLeakMonitor::removeMonitor(tmentsu);
											delete tmentsu;
										}
										else
										{
											pai* tepai3 = new pai[cpcount - 3];
											MemoryLeakMonitor::addMonitor(tepai3, (cpcount - 3)*sizeof(pai), "TEPAI3 2");
											pai* tepai2 = cpai;
											int tpos = 0;
											for (int j = 1; j < i; j++)
											{
												tepai3[tpos] = tepai2[j];
												tpos++;
											}
											for (int j = i + 1; j < p; j++)
											{
												tepai3[tpos] = tepai2[j];
												tpos++;
											}
											for (int j = p + 1; j < cpcount; j++)
											{
												tepai3[tpos] = tepai2[j];
												tpos++;
											}
											mentsu* tmentsu = new mentsu;
											MemoryLeakMonitor::addMonitor(tmentsu, sizeof(mentsu), "TENPAI RECUR 2 2");
											tmentsu->next = NULL;
											tmentsu->prev = mentsutachi.tail;
											tmentsu->type = mentsu_SHUNZ;
											tmentsu->start = cpai[0];
											tmentsu->middle = cpai[i];
											tmentsu->last = cpai[p];
											(mentsutachi.tail)->next = tmentsu;
											mentsutachi.tail = tmentsu;
											tenpai_detect_recur_2(tepai3, cpcount - 3);
											mentsutachi.tail = tmentsu->prev;
											(mentsutachi.tail)->next = NULL;
											MemoryLeakMonitor::removeMonitor(tmentsu);
											delete tmentsu;
											MemoryLeakMonitor::removeMonitor(tepai3);
											delete[] tepai3;
										}
									}
								}
							if (cpai[p].type != cpai[0].type)
								break;
						}
					}
				}
			if (cpai[i].type != cpai[0].type)
				return;
		}
	}
	void taj::tenpai_detect_recur(const pai* opai, int cpcount)
	{
		pure_chiitoitsu = false;
		chiitoitsu = false;
		kyuurenboudo = false;
		pure_kyuurenboudo = false;
		danyo = false;
		muuji = false;
		isou = false;
		laotou = false;
		jiisou = false;
		int count_m = 0, count_s = 0, count_p = 0, count_z = 0;
		bool janto_available = false;
		danyo = true;
		laotou = true;
		for (int i = 0; i<cpcount; i++)
		{
			switch (opai[i].type)
			{
			case 'M':
				count_m++;
				break;
			case 'S':
				count_s++;
				break;
			case 'P':
				count_p++;
				break;
			default:
				count_z++;
				break;
			}
			if (opai[i].fig == 1 || opai[i].fig == 9)
				danyo = false;
			else
				laotou = false;
		}
		mentsu* next = mentsutachi.first->next;
		while (next != NULL)
		{
			switch (next->start.type)
			{
			case 'M':
				count_m += 3;
				break;
			case 'S':
				count_s += 3;
				break;
			case 'P':
				count_p += 3;
				break;
			default:
				count_z += 3;
				break;
			}
			if (next->start.fig == 1 || next->start.fig == 9 || next->last.fig == 9)
				danyo = false;
			if (!(isKez(next) && (next->start.fig == 1 || next->start.fig == 9)))
				laotou = false;
			next = next->next;
		}
		jiisou = (count_m == 0 && count_s == 0 && count_p == 0);
		muuji = count_z == 0;
		isou = !jiisou && ((count_m == 0 && count_s == 0) || (count_p == 0 && count_s == 0) || (count_m == 0 && count_p == 0));
		if (count_m % 2 == 0 && count_s % 2 == 0 && count_p % 2 == 0 && count_z % 2 == 0)
		{
			pai* cpai2;
			cpai2 = new pai[cpcount];
			MemoryLeakMonitor::addMonitor(cpai2, cpcount * sizeof(pai), "CPAI2");
			for (int i = 0; i<cpcount; i++)
				cpai2[i] = opai[i];
			std::sort(cpai2, cpai2 + cpcount, paiSort);
			if (JudgeChiitoitsu(cpai2, &chiitoitsu))
				if (calculate_yaku)
				{
					pure_chiitoitsu = true;
					start();
					calculate();
					finish();
					pure_chiitoitsu = false;
				}
			MemoryLeakMonitor::removeMonitor(cpai2);
			delete[] cpai2;
		}
		//插入判定是否为九连宝灯形状，是则置flag
		//因为这里还没有排序，通过统计牌的张数来判定
		if (isou && cpcount == 14)
		{
			int num_cnt[10] = { 0,0,0,0,0,0,0,0,0,0 };
			for (int i = 0; i<cpcount - 1; i++)
				num_cnt[opai[i].fig]++;
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
				pure_kyuurenboudo = true;
				//cout<<"PURE _KYUU"<<endl;
			}

			else
			{
				num_cnt[tepai[tepos].fig]++;
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
						//cout<<"_KYUU"<<endl;
						break;
					}
					num_cnt[i]++;
				}
			}
		}
		//九连宝灯判定结束
		if (count_m % 3 == 2)
		{
			if (janto_available)
			{
				return;
			}
			else
			{
				count_m -= 2;
				janto_available = true;
			}
		}
		if (count_s % 3 == 2)
		{
			if (janto_available)
			{
				return;
			}
			else
			{
				count_s -= 2;
				janto_available = true;
			}
		}
		if (count_p % 3 == 2)
		{
			if (janto_available)
			{
				return;
			}
			else
			{
				count_p -= 2;
				janto_available = true;
			}
		}
		if ((count_m % 3 != 0) || (count_s % 3 != 0) || (count_p % 3 != 0))
			return;
		if (cpcount == 2)
		{
			if (compare_pai(opai[0], opai[1]))
			{
				janto_now[0] = opai[0];
				janto_now[1] = opai[1];
				output_tenpai(tepai[tepos]);
			}
			return;
		}
		pai lastvalid;
		lastvalid.fig = 0;
		lastvalid.type = 'Q';
		lastvalid.trait = 0;
		pai* cpai;
		cpai = new pai[cpcount];
		MemoryLeakMonitor::addMonitor(cpai, cpcount * sizeof(pai), "CPAI");
		for (int i = 0; i<cpcount; i++)
			cpai[i] = opai[i];
		std::sort(cpai, cpai + cpcount, paiSort);
		for (int i = 0; i<cpcount - 1; i++)
		{
			//output_pai(cpai[i]);
			//cout<<"current"<<endl;

			if (compare_pai(cpai[i], cpai[i + 1]))
				if (!compare_pai_thesame(cpai[i], lastvalid))
				{
					janto_now[0] = cpai[i];
					janto_now[1] = cpai[i + 1];
					lastvalid = cpai[i];
					pai* tpai2;
					tpai2 = new pai[cpcount - 2];
					MemoryLeakMonitor::addMonitor(tpai2, (cpcount - 2) * sizeof(pai), "TPAI2");
					for (int j = 0; j<i; j++)
						tpai2[j] = cpai[j];
					for (int j = i + 2; j<cpcount; j++)
						tpai2[j - 2] = cpai[j];
					tenpai_detect_recur_2(tpai2, cpcount - 2);
					MemoryLeakMonitor::removeMonitor(tpai2);
					delete[] tpai2;
				}
		}
		MemoryLeakMonitor::removeMonitor(cpai);
		delete[] cpai;
	}
	int taj::tenpai_detect_internal(judgeRequest* rpai, judgeResult* resultEx)
	{
		pai paiff;
		effeci_cnt = 0;
		hule = false;
		tenpai_mianshu = 0;
		memset(sresult, 0, sizeof(sresult));
		resx = resultEx;
		fulu_status = rpai->fulucnt != 0;
		akari_status = rpai->akari_status, norelease = rpai->norelease;
		riichi = bit(rpai->flags, 1), ihatsu = bit(rpai->flags, 4), wriichii = bit(rpai->flags, 2), haidei = bit(rpai->flags, 8), houdei = bit(rpai->flags, 16), rinnsyo = bit(rpai->flags, 32), tyankan = bit(rpai->flags, 64);
		jyouhuun = rpai->jyouhuun, jihuun = rpai->jihuun;
		int result;
		targetcalculate = calculate_yaku = rpai->mode == 1;
		tepai = rpai->pais;
		paicount = rpai->paicnt;
		memset(paicnt, 0, sizeof(paicnt));
		dora = rpai -> doracnt;

		for (int i = 0; i<paicount; i++)
			paicnt[retrieveID((rpai->pais)[i])]++;
		if (specialized_tenpai_detect(rpai, resultEx))
			return 0;

		//init mentsu
		mentsu* ta = new mentsu;
		MemoryLeakMonitor::addMonitor(ta, sizeof(mentsu), "TA 2");
		ta->type = 0;
		ta->prev = ta->next = NULL;
		mentsutachi.first = mentsutachi.tail = ta;

		for (int i = 0; i<rpai->fulucnt; i++)
		{
			mentsu* tmentsu = new mentsu;
			MemoryLeakMonitor::addMonitor(tmentsu, sizeof(mentsu), "Tmentsu 2");
			*tmentsu = (rpai->fulus)[i];
			tmentsu->prev = mentsutachi.tail;
			tmentsu->next = NULL;
			(mentsutachi.tail)->next = tmentsu;
			mentsutachi.tail = tmentsu;
		}

		tepos = (paicount++);
		hule = false;
		if (rpai->mode == 1)
		{
			tepai[tepos] = rpai->tgtpai;
			tepai[tepos].trait = TRAIT_AKARIPAI;
			reset();
			tenpai_detect_recur(tepai, paicount);
			result = 0;
			if (!show_result(resultEx) && calculate_yaku)
				if (hule)
				{
					result = -2; //形式听牌（无役）
				}
			if (!hule)
				result = -1; //未组成和牌牌型
			reset();
			mentsu* tmentsu = mentsutachi.tail, *tmentsu2;
			while (tmentsu != NULL)
			{
				tmentsu2 = tmentsu->prev;
				MemoryLeakMonitor::removeMonitor(tmentsu);
				delete tmentsu;
				tmentsu = tmentsu2;
			}
			return result;
		}
		resultEx->cnt = 0;
		tepai[tepos].trait = TRAIT_AKARIPAI;
		tepai[tepos].type = 'M';
		for (int i = 1; i <= 9; i++)
		{
			hule = false;
			tepai[tepos].fig = i;
			if (paicnt[retrieveID(tepai[tepos])] == 4) continue;
			//::reset();
			paiff = tepai[tepos];
			tenpai_detect_recur(tepai, paicount);
			//if(!::show_result() && calculate_yaku)
			//if (false)
				//(resultEx->t)[(resultEx->cnt)++] = paiff;
		}
		tepai[tepos].type = 'S';
		for (int i = 1; i <= 9; i++)
		{
			hule = false;
			tepai[tepos].fig = i;
			if (paicnt[retrieveID(tepai[tepos])] == 4) continue;
			//::reset();
			paiff = tepai[tepos];
			tenpai_detect_recur(tepai, paicount);
			//if(!::show_result() && calculate_yaku)
			//if (false)
				//(resultEx->t)[(resultEx->cnt)++] = paiff;
		}
		tepai[tepos].type = 'P';
		for (int i = 1; i <= 9; i++)
		{
			hule = false;
			tepai[tepos].fig = i;
			if (paicnt[retrieveID(tepai[tepos])] == 4) continue;
			//::reset();
			paiff = tepai[tepos];
			tenpai_detect_recur(tepai, paicount);
			//if(!::show_result() && calculate_yaku)
			//if(hule)
			//cout<<"形式听牌（无役）。"<<endl;
			//if (false)
				//(resultEx->t)[(resultEx->cnt)++] = paiff;
		}
		tepai[tepos].fig = 1;
		for (int i = 0; i<7; i++)
		{
			hule = false;
			tepai[tepos].type = funpai[i];
			if (paicnt[retrieveID(tepai[tepos])] == 4) continue;
			//::reset();
			paiff = tepai[tepos];
			tenpai_detect_recur(tepai, paicount);
			//if(!::show_result() && calculate_yaku)
			//if(hule)
			;//cout<<"形式听牌（无役）。"<<endl;
			//if (false)
				//(resultEx->t)[(resultEx->cnt)++] = paiff;
		}
		reset();
		if (tenpai_mianshu == 0)
			;
		mentsu* tmentsu = mentsutachi.tail, *tmentsu2;
		while (tmentsu != NULL)
		{
			tmentsu2 = tmentsu->prev;
			MemoryLeakMonitor::removeMonitor(tmentsu);
			delete tmentsu;
			tmentsu = tmentsu2;
		}
		return 1;
	}

	int taj::tenpaiDetect(const judgeRequest* rpai, judgeResult* resultEx)
	{
		return tenpai_detect(rpai, resultEx);
	}

	int taj::tenpai_detect(const judgeRequest* rpai, judgeResult* resultEx)
	{
		judgeRequest rpai2;
		rpai2.paicnt = rpai->paicnt;
		rpai2.fulucnt = rpai->fulucnt;
		for (int i = 0; i<4; i++)
			rpai2.fulus[i] = (rpai->fulus)[i];
		rpai2.mode = rpai->mode;
		rpai2.tgtpai = rpai->tgtpai;
		rpai2.akari_status = rpai->akari_status;
		rpai2.norelease = rpai->norelease;
		rpai2.jyouhuun = rpai->jyouhuun;
		rpai2.jihuun = rpai->jihuun;
		rpai2.flags = rpai->flags;
		rpai2.doracnt = rpai->doracnt;
		for (int i = 0; i<rpai->paicnt; i++)
			rpai2.pais[i] = rpai->pais[i];
		resultEx->yakucnt = 0;
		resultEx->yakutotal = 0;
		resultEx->basicpt = 0;
		resultEx->huutotal = 0;
		resultEx->cnt = 0;
		int result = tenpai_detect_internal(&rpai2, resultEx);
		return result;
	}

	bool JudgeChiitoitsu(const pai* ppai, bool* nostrict)
	{
		if (ppai[0] == ppai[1])
			if (ppai[2] == ppai[3])
				if (ppai[4] == ppai[5])
					if (ppai[6] == ppai[7])
						if (ppai[8] == ppai[9])
							if (ppai[10] == ppai[11])
								if (ppai[12] == ppai[13])
								{
									if (nostrict != NULL)
										*nostrict = true;
									if (ppai[0] != ppai[2])
										if (ppai[2] != ppai[4])
											if (ppai[4] != ppai[6])
												if (ppai[6] != ppai[8])
													if (ppai[8] != ppai[10])
														if (ppai[10] != ppai[12])
															return true;
								}
		return false;
	}

	int JudgeDaisyarin(const pai* ppai)
	{
		int se = ppai[0].type;
		if (se != 'M' && se != 'S' && se != 'P')
			return 0;
		int ncount[9], count_ni = 0;
		memset(ncount, 0, sizeof(int) * 9);
		(ncount[ppai[0].fig])++;
		for (int i = 1; i<14; i++)
		{
			if (se != ppai[i].type)
				return 0;
			if (ppai[i].fig == 1 || ppai[i].fig == 9)
				return 0;
			ncount[ppai[i].fig]++;
		}
		for (int i = 2; i <= 8; i++)
		{
			//std::cout<<i<<"M"<<ncount[i]<<std::endl;
			switch (ncount[i])
			{
			case 2:
				count_ni++;
				break;
			default:
				return 0;
			}
		}
		if (count_ni == 7)
			switch (se)
			{
			case 'M':
				return Daisuurin;
				break;
			case 'S':
				return Daichikurin;
				break;
			case 'P':
				return Daisyarin;
				break;
			}
		return 0;
	}

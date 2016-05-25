#include "stdafx.h"

#include "PublicFunction.h"
#include "MemoryLeakMonitor.h"
#include "YamaControl.h"

Yama::Yama()
{
}
	void Yama::initalize_internal_func(int paiCnt)
	{
		if (e1 != NULL)
		{
			delete e1;
			e1 = NULL;
		}
		if (yama != NULL)
			finalize();
		e1 = new std::default_random_engine((unsigned int)(std::chrono::system_clock::now().time_since_epoch().count()));
		kancount = 0;
		yama = new pai[paiCnt];
		//yama = (pai*)VirtualAlloc(NULL, sizeof(pai)*paiCnt, MEM_COMMIT, PAGE_READWRITE);
		yamacnt = paiCnt;
		MemoryLeakMonitor::addMonitor(yama, sizeof(pai)*paiCnt, "YAMA INITALIZE");
		cpos = 0;
	}
	

	pai* Yama::getYama()
	{
		return yama;
	}

	void Yama::finalize()
	{
		if (e1 != NULL)
		{
			delete e1;
			e1 = NULL;
		}
		if (yama != NULL)
		{
			MemoryLeakMonitor::removeMonitor(yama);
			delete[] yama;
			yama = NULL;
		}
	}

	void Yama::open_pai(int id)
	{
		yama[id].trait &= TRAIT_OPEN;
	}
	void Yama::init()
	{
		initalize_internal_func(136);
		for (int i = 0; i<136; i++)
			yama[i].trait = 0;
		int pos = 0;
		for (int i = 1; i <= 9; i++)
		{
			yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'M';
			yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = i;
			pos += 4;
		}
		for (int i = 1; i <= 9; i++)
		{
			yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'S';
			yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = i;
			pos += 4;
		}
		for (int i = 1; i <= 9; i++)
		{
			yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'P';
			yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = i;
			pos += 4;
		}

		yama[16].trait = TRAIT_AKA;
		yama[52].trait = TRAIT_AKA;
		yama[88].trait = TRAIT_AKA;

		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'D';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;
		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'N';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;
		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'X';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;
		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'B';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;
		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'W';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;
		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'F';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;
		yama[pos].type = yama[pos + 1].type = yama[pos + 2].type = yama[pos + 3].type = 'Z';
		yama[pos].fig = yama[pos + 1].fig = yama[pos + 2].fig = yama[pos + 3].fig = 1;
		pos += 4;

		std::random_shuffle(yama + 0, yama + yamacnt, [this](int i) {return (*e1)() % i;});

		open_pai(130); //翻第一张DORA指示牌
	}
	bool Yama::swap(int ID1, int ID2)
	{
		if (ID1 < cpos || ID1 >= yamacnt)
			return false;
		if (ID2 < cpos || ID2 >= yamacnt)
			return false;
		if ((yama[ID1].trait & TRAIT_OPEN) == TRAIT_OPEN)
			return false;
		if ((yama[ID2].trait & TRAIT_OPEN) == TRAIT_OPEN)
			return false;
		pai tmp_pai;
		tmp_pai = yama[ID2];
		yama[ID2] = yama[ID1];
		yama[ID1] = tmp_pai;
		return true;
	}
	bool Yama::next(pai* receive)
	{
		if (cpos >= yamacnt - 14)
			return false;
		*receive = yama[cpos++];
		return true;
	}
	int Yama::get_remaining()
	{
		return yamacnt - cpos;
	}
	int Yama::next_swapable_pai(int type, int fig)
	{
		for (int i = cpos; i<yamacnt; i++)
		{
			if (yama[i].type == type)
				if (yama[i].fig == fig)
					if ((yama[i].trait & TRAIT_OPEN) != TRAIT_OPEN)
						return i;
		}
		return -1;
	}

	bool Yama::kang(pai* receive, pai* kandora)
	{
		if (kancount == 4)
			return false;
		*receive = yama[--yamacnt];
		open_pai(kanpos[kancount++]);
		if (kandora != NULL)
			*kandora = yama[kanpos[kancount]];
		return true;
	}


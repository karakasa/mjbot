#include "stdafx.h"

#include "PublicFunction.h"

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
								if(nostrict != NULL)
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

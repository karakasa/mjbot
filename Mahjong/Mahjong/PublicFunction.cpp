#include "stdafx.h"
#include "PublicFunction.h"

bool operator == (const pai& a, const pai& b)
{
	return (a.type == b.type) && (a.fig == b.fig);
}

bool operator != (const pai& a, const pai& b)
{
	return (a.type != b.type) || (a.fig != b.fig);
}

bool compare_pai (const pai& a, const pai& b)
{
	return (a.type == b.type) && (a.fig == b.fig);
}

bool compare_pai_aka(const pai& a, const pai& b)
{
	return (a.type == b.type) && (a.fig == b.fig) && ((a.trait & TRAIT_AKA) == (b.trait & TRAIT_AKA));
}
bool compare_pai_same(const pai& a, const pai& b)
{
	return (a.type == b.type) && (a.fig == b.fig) && (a.trait == b.trait);
}
int retrieveID(const pai& cpai)
{
	switch (cpai.type)
	{
	case 'M':
		return cpai.fig - 1;
		break;
	case 'S':
		return cpai.fig + 8;
		break;
	case 'P':
		return cpai.fig + 17;
		break;
	case 'D':
		return 27;
		break;
	case 'N':
		return 28;
		break;
	case 'X':
		return 29;
		break;
	case 'B':
		return 30;
		break;
	case 'W':
		return 31;
		break;
	case 'F':
		return 32;
		break;
	case 'Z':
		return 33;
		break;
	}
	return -1;
}

int retrieveID2(const pai& cpai)
{
	if ((cpai.trait & TRAIT_AKA) == TRAIT_AKA)
	{
		switch (cpai.type)
		{
		case 'M':
			return 35;
			break;
		case 'S':
			return 36;
			break;
		case 'P':
			return 37;
			break;
		default:
			return -1;
		}
	}
	return retrieveID(cpai) + 1;
}

int retrieveID3(const pai& cpai)
{
	return retrieveID(cpai) + 1;
}

bool retrievePai3(pai* pai, int id)
{
	switch (id)
	{
	case 28:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'D';
		break;
	case 29:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'N';
		break;
	case 30:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'X';
		break;
	case 31:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'B';
		break;
	case 32:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'W';
		break;
	case 33:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'F';
		break;
	case 34:
		pai->trait = 0;
		pai->fig = 1;
		pai->type = 'Z';
		break;
	case 35:
		pai->trait = TRAIT_AKA;
		pai->fig = 5;
		pai->type = 'M';
		break;
	case 36:
		pai->trait = TRAIT_AKA;
		pai->fig = 5;
		pai->type = 'S';
		break;
	case 37:
		pai->trait = TRAIT_AKA;
		pai->fig = 5;
		pai->type = 'P';
		break;
	default:
		if (id >= 1 && id <= 9)
		{
			pai->trait = 0;
			pai->type = 'M';
			pai->fig = id;
		}
		else if (id >= 10 && id <= 18)
		{
			pai->trait = 0;
			pai->type = 'S';
			pai->fig = id - 9;
		}
		else if (id >= 19 && id <= 27)
		{
			pai->trait = 0;
			pai->type = 'P';
			pai->fig = id - 18;
		}
		else
			return false;
	}
	return true;
}

//bool retrievePai(pai* pai, int id)
//{
//	return retrievePai(pai, id + 1);
//}

void doraNext(const pai& show, pai* result)
{
	result->trait = 0;
	switch (show.type)
	{
	case 'M':
	case 'S':
	case 'P':
		result->type = show.type;
		if (show.fig == 9)
			result->fig = 1;
		else
			result->fig = (show.fig) + 1;
		break;
	case 'D':
		result->fig = 1;
		result->type = 'N';
		break;
	case 'N':
		result->fig = 1;
		result->type = 'X';
		break;
	case 'X':
		result->fig = 1;
		result->type = 'B';
		break;
	case 'B':
		result->fig = 1;
		result->type = 'D';
		break;
	case 'W':
		result->fig = 1;
		result->type = 'F';
		break;
	case 'F':
		result->fig = 1;
		result->type = 'Z';
		break;
	case 'Z':
		result->fig = 1;
		result->type = 'W';
		break;
	}
}

bool paiSort(const pai& a, const pai& b)
{
	return (a.type == b.type) ? (a.fig<b.fig) : (paiorder[a.type - 'A']<paiorder[b.type - 'A']);
}

bool isKez(const mentsu* mc)
{
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
		return true;
	default:
		break;
	}
	return false;
}

bool isKangz(const mentsu* mc)
{
	switch (mc->type)
	{
	case mentsu_KEZ_KANG_A:
	case mentsu_KEZ_KANG_B:
	case mentsu_KEZ_KANG_C:
	case mentsu_KEZ_KANG_S:
		return true;
	default:
		break;
	}
	return false;
}

int getRelativePosition(int self, int other) //0下家 1对家 2上家 -1错误
{
	switch (self)
	{
	case 0:
		return other - 1;
	case 1:
		if (other == 0)
			return 3;
		else return other - 2;
	case 2:
		if (other == 3)
			return 0;
		else return other + 2;
	case 3:
		return other;
	}
	return -1;
}
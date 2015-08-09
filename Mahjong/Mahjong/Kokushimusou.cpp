#include "stdafx.h"
#include "PublicFunction.h"

int get_yaotyuu_id(const pai& wpai)
{
	switch (wpai.type)
	{
	case 'M':
		switch (wpai.fig)
		{
		case 1:
			return 0;
		case 9:
			return 1;
		default:
			return 13;
		}
	case 'S':
		switch (wpai.fig)
		{
		case 1:
			return 2;
		case 9:
			return 3;
		default:
			return 13;
		}
	case 'P':
		switch (wpai.fig)
		{
		case 1:
			return 4;
		case 9:
			return 5;
		default:
			return 13;
		}
	case 'D':
		return 6;
	case 'N':
		return 7;
	case 'X':
		return 8;
	case 'B':
		return 9;
	case 'W':
		return 10;
	case 'F':
		return 11;
	case 'Z':
		return 12;
	}
	return 13;
}

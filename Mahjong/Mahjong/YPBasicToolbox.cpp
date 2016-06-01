#include "stdafx.h"
#include "YPBasicToolbox.h"

namespace utility
{
	YakuBasicC _yb1;
	YakuBasicCYM _yb2;
	YakuBasicM _yb3;
	YakuBasicMYM _yb4;
	YakuBasicDora _yb5;
}

bool utility::prepareSystemYakuProvider()
{
	if (!(taj::systemProvider.unregsiterAll()))
		return false;
	taj::systemProvider.registerProvider(&_yb1);
	taj::systemProvider.registerProvider(&_yb2);
	taj::systemProvider.registerProvider(&_yb3);
	taj::systemProvider.registerProvider(&_yb4);
	taj::systemProvider.registerProvider(&_yb5);
	taj::systemProvider.rebuildProviders();
	return true;
}

void utility::setDora(int dora)
{
	_yb5.doraCnt = dora;
}
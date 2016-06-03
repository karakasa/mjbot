#include "stdafx.h"
#include "YPBasicToolbox.h"

namespace utility
{
	YakuBasicC _yb1;
	YakuBasicCYM _yb2;
	YakuBasicM _yb3;
	YakuBasicMYM _yb4;
	YakuBasicDora _yb5;
	YakuBasicShape _yb6;
	YakuBasicShapeYM _yb7;
	SysPtProvider _yb8;
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
	taj::systemProvider.registerProvider(&_yb6);
	taj::systemProvider.registerProvider(&_yb7);
	taj::systemProvider.rebuildProviders();
	taj::systemProvider.ptProvider = &_yb8;
	return true;
}
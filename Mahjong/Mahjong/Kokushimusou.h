#pragma once

// 获得幺九牌的 ID，用于听牌、役、向听判定中。

#include "PublicFunction.h"

#define KOKUSHIMUSOU 1
#define KOKUSHIMUSOU_JUUSANMENMACHI 2


// 获得幺九ID
// pai : 牌
// 返回值 : ID，19M19S19P东南西北白发中分别是 0-12，其他均为 13
int get_yaotyuu_id(const pai& wpai);
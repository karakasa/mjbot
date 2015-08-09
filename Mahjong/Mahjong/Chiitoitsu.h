#pragma once

// 七对子相关判定，包括大竹林、大数邻和大车轮判定

#include "PublicFunction.h"


// 判断七对子。
// ppai : 长度为 14 的排序后的 pai 数组
// nostrict : 可选，接收参数。不管是不是龙七对，只要满足有 7 组一样的两张牌，本参数即为 true。
// 返回值 : 是或否。
bool JudgeChiitoitsu(const pai* ppai, bool* nostrict = NULL);


// 判断特殊役满。
// ppai : 长度为 14 的排序后的 pai 数组
// 返回值 : 0 为均不是，否则为本文件头部定义的宏。
int JudgeDaisyarin(const pai* ppai);
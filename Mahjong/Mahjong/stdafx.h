// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#if defined(_MSC_VER)
#   if _MSC_VER >= 1900 
#       define C11
#   endif
#elif __cplusplus > 199711L
#   define C11
#endif

#include "targetver.h"
#include "Winsock2.h"

#include <stdio.h>
#include <tchar.h>
#include <cstring>

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <map>
#include <random>
#include <chrono>
#include <queue>

// TODO:  在此处引用程序需要的其他头文件

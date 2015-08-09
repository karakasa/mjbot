#pragma once

// 主要定义了一些公用过程与数据类型。

#define RON false
#define TSUMO true
#define is_akari(x) (((x).trait&TRAIT_AKARIPAI)==TRAIT_AKARIPAI)
#define is_aka(x) (((x).trait&TRAIT_AKA)==TRAIT_AKA)
#define compare_pai_thesame compare_pai_same


#define Daisyarin 1
#define Daichikurin 2
#define Daisuurin 3


const char yakuname[64][20] = { "立直","一发","门前自摸","断幺","平和","一杯口","役牌/白","役牌/发","役牌/中","役牌/场风东","役牌/场风南",
"役牌/场风西","役牌/场风北","役牌/自风东","役牌/自风南","役牌/自风西","役牌/自风北","役牌/连风东",
"役牌/连风南","役牌/连风西","役牌/连风北","岭上","抢杠","海底","河底","三色同顺","一气","混全带幺九",
"七对子","对对","三暗","三杠子","三色同刻","混老头","小三元","W立直","混一","纯全带幺九","二杯口",
"清一","国士","四暗刻","大三元","小四喜","字一色","绿一色","清老头","九连","四杠子","天和","地和",
"人和","国士13面","大四喜","四暗单骑","大车轮","场风(未定义)","自风(未定义)","连风(未定义)","纯九连",
"宝牌","北宝牌","未定义役" };
//具体排列参考 役种id.txt

//paiorder 定义了在处理手牌时排序的顺序，本身应该是一个26成员的数组，
//每个字母，如M、S、P对应一个次序，不硬编码的主要原因是可以让用户方便的进行重定义
//数组中为0的成员都是无意义的
const int paiorder[30] = { 0,7,0,4,0,9,0,0,0,0,0,0,1,5,0,3,0,0,2,0,0,0,8,6,0,10,0,0,0,0 };
//                   B   D   F             M N   P     S       W X   Z

const int funpai[7] = { 'D','N','X','B','Z','W','F' };

// 牌的特征
#define TRAIT_AKA 1
#define TRAIT_AKARIPAI 2
#define TRAIT_OPEN 4

//牌数据结构。type 为颜色，分'M','S','P','D','N','X','B','W','F','Z'，这几种；fig 为数字；trait 为特征（包括赤等）
struct pai {
	unsigned char type;
	unsigned char fig;
	int trait;
};

//面子数据结构，包括一个双向链表；在大多数场合下，调用者不需要维护链表结构，链表指针是在内部起到提高效率的作用的。
struct mentsu {
	char type;
	pai start;
	pai middle;
	pai last;
	mentsu* prev;
	mentsu* next;
};

//面子表，内部数据结构。
struct mentsutable {
	mentsu* first;
	mentsu* tail;
};

//面子类型
enum mentsu_TYPE {
	mentsu_SHUNZ, //顺子面子
	mentsu_KEZ, //刻子面子
	mentsu_SHUNZ_CHI_A, //顺子面子，吃上家
	mentsu_SHUNZ_CHI_B, //保留
	mentsu_SHUNZ_CHI_C, //保留
	mentsu_KEZ_PON_A, //刻子面子，碰上家
	mentsu_KEZ_PON_B, //刻子面子，碰对家
	mentsu_KEZ_PON_C, //刻子面子，碰下家
	mentsu_KEZ_KANG_A, //杠子面子，杠上家
	mentsu_KEZ_KANG_B, //杠子面子，杠对家
	mentsu_KEZ_KANG_C, //杠子面子，杠下家
	mentsu_KEZ_KANG_S, //杠子面子，暗杠
};

//役数据结构，内部数据类型
struct yaku {
	int yakuid;
	int pt;
	yaku* prev;
	yaku* next;
};

//役表数据结构，内部数据类型
struct yaku_table {
	yaku* first;
	yaku* tail;
	int yakutotal;
	int huutotal;
	int basicpt;
};

//传递给 TenpaiAkariJudge 的主要请求
struct judgeRequest
{
	int paicnt; //手牌数量，必须为 3n+1
	pai pais[13]; //手牌，必须是从小到大有序的
	int fulucnt; //副露数量 0 or 1 or 2 or 3 or 4
	mentsu fulus[4]; //副露 mentsu[0~fulucnt] 该结构中的 prev / next 值没有意义
	int mode; //检测模式 0为听牌种类检测 1为和了役检测，为0时，后面的参数无意义
	pai tgtpai; //和了役检测时 和了的那张牌
	bool akari_status; //胡了种类 自摸/荣和  TSUMO or RON
	bool norelease; //天地人和可以成立（这人还没打过牌，之前无鸣牌）
	char jyouhuun; //场风 'D' or 'N' or 'X' or 'B'，设置为 '\0' 即不判断
	char jihuun; //自风 'D' or 'N' or 'X' or 'B'，设置为 '\0' 即不判断
	int flags; //一组逻辑值，flags&1 立直 flags&2 W立直 flags&4 一发 flags&8 海底 flags&16 河底 flags&32 岭上 flags&64 抢杠
	int doracnt; //几张DORA
};

//TenpaiAkariJudge 模块传递回的结构
struct judgeResult
{
	int cnt; //听牌种类判断模式下，为听的牌的种类数量
	pai t[20]; //pai[cnt]，听的牌
	int yakucnt; //和了役判断吗模式下，役的数量
	int yakutotal; //总翻数
	int huutotal; //符数
	int basicpt; //基本点
	int yakuid[16]; //役的ID
	int pt[16]; //对应役的翻数
};

// 函数定义

// 比较两张牌相同/不同
// 本比较只比较花色和数字，不管是否为赤
bool operator == (const pai& a, const pai& b);
bool operator != (const pai& a, const pai& b);
bool compare_pai (const pai& a, const pai& b);

// 比较两张牌相同/不同 (含赤)
// 本比较比较花色和数字，并比较赤的情况是否相同
bool compare_pai_aka(const pai& a, const pai& b);

// 比较两张牌相同/不同 (全同)
// 本比较要求两张牌全等，除花色和数字外，trait 也要一样
bool compare_pai_same(const pai& a, const pai& b);

// 获得牌的序号。1M-9M：0-8，1S-9S：9-17，1P-9P：18-26，之后依次为东南西北白发中：27-33。
// 本函数不考虑是否为赤宝牌。
// 本函数主要是 TenpaiAkariJudge 模块内部实用。外部使用（网络、界面）请尽量使用 retrieveID3
// cpai : 牌
// 返回值 : 牌的序号
int retrieveID(const pai& cpai);

// 获得牌的序号。等于 retrieveID + 1，即比上面一个函数大一。
// 本函数不考虑是否为赤宝牌。
// 本函数主要用于外部过程，如界面、网络等（因为 0 在外部过程中可能有别的意义）
// cpai : 牌
// 返回值 : 牌的序号
int retrieveID3(const pai& cpai);

// 获得牌的序号。
// 本函数考虑是否为赤宝牌，是对 retrieveID3 函数的扩展，retrieveID3 的范围为 1-33，
// 在本函数中，赤5M、5S、5P分别为 34, 35, 36
// cpai : 牌
// 返回值 : 牌的序号
int retrieveID2(const pai& cpai);

//bool retrievePai(pai* pai, int id);

// 通过序号获得牌。此序号的意义和 retrieveID2 函数返回的相同，范围为 1-36
// pai : 接收的返回值
// id : 需要转换的 id
// 返回值 : 输入的 id 是否有效
bool retrievePai3(pai* pai, int id);

// 取得宝牌，当指示牌无效时，结果未定义
// show : 宝牌指示牌
// result : 指向结果的指针
// 返回值 : 无
void doraNext(const pai& show, pai* result);

// 对两张牌排序（小于等于号），用于 sort (STL)；排序的原则是，同色从小到大；不同色按 paiorder 数组的值指定。
// 实际上直接针对牌 ID 排序也是可以的，也就是 return retrieveID(a)<retrieveID(b); 这里采用如下比较方法是处于效率的考量
// a : 牌 1
// b : 牌 2
// 返回值 : 小于等于为真，大于为假
bool paiSort(const pai& a, const pai& b);

// 判断一个面子是不是刻子（包括杠）
// mc : 面子的指针
// 返回值 : 真或假
bool isKez(const mentsu* mc);

// 判断一个面子是不是杠（包括暗杠）
// mc : 面子的指针
// 返回值 : 真或假
bool isKangz(const mentsu* mc);

// 获得其他家的相对位置，输入参数为逆时针 0-4
// self : 自家位置
// other : 他家位置
// 返回值 : 0下家 1对家 2上家 -1错误 
int getRelativePosition(int self, int other);
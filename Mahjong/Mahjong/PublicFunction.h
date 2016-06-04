
#pragma once

// 主要定义了一些公用过程与数据类型。

#define MAX_MENTSU 4

#define RON false
#define TSUMO true
#define is_akari(x) (((x).trait&TRAIT_AKARIPAI)==TRAIT_AKARIPAI)
#define is_aka(x) (((x).trait&TRAIT_AKA)==TRAIT_AKA)
#define compare_pai_thesame comparePaiSame

#define _char char

#define Daisyarin 1
#define Daichikurin 2
#define Daisuurin 3

#define KOKUSHIMUSOU 1
#define KOKUSHIMUSOU_JUUSANMENMACHI 2

#define isSanyuan(x) (((x).type == 'W') || ((x).type == 'F') || ((x).type == 'Z'))
#define isFeng(x) (((x).type == 'D') || ((x).type == 'N') || ((x).type == 'X') || ((x).type == 'B'))
#define isSameType(a, b) ((isShunz2((a)) && isShunz2((b))) || (isKez2((a)) && isKez2((b))))
#define makeReadonly(x) (x) // not working now

#define resetYakuTable2(x) {(x).yakus.clear(); (x).yakutotal = (x).huutotal = (x).basicpt = 0;}

const int yaotrans[13] = { 1,9,10,18,19,27,28,29,30,31,32,33,34 };

//paiorder 定义了在处理手牌时排序的顺序，本身应该是一个26成员的数组，
//每个字母，如M、S、P对应一个次序，不硬编码的主要原因是可以让用户方便的进行重定义
//数组中为0的成员都是无意义的
const int paiorder[30] = { 0,7,0,4,0,9,0,0,0,0,0,0,1,5,0,3,0,0,2,0,0,0,8,6,0,10,0,0,0,0 };
//                   B   D   F             M N   P     S       W X   Z

const int funpai[7] = { 'D','N','X','B','W','F','Z' };

// 牌的特征
#define TRAIT_AKA 1
#define TRAIT_AKARIPAI 2
#define TRAIT_OPEN 4


// 接口虚类定义
// EventBus 用户的接口，如 UI、AI、网络等，对于非即时返回的而言，*hasReturn 总为 false。
class EventBusUser {
public:
	virtual int aiMessage(unsigned char msgType, int par1, int par2, bool* hasReturn, void* payload) = 0;
};

// EventBus 比赛的接口，一般只用于内部调用。
class MatchingUser {
public:
	virtual void receiveEvent(int clientId, unsigned int response) = 0;
};

//牌数据结构。type 为颜色，分'M','S','P','D','N','X','B','W','F','Z'，这几种；fig 为数字；trait 为特征（包括赤等）
struct pai {
	unsigned char type;
	unsigned char fig;
	int trait;
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

//面子数据结构，包括一个双向链表；在大多数场合下，调用者不需要维护链表结构，链表指针是在内部起到提高效率的作用的。
struct mentsu {
	char type; //面子类型，为 mentsu_TYPE 枚举类型
	pai start; //刻子的开始一张牌
	pai middle; //刻子的中间一张牌
	pai last; //刻子的最后一张牌
};

//役数据结构，内部数据类型
struct yaku2 {
	int yakuid;
	int yakusubid;
	int pt;
};

//役表数据结构，内部数据类型
struct yakuTable {
	std::vector<yaku2> yakus;
	int yakutotal = 0;
	int huutotal = 0;
	int basicpt = 0;
};

//简化的请求，用于描述玩家状态
struct judgeRequestSimple
{
	bool akariStatus; //胡了种类 自摸/荣和  TSUMO or RON
	bool norelease = false; //天地人和可以成立（这人还没打过牌，之前无鸣牌）
	char jyouhuun = '\0'; //场风 'D' or 'N' or 'X' or 'B'，设置为 '\0' 即不判断
	char jihuun = '\0'; //自风 'D' or 'N' or 'X' or 'B'，设置为 '\0' 即不判断
	int flags = 0; //一组逻辑值，flags&1 立直 flags&2 W立直 flags&4 一发 flags&8 海底 flags&16 河底 flags&32 岭上 flags&64 抢杠
	int doracnt = 0; //几张DORA
};

//传递给 TenpaiAkariJudge 的主要请求
struct judgeRequest : public judgeRequestSimple
{
	int paicnt; //手牌数量，必须为 3n+1
	pai pais[13]; //手牌，在判定役时，应是由 paiSort 排序过的有序数组
	int fulucnt = 0; //副露数量 0 or 1 or 2 or 3 or 4
	mentsu fulus[4]; //副露 mentsu[0~fulucnt] 该结构中的 prev / next 值没有意义
	pai tgtpai; //和了役检测时 和了的那张牌
};

//网络端数据包定义
struct Packet
{
	unsigned char packetLength; //包长度, 包括该字节, 为0xFF 时为心跳包, 忽略之后所有数据；0xFE 时为控制包, 详情付
	unsigned int crc; //校验和, 把该成员之后的每一个字节从(unsigned int)0开始相加
	unsigned int id; //包id, 以服务器发送的握手包开始（1）, 客户端和服务器循环加一, 单个客户端会话中保持递增序列
	unsigned char uType; //消息类型, 参interface
	unsigned int p1; //参数1, 参interface
	unsigned int p2; //参数2, 参interface
	unsigned char payloadLength; //额外数据长度（仅有几个事件需要）, 大多数时候为0, 之后为负载内容
};

//网络端控制包定义
struct ControlPacket
{
	unsigned char reserved; //0xFF
	unsigned char packetLength; //真正的包长度, 不包括上面一个字节, 包括下面一个字节
	unsigned int crc; //校验和, 把该成员之后的每一个字节从(unsigned int)0开始相加
	unsigned char type; //控制包类型, 0 = 重发数据包, 1 = 状态通告
	unsigned int param1; //type = 1时, 为需要重发的数据包的id；type = 2时, p1 = 1 无数据包可重发, p1 = 2, 意外错误、请求重新同步状态
};

namespace ai
{

	//事件枚举类型
	enum
	{
		init = 0,
		deinit,
		start,
		initalizeTehai,
		newDora,
		newUra,
		othersTurn,
		othersTurnFinished,
		yourTurn,
		yourTurnSpecial,
		minDone,
		minChance,
		finish,
		finishNaga,
		finishNagaSpecial,
		scoreChange,
		finishAbnormal,
		communicationAbnormal,
		negotiate,
		sessionInitialize,
		sessionCreated,
		sessionRestoration, //未实现
		wait,
		gameFinish,
		othersTepai
	};

	//鸣牌枚举类型
	namespace min {
		enum
		{
			chi = 1, //吃
			pon, //碰
			kang, //大明杠
			kangj, //加杠
			kangs, //暗杠
			riichi //立直
		};
	}

	//Payload : 和牌
	struct AKARI
	{
		unsigned int from; //哪家点的，参考 ai::start 中的相对位置
		unsigned int to; //哪家和的，参考 ai::start 中的相对位置
		unsigned char pai; //和的牌id
						   /*from==to 时为自摸*/
		int huu; //符数
		int fan; //番数
		int pt; //和了点数（自摸时为基本点）
		int reserved;
		int yakucnt;
		int yaku_id[16]; //和了役id，具体id参考mahjong目录内的 役种id.txt
		int yaku_subid[16];
		int yaku_fan[16]; //对应的役的翻数
									/*上述两个成员均为数组类型，下标为0到fan-1*/
	};

	//Payload : 当前状态
	struct CURRENT //未实现
	{
		/*unsigned char pos; //Client 的位置
		unsigned char base; //0~7 东1~4 南1~4 以此类推
		unsigned char richii_bang; */
	};

	//Payload : 流局状态
	struct TENPAI
	{
		unsigned char p[4]; // 0未听牌，此时对应的tepai(cnt)无意义 1听牌 2流满
		int tepaicnt[4];
		pai tepai0[13];
		pai tepai1[13];
		pai tepai2[13];
		pai tepai3[13];
	};
}

// EventBus 请求
struct ebRequest
{
	int id;
	unsigned char msgType;
	unsigned int par1;
	unsigned int par2;
	void* payload;
	int lpayload;
};

// EventBus 客户
struct client
{
	int clientType;
	EventBusUser* clientHandle;
};

// EventBus 事件
struct ebEvent
{
	int id;
	unsigned int response;
};

namespace ct {
	enum
	{
		undefined = 0,
		local_ai,
		local_user,
		local_extended,
		remote
	};
}

const char ji[4] = { 'D','N','X','B' };

// 函数定义

// 比较两张牌相同/不同
// 本比较只比较花色和数字，不管是否为赤
bool operator == (const pai& a, const pai& b);
bool operator != (const pai& a, const pai& b);
bool compare_pai (const pai& a, const pai& b);
bool operator<   (const pai& a, const pai& b);

bool operator == (const mentsu& a, const mentsu& b);
bool operator<   (const mentsu& a, const mentsu& b);

// 比较两张牌相同/不同 (含赤)
// 本比较比较花色和数字，并比较赤的情况是否相同
bool comparePaiAka(const pai& a, const pai& b);

// 比较两张牌相同/不同 (全同)
// 本比较要求两张牌全等，除花色和数字外，trait 也要一样
bool comparePaiSame(const pai& a, const pai& b);
bool compareMentsuSame(const mentsu& a, const mentsu& b);

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

bool isShunz2(const mentsu& mc);

// 判断一个面子是不是刻子（包括杠）
// mc : 面子的指针
// 返回值 : 真或假
bool isKez(const mentsu* mc);
bool isKez2(const mentsu& mc);

// 判断一个面子是不是杠（包括暗杠）
// mc : 面子的指针
// 返回值 : 真或假
bool isKangz(const mentsu* mc);
bool isKangz2(const mentsu& mc);

// 获得其他家的相对位置，输入参数为逆时针 0-4
// self : 自家位置
// other : 他家位置
// 返回值 : 0下家 1对家 2上家 -1错误 
int getRelativePosition(int self, int other);

// 获得幺九ID
// pai : 牌
// 返回值 : ID，19M19S19P东南西北白发中分别是 0-12，其他均为 13
int getYaotyuuId(const pai& wpai);

// 判断是否为幺九，(isYaotyuu 2，不含字牌)
// pai : 牌
// 返回值 : 真或假
bool isYaotyuu(const pai& pai);
bool isYaotyuu2(const pai& pai);

// 判断是否为字
// pai : 牌
// 返回值 : 真或假
bool isJi(const pai& pai);

// 获得面子的类型，可以是无序的
// a,b,c : 牌的 ID，与 retrieveID3 相同
// 返回值 : 1为刻子，2为顺子，-1为不是面子
int getMentsuType(int a, int b, int c);

// 将天凤手牌字符串转换为牌数组。保持输入顺序。
// pstring : 手牌，字母必须为小写字母。1-9m 1-9s 1-9p 1-7z 0m0s0p
// parr : 接收的数组
// *buffersize : 接收数组的最大长度，本参数也会用来接收错误信息。
// 返回值 : 成功与否。如果返回值为否，有这样几种情况。
// buffersize 为 0 : 缓冲区长度不足，-1 / -2 : 格式出错
bool convertPaiString(std::string& pstring, pai* parr, int* buffersize);

// 统计一个天凤手牌字符串中的牌的数量，为 convertPaiString 函数做准备
// pstring : 手牌，字母必须为小写字母。1-9m 1-9s 1-9p 1-7z 0m0s0p
// 返回值 : 数量。如果为负数，则为失败。对于空字符串返回 0。
int convertPaiStringPrepare(std::string& pstring);

// 返回一个面子是否为手牌内的面子（非副露）
bool isMenzenMentsu(const mentsu& mentsuJudgable);

namespace std {

	template <>
	struct hash<pai>
	{
		std::size_t operator()(const pai& k) const;
	};

	inline std::size_t hash<pai>::operator()(const pai & k) const
	{
		return std::size_t(retrieveID2(k));
	}
}
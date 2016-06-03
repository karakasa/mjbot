#pragma once
#include "PublicFunction.h"

// 役的特性，描述见下图
// 相对的特殊情况：
// (1) 当起和役包含 yakumanLike 时，仅 doraLike 的役不会出现，而具有 doraLike & yakumanLike 的会出现
// (2) doraLike & yakumanLike 的役并不会抑制非 yakumanLike 的其他役
enum yakuTrait
{
	doraLike = 1, // 需要其他起胡役
	yakumanLike = 2, // 是最高级别的役，会无视其他非 yakumanLike 役
	shapeYaku = 4,

	default = 0
};

enum yakuType
{
	mentsuLike = 0,
	colorLike = 1
};

// 役判断的基本接口
// 实际使用中可以使用 YAKU_DEFINE(2), YAKU_NAME, YAKU_SUBNAME_* 方便的进行定义
class YakuProviderBase
{
public:
	// 取得一个役的名字
	// buffer : 名字缓冲区
	// bufferSize : 名字缓冲区的尺寸，包括 null-terminator
	// outBufferSize : 返回尺寸。
	// subid : 子役 ID
	// 返回值 : bool，是否成功写入了缓冲区
	// 特殊情况 : 当返回值为 true 而 *outBufferSize 为 0 时，则为*隐藏*的役
	virtual bool queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) {
		if (outBufferSize != NULL)
		{
			*outBufferSize = 0;
		}
		else {
			if (bufferSize >= sizeof(_char))
				buffer[0] = '\0';
			else
				return false;
		}
		return true;
	}

	// 设置役的特性，出于性能考虑，该函数可能只会在 register 时调用一次
	// 为 enum yakuTrait 的或值
	virtual int getTraits()
	{
		return yakuTrait::default;
	}

	// 获得役的种类
	// 返回值 : 0 为面子类（继承自 YakuProviderM），1 为颜色类（继承自 YakuProviderC）
	virtual yakuType getType() = 0;
};

#define MAX_SUBYAKU 16
#define WYAKUMAN false

// 基本点的判断接口（包含符的计算）
class BasicPtProviderBase
{
public:
	virtual bool judgePt(const pai* pais, int paicnt, const mentsu* mentsus, int mentsucnt, const pai* janto, int jantocnt, yakuTable* yTable, void* yakuProvider) = 0;
};

// 颜色类役的判断接口
class YakuProviderC : public YakuProviderBase
{
public:
	// 判断一个役的存在
	// pais : 手牌数组，该数组保证是有序的
	// paicnt : 手牌数量
	// fulu : 副露数组
	// fulucnt : 副露数量
	// 返回值 : bool，有没有该翻
	virtual bool judgeYaku(const pai* pais, int paicnt, bool menzen, void* yakuProvider) = 0;
};

// 面子类役的判断接口
class YakuProviderM : public YakuProviderBase
{
public:
	// 判断一个役的存在
	// mentsu : 面子数组（包括手牌和副露），保证副露和手牌在数组中是分开的。
	// mentsucnt : 面子数量
	// janto : 雀头数组
	// jantocnt : 雀头数量（一般为 2）
	// 返回值 : bool，有没有该翻
	virtual bool judgeYaku(const mentsu* mentsu, int mentsucnt, const pai* janto, int jantocnt, void* yakuProvider) = 0;
};

#define YAKU_CLASS_M(classname) class classname : public YakuProviderM \
                                { \
                                public: \
                                	bool queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) override; \
                                	int getTraits() override; \
                                	yakuType getType() override; \
                                	bool judgeYaku(const mentsu* mentsu, int mentsucnt, const pai* janto, int jantocnt, void* yakuProvider) override; \
                                };

#define YAKU_CLASS_C(classname) class classname : public YakuProviderC \
                                { \
                                public: \
                                	bool queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) override; \
                                	int getTraits() override; \
                                	yakuType getType() override; \
                                	bool judgeYaku(const pai* pais, int paicnt, bool menzen, void* yakuProvider) override; \
                                };

#define YAKU_CLASS_M_DEF(classname) class classname : public YakuProviderM \
                                { \
                                public: \
                                	bool queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) override; \
                                	int getTraits() override; \
                                	yakuType getType() override; \
                                	bool judgeYaku(const mentsu* mentsu, int mentsucnt, const pai* janto, int jantocnt, void* yakuProvider) override;

#define YAKU_CLASS_C_DEF(classname) class classname : public YakuProviderC \
                                { \
                                public: \
                                	bool queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) override; \
                                	int getTraits() override; \
                                	yakuType getType() override; \
                                	bool judgeYaku(const pai* pais, int paicnt, bool menzen, void* yakuProvider) override;

#define YAKU_CLASS_DEF_END() };

#define YAKU_DEFINE(classname, type, traits)       int classname::getTraits() \
												   {  \
													   return (traits); \
												   } \
												   yakuType classname::getType() \
												   { \
													   return (type); \
												   }

#define YAKU_NAME(classname, name) bool classname::queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) \
                                   { \
	                                   if (outBufferSize != NULL) \
		                                   *outBufferSize = sizeof((name)) / sizeof((name)[0]); \
	                                   if (sizeof((name)) / sizeof((name)[0]) > bufferSize) \
		                                   return false; \
	                                   memcpy_s(buffer, bufferSize, (name), sizeof((name))); \
	                                   return true; \
                                   }

#define YAKU_SUBNAME_BEGIN(classname) bool classname::queryName(_char* buffer, const int bufferSize, int* outBufferSize, int subid) \
                                      { \
                                          switch(subid) {
#define YAKU_SUBNAME(subid, name)                  case (subid): \
                                                   if (outBufferSize != NULL) \
                                                       *outBufferSize = sizeof((name)) / sizeof((name)[0]); \
                                                   if (sizeof((name)) / sizeof((name)[0]) > bufferSize) \
                                                       return false; \
                                                   memcpy_s(buffer, bufferSize, (name), sizeof((name))); \
                                                   return true; \
                                                   break;

#define YAKU_SUBNAME_END() } return false; }

#define YAKU_JUDGE_BEGIN_M(className) bool className::judgeYaku(const mentsu* mentsus, int mentsucnt, const pai* janto, int jantocnt, void* yakuProvider) { \
                                          const int __yakuType = 0; bool __retVal = false; \
                                          const bool __menzen = (std::all_of(mentsus + 0, mentsus + MAX_MENTSU, isMenzenMentsu)); 
#define YAKU_JUDGE_BEGIN_C(className) bool className::judgeYaku(const pai* pais, int paicnt, bool menzen, void* yakuProvider) { \
                                          const int __yakuType = 1; bool __retVal = false; \
                                          const bool __menzen = menzen;
#define YAKU_JUDGE_END() return __retVal; }
#define YAKU_ADD(subyakuid,yakuvalue)                               (__retVal = ((YakuProvider*)yakuProvider)->queueYaku((subyakuid),(yakuvalue)))
#define YAKU_ADD_PEND(subyakuid,yakuvalue,token)                    (__retVal = ((YakuProvider*)yakuProvider)->queueYaku((subyakuid),(yakuvalue),(token)))
#define YAKU_ADD_IF(cond,subyakuid,yakuvalue)            if((cond)) (__retVal = ((YakuProvider*)yakuProvider)->queueYaku((subyakuid),(yakuvalue)));
#define YAKU_ADD_PEND_IF(cond,subyakuid,yakuvalue,token) if((cond)) (__retVal = ((YakuProvider*)yakuProvider)->queueYaku((subyakuid),(yakuvalue),(token)));
#define YAKU_MENZEN (__menzen)
#define YAKU_REDUCED(yakuValue) (((yakuValue) == 26) ? (WYAKUMAN ? 26 : 13) : (YAKU_MENZEN ? (yakuValue) : ((yakuValue) - 1)))
#define YAKU_SUPPRESS(token) (((YakuProvider*)yakuProvider)->suppressYaku((token)))
#define YAKU_CURRENT (((YakuProvider*)yakuProvider)->currentStatus)
#define YAKU_NORMAL_BEGIN() if (!(((YakuProvider*)yakuProvider)->tenpaiOnly)) {
#define YAKU_NORMAL_END()   }
#define MENTSU_SELECT3(func) ((mentsucnt == 4) ? ((func)(mentsus[0], mentsus[1], mentsus[2]) || \
                            (func)(mentsus[0], mentsus[1], mentsus[3]) || \
                            (func)(mentsus[0], mentsus[2], mentsus[3]) || \
                            (func)(mentsus[1], mentsus[2], mentsus[3])) : (false))
#define YAKU_SELECT3 MENTSU_SELECT3

#define MENTSU_STDALGO(algo, func) (std::algo(mentsus, mentsus + mentsucnt, (func)))
#define PAI_STDALGO(algo, func)    (std::algo(pais, pais + paicnt, (func)))

#define YAKU_SET(x)      (((YakuProvider*)yakuProvider)->setExtendData((x), 1))
#define YAKU_UNSET(x)    (((YakuProvider*)yakuProvider)->setExtendData((x), 0))
#define YAKU_SETEX(x, v) (((YakuProvider*)yakuProvider)->setExtendData((x), (v)))
#define YAKU_GET(x)      (((YakuProvider*)yakuProvider)->getExtendData((x)))

#define SUBYAKU(x) 

#define isPaiAkari(x) (((x).trait & TRAIT_AKARIPAI) != 0)
#define isMentsuAkari(x) (isPaiAkari((x).start) || isPaiAkari((x).middle) || isPaiAkari((x).last))
#define isNotYakuPai(x) (!isSanyuan((x)) && (x).type != YAKU_CURRENT.jyouhuun && (x).type != YAKU_CURRENT.jihuun)
#define isAnke(x) ((x).type == mentsu_KEZ && ((YAKU_CURRENT.akariStatus == TSUMO) || (YAKU_CURRENT.akariStatus == RON && !isMentsuAkari((x)))))
#define isGreen(x) (((x).type == 'F') || (((x).type == 'S') && ((x).fig == 2 || (x).fig == 3 || (x).fig == 4 || (x).fig == 6 || (x).fig == 8)))

// 内部优化用结构。
struct YakuProviderInternal
{
	YakuProviderBase* provider;
	int type; // 0 = M, 1 = C
	int traits;
};

// 方便扩展的役判断系统
class YakuProvider
{
private:
	int subids[MAX_SUBYAKU];
	int yakus[MAX_SUBYAKU];
	bool immediateYaku[MAX_SUBYAKU];
	std::string tokens[MAX_SUBYAKU];
	int subidcnt;
	std::map<std::string, yaku2> pendingYaku;
	std::vector<std::string> suppressedTokens;
	std::map<std::string, int> extendData;
	bool locked = false;
	bool lastLock = false;
	bool internalLock = false;
	bool doMainJudge(const pai * pais, const int paicnt, const mentsu * mentsus, const int mentsucnt, const pai * janto, int jantocnt, yakuTable * current, const std::vector<YakuProviderInternal>& dispatchTable, bool shapeMode);

protected:
	// 调整内部锁状态。
	// 内部锁的优先级高于 lock / unlockProvider
	// 内部锁开启时，lock / unlockProvider 会失败（但不会有返回值）
	// 用于函数的内部，来确保数据在函数内的一致性
	// 内部锁关闭时，lock / unlockProvider 状态会恢复到开启内部锁前，而不是直接解锁
	void forcedLock(bool lockStatus);
	void addYaku(yakuTable * current, int yaku_id, int yaku_point, int subid);
	bool tenpaiOnly = false;

public:
	BasicPtProviderBase* ptProvider;
	judgeRequestSimple currentStatus;
	int currentYakuId;

	// 存储役的数组
	std::vector<YakuProviderInternal> yakuProviders;
	std::vector<YakuProviderInternal> shapeProviders;

	// 注册一个役
	// yaku : 役的实例化的类，参考 YakuProvider(Base/M/C)的定义。
	bool registerProvider(YakuProviderBase* yaku);

	// 取消注册一个役，实例化的类的指针必须完全相同。（内部依赖指针地址进行比较）
	// yaku : 役的实例化的类
	bool unregisterProvider(YakuProviderBase* yaku);

	// 注册的役全部撤销。
	// GC : 立即回收所占用的内存。（默认 false：将在 YakuProvider / taj 释放时回收）
	bool unregsiterAll(bool GC = false);

	// 设置判例，本函数建议在锁定之前执行。
	void setSpecialCase(const judgeRequestSimple& jreq);

	// 锁定役列表。锁定后，register / unregister / rebuild 操作都会失败。
	// 锁定后，役的状态将保持，queryName 返回的也是期待值。
	// 一般而言，只需要在役添加完成后调用 rebuildProviders 即可，不需要显式的调用本函数。
	void lockProvider();

	// 解锁役列表。
	// 当解锁之后，register / unregister / rebuild 操作会影响到役列表的顺序 / 数量
	// 因而 queryName 可能失败，或返回非预期的值
	void unlockProvider();

	// 根据役的属性（dora、役满等）重建役的顺序，在重建后会自动锁定役列表。
	// 本函数是使用 judgeYaku 的先决条件，否则对 doraLike, yakumanLike 属性的处理可能出现问题
	bool rebuildProviders();

	// 判断役，将役追加到双向链表 current 中
	// pais : 手牌数组
	// paicnt : 手牌数量
	// mentsu : 面子数组（包括手牌和副露），副露总是在手牌面子的后面
	// mentsucnt : 面子数量
	// current : 当前役表
	bool judgeYaku(const pai* pais, int paicnt, const mentsu* mentsus, int mentsucnt, const pai* janto, int jantocnt, yakuTable* current);
	bool judgeYakuExtended(const pai * pais, const int paicnt, yakuTable * current);

	// 获得役的名称
	bool queryName(int id, int subid, char* buffer, int bufferSize);

	// 在 judgeYaku 的过程中添加役，请通过 YAKU_ADD(*) 宏进行添加
	// YAKU_ADD 为直接添加一个役
	// YAKU_ADD_PEND 为添加一个可能被覆盖的役
	bool queueYaku(int yakuSubId, int yakuValue);
	bool queueYaku(int yakuSubId, int yakuValue, const char* token);

	// 抑制一个役，这个役会被加入抑制列表。
	// 该函数请通过 YakuProviderBase::judgeYaku 过程中通过 YAKU_SUPPRESS 宏调用。
	// 一个典型的事例：
	// 在七对子中，使用 YAKU_ADD_PEND 声明了七对子，在二杯口中使用 YAKU_SUPPRESS 抑制了七对子，防止重复出现。
	// 该函数会影响性能，所以在役判断内部可以解决的互相覆盖关系，建议在内部解决。
	// 消除覆盖的复杂度为 O(k logm)，k 为抑制的数量，m 为 YAKU_ADD_PEND 的数量
	// token : 该役的唯一标识符。
	void suppressYaku(const char* token);

	// 设定附加值，该值将在一次 YakuProvider.judgeYaku 调用起到下一次调用时存在。
	// 所以可以用来存储调用者需要知悉的内容。
	void setExtendData(const char* token, int value);

	// 读取附加值，该值将在一次 YakuProvider.judgeYaku 调用起到下一次调用时存在。
	// 所以可以用来存储调用者需要知悉的内容。
	int getExtendData(const char* token);
	void clearExtendData();
};

bool operator< (YakuProviderInternal& a, YakuProviderInternal& b);
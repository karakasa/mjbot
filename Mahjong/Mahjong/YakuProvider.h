#pragma once
#include "PublicFunction.h"

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
	bool locked = false;
	bool lastLock = false;
	bool internalLock = false;
	judgeRequestSimple currentStatus;
	void internalRebuild(std::vector<YakuProviderInternal>& providerTable);
	void addYaku(yaku_table* current, int yaku_id, int yaku_point, int subid = 0);

protected:
	// 调整内部锁状态。
	// 内部锁的优先级高于 lock / unlockProvider
	// 内部锁开启时，lock / unlockProvider 会失败（但不会有返回值）
	// 用于函数的内部，来确保数据在函数内的一致性
	void forcedLock(bool lockStatus);

public:
	// 存储役的数组
	std::vector<YakuProviderInternal> yakuProviders;

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
	void setSpecialCase(const judgeRequest& jreq);

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
	// mentsu : 面子数组（包括手牌和副露）
	// mentsucnt : 面子数量
	// current : 当前役表
	void judgeYaku(const pai* pais, int paicnt, const mentsu* mentsus, int mentsucnt, yaku_table* current);

	// 获得役的名称
	bool queryName(unsigned int id, int subid, char* buffer, int bufferSize);
};
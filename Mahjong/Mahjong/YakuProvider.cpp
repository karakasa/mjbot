#include "stdafx.h"

#include "PublicFunction.h"
#include "YakuProvider.h"
#include "MemoryLeakMonitor.h"

const int rebuildOrder[] = { 1, 3, 0, 2 }; // magic

bool YakuProvider::registerProvider(YakuProviderBase* yaku)
{
	if (locked)
		return false;
	YakuProviderInternal provider;
	provider.provider = yaku;
	provider.traits = yaku->getTraits();
	provider.type = yaku->getType();
	yakuProviders.push_back(provider);
	return true;
}

bool YakuProvider::unregisterProvider(YakuProviderBase* yaku)
{
	if (locked)
		return false;
	for (auto it = yakuProviders.begin(); it != yakuProviders.end(); it++)
	{
		if (it->provider == yaku)
		{
			yakuProviders.erase(it);
			return true;
		}
	}
	return false;
}

bool YakuProvider::unregsiterAll(bool GC)
{
	if (locked)
		return false;
	yakuProviders.resize(0);
	if (GC)
		yakuProviders.shrink_to_fit();
	return true;
}

void YakuProvider::internalRebuild(std::vector<YakuProviderInternal>& providerTable)
{
	std::vector<YakuProviderInternal> tempTable[4];
	std::for_each(tempTable, tempTable + 4,                                         \
		[&providerTable](auto &item) {item.reserve(providerTable.size()); });
	for (auto& item : providerTable)
	{
		tempTable[rebuildOrder[                                                     \
			((item.traits & yakuTrait::yakumanLike) ? 2 : 0) |                     \
			((item.traits & yakuTrait::doraLike) ? 1 : 0)]].push_back(item);
	}
	std::for_each(tempTable + 1, tempTable + 4,                                     \
		[&tempTable](auto &item) {tempTable[0].insert(tempTable[0].end(), item.begin(), item.end()); });
	providerTable = std::move(tempTable[0]);
}

bool YakuProvider::rebuildProviders()
{
	if (locked)
		return false;
	internalRebuild(yakuProviders);
	lockProvider();
	return true;
}

void YakuProvider::lockProvider()
{
	if (internalLock)
		return;
	locked = true;
}

void YakuProvider::unlockProvider()
{
	if (internalLock)
		return;
	locked = false;
}

void YakuProvider::setSpecialCase(const judgeRequest& jreq)
{
	currentStatus.akariStatus = jreq.akariStatus;
	currentStatus.norelease = jreq.norelease;
	currentStatus.jyouhuun = jreq.jyouhuun;
	currentStatus.jihuun = jreq.jihuun;
	currentStatus.flags = jreq.flags;
}

void YakuProvider::forcedLock(bool lockStatus)
{
	internalLock = lockStatus;
	if (lockStatus)
	{
		lastLock = locked;
		locked = true;
	}
	else {
		locked = lastLock;
	}
}

void YakuProvider::addYaku(yakuTable* current, int yaku_id, int yaku_point, int subid)
{
	yaku* yaku_tmp = new yaku;
	MemoryLeakMonitor::addMonitor(yaku_tmp, sizeof(yaku), "YAKU_TMP CURRENT in Provider");
	yaku_tmp->yakuid = yaku_id;
	yaku_tmp->pt = yaku_point;
	yaku_tmp->yakusubid = subid;
	yaku_tmp->prev = NULL;
	yaku_tmp->next = NULL;
	if (current->first == NULL)
	{
		current->first = yaku_tmp;
		current->tail = yaku_tmp;
	}
	else
	{
		current->tail->next = yaku_tmp;
		yaku_tmp->prev = current->tail;
		current->tail = yaku_tmp;
	}
	current->yakutotal += yaku_point;
}

void YakuProvider::judgeYaku(const pai* pais, int paicnt, const mentsu* mentsus, int mentsucnt, const pai* janto, int jantocnt, yakuTable* current)
{
	mentsu fulu[MAX_MENTSU];
	int fulucnt = 0;
	bool judgeResult;
	bool colorOnly = mentsus == NULL;

	pendingYaku.clear();
	suppressedTokens.clear();

	if (!colorOnly)
	{
		for (int i = 0; i < mentsucnt; i++)
			switch (mentsus[i].type)
			{
			case mentsu_TYPE::mentsu_KEZ:
			case mentsu_TYPE::mentsu_SHUNZ:
			case mentsu_TYPE::mentsu_KEZ_KANG_S:
				break;
			default:
				fulu[fulucnt++] = mentsus[i];
			}
	}

	bool yakuMan = false, yakuExist = false, yakuShouldBeIncluded = false;

	this->forcedLock(true);

	currentYakuId = 0;

	for (auto c = yakuProviders.begin(); c != yakuProviders.end(); c++)
	{
		yakuShouldBeIncluded = false;
		judgeResult = false;
		if (c->traits & yakuTrait::doraLike)
			if (!yakuExist)
				break;
		if (yakuMan)
			if (!(c->traits & yakuTrait::yakumanLike))
				continue;

		subidcnt = 0;
		switch (c->type)
		{
		case yakuType::mentsuLike: // M
			if(!colorOnly)
				judgeResult = ((YakuProviderM*)(c->provider))->judgeYaku(mentsus, mentsucnt, janto, jantocnt, this);
			break;
		case yakuType::colorLike: // C
			judgeResult = ((YakuProviderC*)(c->provider))->judgeYaku(pais, paicnt, fulucnt == 0, this);
			break;
		}
		if (judgeResult)
		{
			if (!(c->traits & yakuTrait::doraLike))
			{
				yakuExist = true;
				if (c->traits & yakuTrait::yakumanLike)
				{
					yakuShouldBeIncluded = true;
					yakuMan = true;
				}
				else {
					yakuShouldBeIncluded = !yakuMan;
				}
			}
			else {
				if (yakuExist)
				{
					if (yakuMan)
					{
						yakuShouldBeIncluded = (c->traits & yakuTrait::yakumanLike) != 0;
					}
					else {
						yakuShouldBeIncluded = true;
					}
				}
			}
			if (yakuShouldBeIncluded)
				for (int i = 0; i < subidcnt; i++)
				{
					if(immediateYaku[i])
						addYaku(current, currentYakuId, yakus[i], subids[i]);
					else
					{
						yaku2 p;
						p.yakuid = currentYakuId;
						p.yakusubid = subids[i];
						p.pt = yakus[i];
						pendingYaku[tokens[i]] = std::move(p);
					}
				}
		}
		++currentYakuId;
	}

	this->forcedLock(false);

	for (int& token : suppressedTokens)
		pendingYaku.erase(token);
	for (auto& yakuFinal : pendingYaku)
		addYaku(current, yakuFinal.second.yakuid, yakuFinal.second.pt, yakuFinal.second.yakusubid);

	pendingYaku.clear();
	suppressedTokens.clear();
}

bool YakuProvider::queryName(unsigned int id, int subid, char* buffer, int bufferSize)
{
	if (id < 0 || id > yakuProviders.size())
		return false;
	yakuProviders[id].provider->queryName(buffer, bufferSize, NULL, subid);
	return true;
}

bool YakuProvider::queueYaku(int yakuSubId, int yakuValue)
{
	if (subidcnt < MAX_SUBYAKU)
	{
		subids[subidcnt] = yakuSubId;
		yakus[subidcnt] = yakuValue;
		immediateYaku[subidcnt] = true;
		++subidcnt;
		return true;
	}
	return false;
}

bool YakuProvider::queueYaku(int yakuSubId, int yakuValue, int token)
{
	if (subidcnt < MAX_SUBYAKU)
	{
		subids[subidcnt] = yakuSubId;
		yakus[subidcnt] = yakuValue;
		immediateYaku[subidcnt] = false;
		tokens[subidcnt] = token;
		++subidcnt;
		return true;
	}
	return false;
}

void YakuProvider::suppressYaku(int token)
{
	suppressedTokens.push_back(token);
}

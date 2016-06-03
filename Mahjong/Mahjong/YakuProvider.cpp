#include "stdafx.h"

#include "PublicFunction.h"
#include "YakuProvider.h"
#include "MemoryLeakMonitor.h"

const int rebuildOrder[] = { 1, 3, 0, 2 }; // magic

bool operator< (YakuProviderInternal& a, YakuProviderInternal& b)
{
	if ((a.traits & yakuTrait::yakumanLike) != (b.traits & yakuTrait::yakumanLike))
		return (a.traits & yakuTrait::yakumanLike) > (b.traits & yakuTrait::yakumanLike);
	if ((a.traits & yakuTrait::doraLike) != (b.traits & yakuTrait::doraLike))
		return (a.traits & yakuTrait::doraLike) < (b.traits & yakuTrait::doraLike);
	return false;
}

bool YakuProvider::registerProvider(YakuProviderBase* yaku)
{
	if (locked)
		return false;
	YakuProviderInternal provider;
	provider.provider = yaku;
	provider.traits = yaku->getTraits();
	provider.type = yaku->getType();
	if (provider.traits & yakuTrait::shapeYaku)
		shapeProviders.push_back(provider);
	else
		yakuProviders.push_back(provider);
	return true;
}

bool YakuProvider::unregisterProvider(YakuProviderBase* yaku)
{
	if (locked)
		return false;
	if ((yaku->getTraits()) & yakuTrait::shapeYaku)
	{
		for (auto it = shapeProviders.begin(); it != shapeProviders.end(); it++)
		{
			if (it->provider == yaku)
			{
				shapeProviders.erase(it);
				return true;
			}
		}
	}
	else {
		for (auto it = yakuProviders.begin(); it != yakuProviders.end(); it++)
		{
			if (it->provider == yaku)
			{
				yakuProviders.erase(it);
				return true;
			}
		}
	}
	return false;
}

bool YakuProvider::unregsiterAll(bool GC)
{
	if (locked)
		return false;
	yakuProviders.resize(0);
	shapeProviders.resize(0);
	if (GC)
	{
		yakuProviders.shrink_to_fit();
		shapeProviders.shrink_to_fit();
	}
	return true;
}

bool YakuProvider::rebuildProviders()
{
	if (locked)
		return false;
	std::sort(yakuProviders.begin(), yakuProviders.end());
	std::sort(shapeProviders.begin(), shapeProviders.end());
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

void YakuProvider::setSpecialCase(const judgeRequestSimple& jreq)
{
	currentStatus = jreq;
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
	yaku2 yk;
	yk.yakuid = yaku_id;
	yk.yakusubid = subid;
	yk.pt = yaku_point;
	current->yakus.push_back(std::move(yk));
	current->yakutotal += yaku_point;
}

bool YakuProvider::doMainJudge(const pai* pais, const int paicnt, const mentsu* mentsus, const int mentsucnt, const pai* janto, int jantocnt, yakuTable* current, const std::vector<YakuProviderInternal>& dispatchTable, bool shapeMode = false)
{
	mentsu fulu[MAX_MENTSU];
	int fulucnt = 0;
	bool judgeResult;
	bool colorOnly = (mentsus == NULL) || (mentsucnt == 0) || shapeMode;

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

	this->forcedLock(true);

	currentYakuId = shapeMode ? -1 : 0;
	bool yakuMan = false, yakuExist = false;

	for (auto& c : dispatchTable)
	{
		judgeResult = false;
		subidcnt = 0;

		if (c.traits & yakuTrait::doraLike)
			if (!yakuExist)
				break;
		if (yakuMan)
			if (!(c.traits & yakuTrait::yakumanLike))
				continue;

		switch (c.type)
		{
		case yakuType::mentsuLike: // M
			if (!colorOnly)
				judgeResult = ((YakuProviderM*)(c.provider))->judgeYaku(mentsus, mentsucnt, janto, jantocnt, this);
			break;
		case yakuType::colorLike: // C
			judgeResult = ((YakuProviderC*)(c.provider))->judgeYaku(pais, paicnt, fulucnt == 0, this);
			break;
		}
		if (judgeResult)
		{
			if (tenpaiOnly)
			{
				this->forcedLock(false);
				return true;
			}
			if (yakuExist || !(c.traits & yakuTrait::doraLike))
			{
				yakuExist = true;
				yakuMan = yakuMan || ((c.traits & yakuTrait::yakumanLike) != 0);
				if (!yakuMan || (c.traits & yakuTrait::yakumanLike) != 0)
					for (int i = 0; i < subidcnt; i++)
					{
						if (immediateYaku[i])
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
		}
		if (shapeMode)
			--currentYakuId;
		else
			++currentYakuId;
	}

	this->forcedLock(false);

	return yakuExist;
}

bool YakuProvider::judgeYaku(const pai* pais, const int paicnt, const mentsu* mentsus, const int mentsucnt, const pai* janto, int jantocnt, yakuTable* current)
{
	tenpaiOnly = false;
	pendingYaku.clear();
	suppressedTokens.clear();

	doMainJudge(pais, paicnt, mentsus, mentsucnt, janto, jantocnt, current, yakuProviders, false);

	for (auto& token : suppressedTokens)
		pendingYaku.erase(token);
	for (auto& yakuFinal : pendingYaku)
		addYaku(current, yakuFinal.second.yakuid, yakuFinal.second.pt, yakuFinal.second.yakusubid);
	if ((current->yakus).size() != 0)
		ptProvider->judgePt(pais, paicnt, mentsus, mentsucnt, janto, jantocnt, current, this);

	pendingYaku.clear();
	suppressedTokens.clear();

	return (current->yakus).size() != 0;
}

bool YakuProvider::judgeYakuExtended(const pai* pais, const int paicnt, yakuTable* current)
{
	tenpaiOnly = current == NULL;
	if (!tenpaiOnly)
	{
		pendingYaku.clear();
		suppressedTokens.clear();
	}
	
	bool result = doMainJudge(pais, paicnt, NULL, 0, NULL, 0, current, shapeProviders, true);
	if (tenpaiOnly)
		return result;
	
	if (result)
	{
		doMainJudge(pais, paicnt, NULL, 0, NULL, 0, current, yakuProviders, false);
		for (auto& token : suppressedTokens)
			pendingYaku.erase(token);
		for (auto& yakuFinal : pendingYaku)
			addYaku(current, yakuFinal.second.yakuid, yakuFinal.second.pt, yakuFinal.second.yakusubid);
		if ((current->yakus).size() != 0)
			ptProvider->judgePt(pais, paicnt, NULL, 0, NULL, 0, current, this);

		pendingYaku.clear();
		suppressedTokens.clear();
		return (current->yakus).size() != 0;
	}else{
		pendingYaku.clear();
		suppressedTokens.clear();
		return false;
	}
}

bool YakuProvider::queryName(int id, int subid, char* buffer, int bufferSize)
{
	bool shape = false;
	if (id < 0)
	{
		shape = true;
		id = -id - 1;
	}
	if (shape)
	{
		if (id >(signed int)shapeProviders.size())
			return false;
		shapeProviders[id].provider->queryName(buffer, bufferSize, NULL, subid);
	}
	else {
		if (id > (signed int)yakuProviders.size())
			return false;
		yakuProviders[id].provider->queryName(buffer, bufferSize, NULL, subid);
	}
	return true;
}

bool YakuProvider::queueYaku(int yakuSubId, int yakuValue)
{
	if (tenpaiOnly)
		return true;
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

bool YakuProvider::queueYaku(int yakuSubId, int yakuValue, const char* token)
{
	if (tenpaiOnly)
		return true;
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

void YakuProvider::suppressYaku(const char* token)
{
	if (tenpaiOnly)
		return;
	suppressedTokens.push_back(token);
}

void YakuProvider::setExtendData(const char* token, int value)
{
	if (tenpaiOnly)
		return;
	extendData[token] = value;
}

int YakuProvider::getExtendData(const char* token)
{
	auto it = extendData.find(token);
	if (it != extendData.end())
		return it->second;
	return 0;
}

void YakuProvider::clearExtendData()
{
	extendData.clear();
}
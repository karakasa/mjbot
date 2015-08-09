#include "stdafx.h"

#ifdef _DEBUG
//Only valid when mode is 'Debug' not 'Release'
#include <iostream>
#include <algorithm>
#include <map>
#include <string>
namespace MemoryLeakMonitor
{
	std::map<int, std::pair<int, std::string> > addrs;
	int byteInTotal = 0, byteMax = 0, allocCnt = 0, currentMemAreaCnt = 0;
	void addMonitor(void* pointer, int size, const char* desc)
	{
		if (size == 0)
			std::cerr<<"SIZE 0 ALLOCATION."<<std::endl;
		std::string ntp(desc);
		addrs[(int)pointer] = std::make_pair(size, ntp);
		//std::cerr<<"ALLOC ID: "<<allocCnt<<" "<<std::hex<<(int)pointer<<std::dec<<", Size: "<<size<<std::endl;
		allocCnt++;
		currentMemAreaCnt++;
		byteInTotal += size;
		if (byteInTotal > byteMax)
			byteMax = byteInTotal;
		ntp = "";
	}
	void removeMonitor(void* pointer)
	{
		std::map<int, std::pair<int, std::string> >::iterator pos = addrs.find((int)pointer);
		if (pos != addrs.end())
		{
			byteInTotal -= (pos->second).first;
			currentMemAreaCnt--;
			addrs.erase(pos);
		}
		else {
			std::cerr << "Invalid Pointer." << std::endl;
		}
	}
	int gc()
	{
		if (currentMemAreaCnt != 0)
			for (std::map<int, std::pair<int, std::string> >::iterator pos = addrs.begin(); pos != addrs.end(); pos++)
				std::cerr << std::hex << pos->first << std::dec << " " << pos->second.first << " " << pos->second.second << std::endl;
		std::cerr << "Memory Leaked: " << byteInTotal << std::endl << "Leak Count: " << currentMemAreaCnt << std::endl << "Maximium Memory In Use: " << byteMax << std::endl << "Allocation Count: " << allocCnt << std::endl << std::endl;
		return currentMemAreaCnt != 0;
	}
}
#else
namespace MemoryLeakMonitor
{
	void addMonitor(void* pointer, int size, const char* desc)
	{
	}
	void removeMonitor(void* pointer)
	{
	}
	int gc()
	{
		return 0;
	}
}
#endif
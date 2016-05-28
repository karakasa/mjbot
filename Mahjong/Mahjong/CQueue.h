#pragma once

// 循环队列，用于网络模块维护数据包
// 用法和 STL 中的 queue 类似

template <typename T>
class CQueue
{
public:
	// 初始化循环队列。
	// init : 队列长度
	// start : 起始 ID
	// 返回值: 无
	void init(int init, int start);

	// 释放循环队列。
	~CQueue();

	// 在最后插入一个成员
	// 返回值: 无
	void push_back(const T& data);

	// 获得指定 ID 处的成员
	// id : ID
	// container : 接收变量
	// 返回值: 无
	void get(const int id, T* container);

private:
	int size; //允许的最大存储空间以元素为单位
	int pos;
	int startId;
	T *base; //存储空间基址

};

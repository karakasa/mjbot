#pragma once
#include "stdafx.h"

// ��������

#include "PublicFunction.h"

class Syanten
{
	private:
		int calculateKokushiSyanten(const pai* tpai, const int paicnt);
		int calculateChiitoitsu(const pai* tpai, const int paicnt);
		int normalCalculate3(pai* tpai, const int paicnt, int currentMenchi);
		int normalCalculate2(const pai* tpai, const int paicnt, int currentMenchi);
		int normalCalculate(const pai* tpai, const int paicnt);

	public:

		// ������������ǰ���ú��������߶��������ļ��㻹��һЩ���⡣�÷����� TenpaiAkariJudge ���ж�Ҫ��ö࣬�ڽ���Ҫ�ж����Ƶ���״�����Ƶ���״ʱ�Ƽ�ʹ�ñ����
		// tpai : �������顣���������Ϊ���ⳤ�ȡ�3n + 1 ʱ������ǵ�ǰ������3n + 2 ʱ������Ǵ��ĳ���ƺ����С������3n ʱ��������塣
		// paicnt : ���鳤��
		// normalonly : �������׼�ͣ��������߶��Ӻ͹�ʿ��
		// ����ֵ : (��С)��������0 ��Ϊ���ƣ�-1 ��Ϊ���ơ�
		int calculateSyanten(const pai* tpai, const int paicnt, bool normalonly = false);
};
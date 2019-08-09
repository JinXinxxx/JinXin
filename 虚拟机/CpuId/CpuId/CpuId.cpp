// CpuId.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include<iostream>
#include<intrin.h>
#include<Windows.h>
using namespace std;

int main()
{
	INT32 deBuf[5] = {0};

	__cpuid(deBuf, 0x40000000);
	printf("%s\n", (PVOID)(deBuf + 0x2));
	system("pause");
	return 0;
}

// 素数运算.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <time.h>
//求取16位以内的所有素数2的16次方=65536
__int64 Primer[6541];
void Init()
{
	bool tag[65536];
	memset(tag, 0x00, sizeof(tag));
	__int64 i, j;
	for (i = 3; i < 256; i += 2)
		if (tag[i] == false)
			for (j = i * i; j < 65536; j += i)
				tag[j] = true;
	for (Primer[0] = 2, j = 1, i = 3; i < 65536; i += 2)
		if (tag[i] == false)
			Primer[j++] = i;
}
int main()
{
	clock_t start, finish;
	float totaltime;
	int p;
	bool bRet = true;
	srand(time(NULL));
	start = clock();
	{
		for (int j = 0; j < 100; j++) {
			Init();
			while (bRet) {
				bRet = false;
				p = rand() * 1000 + rand() % 100 + 100000000;
				__int64 m = (__int64)sqrt((double)p) + 1, i;
				for (i = 0; Primer[i] <= m; i++)
					if (p%Primer[i] == 0) {
						bRet = true;
						break;
					}
			}
			bRet = true;
		}
		std::cout << "素数为" << p << std::endl;
	}
	finish = clock();
	totaltime = (float)(finish - start) / CLOCKS_PER_SEC / 100;
	printf("此程序的运行时间为%.8f秒", totaltime);
	//std::cout << "\n此程序的运行时间为" << totaltime << "秒！" << std::endl;
	system("pause");
}


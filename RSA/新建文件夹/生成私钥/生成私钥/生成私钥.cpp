// 生成私钥.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <time.h>
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
bool IsPrimer(__int64 n)
{
	__int64 m = (__int64)sqrt((double)n) + 1, i;
	for (i = 0; Primer[i] <= m; i++)
		if (n%Primer[i] == 0)
			return false;
	return true;
}
void GetPrimers(__int64 &p, __int64 &q)
{
	srand(time(NULL));
	do
	{
		p = rand() % 1000 + 4000;
	} while (IsPrimer(p) == false);
	do
	{
		q = rand() % 1000 + 4000;
	} while (IsPrimer(q) == false);
}
__int64 Gcd(__int64 a, __int64 b)
{
	for (__int64 t; b; t = a % b, a = b, b = t);
	return a;
}
void Ex_Gcd(__int64 a, __int64 b, __int64 &x, __int64 &y)
{
	if (b == 0)   //当b为0时,这也是欧几里得算法终止的条件,此时x1=1,y1=0
	{
		x = 1, y = 0;
		return;
	}
	Ex_Gcd(b, a%b, x, y);   //没有终止,再向下再次递归,参数改为b,a%b
	__int64 t = x;  //已经计算出了b*x1+(a%b)*y1=gcd(b,a)
	x = y, y = t - a / b * y; //可以推导出x,y
}
int main()
{
	clock_t start, finish;
	float totaltime;
	start = clock();
	__int64 p, q, pub_key, pri_key, m, n, t;  //pub_key公钥,pri_kry私钥
	Init();
	do
	{
		GetPrimers(p, q);
		n = p * q;
		m = (p - 1)*(q - 1);
		do
			pub_key = (rand() % 1000 + 500)*(rand() % 1000 + 500);
		while (Gcd(m, pub_key) != 1);
		Ex_Gcd(pub_key, m, pri_key, t);  //pub_key*pri_key=1mod(m);
	} while (pri_key < 1e5);
	finish = clock();
	std::cout << "私钥为" << pri_key << std::endl;
	totaltime = (float)(finish - start) / CLOCKS_PER_SEC / 100;
	printf("此程序的运行时间为%.8f秒", totaltime);
}
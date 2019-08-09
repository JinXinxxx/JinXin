// OpenHandle.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <Windows.h>

int main()
{
	CreateFile("D:\\IDA 7.0\\ida.exe", GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	printf("%x", GetLastError());
}


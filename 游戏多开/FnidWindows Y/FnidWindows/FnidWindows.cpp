// FnidWindows.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <Windows.h>
#include <iostream>
int count = 0;

BOOL CALLBACK lpEnumFunc(HWND hwnd, LPARAM lParam)
{
	char str[MAX_PATH];
	GetWindowText(hwnd, str, MAX_PATH);
	//弊端被改了标题就无法使用了
	if (memcmp(str, "《新天龙八部》永恒经典版", 12) == 0) {
		count++;

	}
	return TRUE;
}

int main()
{
	while (TRUE) {
		if (EnumWindows(lpEnumFunc, NULL)) {
			std::cout << "当前游戏数量:" << count << std::endl;
			count = 0;
			Sleep(1000);
		}
		else {
			printf("Error");
			break;
		}
	}
}

// ScanVmDriverFile.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//只要有一个文件存在，那么这个机器就是虚拟机
//vm3dmp这个是虚拟机的显卡，也是虚拟机中特殊存在的，以及vmmouse,vmusbmouse
//这里注意一下，32位文件是无法打开system32文件夹的所以必须生成64位文件
//这个方法极好绕过，改文件名以及删除文件都可以绕过此方法
#include "pch.h"
#include <iostream>
#include <Windows.h>
int main()
{
	HANDLE hFile = CreateFile(L"C:\\Windows\\System32\\drivers\\vm3dmp.sys", GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (GetLastError() != 2) {
		printf("这是虚拟机");
	}
	system("pause");
}


// ScanVirtualComputer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#include "ScanVirtualComputer.h"
int main()
{
	if (myIsVirtualComputer()) {
		std::cout << "这是虚拟机" << std::endl;
	}
	else {
		std::cout << "这不是虚拟机" << std::endl;
	}
	system("pause");
}

BOOL myIsVirtualComputer() {
	DWORD nRet = 0;
	BOOL bRet = false;
	do {
		nRet = myScanByRserved();
		if (nRet != 0) {
			bRet = true;
			break;
		}
	} while (false);
	return bRet;
}


DWORD myScanByRserved() {
	DWORD cpuReserved = 0;
	_asm {
		pushad
		mov eax, 1
		cpuid
		mov dword ptr[cpuReserved], ecx
		popad
	}
	cpuReserved &= 0x80000000;
	return cpuReserved;
}
// CPU查询.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <intrin.h>
#include "stdio.h"
#include "windows.h"

typedef struct _CPU_INFORMATION{
/*0x 0*/	char cpuName[52];//获取CPU NAME ，48字节为CPU的名字，后面4个字节为空
}CpuName, *PCpuName;

BOOL _stdcall myGetCpuName(PVOID cpuName) {
	_asm {
		pushad
		mov ecx, 0
		push ecx
Label1:
		push ecx
		mov eax, 80000002H
		add eax, ecx
		cpuid
		push ecx
		push eax
		mov eax, [ebp + 8]
		mov ecx, [ebp - 0F0H]
		pop [eax + ecx]
		pop [eax + 2 + ecx]
		mov [eax + 4 + ecx], ebx
		pop [eax + 8 + ecx]
		pop [eax + 10 + ecx]
		mov [eax + 0CH + ecx], edx
		add ecx,10H
		mov [ebp - 0F0H], ecx
		pop ecx
		inc ecx
		cmp ecx,3
		JL Label1
		pop ecx
		popad
	}
}


int main(void)
{
	CpuName cpuName = { 0 };
	myGetCpuName(&cpuName);
	printf("%s", cpuName.cpuName);
	return 0;
}
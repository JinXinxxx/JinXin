// multiGame.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include "pch.h"
#include <windows.h>
#include <stdio.h>
typedef enum _OBJECT_INFORMATION_CLASS {
	ObjectBasicInformation,
	ObjectNameInformation,
	ObjectTypeInformation,
	ObjectAllInformation,
	ObjectDataInformation,

} OBJECT_INFORMATION_CLASS;

typedef struct _PUBLIC_OBJECT_BASIC_INFORMATION {
	ULONG Attributes;
	ACCESS_MASK GrantedAccess;
	ULONG HandleCount;
	ULONG PointerCount;
	ULONG Reserved[10];    // reserved for internal use
} PUBLIC_OBJECT_BASIC_INFORMATION, *PPUBLIC_OBJECT_BASIC_INFORMATION;

typedef  NTSTATUS(NTAPI *NTQUERYOBJECT)(
	HANDLE                   Handle,
	OBJECT_INFORMATION_CLASS ObjectInformationClass,
	PVOID                    ObjectInformation,
	ULONG                    ObjectInformationLength,
	PULONG                   ReturnLength
	);
int main(void)
{

		//Map句柄不Close等程序关闭后Close
		HANDLE hFile = CreateFileA("E:\\TLBB\\Bin\\Game.exe", GENERIC_READ|GENERIC_EXECUTE, FILE_SHARE_READ , NULL, OPEN_EXISTING, 0, NULL);
		HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_EXECUTE_READ, 0, 1, L"Game.exe");
		CloseHandle(hFile);
		HMODULE hModule = LoadLibraryA("ntdll.dll");
		NTQUERYOBJECT lpFunction = (NTQUERYOBJECT)GetProcAddress(hModule, "NtQueryObject");
		PUBLIC_OBJECT_BASIC_INFORMATION objectBasicInformation = { 0 };
		DWORD dwRead = 0;
		while (TRUE) {
			NTSTATUS a = lpFunction(hMap, ObjectBasicInformation, &objectBasicInformation, sizeof(PUBLIC_OBJECT_BASIC_INFORMATION), &dwRead);
			printf("目前游戏数量:%d\n", objectBasicInformation.HandleCount);
			Sleep(1000);
		}
	system("pause");

}


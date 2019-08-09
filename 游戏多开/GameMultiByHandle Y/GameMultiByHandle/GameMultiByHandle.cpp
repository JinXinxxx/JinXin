// GameMultiByHandle.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <Windows.h>

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

int main()
{
	//这里创建了对象并没有释放，程序结束后自动释放
	HANDLE hMutex = CreateMutex(NULL, FALSE, L"TLBB");

	HMODULE hModule = LoadLibrary(L"ntdll.dll");
	NTQUERYOBJECT lpFunction = (NTQUERYOBJECT)GetProcAddress(hModule, "NtQueryObject");
	while (true) {
		PUBLIC_OBJECT_BASIC_INFORMATION publicObjectBasicInfo = { 0 };
		DWORD dwRead = 0;
		lpFunction(hMutex, ObjectBasicInformation, &publicObjectBasicInfo, sizeof(PUBLIC_OBJECT_BASIC_INFORMATION), &dwRead);
		printf("%d\n", publicObjectBasicInfo.HandleCount);
		Sleep(10000);
	}
	system("pause");
}

// ScanTargetCpu.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include "ScanTargetCpu.h"
CpuInformation cpuInformation = { 0 };
int main()
{
	if (myGetCpuInformation()) {
		cpuInformation.numberOfPerCoreAboutL1 = myGetPerCoreL1Cache();
		if (cpuInformation.numberOfCore == cpuInformation.numberOfPackage) {}
		if (cpuInformation.numberOfPerCoreAboutL1 != 0)
		{
			if (myIsVirtualPc()) {
				printf("是虚拟机\n");
			}
			else {
				printf("这不是虚拟机\n");
			}
		}
	}
	else {
		printf("无法获取CPUID中Cache信息\n");
	}
	system("pause");
}

//判断是否为IntelCPU：FALSE代表不是，TRUE代表是
BOOL myIsIntelCpu() {
	char cpuType[16] = { 0 };
	char intelCpu[16] = "GenuineIntel";
	//char amdCpu[16] = "AuthenticAMD";  目前来说不是AMD就是Intel吧
	BOOL bRet = FALSE;
	__cpuid(reinterpret_cast<int *>(cpuType), 0);
	memcpy(cpuType, (cpuType + 0x4), 4);
	memcpy((cpuType + 0x4), (cpuType + 0xc), 4);
	memset((cpuType + 0xc), 0, 4);
	if (memcmp(cpuType, intelCpu, 16) == 0) {
		bRet = TRUE;
	}
	return bRet;
}
//检测是否支持VT-X
//通过L1与核心数进行比较
BOOL myIsVirtualPc() {
	DWORD processorL1CacheCount = cpuInformation.numberOfCore * cpuInformation.numberOfPerCoreAboutL1;
	DWORD bRet = FALSE;
	char szCpuName[65] = { 0 };
	do {
		
		if (cpuInformation.numberOfCore == cpuInformation.numberOfPackage) {
			bRet = TRUE;
			myGetCpuName(szCpuName);
			printf("CPU型号：%s很大可能", szCpuName);
			//这里直接上传CPU型号就好
			break;
		}
		if (cpuInformation.numberOfL1Cache != processorL1CacheCount) {
			bRet = TRUE;
			break;
		}
	
	} while (false);
	return bRet;
}

//获取CPU核心数以及L1的数目
BOOL myGetCpuInformation() {
	LPFN_GLPI glpi;
	BOOL done = FALSE;
	BOOL bRet = TRUE;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	DWORD returnLength = 0;
	DWORD processorPackageCount = 0;
	DWORD processorCoreCount = 0;
	DWORD processorL1CacheCount = 0;
	DWORD byteOffset = 0;
	PCACHE_DESCRIPTOR Cache;

	do {
		glpi = (LPFN_GLPI)GetProcAddress(
			GetModuleHandle(TEXT("kernel32")),
			"GetLogicalProcessorInformation");
		if (glpi == NULL)
		{
			bRet = FALSE;
			break;
		}
		while (!done)
		{
			DWORD rc = glpi(buffer, &returnLength);

			if (rc == FALSE)
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					if (buffer)
						free(buffer);

					buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(
						returnLength);

					if (buffer == NULL)
					{
						bRet = FALSE;
						break;
					}
				}
				else
				{
					bRet = FALSE;
					break;
				}
			}
			else
			{
				done = TRUE;
			}
		}
		ptr = buffer;

		while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength)
		{
			switch (ptr->Relationship)
			{
			case RelationProcessorCore:
				processorCoreCount++;
				break;

			case RelationCache: 
				Cache = &ptr->Cache;
				if (Cache->Level == 1)
				{
					processorL1CacheCount++;
				}
				break;
			case RelationProcessorPackage:
				// Logical processors share a physical package.
				processorPackageCount++;
				break;
			default:
				break;
			}
			byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			ptr++;
		}
	} while (false);
	if (bRet == TRUE) {
		cpuInformation.numberOfPackage = processorPackageCount;
		cpuInformation.numberOfCore = processorCoreCount;
		cpuInformation.numberOfL1Cache = processorL1CacheCount;
	}
	return bRet;
}
//获取CPU每个核心的L1数目
DWORD myGetPerCoreL1Cache() {
	DWORD nRet = 0;
	int cacheInformation[4] = { 0 };
	if (myIsIntelCpu()) {
		__cpuidex(cacheInformation, 0x4, 0x0);
		if ((cacheInformation[0] & 0xE0) == 0x20) {
			nRet++;
		}
		__cpuidex(cacheInformation, 0x4, 0x1);
		if ((cacheInformation[0] & 0xE0) == 0x20) {
			nRet++;
		}
	}
	else {
		__cpuidex(cacheInformation, 0x8000001D, 0x0);
		if ((cacheInformation[0] & 0xE0) == 0x20) {
			nRet++;
		}
		__cpuidex(cacheInformation, 0x8000001D, 0x1);
		if ((cacheInformation[0] & 0xE0) == 0x20) {
			nRet++;
		}
	}
	return nRet;
}

//获取CPU型号信息
VOID myGetCpuName(char *szCpuName) {
	__cpuid((int *)szCpuName, 0x80000002);
	__cpuid((int *)(szCpuName + 0x10), 0x80000003);
	__cpuid((int *)(szCpuName + 0x20), 0x80000004);
	__cpuid((int *)(szCpuName + 0x30), 0x80000005);
}
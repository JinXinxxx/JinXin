// ScanDevice.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <cfgmgr32.h>  
#include <SetupAPI.h>

#pragma comment(lib,"Setupapi.lib")
BOOL IsInstallDriver()
{
	HDEVINFO hDevInfo = NULL;
	SP_DEVINFO_DATA DeviceInfoData = {0};
	DWORD i = 0;
	BOOL bRet = FALSE;

	hDevInfo = SetupDiGetClassDevs(NULL,
		NULL, 
		NULL,
		DIGCF_PRESENT | DIGCF_ALLCLASSES);

	if (hDevInfo == INVALID_HANDLE_VALUE)
	{
		printf("无法获取硬件信息列表%x", GetLastError());
		return bRet;
	}
	DWORD dwStatuts = -1;
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
	{
		DWORD DataT;
		LPTSTR buffer = NULL;
		DWORD buffersize = 0;
		while (!SetupDiGetDeviceRegistryProperty(
			hDevInfo,
			&DeviceInfoData,
			SPDRP_HARDWAREID,
			&DataT,
			(PBYTE)buffer,
			buffersize,
			&buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				if (buffer) LocalFree(buffer);
				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);
			}
			else
			{
				break;
			}
		}
		if (buffer){
			if (memcmp(buffer, L"COMPOSITE_BATTERY", 17) == 0){
				printf("有电池驱动存在\n");
				bRet = TRUE;
			}
			LocalFree(buffer);
		}
	}

	SetupDiDestroyDeviceInfoList(hDevInfo);

	return bRet;
}

int main() {
	SYSTEM_POWER_STATUS SystemPowerStatus = { 0 };
	BOOL bRet = IsInstallDriver();
	if (bRet) {
		if (!GetSystemPowerStatus(&SystemPowerStatus)) {
			printf("Error Get Power Status:%x", GetLastError());
		}
		else {
			if (SystemPowerStatus.BatteryFlag == 0x80) {
				printf("这台电脑没有电池\n");
			}
		}
	}
	system("pause");
}
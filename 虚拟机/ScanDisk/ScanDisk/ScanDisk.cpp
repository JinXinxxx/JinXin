// ScanDisk.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//因为虚拟机的磁盘是人为创建的，是1024*1024*1024的整数倍，而现实中的磁盘一定不是这样的。

#include "pch.h"
#include <windows.h>
#include <winioctl.h>
#include <stdio.h>

BOOL GetDriveGeometry(LPWSTR wszPath, DISK_GEOMETRY_EX *pdg)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	BOOL bResult = FALSE;                 // results flag
	DWORD junk = 0;                     // discard results

	hDevice = CreateFileW(wszPath,          // drive to open
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	{
		return (FALSE);
	}

	bResult = DeviceIoControl(hDevice,                       // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, // operation to perform
		NULL, 0,                       // no input buffer
		pdg, sizeof(*pdg),            // output buffer
		&junk,                         // # bytes returned
		(LPOVERLAPPED)NULL);          // synchronous I/O

	CloseHandle(hDevice);

	return (bResult);
}

int wmain(int argc, wchar_t *argv[])
{
	DISK_GEOMETRY_EX pdg = { 0 }; // disk drive geometry structure
	BOOL bResult = FALSE;      // generic results flag
	ULONGLONG DiskSize = 0;    // size of the drive, in bytes
	DWORD dwRet = -1;
	WCHAR wszDrive[] = L"\\\\.\\PhysicalDrive0";
	bResult = GetDriveGeometry(wszDrive, &pdg);
	do {
		if (!bResult) {
			printf("访问磁盘失败");
			break;
		}
		printf("%d", (pdg.DiskSize.QuadPart / 1024 / 1024 / 1024));
		dwRet = pdg.DiskSize.QuadPart % (1024 * 1024 * 1024);
		if (dwRet == 0) {
			printf("在虚拟机中");
		}
		else {
			printf("不是虚拟机");
		}
	} while (FALSE);
	system("pause");
	return ((int)bResult);
}
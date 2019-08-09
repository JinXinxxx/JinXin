// Ntfs_Mft.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include "NtfsFileLocation.h"
#include "CreateDirectory.h"

int _tmain(int argc, _TCHAR* argv[])
{
	char szFilePath[MAX_PATH] = { 0 };
	GetCurrentDirectory(MAX_PATH, szFilePath);
	//需要保存的数据
	BYTE computerUuid[0x3E] = { 0 };

	// 打开磁盘
	HANDLE hFile = NULL;
	if (!OpenDisk(hFile, szFilePath))
	{
		return 1;
	}
	// 获取扇区大小(2)、簇大小(1)、$MFT起始簇号(8)
	WORD wSizeOfSector = 0;
	BYTE bSizeOfCluster = 0;
	LARGE_INTEGER liClusterNumberOfMFT = { 0 };
	GetDataFromDBR(hFile, wSizeOfSector, bSizeOfCluster, liClusterNumberOfMFT);

	// 计算$MFT元文件的字节偏移
	LARGE_INTEGER liMFTOffset;
	liMFTOffset.QuadPart = liClusterNumberOfMFT.QuadPart * bSizeOfCluster * wSizeOfSector;

	// 计算根目录，与$MFT相距5个目录，每个目录大小固定为1KB(0x400)
	LARGE_INTEGER liRootOffset;
	liRootOffset.QuadPart = liMFTOffset.QuadPart + 5 * 0x400;

	// 文件定位
	LocationFile(hFile, szFilePath, wSizeOfSector, bSizeOfCluster, liMFTOffset, liRootOffset);

	//通过几个定值获取数据
	if (!myDiskData(hFile, wSizeOfSector, bSizeOfCluster, liMFTOffset, liRootOffset, computerUuid)) {
		if (myCreateDirectoryAndDeleteDirectory()) {
			//在这里申请一下变量，
			DWORD dwRead = 0;
			BYTE lpBuf[0x88] = { 0 };
			SetFilePointer(hFile, liRootOffset.LowPart, &liRootOffset.HighPart, FILE_BEGIN);
			ReadFile(hFile, lpBuf, 0x88, &dwRead, NULL);
			memcpy_s(computerUuid, 0x36, (lpBuf + 0x52), 0x36);
			memcpy_s((computerUuid + 0x36), 0x8, (lpBuf + 0x18), 0x8);
			WriteLog(computerUuid);
		}
		else
		{
			printf("无法创建文件夹");
		}
	}
	// 关闭文件并退出
	CloseHandle(hFile);
	system("pause");
	return 0;
}

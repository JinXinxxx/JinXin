/*********************************************************************************
  *Copyright(C),Your Company
  *FileName:  // 文件名
  *Author:  //Jinxin
  *Version:  //版本
  *Date:  //完成日期
  *Description:  //用于主要说明此程序文件完成的主要功能
				//与其他模块或函数的接口、输出值、取值范围、
				//含义及参数间的控制、顺序、独立及依赖关系
  *Others:  //其他内容说明
  *Function List:  //主要函数列表，每条记录应包含函数名及功能简要说明
	 1.…………
	 2.…………
  *History:  //修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简介
	 1.Date:
	   Author:
	   Modification:
	 2.…………
**********************************************************************************/

#include "pch.h"
#include "NtfsFileLocation.h"


// 打开磁盘
BOOL OpenDisk(HANDLE &hFile, char *lpszFilePath)
{
	// 构造磁盘设备字符串
	char szDrive[7] = { '\\', '\\', '.', '\\', 'C', ':', '\0' };
	szDrive[4] = lpszFilePath[0];
	// 打开磁盘
	hFile = CreateFile(szDrive, GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		char szErr[MAX_PATH] = { 0 };
		::wsprintf(szErr, "Create File Error!\nError Code Is:%d\n", ::GetLastError());
		printf("%s", szErr);
		system("pause");
		return FALSE;
	}

	return TRUE;
}


// 从DBR中获取数据：每个扇区字节数、每个簇的扇区数、原文件$MFT的起始簇号
BOOL GetDataFromDBR(HANDLE hFile, WORD &wSizeOfSector, BYTE &bSizeOfCluster, LARGE_INTEGER &liClusterNumberOfMFT)
{
	// 获取扇区大小(2)、簇大小(1)、$MFT起始簇号(8)
	BYTE bBuffer[512] = { 0 };
	DWORD dwRead = 0;
	// 注意：数据读取的大小最小单位是扇区!!!
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, bBuffer, 512, &dwRead, NULL);
	wSizeOfSector = MAKEWORD(bBuffer[0x0B], bBuffer[0x0C]);
	bSizeOfCluster = bBuffer[0x0D];
	liClusterNumberOfMFT.LowPart = MAKELONG(MAKEWORD(bBuffer[0x30], bBuffer[0x31]), MAKEWORD(bBuffer[0x32], bBuffer[0x33]));
	liClusterNumberOfMFT.HighPart = MAKELONG(MAKEWORD(bBuffer[0x34], bBuffer[0x35]), MAKEWORD(bBuffer[0x36], bBuffer[0x37]));

	return TRUE;
}


// 定位文件
BOOL LocationFile(HANDLE hFile, char *lpszFilePath, WORD wSizeOfSector, BYTE bSizeOfCluster, LARGE_INTEGER liMFTOffset, LARGE_INTEGER &liRootOffset)
{
	BYTE bBuffer[1024] = { 0 };
	DWORD dwRead = 0;
	// 分割文件路径
	char szNewFile[MAX_PATH] = { 0 };
	lstrcpy(szNewFile, (lpszFilePath + 3));
	char szDelim[] = "\\";
	char *lpResult = strtok(szNewFile, szDelim);
	BYTE bUnicode[MAX_PATH] = { 0 };
	while (NULL != lpResult)
	{
		BOOL bFlag = FALSE;
		DWORD dwNameOffset = 0;
		// 将分割的目录转换成2字节表示的Unicode数据
		DWORD dwLen = lstrlen(lpResult);
		RtlZeroMemory(bUnicode, MAX_PATH);
		for (DWORD i = 0, j = 0; i < dwLen; i++)
		{
			bUnicode[j++] = lpResult[i];
			bUnicode[j++] = 0;
		}

		// 读取目录的数据，大小为1KB
		SetFilePointer(hFile, liRootOffset.LowPart, &liRootOffset.HighPart, FILE_BEGIN);
		ReadFile(hFile, bBuffer, 1024, &dwRead, NULL);
		// 获取第一个属性的偏移
		WORD wAttributeOffset MAKEWORD(bBuffer[0x14], bBuffer[0x15]);
		// 遍历文件目录的属性
		DWORD dwAttribute = 0;
		DWORD dwSizeOfAttribute = 0;
		while (TRUE)
		{
			if (bFlag)
			{
				break;
			}
			// 获取当前属性
			dwAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset], bBuffer[wAttributeOffset + 1]),
				MAKEWORD(bBuffer[wAttributeOffset + 2], bBuffer[wAttributeOffset + 3]));

			// 判断属性
			
			if (0x90 == dwAttribute)
			{
				bFlag = HandleAttribute_90(bBuffer, wAttributeOffset, bUnicode, dwLen, liMFTOffset, liRootOffset);
			}
			
			else if (0xA0 == dwAttribute)
			{
				bFlag = HandleAttribute_A0(hFile, bBuffer, wSizeOfSector, bSizeOfCluster, wAttributeOffset, bUnicode, dwLen, liMFTOffset, liRootOffset);
			}
			// 获取当前属性的大小
			dwSizeOfAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset + 4], bBuffer[wAttributeOffset + 5]),
				MAKEWORD(bBuffer[wAttributeOffset + 6], bBuffer[wAttributeOffset + 7]));
			// 计算下一属性的偏移
			wAttributeOffset = wAttributeOffset + dwSizeOfAttribute;
		}

		// 继续分割目录
		lpResult = strtok(NULL, szDelim);
	}

	return TRUE;
}


// 0x90属性的处理
// 当一个文件的文件夹数，小于一定的数量的时候，就会使用90属性。即所有数据可以存放在1024字节下，如果大于这个字节数就会使用A0属性
BOOL HandleAttribute_90(BYTE *lpBuffer, WORD wAttributeOffset, BYTE *lpUnicode, DWORD dwLen, LARGE_INTEGER liMFTOffset, LARGE_INTEGER &liRootOffset)
{
	// 先遍历判断0x90属性里是否有此目录或文件(UNICODE)
	// 获取当前属性的大小
	DWORD dwSizeOfAttribute = MAKELONG(MAKEWORD(lpBuffer[wAttributeOffset + 4], lpBuffer[wAttributeOffset + 5]),
		MAKEWORD(lpBuffer[wAttributeOffset + 6], lpBuffer[wAttributeOffset + 7]));
	for (DWORD i = 0; i < dwSizeOfAttribute; i++)
	{
		if (CompareMemory(lpUnicode, (lpBuffer + wAttributeOffset + i), 2 * dwLen))
		{
			DWORD dwNameOffset = wAttributeOffset + i;
			// 计算文件号
			dwNameOffset = dwNameOffset / 8;
			dwNameOffset = 8 * dwNameOffset;
			dwNameOffset = dwNameOffset - 0x50;
			// 获取文件号(6)
			LARGE_INTEGER liNumberOfFile;
			liNumberOfFile.LowPart = MAKELONG(MAKEWORD(lpBuffer[dwNameOffset], lpBuffer[dwNameOffset + 1]),
				MAKEWORD(lpBuffer[dwNameOffset + 2], lpBuffer[dwNameOffset + 3]));
			liNumberOfFile.HighPart = MAKELONG(MAKEWORD(lpBuffer[dwNameOffset + 4], lpBuffer[dwNameOffset + 5]),
				0);

			// 计算文件号的偏移,文件号是相对$MFT为偏移说的
			liRootOffset = liNumberOfFile;
			liRootOffset.QuadPart = liMFTOffset.QuadPart + liRootOffset.QuadPart * 0x400;

			return TRUE;
		}
	}
	// 读取Data Run List，去到索引处INDX遍历UNICODE，获取文件号

	return FALSE;
}


// 0xA0属性的处理
BOOL HandleAttribute_A0(HANDLE hFile, BYTE *lpBuffer, WORD wSizeOfSector, BYTE bSizeOfCluster, WORD wAttributeOffset, BYTE *lpUnicode, DWORD dwLen, LARGE_INTEGER liMFTOffset, LARGE_INTEGER &liRootOffset)
{
	// 读取Data Run List，去到索引处INDX遍历UNICODE，获取文件号
	DWORD dwCount = 0;
	DWORD dwClusterOffet = 0;
	DWORD dwFlag = 0;
	// 获取索引号的偏移
	WORD wIndxOffset = MAKEWORD(lpBuffer[wAttributeOffset + 0x20], lpBuffer[wAttributeOffset + 0x21]);
	// 读取Data Run List
	while (TRUE)
	{
		dwFlag = 0;
		BYTE bTemp = lpBuffer[wAttributeOffset + wIndxOffset + dwCount];
		// 读取Data Run List,分解并计算Data Run中的信息
		BYTE bHi = bTemp >> 4;
		BYTE bLo = bTemp & 0x0F;
		if (0x0F == bHi || 0x0F == bLo || 0 == bHi || 0 == bLo)
		{
			break;
		}
		LARGE_INTEGER liDataRunSize, liDataRunOffset;
		liDataRunSize.QuadPart = 0;
		liDataRunOffset.QuadPart = 0;
		for (DWORD i = bLo; i > 0; i--)
		{
			liDataRunSize.QuadPart = liDataRunSize.QuadPart << 8;
			liDataRunSize.QuadPart = liDataRunSize.QuadPart | lpBuffer[wAttributeOffset + wIndxOffset + dwCount + i];
		}
		for (DWORD i = bHi; i > 0; i--)
		{
			liDataRunOffset.QuadPart = liDataRunOffset.QuadPart << 8;
			liDataRunOffset.QuadPart = liDataRunOffset.QuadPart | lpBuffer[wAttributeOffset + wIndxOffset + dwCount + bLo + i];
		}
		// 注意加上上一个Data Run的逻辑簇号
		dwFlag = 1 << (bHi * 8 - 1);
		if ((liDataRunOffset.QuadPart & dwFlag) > 0) {
			liDataRunOffset.QuadPart = (((1 << (bHi * 8)) - 1) ^ liDataRunOffset.QuadPart) + 1;
			liDataRunOffset.QuadPart = dwClusterOffet - liDataRunOffset.QuadPart;
			dwClusterOffet = liDataRunOffset.QuadPart;
		}
		else {
			liDataRunOffset.QuadPart = liDataRunOffset.QuadPart + dwClusterOffet;
			dwClusterOffet = liDataRunOffset.QuadPart;
		}
		// 去到索引处INDX遍历UNICODE，获取文件号
		LARGE_INTEGER liIndxOffset, liIndxSize;
		liIndxOffset.QuadPart = liDataRunOffset.QuadPart * bSizeOfCluster * wSizeOfSector;
		liIndxSize.QuadPart = liDataRunSize.QuadPart * bSizeOfCluster * wSizeOfSector;
		// 读取索引的数据，大小为1KB
		BYTE *lpBuf = new BYTE[liIndxSize.QuadPart];
		DWORD dwRead = 0;
		SetFilePointer(hFile, liIndxOffset.LowPart, &liIndxOffset.HighPart, FILE_BEGIN);
		ReadFile(hFile, lpBuf, liIndxSize.LowPart, &dwRead, NULL);
		// 遍历Unicode数据
		for (DWORD i = 0; i < liIndxSize.LowPart; i++)
		{
			if (CompareMemory(lpUnicode, (lpBuf + i), 2 * dwLen))
			{
				DWORD dwNameOffset = i;
				// 计算文件号
				dwNameOffset = dwNameOffset / 8;
				dwNameOffset = 8 * dwNameOffset;
				dwNameOffset = dwNameOffset - 0x50;
				// 获取文件号(6)
				LARGE_INTEGER liNumberOfFile;
				liNumberOfFile.LowPart = MAKELONG(MAKEWORD(lpBuf[dwNameOffset], lpBuf[dwNameOffset + 1]),
					MAKEWORD(lpBuf[dwNameOffset + 2], lpBuf[dwNameOffset + 3]));
				liNumberOfFile.HighPart = MAKELONG(MAKEWORD(lpBuf[dwNameOffset + 4], lpBuf[dwNameOffset + 5]),
					0);

				// 计算文件号的偏移,文件号是相对$MFT为偏移说的
				liRootOffset = liNumberOfFile;
				liRootOffset.QuadPart = liMFTOffset.QuadPart + liRootOffset.QuadPart * 0x400;

				return TRUE;
			}
		}
		delete[]lpBuf;
		lpBuf = NULL;
		// 计算下一个Data Run List偏移
		dwCount = dwCount + bLo + bHi + 1;
	}

	return FALSE;
}


// 内存匹配
BOOL CompareMemory(BYTE *lpSrc, BYTE *lpDst, DWORD dwLen)
{
	if (0 == _memicmp(lpSrc, lpDst, dwLen))
	{
		return TRUE;
	}
	else
	{
		// 此方法仅用于6个字符以上
		if (12 >= dwLen)
		{
			return FALSE;
		}
		// 判断前后两个字符是否匹配
		if ((lpSrc[0] != lpDst[0]) || (lpSrc[1] != lpDst[1]) ||
			(lpSrc[dwLen - 2] != lpDst[dwLen - 2]) || (lpSrc[dwLen - 1] != lpDst[dwLen - 1]))
		{
			return FALSE;
		}
		// 前后字符匹配后，只允许2个字符不一样
		DWORD dwCount = 0;
		for (DWORD i = 0; i < dwLen; i++)
		{
			if (lpSrc[i] == lpDst[i])
			{
				dwCount++;
			}
		}
		if (2 < (dwLen - dwCount))
		{
			return FALSE;
		}
	}
	return TRUE;
}


// 获取文件数据

BOOL myDiskData(HANDLE hFile, WORD wSizeOfSector, BYTE bSizeOfCluster, LARGE_INTEGER liMFTOffset, LARGE_INTEGER liRootOffset, BYTE *computerUuid)
{
	//因为选取文件夹特殊的原因，所以直接寻找A0属性
	LARGE_INTEGER	liDataRunSize = { 0 };
	LARGE_INTEGER	liDataRunOffset = { 0 };
	LARGE_INTEGER	liIndxOffset = { 0 };
	LARGE_INTEGER	liIndxSize = { 0 };
	DWORD			dwRead = 0;
	DWORD			dwAttribute = 0;
	DWORD			dwSizeOfAttribute = 0;
	DWORD			dwIndexOffset = 0;
	BOOL			bRet = TRUE;
	WORD			wAttributeOffset = 0;
	WORD			wSizeOfIndex = 0;
	BYTE			bHi = 0;
	BYTE			bLow = 0;
	BYTE			bBuffer[1024] = { 0 };
	SetFilePointer(hFile, liRootOffset.LowPart, &liRootOffset.HighPart, FILE_BEGIN);
	ReadFile(hFile, bBuffer, 1024, &dwRead, NULL);
	wAttributeOffset = MAKEWORD(bBuffer[0x14], bBuffer[0x15]);
	// 遍历文件目录的属性
	//读取A0偏移处的代码，因为这里肯定只有一个Data_Run所以可以非常容易取得
	while (TRUE) {
		dwAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset], bBuffer[wAttributeOffset + 1]),
			MAKEWORD(bBuffer[wAttributeOffset + 2], bBuffer[wAttributeOffset + 3]));

		// 判断属性
		if (0xA0 == dwAttribute)
		{
			break;
		}
		// 获取当前属性的大小
		dwSizeOfAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset + 4], bBuffer[wAttributeOffset + 5]),
			MAKEWORD(bBuffer[wAttributeOffset + 6], bBuffer[wAttributeOffset + 7]));
		// 计算下一属性的偏移
		wAttributeOffset = wAttributeOffset + dwSizeOfAttribute;
	}

	//遍历Index，获取最后一个位置的名称
	//已经获取到了A0属性的偏移 wAttributeOffset然后偏移20字节后取2字节得到Data RUN的偏移地址。
	wAttributeOffset = MAKEWORD(bBuffer[wAttributeOffset + 0x20], bBuffer[wAttributeOffset + 0x21]) + wAttributeOffset;
	bHi = bBuffer[wAttributeOffset] >> 4;
	bLow = bBuffer[wAttributeOffset] & 0x0F;
	for (DWORD i = bLow; i > 0; i--)
	{
		liDataRunSize.QuadPart = liDataRunSize.QuadPart << 8;
		liDataRunSize.QuadPart = liDataRunSize.QuadPart | bBuffer[wAttributeOffset + i];
	}
	for (DWORD i = bHi; i > 0; i--)
	{
		liDataRunOffset.QuadPart = liDataRunOffset.QuadPart << 8;
		liDataRunOffset.QuadPart = liDataRunOffset.QuadPart | bBuffer[wAttributeOffset + bLow + i];
	}
	//INDEX块的大小
	liIndxOffset.QuadPart = liDataRunOffset.QuadPart * bSizeOfCluster * wSizeOfSector;
	liIndxSize.QuadPart = liDataRunSize.QuadPart * bSizeOfCluster * wSizeOfSector;
	BYTE *lpBuf = new BYTE[liIndxSize.QuadPart];
	SetFilePointer(hFile, liIndxOffset.LowPart, &liIndxOffset.HighPart, FILE_BEGIN);
	ReadFile(hFile, lpBuf, liIndxSize.LowPart, &dwRead, NULL);
	//获取第一个索引项
	dwIndexOffset = MAKELONG(MAKEWORD(lpBuf[0x18], lpBuf[0x19]),
		MAKEWORD(lpBuf[0x1A], lpBuf[0x1B])) + 0x18;
	//直接获得最后一个我们安插的最后一个索引块的长度
	while (TRUE)
	{
		if (lpBuf[dwIndexOffset] == 0) {
			dwIndexOffset = dwIndexOffset + 0x88 * 4;
			break;
		}
		wSizeOfIndex = MAKEWORD(lpBuf[dwIndexOffset + 0x8], lpBuf[dwIndexOffset + 0x9]);
		dwIndexOffset += wSizeOfIndex;
	}
	//取0x52处的值，判断是否为我们的唯一码，首先是汉字判断，由于上面的原因，此处的值肯定不包含文件夹
	if (lpBuf[dwIndexOffset + 0x52] == 0) {
		//保存当前位置。准备执行生成唯一码
		liRootOffset.QuadPart = liIndxOffset.QuadPart + dwIndexOffset;
		bRet = FALSE;

	}
	else {
		//获取唯一码信息0x52处的字符串长度为36，以及0x18处的创建时间长度为8
		memcpy_s(computerUuid, 0x36, (lpBuf + dwIndexOffset + 0x52), 0x36);
		memcpy_s((computerUuid + 0x36), 0x8, (lpBuf + dwIndexOffset + 0x18), 0x8);
		setlocale(LC_ALL, "");
		printf("%ls", computerUuid);
		WriteLog(computerUuid);
	}
	return bRet;
}


//记录信息因为Printf打印不出来
void WriteLog(BYTE *szLog)
{
	SYSTEMTIME st;
	GetLocalTime(&st);
	FILE *fp;
	fp = fopen("D:\\log.txt", "at");
	fwprintf(fp, L"MyLogInfo: %d:%d:%d:%d ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	fwprintf(fp,L"%x%ls%x%x\n" ,*(PWORD)szLog,(szLog + 0x2),*(PLONG)(szLog + 0X36), *(PLONG)(szLog + 0X3A));
	fclose(fp);
	//OutputDebugStringW(szLog);
}
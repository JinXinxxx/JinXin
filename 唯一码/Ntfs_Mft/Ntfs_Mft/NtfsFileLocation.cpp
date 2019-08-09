/*********************************************************************************
  *Copyright(C),Your Company
  *FileName:  // �ļ���
  *Author:  //Jinxin
  *Version:  //�汾
  *Date:  //�������
  *Description:  //������Ҫ˵���˳����ļ���ɵ���Ҫ����
				//������ģ������Ľӿڡ����ֵ��ȡֵ��Χ��
				//���弰������Ŀ��ơ�˳�򡢶�����������ϵ
  *Others:  //��������˵��
  *Function List:  //��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
	 1.��������
	 2.��������
  *History:  //�޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ��
	 1.Date:
	   Author:
	   Modification:
	 2.��������
**********************************************************************************/

#include "pch.h"
#include "NtfsFileLocation.h"


// �򿪴���
BOOL OpenDisk(HANDLE &hFile, char *lpszFilePath)
{
	// ��������豸�ַ���
	char szDrive[7] = { '\\', '\\', '.', '\\', 'C', ':', '\0' };
	szDrive[4] = lpszFilePath[0];
	// �򿪴���
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


// ��DBR�л�ȡ���ݣ�ÿ�������ֽ�����ÿ���ص���������ԭ�ļ�$MFT����ʼ�غ�
BOOL GetDataFromDBR(HANDLE hFile, WORD &wSizeOfSector, BYTE &bSizeOfCluster, LARGE_INTEGER &liClusterNumberOfMFT)
{
	// ��ȡ������С(2)���ش�С(1)��$MFT��ʼ�غ�(8)
	BYTE bBuffer[512] = { 0 };
	DWORD dwRead = 0;
	// ע�⣺���ݶ�ȡ�Ĵ�С��С��λ������!!!
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	ReadFile(hFile, bBuffer, 512, &dwRead, NULL);
	wSizeOfSector = MAKEWORD(bBuffer[0x0B], bBuffer[0x0C]);
	bSizeOfCluster = bBuffer[0x0D];
	liClusterNumberOfMFT.LowPart = MAKELONG(MAKEWORD(bBuffer[0x30], bBuffer[0x31]), MAKEWORD(bBuffer[0x32], bBuffer[0x33]));
	liClusterNumberOfMFT.HighPart = MAKELONG(MAKEWORD(bBuffer[0x34], bBuffer[0x35]), MAKEWORD(bBuffer[0x36], bBuffer[0x37]));

	return TRUE;
}


// ��λ�ļ�
BOOL LocationFile(HANDLE hFile, char *lpszFilePath, WORD wSizeOfSector, BYTE bSizeOfCluster, LARGE_INTEGER liMFTOffset, LARGE_INTEGER &liRootOffset)
{
	BYTE bBuffer[1024] = { 0 };
	DWORD dwRead = 0;
	// �ָ��ļ�·��
	char szNewFile[MAX_PATH] = { 0 };
	lstrcpy(szNewFile, (lpszFilePath + 3));
	char szDelim[] = "\\";
	char *lpResult = strtok(szNewFile, szDelim);
	BYTE bUnicode[MAX_PATH] = { 0 };
	while (NULL != lpResult)
	{
		BOOL bFlag = FALSE;
		DWORD dwNameOffset = 0;
		// ���ָ��Ŀ¼ת����2�ֽڱ�ʾ��Unicode����
		DWORD dwLen = lstrlen(lpResult);
		RtlZeroMemory(bUnicode, MAX_PATH);
		for (DWORD i = 0, j = 0; i < dwLen; i++)
		{
			bUnicode[j++] = lpResult[i];
			bUnicode[j++] = 0;
		}

		// ��ȡĿ¼�����ݣ���СΪ1KB
		SetFilePointer(hFile, liRootOffset.LowPart, &liRootOffset.HighPart, FILE_BEGIN);
		ReadFile(hFile, bBuffer, 1024, &dwRead, NULL);
		// ��ȡ��һ�����Ե�ƫ��
		WORD wAttributeOffset MAKEWORD(bBuffer[0x14], bBuffer[0x15]);
		// �����ļ�Ŀ¼������
		DWORD dwAttribute = 0;
		DWORD dwSizeOfAttribute = 0;
		while (TRUE)
		{
			if (bFlag)
			{
				break;
			}
			// ��ȡ��ǰ����
			dwAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset], bBuffer[wAttributeOffset + 1]),
				MAKEWORD(bBuffer[wAttributeOffset + 2], bBuffer[wAttributeOffset + 3]));

			// �ж�����
			
			if (0x90 == dwAttribute)
			{
				bFlag = HandleAttribute_90(bBuffer, wAttributeOffset, bUnicode, dwLen, liMFTOffset, liRootOffset);
			}
			
			else if (0xA0 == dwAttribute)
			{
				bFlag = HandleAttribute_A0(hFile, bBuffer, wSizeOfSector, bSizeOfCluster, wAttributeOffset, bUnicode, dwLen, liMFTOffset, liRootOffset);
			}
			// ��ȡ��ǰ���ԵĴ�С
			dwSizeOfAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset + 4], bBuffer[wAttributeOffset + 5]),
				MAKEWORD(bBuffer[wAttributeOffset + 6], bBuffer[wAttributeOffset + 7]));
			// ������һ���Ե�ƫ��
			wAttributeOffset = wAttributeOffset + dwSizeOfAttribute;
		}

		// �����ָ�Ŀ¼
		lpResult = strtok(NULL, szDelim);
	}

	return TRUE;
}


// 0x90���ԵĴ���
// ��һ���ļ����ļ�������С��һ����������ʱ�򣬾ͻ�ʹ��90���ԡ����������ݿ��Դ����1024�ֽ��£������������ֽ����ͻ�ʹ��A0����
BOOL HandleAttribute_90(BYTE *lpBuffer, WORD wAttributeOffset, BYTE *lpUnicode, DWORD dwLen, LARGE_INTEGER liMFTOffset, LARGE_INTEGER &liRootOffset)
{
	// �ȱ����ж�0x90�������Ƿ��д�Ŀ¼���ļ�(UNICODE)
	// ��ȡ��ǰ���ԵĴ�С
	DWORD dwSizeOfAttribute = MAKELONG(MAKEWORD(lpBuffer[wAttributeOffset + 4], lpBuffer[wAttributeOffset + 5]),
		MAKEWORD(lpBuffer[wAttributeOffset + 6], lpBuffer[wAttributeOffset + 7]));
	for (DWORD i = 0; i < dwSizeOfAttribute; i++)
	{
		if (CompareMemory(lpUnicode, (lpBuffer + wAttributeOffset + i), 2 * dwLen))
		{
			DWORD dwNameOffset = wAttributeOffset + i;
			// �����ļ���
			dwNameOffset = dwNameOffset / 8;
			dwNameOffset = 8 * dwNameOffset;
			dwNameOffset = dwNameOffset - 0x50;
			// ��ȡ�ļ���(6)
			LARGE_INTEGER liNumberOfFile;
			liNumberOfFile.LowPart = MAKELONG(MAKEWORD(lpBuffer[dwNameOffset], lpBuffer[dwNameOffset + 1]),
				MAKEWORD(lpBuffer[dwNameOffset + 2], lpBuffer[dwNameOffset + 3]));
			liNumberOfFile.HighPart = MAKELONG(MAKEWORD(lpBuffer[dwNameOffset + 4], lpBuffer[dwNameOffset + 5]),
				0);

			// �����ļ��ŵ�ƫ��,�ļ��������$MFTΪƫ��˵��
			liRootOffset = liNumberOfFile;
			liRootOffset.QuadPart = liMFTOffset.QuadPart + liRootOffset.QuadPart * 0x400;

			return TRUE;
		}
	}
	// ��ȡData Run List��ȥ��������INDX����UNICODE����ȡ�ļ���

	return FALSE;
}


// 0xA0���ԵĴ���
BOOL HandleAttribute_A0(HANDLE hFile, BYTE *lpBuffer, WORD wSizeOfSector, BYTE bSizeOfCluster, WORD wAttributeOffset, BYTE *lpUnicode, DWORD dwLen, LARGE_INTEGER liMFTOffset, LARGE_INTEGER &liRootOffset)
{
	// ��ȡData Run List��ȥ��������INDX����UNICODE����ȡ�ļ���
	DWORD dwCount = 0;
	DWORD dwClusterOffet = 0;
	DWORD dwFlag = 0;
	// ��ȡ�����ŵ�ƫ��
	WORD wIndxOffset = MAKEWORD(lpBuffer[wAttributeOffset + 0x20], lpBuffer[wAttributeOffset + 0x21]);
	// ��ȡData Run List
	while (TRUE)
	{
		dwFlag = 0;
		BYTE bTemp = lpBuffer[wAttributeOffset + wIndxOffset + dwCount];
		// ��ȡData Run List,�ֽⲢ����Data Run�е���Ϣ
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
		// ע�������һ��Data Run���߼��غ�
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
		// ȥ��������INDX����UNICODE����ȡ�ļ���
		LARGE_INTEGER liIndxOffset, liIndxSize;
		liIndxOffset.QuadPart = liDataRunOffset.QuadPart * bSizeOfCluster * wSizeOfSector;
		liIndxSize.QuadPart = liDataRunSize.QuadPart * bSizeOfCluster * wSizeOfSector;
		// ��ȡ���������ݣ���СΪ1KB
		BYTE *lpBuf = new BYTE[liIndxSize.QuadPart];
		DWORD dwRead = 0;
		SetFilePointer(hFile, liIndxOffset.LowPart, &liIndxOffset.HighPart, FILE_BEGIN);
		ReadFile(hFile, lpBuf, liIndxSize.LowPart, &dwRead, NULL);
		// ����Unicode����
		for (DWORD i = 0; i < liIndxSize.LowPart; i++)
		{
			if (CompareMemory(lpUnicode, (lpBuf + i), 2 * dwLen))
			{
				DWORD dwNameOffset = i;
				// �����ļ���
				dwNameOffset = dwNameOffset / 8;
				dwNameOffset = 8 * dwNameOffset;
				dwNameOffset = dwNameOffset - 0x50;
				// ��ȡ�ļ���(6)
				LARGE_INTEGER liNumberOfFile;
				liNumberOfFile.LowPart = MAKELONG(MAKEWORD(lpBuf[dwNameOffset], lpBuf[dwNameOffset + 1]),
					MAKEWORD(lpBuf[dwNameOffset + 2], lpBuf[dwNameOffset + 3]));
				liNumberOfFile.HighPart = MAKELONG(MAKEWORD(lpBuf[dwNameOffset + 4], lpBuf[dwNameOffset + 5]),
					0);

				// �����ļ��ŵ�ƫ��,�ļ��������$MFTΪƫ��˵��
				liRootOffset = liNumberOfFile;
				liRootOffset.QuadPart = liMFTOffset.QuadPart + liRootOffset.QuadPart * 0x400;

				return TRUE;
			}
		}
		delete[]lpBuf;
		lpBuf = NULL;
		// ������һ��Data Run Listƫ��
		dwCount = dwCount + bLo + bHi + 1;
	}

	return FALSE;
}


// �ڴ�ƥ��
BOOL CompareMemory(BYTE *lpSrc, BYTE *lpDst, DWORD dwLen)
{
	if (0 == _memicmp(lpSrc, lpDst, dwLen))
	{
		return TRUE;
	}
	else
	{
		// �˷���������6���ַ�����
		if (12 >= dwLen)
		{
			return FALSE;
		}
		// �ж�ǰ�������ַ��Ƿ�ƥ��
		if ((lpSrc[0] != lpDst[0]) || (lpSrc[1] != lpDst[1]) ||
			(lpSrc[dwLen - 2] != lpDst[dwLen - 2]) || (lpSrc[dwLen - 1] != lpDst[dwLen - 1]))
		{
			return FALSE;
		}
		// ǰ���ַ�ƥ���ֻ����2���ַ���һ��
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


// ��ȡ�ļ�����

BOOL myDiskData(HANDLE hFile, WORD wSizeOfSector, BYTE bSizeOfCluster, LARGE_INTEGER liMFTOffset, LARGE_INTEGER liRootOffset, BYTE *computerUuid)
{
	//��Ϊѡȡ�ļ��������ԭ������ֱ��Ѱ��A0����
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
	// �����ļ�Ŀ¼������
	//��ȡA0ƫ�ƴ��Ĵ��룬��Ϊ����϶�ֻ��һ��Data_Run���Կ��Էǳ�����ȡ��
	while (TRUE) {
		dwAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset], bBuffer[wAttributeOffset + 1]),
			MAKEWORD(bBuffer[wAttributeOffset + 2], bBuffer[wAttributeOffset + 3]));

		// �ж�����
		if (0xA0 == dwAttribute)
		{
			break;
		}
		// ��ȡ��ǰ���ԵĴ�С
		dwSizeOfAttribute = MAKELONG(MAKEWORD(bBuffer[wAttributeOffset + 4], bBuffer[wAttributeOffset + 5]),
			MAKEWORD(bBuffer[wAttributeOffset + 6], bBuffer[wAttributeOffset + 7]));
		// ������һ���Ե�ƫ��
		wAttributeOffset = wAttributeOffset + dwSizeOfAttribute;
	}

	//����Index����ȡ���һ��λ�õ�����
	//�Ѿ���ȡ����A0���Ե�ƫ�� wAttributeOffsetȻ��ƫ��20�ֽں�ȡ2�ֽڵõ�Data RUN��ƫ�Ƶ�ַ��
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
	//INDEX��Ĵ�С
	liIndxOffset.QuadPart = liDataRunOffset.QuadPart * bSizeOfCluster * wSizeOfSector;
	liIndxSize.QuadPart = liDataRunSize.QuadPart * bSizeOfCluster * wSizeOfSector;
	BYTE *lpBuf = new BYTE[liIndxSize.QuadPart];
	SetFilePointer(hFile, liIndxOffset.LowPart, &liIndxOffset.HighPart, FILE_BEGIN);
	ReadFile(hFile, lpBuf, liIndxSize.LowPart, &dwRead, NULL);
	//��ȡ��һ��������
	dwIndexOffset = MAKELONG(MAKEWORD(lpBuf[0x18], lpBuf[0x19]),
		MAKEWORD(lpBuf[0x1A], lpBuf[0x1B])) + 0x18;
	//ֱ�ӻ�����һ�����ǰ�������һ��������ĳ���
	while (TRUE)
	{
		if (lpBuf[dwIndexOffset] == 0) {
			dwIndexOffset = dwIndexOffset + 0x88 * 4;
			break;
		}
		wSizeOfIndex = MAKEWORD(lpBuf[dwIndexOffset + 0x8], lpBuf[dwIndexOffset + 0x9]);
		dwIndexOffset += wSizeOfIndex;
	}
	//ȡ0x52����ֵ���ж��Ƿ�Ϊ���ǵ�Ψһ�룬�����Ǻ����жϣ����������ԭ�򣬴˴���ֵ�϶��������ļ���
	if (lpBuf[dwIndexOffset + 0x52] == 0) {
		//���浱ǰλ�á�׼��ִ������Ψһ��
		liRootOffset.QuadPart = liIndxOffset.QuadPart + dwIndexOffset;
		bRet = FALSE;

	}
	else {
		//��ȡΨһ����Ϣ0x52�����ַ�������Ϊ36���Լ�0x18���Ĵ���ʱ�䳤��Ϊ8
		memcpy_s(computerUuid, 0x36, (lpBuf + dwIndexOffset + 0x52), 0x36);
		memcpy_s((computerUuid + 0x36), 0x8, (lpBuf + dwIndexOffset + 0x18), 0x8);
		setlocale(LC_ALL, "");
		printf("%ls", computerUuid);
		WriteLog(computerUuid);
	}
	return bRet;
}


//��¼��Ϣ��ΪPrintf��ӡ������
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
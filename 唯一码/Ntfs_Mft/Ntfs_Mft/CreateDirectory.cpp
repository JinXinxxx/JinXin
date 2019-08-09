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
#include "CreateDirectory.h"



BOOL myCreateDirectoryAndDeleteDirectory() {
	BOOL bRet = TRUE;
	UUID Uuid = { 0 };
	WCHAR unicodeCharacter[4] = { 0 };
	UnicodeStringArray unicodeStringArrary[5] = { 0 };
	//连续创建5个目录
	for (int i = 0; i < 5; i++) {
		CreateUUid(&Uuid);
		myGetChineseFileName(unicodeCharacter);
		wsprintfW(unicodeStringArrary[i].unicodeString, L"%ls%x%x%x%x%x", unicodeCharacter, Uuid.Data1, Uuid.Data2, Uuid.Data3, Uuid.Data4 + 2, Uuid.Data4[7]);
		//这里判断一下
		if (!CreateDirectoryW(unicodeStringArrary[i].unicodeString, NULL)) {
			bRet = FALSE;
			break;
		}
		ZeroMemory(&Uuid, sizeof(UUID));
		ZeroMemory(unicodeCharacter, sizeof(unicodeCharacter));
		Sleep(100);
	}
	//连续删除5个目录
	if (bRet) {
		for (int i = 0; i < 5; i++) {
			RemoveDirectoryW(unicodeStringArrary[i].unicodeString);
		}
	}
	return bRet;

}

BOOL CreateUUid(UUID *Uuid) {
	RPC_STATUS rpcStatus = UuidCreateSequential(Uuid);
	if (rpcStatus == RPC_S_OK || rpcStatus == RPC_S_UUID_LOCAL_ONLY) {
		return TRUE;
	}
	return FALSE;
}

BOOL myGetChineseFileName(WCHAR *unicodeCharacter) {
	DWORD wordOffset = 0;
	DWORD wordSize = 0;
	BOOL bRet = TRUE;
	//WCHAR unicodeCharacter[4] = { 0 };
	struct timeb timeSeed;
	ftime(&timeSeed);
	srand(timeSeed.time * 1000 + timeSeed.millitm);
	DWORD selectChinese = rand() % 3;
	switch (selectChinese) {
	case 0: {
		wordOffset = 0x4E00;
		wordSize = 20902;
		break;
	}
	case 1: {
		wordOffset = 0x3400;
		wordSize = 6582;
		break;
	}
	case 2: {
		wordOffset = 0x3400;
		wordSize = 6582;
		break;
	}
			/* 后面可以考虑加
			case 3: {
				wordOffset = 0x20000;
				wordSize = 42711;
				break;
			}
			case 4: {
				wordOffset = 0x2a700;
				wordSize = 4149;
				break;
			}
			case 5: {
				wordOffset = 0x2B820;
				wordSize = 5762;
				break;
			}
			case 6: {
				wordOffset = 0x2CEB0;
				wordSize = 7473;
				break;
			}
			*/
	default:
		bRet = FALSE;
		break;
	}
	DWORD a = rand();
	wordOffset += rand() % wordSize;
	unicodeCharacter[0] = wordOffset;
	return TRUE;
}
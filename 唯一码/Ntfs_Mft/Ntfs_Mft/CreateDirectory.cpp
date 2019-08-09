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
#include "CreateDirectory.h"



BOOL myCreateDirectoryAndDeleteDirectory() {
	BOOL bRet = TRUE;
	UUID Uuid = { 0 };
	WCHAR unicodeCharacter[4] = { 0 };
	UnicodeStringArray unicodeStringArrary[5] = { 0 };
	//��������5��Ŀ¼
	for (int i = 0; i < 5; i++) {
		CreateUUid(&Uuid);
		myGetChineseFileName(unicodeCharacter);
		wsprintfW(unicodeStringArrary[i].unicodeString, L"%ls%x%x%x%x%x", unicodeCharacter, Uuid.Data1, Uuid.Data2, Uuid.Data3, Uuid.Data4 + 2, Uuid.Data4[7]);
		//�����ж�һ��
		if (!CreateDirectoryW(unicodeStringArrary[i].unicodeString, NULL)) {
			bRet = FALSE;
			break;
		}
		ZeroMemory(&Uuid, sizeof(UUID));
		ZeroMemory(unicodeCharacter, sizeof(unicodeCharacter));
		Sleep(100);
	}
	//����ɾ��5��Ŀ¼
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
			/* ������Կ��Ǽ�
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
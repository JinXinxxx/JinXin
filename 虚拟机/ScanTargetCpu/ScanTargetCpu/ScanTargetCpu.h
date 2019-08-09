/*********************************************************************************
  *Copyright(C),Your Company
  *FileName:  // �ļ���
  *Author:  //Jinxin
  *Version:  //�汾
  *Date:  //�������
  *Description:  //ʹ��CPUid��ȡ�ж�CPU��AMD����Intel
				//ʹ�÷�װ�ĺ�����ȡCPU�ĺ��������Լ�CPU��L1����Ŀ�����ж��Ƿ�Ϊ�������
  *Others:  //��������˵��
  *Function List:  //��Ҫ�����б�ÿ����¼Ӧ���������������ܼ�Ҫ˵��
	 1.myIsIntelCpu �ж�CPU�ͺ�
	 2.myGetCpuInformation �ռ�CPU��Ϣ
	 3.myIsVirtualPc �ж��Ƿ�Ϊ�����
  *History:  //�޸���ʷ��¼�б�ÿ���޸ļ�¼Ӧ�����޸����ڡ��޸��߼��޸����ݼ��
	 1.Date:
	   Author:
	   Modification:
	 2.��������
**********************************************************************************/

#include <iostream>
#include <intrin.h>
#include <windows.h>

#define Amd 0
#define Intel 1


typedef BOOL(WINAPI *LPFN_GLPI)(
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION,
	PDWORD);

typedef struct _CPU_INFORMATION {
	DWORD numberOfPackage;
	DWORD numberOfCore;
	DWORD numberOfL1Cache;
	DWORD numberOfPerCoreAboutL1;
}CpuInformation, *PCpuInformation;

BOOL myIsIntelCpu();

BOOL myIsVirtualPc();

BOOL myGetCpuInformation();

DWORD myGetPerCoreL1Cache();

VOID myGetCpuName(char * szCpuName);

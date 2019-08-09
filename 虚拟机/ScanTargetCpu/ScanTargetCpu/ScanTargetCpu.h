/*********************************************************************************
  *Copyright(C),Your Company
  *FileName:  // 文件名
  *Author:  //Jinxin
  *Version:  //版本
  *Date:  //完成日期
  *Description:  //使用CPUid获取判断CPU是AMD还是Intel
				//使用封装的函数获取CPU的核心数，以及CPU的L1的数目，来判断是否为虚拟机。
  *Others:  //其他内容说明
  *Function List:  //主要函数列表，每条记录应包含函数名及功能简要说明
	 1.myIsIntelCpu 判断CPU型号
	 2.myGetCpuInformation 收集CPU信息
	 3.myIsVirtualPc 判断是否为虚拟机
  *History:  //修改历史记录列表，每条修改记录应包含修改日期、修改者及修改内容简介
	 1.Date:
	   Author:
	   Modification:
	 2.…………
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

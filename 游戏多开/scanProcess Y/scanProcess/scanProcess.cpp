// scanProcess.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。


#include "pch.h"
#include"scanProcess.h"
int main()
{
	while (TRUE) {
		DWORD gameCount = 0;
		gameCount = myScanProcess();
		printf("游戏数量为:%d\n", gameCount);
		Sleep(1000);
	}
}

DWORD myScanProcess() {
	DWORD currentProcessPid = 0;
	DWORD gameCount = 1;
	PROCESSENTRY32 pe32 = { 0 };
	currentProcessPid = GetCurrentProcessId();
	pe32.dwSize = sizeof(PROCESSENTRY32);
	do {
		HANDLE hProcessSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnapshot == INVALID_HANDLE_VALUE) {
			gameCount = -1;
			break;
		}
		Process32First(hProcessSnapshot, &pe32);
		while (Process32Next(hProcessSnapshot, &pe32)) {
			//检查Game.exe不能作为判断依据，Game.exe可能有很多所以获取其Path/TLBB/Bin又因为Path可能不是TLBB，所以这些判断都有问题，最好枚举每个进程中的模块
			//或者判断进程文件夹下面Game.exe的描述信息文件说明，版权,首先排除一个PID即本进程PID
			if (pe32.th32ProcessID == currentProcessPid) {
				continue;
			}
			/*测试定位用
			if (memcmp(pe32.szExeFile, "Game.exe", 8) == 0) {
				printf("a");
			}
			*/
			if (myScanModule_2(pe32.th32ProcessID)) {
				gameCount++;
			}
		}
		CloseHandle(hProcessSnapshot);
	} while (FALSE);
	return gameCount;
}

BOOL myScanModule(DWORD pid) {
	BOOL bRet = TRUE;
	MODULEENTRY32 module32 = { 0 };
	char szFullPath[MAX_PATH] = { 0 };
	char szFileDescription[MAX_PATH] = { 0 };
	char *szExeFile = NULL;
	module32.dwSize = sizeof(MODULEENTRY32);
	HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	do {
		if (hModuleSnapshot == INVALID_HANDLE_VALUE) {
			bRet = FALSE;
			break;
		}
		if (!Module32First(hModuleSnapshot, &module32)) {
			bRet = FALSE;
			CloseHandle(hModuleSnapshot);
			break;
		}
		szExeFile = strrchr(module32.szExePath, '\\');
		memcpy_s(szFullPath, MAX_PATH, module32.szExePath, strlen(module32.szExePath) - strlen(szExeFile) + 1);
		wsprintf(szFullPath, "%sGame.exe", szFullPath);
		if (!myGetPeInfomarion(szFullPath, "FileDescription", szFileDescription)) {
			bRet = FALSE;
			CloseHandle(hModuleSnapshot);
			break;
		}
		//还可以取一些别的信息
		if (memcmp(szFileDescription, "《新天龙八部》", 7) != 0) {
			bRet = FALSE;
		}
		CloseHandle(hModuleSnapshot);
	} while (FALSE);
	return bRet;
}
//扫描文件信息前面可以换成别的取进程的方法
BOOL myGetPeInfomarion(char *szFullPathName,const char *SourceName, char *szFileDescription) {
	UINT cbTranslate = 0;
	BOOL bRet = TRUE;
	LPTranslate lpTranslate = NULL;
	LPVOID pBlock = NULL;
	UINT dwReturnBytes = 0;
	LPVOID lpszPeInfo = NULL;
	char subBlock[MAX_PATH] = { 0 };
	DWORD iVerInfoSize = GetFileVersionInfoSize(szFullPathName, NULL);
	do {
		if (iVerInfoSize == 0) {
			bRet = FALSE;
			break;
		}
		pBlock = new BYTE[iVerInfoSize];
		ZeroMemory(pBlock, iVerInfoSize);
		if (!GetFileVersionInfoA(szFullPathName, 0, iVerInfoSize, pBlock)) {
			bRet = FALSE;
			break;
		}
		if (!VerQueryValue(pBlock,
			TEXT("\\VarFileInfo\\Translation"),
			(LPVOID*)&lpTranslate,
			&cbTranslate)) {
			bRet = FALSE;
			break;
		}
		
		wsprintf(subBlock, "\\StringFileInfo\\%04x%04x\\%s", lpTranslate[0].wLanguage, lpTranslate[0].wCodePage, SourceName);
		if (!VerQueryValue(pBlock,
			subBlock,
			&lpszPeInfo,
			&dwReturnBytes)) {
			bRet = FALSE;
			break;
		}
		wsprintf(szFileDescription, "%s", lpszPeInfo);
	} while (FALSE);
	if (pBlock != NULL) {
		delete[] pBlock;
	}
	return bRet;
}
//扫描进程中的模块
BOOL myScanModule_2(DWORD pid) {
	BOOL bRet = FALSE;
	MODULEENTRY32 module32 = { 0 };
	char szFullPath[MAX_PATH] = { 0 };
	char szFileDescription[MAX_PATH] = { 0 };
	char *szExeFile = NULL;
	module32.dwSize = sizeof(MODULEENTRY32);
	HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);
	do {
		if (hModuleSnapshot == INVALID_HANDLE_VALUE) {
			break;
		}
		if (!Module32First(hModuleSnapshot, &module32)) {
			CloseHandle(hModuleSnapshot);
			break;
		}
	
		while (Module32Next(hModuleSnapshot, &module32)) {
			//如果说可能碰到同名DLL可以考虑多DLL扫描，或者使用上面扫描文件的信息
			if (memcmp(module32.szModule, "Render.dll", 10) == 0) {
				bRet = TRUE;
			}
		}
		CloseHandle(hModuleSnapshot);
	} while (FALSE);
	return bRet;
}
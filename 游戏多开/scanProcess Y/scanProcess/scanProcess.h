#pragma once
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>


#pragma comment(lib,"Version.lib")
typedef struct LANGANDCODEPAGE {
	WORD wLanguage;
	WORD wCodePage;
} *LPTranslate;

BOOL myGetPeInfomarion(char *szFullPathName, const char *SourceName, char *szFileDescription);
DWORD myScanProcess();
BOOL myScanModule(DWORD pid);
BOOL myScanModule_2(DWORD pid);
#pragma once
#include "pch.h"
#pragma comment(lib, "rpcrt4.lib") 
#include <iostream>
#include <Windows.h>
#include <rpcdce.h>
#include <stdlib.h>
#include <time.h> 
#include <sys/timeb.h>

typedef struct _UnicodeString {
	WCHAR unicodeString[MAX_PATH];
}UnicodeStringArray, *PunicodeStringArray;
BOOL myCreateDirectoryAndDeleteDirectory();
BOOL CreateUUid(UUID * Uuid);

BOOL myGetChineseFileName(WCHAR *unicodeCharacter);

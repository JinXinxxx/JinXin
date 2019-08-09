#pragma once
#include <iostream>
#include <ws2tcpip.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <windows.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

BOOL myGetPcInformation();

BOOL myIsVirtualPc(PBYTE lpMacAddress);

BOOL myGetRouterMacAddress(char * SrcIpString, char * DestIpString, PBYTE routerMacAddress);

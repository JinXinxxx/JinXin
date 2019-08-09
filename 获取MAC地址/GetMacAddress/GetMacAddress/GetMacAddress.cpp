// GetMacAddress.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include "pch.h"
#include "GetMacAddress.h"
int main()
{
	BOOL bRet = myGetPcInformation();
	if (bRet == FALSE) {
		std::cout << "在虚拟机中" << std::endl;
	}
	else {
		std::cout << "不在虚拟机中" << std::endl;
	}
	system("pause");
}



BOOL myGetPcInformation() {
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	BOOL bRet = TRUE;
	BYTE ZeroIpAddress[8] = { 0x30, 0x2e, 0x30, 0x2e, 0x30, 0x2e, 0x30, 0x0};
	BYTE routerMacAddress[16] = { 0 };
	//因为不知道有几个网卡所以需要动态获取一下
	do {
		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL) {
			//printf("Error allocating memory needed to call GetAdaptersinfo\n");
			bRet = FALSE;
			break;
		}
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL) {
				//printf("Error allocating memory needed to call GetAdaptersinfo\n");
				bRet = FALSE;
				break;
			}
		}
		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) != NO_ERROR) {
			//printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);
			bRet = FALSE;
			if (pAdapterInfo) {
				FREE(pAdapterInfo);
			}
			break;
		}
		pAdapter = pAdapterInfo;
		while (pAdapter) {
			if (myIsVirtualPc(pAdapter->Address)) {
				if (memcmp(pAdapter->GatewayList.IpAddress.String, ZeroIpAddress, 8) == 0) {
					pAdapter = pAdapter->Next;
					continue;
				}
				else {
					bRet = FALSE;
					break;
				}
			}
			else {
				if (memcmp(pAdapter->GatewayList.IpAddress.String, ZeroIpAddress, 8) != 0) {
					if (myGetRouterMacAddress(pAdapter->IpAddressList.IpAddress.String, pAdapter->GatewayList.IpAddress.String, routerMacAddress)) {
						if (myIsVirtualPc(routerMacAddress)) {
							bRet = FALSE;
							break;
						}
					}
				}
			}
			pAdapter = pAdapter->Next;
		}
	} while (false);
	return bRet;
}

BOOL myIsVirtualPc(PBYTE lpMacAddress) {
	DWORD nRet = 0;
	DWORD i = 0;
	BOOL bRet = TRUE;
	BYTE virtualMacAddress[] = { 0x00, 0x50, 0X56, 0x0c, 0x29 };
	do {
		if (virtualMacAddress[0] != lpMacAddress[0]) {
			bRet = FALSE;
			break;
		}
		if (memcmp(lpMacAddress + 1, virtualMacAddress + 1, 2) == 0) {
			bRet = TRUE;
			break;
		}
		if (memcmp(lpMacAddress + 1, virtualMacAddress + 3, 2) == 0) {
			bRet = TRUE;
		}

	} while (false);
	return bRet;

}


BOOL myGetRouterMacAddress(char *SrcIpString, char *DestIpString, PBYTE routerMacAddress) {
	DWORD dwRetVal;
	IPAddr DestIp = 0;
	IPAddr SrcIp = 0;
	ULONG MacAddr[2];
	ULONG PhysAddrLen = 6;
	BOOL bRet = FALSE;
	dwRetVal = inet_pton(AF_INET, DestIpString, &DestIp);
	inet_pton(AF_INET, SrcIpString, &SrcIp);
	memset(&MacAddr, 0xff, sizeof(MacAddr));
	dwRetVal = SendARP(DestIp, SrcIp, &MacAddr, &PhysAddrLen);
	if (dwRetVal == NO_ERROR) {
		memcpy_s(routerMacAddress, 16, MacAddr, 16);
		bRet = TRUE;
	}
	return bRet;
}

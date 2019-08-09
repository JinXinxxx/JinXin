// getScreenInformation.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"

#include <Windows.h>  
#include <iostream>  
#include <DXGI.h>  
#include <vector>  
#pragma comment(lib,"DXGI.lib")
using namespace std;
std::string WStringToString(const std::wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}
int main()
{
	// 参数定义  
	IDXGIFactory * pFactory;
	IDXGIAdapter * pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;            // 显卡  


	// 显卡的数量  
	int iAdapterNum = 0;

	
	HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));
	
	if (FAILED(hr))
		return -1;

	// 枚举适配器  
	while (pFactory->EnumAdapters(iAdapterNum, &pAdapter) != DXGI_ERROR_NOT_FOUND)
	{
		vAdapters.push_back(pAdapter);
		++iAdapterNum;
	}

	// 信息输出 
	//虚拟机信息为：
	//设备ID:1029
	//PCI ID修正版本 : 0
	//子系统PIC ID : 67442093
	//厂商编号 : 5549
	cout << "===============获取到" << iAdapterNum << "块显卡===============" << endl;
	for (size_t i = 0; i < vAdapters.size(); i++)
	{
		cout << "===============第" << i+1 << "块显卡===============" << endl;
		// 获取信息  
		DXGI_ADAPTER_DESC adapterDesc;
		vAdapters[i]->GetDesc(&adapterDesc);
		//wstring aa(adapterDesc.Description);
		//std::string bb = WStringToString(aa);
		// 输出显卡信息  
		cout << "系统视频内存:" << adapterDesc.DedicatedSystemMemory / 1024 / 1024 << "M" << endl;
		cout << "专用视频内存:" << adapterDesc.DedicatedVideoMemory / 1024 / 1024 << "M" << endl;
		cout << "共享系统内存:" << adapterDesc.SharedSystemMemory / 1024 / 1024 << "M" << endl;
		//cout << "设备描述:" << bb.c_str() << endl;
		cout << "设备ID:" << adapterDesc.DeviceId << endl;
		if (adapterDesc.DeviceId == 1029) {
			cout << "根据设备ID这个是虚拟机" << endl;
		}
		cout << "PCI ID修正版本:" << adapterDesc.Revision << endl;
		//这个可以先不要因为CPU核显，但CPU核显也可以作为判断虚拟机的一个方法
		/*
		if (adapterDesc.Revision == 0) {
			cout << "根据PCI ID修正版本这个是虚拟机" << endl;
		}
		*/
		cout << "子系统PIC ID:" << adapterDesc.SubSysId << endl;

		if (adapterDesc.SubSysId == 67442093) {
			cout << "根据子系统PIC ID这个是虚拟机" << endl;
		}

		cout << "厂商编号:" << adapterDesc.VendorId << endl;
		if (adapterDesc.VendorId == 5549) {
			cout << "根据厂商编号这个是虚拟机" << endl;
		}
	}
	vAdapters.clear();
	pFactory->Release();
	system("pause");
	return 0;
}

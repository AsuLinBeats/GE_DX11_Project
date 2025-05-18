#pragma once
#include <D3D11.h>
#include<vector>
#include<dxgi1_6.h>

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")
// for laptop with multiple GPU, run NVIDIA GPU by default
// 通知NVIDIA驱动强制启用NVIDIA显卡
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	// _declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; // AMD 版本
}


class Adapter {
public:
	IDXGIAdapter1* adapter; // point to interface Adapter
	// define what adaptor should use
	void FindAdapter() {
		// defind all adaptors in sys.
		IDXGIAdapter1* adapterf;
		std::vector<IDXGIAdapter1*> adapters;
		IDXGIFactory6* factory = NULL;
		// 创建DXGI工厂
		CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);
		int i = 0;
		// 枚举所有Adapter并保存到数组
		while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND)
		{
			adapters.push_back(adapterf);
			i++;
		}
		// find the best adapter through comparing max video memory
		long long maxVideoMemory = 0;
		int useAdapterIndex = 0;
			// 遍历选择最佳adapter
		for (int i = 0; i < adapters.size(); i++)
		{
			DXGI_ADAPTER_DESC desc;
			adapters[i]->GetDesc(&desc);
			if (desc.DedicatedVideoMemory > maxVideoMemory)
			{
				maxVideoMemory = desc.DedicatedVideoMemory;
				useAdapterIndex = i;
			}
		}
		adapter = adapters[useAdapterIndex];
		factory->Release();
	}
};

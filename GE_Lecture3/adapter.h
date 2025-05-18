#pragma once
#include <D3D11.h>
#include<vector>
#include<dxgi1_6.h>

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")
// for laptop with multiple GPU, run NVIDIA GPU by default
// ֪ͨNVIDIA����ǿ������NVIDIA�Կ�
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	// _declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001; // AMD �汾
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
		// ����DXGI����
		CreateDXGIFactory(__uuidof(IDXGIFactory6), (void**)&factory);
		int i = 0;
		// ö������Adapter�����浽����
		while (factory->EnumAdapters1(i, &adapterf) != DXGI_ERROR_NOT_FOUND)
		{
			adapters.push_back(adapterf);
			i++;
		}
		// find the best adapter through comparing max video memory
		long long maxVideoMemory = 0;
		int useAdapterIndex = 0;
			// ����ѡ�����adapter
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

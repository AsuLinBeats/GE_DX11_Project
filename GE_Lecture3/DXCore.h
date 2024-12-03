#pragma once
#include <D3D11.h>
#include<vector>
#include<dxgi1_6.h>
#include"adapter.h"
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")

class DXCore {
public:

	// device setting
	// represents a graphic card, feature: create resources(ID3D11Resource, Shaders, viewpoint class...)
	ID3D11Device* device; 
	// served as pipeline, used to shading and computing, Bond with D3DDevice and resources it created
	ID3D11DeviceContext* devicecontext;
	// cache one or multiple textures, which can be called back buffer, the main place for rendering	
	IDXGISwapChain* swapchain;

	// define buffers
	ID3D11RenderTargetView* backbufferRenderTargetView; // pointer to RTV, which is the place of output of rendered image, usually a texture or back buffer.
	ID3D11Texture2D* backbuffer; // pointer, represent back buffer, part of swap chain
	ID3D11DepthStencilView* depthStencilView; // pointer to DSV, used for z-buffering and stencil test
	ID3D11Texture2D* depthbuffer; // stores depth information for each pixel for depth testing

	D3D11_RASTERIZER_DESC rsdesc; // structure to describe the rastizer state, controls how polygons are converted to pixels
	ID3D11RasterizerState* rasterizerState; // pointer to state/object configuring rasterizer


	// initialise dx swapchain, device, and rendering resources: HWND:Handle to window where rendering will take place
	void init(int width, int height,HWND hwnd, bool window_fullscreen) {
		// initialise swap chain
		DXGI_SWAP_CHAIN_DESC sd; // descripter for swap chain
		memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

		sd.BufferCount = 1; // 1 buffer in swap chain(double buffering)
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;

		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // back buffer format, 8 bit per channel, normalised
		sd.BufferDesc.RefreshRate.Numerator = 60; // refresh rate
		sd.BufferDesc.RefreshRate.Denominator = 1; // refresh rate
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // buffer will be used as a render target
		sd.OutputWindow = hwnd; // associate swap chain with window: hwnd
		sd.SampleDesc.Count = 1; // MSAA
		sd.SampleDesc.Quality = 0; // MSAA related
		sd.Windowed = window_fullscreen ? false : true; // fullscreen/windowed screen
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow switch between two type of screen, (alt+enter)
		// choose DX Version
		D3D_FEATURE_LEVEL fl;
		fl = D3D_FEATURE_LEVEL_11_0;

		Adapter adapter;
		adapter.FindAdapter();
		// Remember: Initialise != create
		// create Direct3D device, swap chain
		D3D11CreateDeviceAndSwapChain(adapter.adapter,
			D3D_DRIVER_TYPE_UNKNOWN,
			NULL,
			D3D11_CREATE_DEVICE_DEBUG,
			&fl,
			1,
			D3D11_SDK_VERSION,
			&sd,
			&swapchain,
			&device,
			NULL,
			&devicecontext);
		// sets full screen state for swap chain
		swapchain->SetFullscreenState(window_fullscreen, NULL);

		// setup back buffer
		swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer); // retrieve back buffer from swap chain
		device->CreateRenderTargetView(backbuffer, NULL, &backbufferRenderTargetView); // create RTV
		// depth buffer(z-buffer?)
		D3D11_TEXTURE2D_DESC dsvDesc; // Descriptor for depth-stencil buffer
		dsvDesc.Width = width; // width&height same with other buffers and screen
		dsvDesc.Height = height;
		dsvDesc.MipLevels = 1;
		dsvDesc.ArraySize = 1;
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // format: 24bits for depth, 8 bits for stencil
		dsvDesc.SampleDesc.Count = 1;
		dsvDesc.SampleDesc.Quality = 0; // msaa related
		dsvDesc.Usage = D3D11_USAGE_DEFAULT;// sets buffer for default read/write usage
		dsvDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL; // indicates the texture be used as depth-stencil buffer
		dsvDesc.CPUAccessFlags = 0;
		dsvDesc.MiscFlags = 0;
		//create depth buffer as a 2d texture
		device->CreateTexture2D(&dsvDesc, NULL, &depthbuffer);
		device->CreateDepthStencilView(depthbuffer, NULL, &depthStencilView); // creates DSV to use depth buffer in rendering
		// bind back buffer and depth-stencil buffer to output merger stage of pipeline
		devicecontext->OMSetRenderTargets(1, &backbufferRenderTargetView, depthStencilView);

		// create viewport
		// initialise viewport
		D3D11_VIEWPORT viewport;
		viewport.Width = (float)width;
		viewport.Height = (float)height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f; // specify cube
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		devicecontext->RSSetViewports(1, &viewport); // set viewport to gpu
		// Rasterizer state configuation
		ZeroMemory(&rsdesc, sizeof(D3D11_RASTERIZER_DESC)); //Initializes the D3D11_RASTERIZER_DESC structure to zero to ensure all fields are explicitly defined.
		rsdesc.FillMode = D3D11_FILL_SOLID; // Renders polygons as filled shapes.
		rsdesc.CullMode = D3D11_CULL_NONE; // No faces are culled, and all polygons are rendered.
		device->CreateRasterizerState(&rsdesc, &rasterizerState); // Creates a Rasterizer State object from the description (rsdesc).
		devicecontext->RSSetState(rasterizerState); // Sets the created rasterizer state to GPU pipeline

	}

	void Clear() {
		// clear buffer to a colour at beginning
		// clear depth to deepest depth
		float ClearColour[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; // set to blue
		devicecontext->ClearRenderTargetView(backbufferRenderTargetView, ClearColour); // clear render target view
		devicecontext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,
			0); // 
	}	

	void Present() {
		// present at end
		swapchain->Present(0,0);
	}

	void Release() {
		backbuffer->Release();
		depthStencilView->Release();
		depthbuffer->Release();
		backbufferRenderTargetView->Release();
		swapchain->Release();
		devicecontext->Release();
		device->Release();

	}
};
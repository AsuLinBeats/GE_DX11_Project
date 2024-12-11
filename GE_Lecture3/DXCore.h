#pragma once
#include <D3D11.h>
#include<vector>
#include<dxgi1_6.h>
#include"adapter.h"
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "DXGI.lib")
// TODO THE COMMITMENT IN THIS FILE IS ADDED FOR LEARNING DX11 PURPOSE DURING FIRST FEW WEEKS OF MODULE, AND NOT RELATED WITH MY IDEA FOR ASSESSMENT (so just skip them while marking)
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

	// deferred shading related:
	// texture:
	ID3D11RenderTargetView* renderTargetView;
	ID3D11RenderTargetView* shadowRTV;
	ID3D11RenderTargetView* normalRTV;
	ID3D11Texture2D* texture;
	ID3D11Texture2D* shadow;
	ID3D11Texture2D* normal;
	ID3D11ShaderResourceView* srv;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11ShaderResourceView* normalSRV;

	//ID3D11RenderTargetView* gBufferRTVs[3]; // Albedo, Normals, Depth
	//ID3D11ShaderResourceView* gBufferSRVs[3]; // Shader Resource Views for G-Buffer
	// 
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

		// Deferred shading, G-buffer, from slides
		// 1 G-buffer for now, which is colour. The slides shows here should be 4 G0buffers
		// may try to add normal and depth for normal mapping and shadow mapping but now, just use colour as example on slides
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		device->CreateTexture2D(&textureDesc, NULL, &texture);
		// Create render target view
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		device->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTargetView);
		// Create shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		ZeroMemory(&shaderResourceViewDesc, sizeof(shaderResourceViewDesc));
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &srv);

		// depth
		D3D11_TEXTURE2D_DESC shadowDesc = {};
		shadowDesc.Width = width;
		shadowDesc.Height = height;
		shadowDesc.MipLevels = 1;
		shadowDesc.ArraySize = 1;
		shadowDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		shadowDesc.SampleDesc.Count = 1;
		shadowDesc.SampleDesc.Quality = 0;
		shadowDesc.Usage = D3D11_USAGE_DEFAULT;
		shadowDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		shadowDesc.CPUAccessFlags = 0;
		shadowDesc.MiscFlags = 0;

		device->CreateTexture2D(&shadowDesc, NULL, &shadow);
		// Depth stencil view
		D3D11_RENDER_TARGET_VIEW_DESC shadowRTVDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		shadowRTVDesc.Format = textureDesc.Format;
		shadowRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		shadowRTVDesc.Texture2D.MipSlice = 0;
		device->CreateRenderTargetView(shadow, &shadowRTVDesc, &shadowRTV);
		

		// Shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC shadowsrvDesc = {};
		shadowsrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		shadowsrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shadowsrvDesc.Texture2D.MostDetailedMip = 0;
		shadowsrvDesc.Texture2D.MipLevels = 1;

		device->CreateShaderResourceView(shadow, &shadowsrvDesc, &shadowSRV);

		// maybe normal?

		D3D11_TEXTURE2D_DESC normalDesc = {};
		normalDesc.Width = width;
		normalDesc.Height = height;
		normalDesc.MipLevels = 1;
		normalDesc.ArraySize = 1;
		normalDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		normalDesc.SampleDesc.Count = 1;
		normalDesc.SampleDesc.Quality = 0;
		normalDesc.Usage = D3D11_USAGE_DEFAULT;
		normalDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		normalDesc.CPUAccessFlags = 0;
		normalDesc.MiscFlags = 0;

		device->CreateTexture2D(&normalDesc, NULL, &normal);
		
		D3D11_RENDER_TARGET_VIEW_DESC normalRTVDesc;
		ZeroMemory(&renderTargetViewDesc, sizeof(renderTargetViewDesc));
		normalRTVDesc.Format = textureDesc.Format;
		normalRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		normalRTVDesc.Texture2D.MipSlice = 0;
		device->CreateRenderTargetView(normal, &normalRTVDesc, &normalRTV);
		

		// Shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC normalsrvDesc = {};
		normalsrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		normalsrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		normalsrvDesc.Texture2D.MostDetailedMip = 0;
		normalsrvDesc.Texture2D.MipLevels = 1;

		device->CreateShaderResourceView(normal, &normalsrvDesc, &normalSRV);
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
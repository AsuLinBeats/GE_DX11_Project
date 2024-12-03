#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include<string>
#include "DXCore.h"
#include"shaders.h"

class Sampler {
public:

    // decide what types of samplings, linear? points? 
    ID3D11SamplerState* state;
    DXCore* core;
    D3D11_SAMPLER_DESC samplerDesc;

    void init(DXCore* core) {
        ZeroMemory(&samplerDesc, sizeof(samplerDesc)); // sampler satte
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // mip map, bilinear
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; // wrap, so make points out of range match with texture mapo
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        core->device->CreateSamplerState(&samplerDesc, &state);
        core->devicecontext->PSSetSamplers(0, 1, &state);

    }


};

class Texture {
public:
	// manage texture
    ID3D11Texture2D* texture;
    ID3D11ShaderResourceView* srv; // allow shader access texture
    Sampler sam;
    void init(DXCore * core, int width, int height, int channels, DXGI_FORMAT format, unsigned char* data) {
        // for data, void* is little bad, but acceptable
        // description
        // load 
        D3D11_TEXTURE2D_DESC texDesc;
        memset(&texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC));
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1; // 
        texDesc.Format = format; // 
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT; // shader resource
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;

        //format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        // copy and pass texture to 
        D3D11_SUBRESOURCE_DATA initData;
        memset(&initData, 0, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = data; // just point to something, create a copy queue and put data into it.
        initData.SysMemPitch = width * channels; 
        core->device->CreateTexture2D(&texDesc, &initData, &texture); // texture created, texture data set into gpu

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        core->device->CreateShaderResourceView(texture, &srvDesc, &srv); // create shader resource to view, allow shader access data.
        core->devicecontext->PSSetShaderResources(0, 1, &srv);


    }


	void load(DXCore* core, std::string filename) {

        int width = 0;
        int height = 0;
        int channels = 0;
        unsigned char* texels = stbi_load(filename.c_str(), &width, &height, &channels, 0); // channels normally 4, rgba, but for gpu, we set it to 255 channels
        if (channels == 3) {
            channels = 4;
            unsigned char* texelsWithAlpha = new unsigned char[width * height * channels];
            for (int i = 0; i < (width * height); i++) {
                texelsWithAlpha[i * 4] = texels[i * 3];
                texelsWithAlpha[(i * 4) + 1] = texels[(i * 3) + 1];
                texelsWithAlpha[(i * 4) + 2] = texels[(i * 3) + 2];
                texelsWithAlpha[(i * 4) + 3] = 255; // channels for GPU
            }
            // Initialize texture using width, height, channels, and texelsWithAlpha
            init(core, width, height, channels, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, texelsWithAlpha);
            
            delete[] texelsWithAlpha;
        }
        else {
            // Initialize texture using width, height, channels, and texels
            init(core, width, height, channels, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, texels);
            
        }
        sam.init(core);
        stbi_image_free(texels);

	}


    void apply(DXCore* core, shader shad, std::string name) {
        // shad.apply(core);#
        // run everytime when change texture!!!
        shad.updateShaderPS(core, name, srv);
       
        // t0 in shader related with 0 here. 

    }

    void free() {
        srv->Release();
        texture->Release();
    }

    ~Texture() {
        free();
    }
};


//class TextManager {
//public:
//    std::map<std::string, Texture> textfilenames;
//
//
//    ID3D11ShaderResourceView* find(std::string name) {
//        return textfilenames[name].srv;
//    }
//
//};

class TextureManager
{
public:
    std::map<std::string, Texture*> textures;

    void load(DXCore* core, std::string filename)
    {
        std::map<std::string, Texture*>::iterator it = textures.find(filename);
        if (it != textures.end())
        {
            return;
        }
        Texture* texture = new Texture();
        texture->load(core, filename);
        textures.insert({ filename, texture });
    }
    ID3D11ShaderResourceView* find(std::string name)
    {
        return textures[name]->srv;
    }
    void unload(std::string name)
    {
        textures[name]->free();
        textures.erase(name);
    }
    ~TextureManager()
    {
        for (auto it = textures.cbegin(); it != textures.cend(); )
        {
            it->second->free();
            textures.erase(it++);
        }
    }
};
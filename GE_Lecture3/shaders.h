#pragma once
#include"DXCore.h"
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<D3Dcompiler.h>
#include<d3d11shader.h>
#include"DXCore.h"
#include"ShaderReflection.h"
#pragma comment(lib, "dxguid.lib") // connect to static library,will be useful later

class shader {
public:
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* layout;
    ID3D11Buffer* constantBuffer;
    ConstantBuffer constBufferCPU;
    // Constant buffer related
    std::vector<ConstantBuffer> psConstantBuffers;
    std::vector<ConstantBuffer> vsConstantBuffers;
    std::map<std::string, int> textureBindPointsVS;
    std::map<std::string, int> textureBindPointsPS;
    std::map<std::string, int> textureBindPoints;

    // DXCore core;
     // DXCore core;
    std::string LoadShaderFromFile(const std::string& filePath) {
        std::ifstream file(filePath); // Open the file
        if (!file.is_open()) {
            std::cerr << "Failed to open shader file: " << filePath << std::endl;
            return "";
        }

        std::stringstream shaderStream;
        shaderStream << file.rdbuf(); // Read the file into a string
        file.close(); // close file

        return shaderStream.str(); // Return the file's contents as a string
    }


    void compileVs(std::string filename, DXCore* core) {
        // compile vertex shader
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        // number is version use.
        HRESULT hr = D3DCompile(filename.c_str(), strlen(filename.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "vertex shader error", 0); // check if open successfully
            exit(0);
        }

       //  create layout only for vertices.
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOUR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 2, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
    }

    void compilePs(std::string filename, DXCore* core) {
        // compile pixel shader
        ID3DBlob* compiledPixelShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(filename.c_str(), strlen(filename.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPixelShader, &status);
        core->device->CreatePixelShader(compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &pixelShader);
        if (FAILED(hr)) {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "pixel shader error", 0); // check if open successfully
            exit(0);
        }
    }

    void updateConstant(std::string constantBufferName, std::string variableName, void* data, std::vector<ConstantBuffer>& buffers)
    {
        for (int i = 0; i < buffers.size(); i++)
        {
            if (buffers[i].name == constantBufferName)
            {
                buffers[i].update(variableName, data);
                return;
            }
        }
    }


    void apply(DXCore* core) {
        core->devicecontext->IASetInputLayout(layout);
        core->devicecontext->VSSetShader(vertexShader, NULL, 0);
        core->devicecontext->PSSetShader(pixelShader, NULL, 0);
        core->devicecontext->PSSetConstantBuffers(0, 1, &constantBuffer);



        for (int i = 0; i < vsConstantBuffers.size(); i++) {
            vsConstantBuffers[i].upload(core);
        }
        for (int i = 0; i < psConstantBuffers.size(); i++) {
            psConstantBuffers[i].upload(core);
        }
    }

    void loadInstanceVS(DXCore* core, std::string hlsl) {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "INSTANCEPOSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, 							D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
    }


    void loadVS(DXCore* core, std::string hlsl)
    {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 4, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
        ConstantBufferReflection reflection;
        reflection.build(core, compiledVertexShader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);

    }

    void loadWaterVS(DXCore* core, std::string hlsl)
    {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
        {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
           
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 3, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
        ConstantBufferReflection reflection;
        reflection.build(core, compiledVertexShader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);

    }

  

    void loadPS(DXCore* core, std::string hlsl)
    {
        ID3DBlob* compiledPixelShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &compiledPixelShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Pixel Shader Error", 0);
            exit(0);
        }
        core->device->CreatePixelShader(compiledPixelShader->GetBufferPointer(), compiledPixelShader->GetBufferSize(), NULL, &pixelShader);
        ConstantBufferReflection reflection;
        reflection.build(core, compiledPixelShader, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
    }

    void loadAnimationVS(DXCore* core, std::string hlsl) {
        ID3DBlob* compiledVertexShader;
        ID3DBlob* status;
        HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "VS", "vs_5_0", 0, 0, &compiledVertexShader, &status);
        if (FAILED(hr))
        {
            MessageBoxA(NULL, (char*)status->GetBufferPointer(), "Vertex Shader Error", 0);
            exit(0);
        }
               
        core->device->CreateVertexShader(compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), NULL, &vertexShader);
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 								D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        core->device->CreateInputLayout(layoutDesc, 6, compiledVertexShader->GetBufferPointer(), compiledVertexShader->GetBufferSize(), &layout);
        ConstantBufferReflection reflection;
        reflection.build(core, compiledVertexShader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
    }


    void updateConstantVS(std::string name, std::string constBuffferName, std::string variableName, void* data) {
        // Bug Fixed: Name of const buffer name
        for (int i = 0; i < vsConstantBuffers.size(); i++)
        {
            if (vsConstantBuffers[i].name == constBuffferName)
            {
                vsConstantBuffers[i].update(variableName, data);
            }
        }
    }

    void updateConstantPS(std::string name, std::string constBuffferName, std::string variableName, void* data) {
        for (int i = 0; i < psConstantBuffers.size(); i++)
        {
            if (psConstantBuffers[i].name == constBuffferName)
            {
                psConstantBuffers[i].update(variableName, data);
            }
        }
    }

    void updateShaderPS(DXCore* core, std::string name, ID3D11ShaderResourceView* tex) {
        //core->devicecontext->PSSetShaderResources(0, 1, &srv);// t0 in shader related with 0 here. 

        // run everytime when change/add texture!!!
        core->devicecontext->PSSetShaderResources(textureBindPointsPS[name], 1, &tex);// t0 in shader related with 0 here. 
    }

    void initAnim(std::string vsdfilenames, std::string psdfilenames, DXCore* core) {

        std::string vsd = LoadShaderFromFile(vsdfilenames);
        std::string psd = LoadShaderFromFile(psdfilenames);
        //compileVs(vsd, core);
        //compilePs(psd, core);
        loadAnimationVS(core, vsd);
        loadPS(core, psd);
    }

    void initStatic(std::string vsdfilenames, std::string psdfilenames, DXCore* core) {

        std::string vsd = LoadShaderFromFile(vsdfilenames);
        std::string psd = LoadShaderFromFile(psdfilenames);
        //compileVs(vsd, core);
        //compilePs(psd, core);
        //loadAnimationVS(core, vsd);
        loadVS(core, vsd);
        loadPS(core, psd);
    }

    void init2D(std::string vsdfilenames, std::string psdfilenames, DXCore* core) {
        std::string vsd = LoadShaderFromFile(vsdfilenames);
        std::string psd = LoadShaderFromFile(psdfilenames);
        compileVs(vsd, core);
        compilePs(psd, core);
    }


    void initWater(std::string vsdfilenames, std::string psdfilenames, DXCore* core) {

        std::string vsd = LoadShaderFromFile(vsdfilenames);
        std::string psd = LoadShaderFromFile(psdfilenames);
        //compileVs(vsd, core);
        //compilePs(psd, core);
        //loadAnimationVS(core, vsd);
        loadWaterVS(core, vsd);
        loadPS(core, psd);
    }
};

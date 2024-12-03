#pragma once

#include <D3D11.h>
#include <D3Dcompiler.h>
#include <d3d11shader.h>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <vector>

#include "DXCore.h" 

#pragma comment(lib, "dxguid.lib")
// define shade stage for constant buffer
enum ShaderStage
{
	VertexShader,
	PixelShader
};
// data in buffer variable
struct ConstantBufferVariable
{
	unsigned int offset;
	unsigned int size;
};

class ConstantBuffer
{
public:
	std::string name;
	std::map<std::string, ConstantBufferVariable> constantBufferData;
	ID3D11Buffer* cb;
	unsigned char* buffer; // CPU side buffer for staging data before sending to GPU
	unsigned int cbSizeInBytes; // size of buffer, in bytes
	int dirty; // a flag, indicating whether buffer data has changed and needs to be re-uploaded to GPT
	int index; // binding index of constant buffer in shader
	ShaderStage shaderStage;
	float time;
	void init(DXCore* core, unsigned int sizeInBytes, int constantBufferIndex, ShaderStage _shaderStage)
	{
		unsigned int sizeInBytes16 = ((sizeInBytes + 15) & -16);//Constant buffers in HLSL require 16-byte alignment for optimal GPU performance
		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DYNAMIC; // buffer can be updated frequently
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow cpu writes to buffer
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		bd.ByteWidth = sizeInBytes16;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // specifics constant buffer
		core->device->CreateBuffer(&bd, NULL, &cb); // create GPU side buffer
		buffer = new unsigned char[sizeInBytes16]; // allocates CPU-side buffer to stage data before uploading to GPU
		// set size, binding index and shader stage
		cbSizeInBytes = sizeInBytes;
		index = constantBufferIndex;
		dirty = 1; // Marks the buffer as dirty so that the first upload call will transfer data to the GPU.
		shaderStage = _shaderStage;
	}
	void update(std::string name, void* data)
	{
		ConstantBufferVariable cbVariable = constantBufferData[name]; // locate variable in constantBufferData
		memcpy(&buffer[cbVariable.offset], data, cbVariable.size); // copy data into CPU side buffer
		dirty = 1; // mark it as dirty again, indicate it needs to be reuploaded to GPU
	}
	void upload(DXCore* core)
	{
		if (dirty == 1) // only upload when dirty == 1
		{
			D3D11_MAPPED_SUBRESOURCE mapped;
			core->devicecontext->Map(cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped); // access GPU buffer for writing
			memcpy(mapped.pData, buffer, cbSizeInBytes);
			core->devicecontext->Unmap(cb, 0); //  making it available for use by the GPU.
			// Binds the buffer to the appropriate shader stage
			if (shaderStage == ShaderStage::VertexShader)
			{
				core->devicecontext->VSSetConstantBuffers(index, 1, &cb);
			}
			if (shaderStage == ShaderStage::PixelShader)
			{
				core->devicecontext->PSSetConstantBuffers(index, 1, &cb);
			}
			dirty = 0;
		}
	}
	void free()
	{
		//Releases the GPU-side buffer to free resources.
		cb->Release();
	}
};

// automates the creation of constant buffer and texture binding
class ConstantBufferReflection
{
public:
	void build(DXCore* core, ID3DBlob* shader, std::vector<ConstantBuffer>& buffers, std::map<std::string, int>& textureBindPoints, ShaderStage shaderStage)
	{
		ID3D11ShaderReflection* reflection;
		D3DReflect(shader->GetBufferPointer(), shader->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflection);// Creates a reflection interface for a compiled shader.
		D3D11_SHADER_DESC desc;
		reflection->GetDesc(&desc);//Retrieves overall shader metadata 
		for (int i = 0; i < desc.ConstantBuffers; i++)
		{
			ConstantBuffer buffer;
			ID3D11ShaderReflectionConstantBuffer* constantBuffer = reflection->GetConstantBufferByIndex(i);//Gets a reflection interface for a specific buffer.
			D3D11_SHADER_BUFFER_DESC cbDesc;
			constantBuffer->GetDesc(&cbDesc); //Retrieves metadata about the buffer
			buffer.name = cbDesc.Name;
			unsigned int totalSize = 0;
			for (int n = 0; n < cbDesc.Variables; n++)
			{
				ID3D11ShaderReflectionVariable* var = constantBuffer->GetVariableByIndex(n);
				D3D11_SHADER_VARIABLE_DESC vDesc;
				var->GetDesc(&vDesc);
				ConstantBufferVariable bufferVariable;
				bufferVariable.offset = vDesc.StartOffset;
				bufferVariable.size = vDesc.Size;
				buffer.constantBufferData.insert({ vDesc.Name, bufferVariable });
				totalSize += bufferVariable.size;
			}
			// store data and initlise buffer
			buffer.init(core, totalSize, i, shaderStage);
			buffers.push_back(buffer);
		}
		// links the name of texture and bipoint(update in texture lectureF)
		for (int i = 0; i < desc.BoundResources; i++)
		{
			D3D11_SHADER_INPUT_BIND_DESC bindDesc;
			reflection->GetResourceBindingDesc(i, &bindDesc);
			if (bindDesc.Type == D3D_SIT_TEXTURE)
			{
				textureBindPoints.insert({ bindDesc.Name, bindDesc.BindPoint });
			}
		}
	}
};

// How to use
/*
Define in shader class:

std::vector<ConstantBuffer> psConstantBuffers;
std::vector<ConstantBuffer> vsConstantBuffers;
std::map<std::string, int> textureBindPointsVS;
std::map<std::string, int> textureBindPointsPS;


	void loadPS(Core *core, std::string hlsl)
	{
		ID3DBlob* shader;
		ID3DBlob* status;
		HRESULT hr = D3DCompile(hlsl.c_str(), strlen(hlsl.c_str()), NULL, NULL, NULL, "PS", "ps_5_0", 0, 0, &shader, &status);
		if (FAILED(hr))
		{
			printf("%s\n", (char*)status->GetBufferPointer());
			exit(0);
		}
		core->device->CreatePixelShader(shader->GetBufferPointer(), shader->GetBufferSize(), NULL, &ps);
		ConstantBufferReflection reflection;
		reflection.build(core, shader, psConstantBuffers, textureBindPointsPS, ShaderStage::PixelShader);
	}

	And repeat for loadVS
		ConstantBufferReflection reflection;
		reflection.build(core, shader, vsConstantBuffers, textureBindPointsVS, ShaderStage::VertexShader);
*/
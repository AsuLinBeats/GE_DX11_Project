#pragma once
#include "shaders.h"
#include"DXCore.h"
//
//class shader3D {
//	DXCore* core;
//	D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
//	{
//		{ "POS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, 							D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	};
//
//	
//
//
//	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DXCore* dxcore) {
//		
//		D3D11_BUFFER_DESC bd;
//		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
//		bd.Usage = D3D11_USAGE_DEFAULT;
//		bd.ByteWidth = sizeof(unsigned int) * numIndices;
//		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		D3D11_SUBRESOURCE_DATA data;
//		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
//		data.pSysMem = indices;
//		dxcore->device->CreateBuffer(&bd, &data, &indexBuffer);
//		bd.ByteWidth = vertexSizeInBytes * numVertices;
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		data.pSysMem = vertices;
//		dxcore->device->CreateBuffer(&bd, &data, &vertexBuffer);
//		indicesSize = numIndices;
//		strides = vertexSizeInBytes;
//	}
//
//
//
//	void apply(DXCore* core) {
//
//		core->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		core->devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
//		core->devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//		core->devicecontext->DrawIndexed(indicesSize, 0, 0);
//	}
//};


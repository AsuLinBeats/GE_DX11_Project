#pragma once
#include "GraphicMath.h"
#include "DXCore.h"
#include<string>
struct STATIC_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	// texture coordinate
	float tu;
	float tv;


};

class Mesh {

public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	int indicesSize;
	UINT strides;
	DXCore* dxcore;

	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DXCore* dxcore) {

		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		dxcore->device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		dxcore->device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}

	void init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices)
	{
		init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(),dxcore);
	}

	void apply3d(DXCore* core) {
		UINT offsets = 0;
		core->devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		core->devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		core->devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		core->devicecontext->DrawIndexed(indicesSize, 0, 0);

	}
};

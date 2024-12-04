#pragma once
#include "GraphicMath.h"
#include "DXCore.h"
#include"GEMLoader.h"
#include "shaders.h"
#include<string>
#include"Texture.h"
struct Vertex {
	Vec3 position;
	Colour colour;
};


struct STATIC_VERTEX
{
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	// texture coordinate
	float tu;
	float tv;
};

// considering moving animation part to separate file
struct ANIMATED_VERTEX
{
	// Not efficient
	Vec3 pos;
	Vec3 normal;
	Vec3 tangent;
	float tu;
	float tv;
	unsigned int bonesIDs[4];
	float boneWeights[4];
};


class Mesh {
public:
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* vertexBuffer;
	int indicesSize;
	UINT strides;

	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DXCore& device) {
		D3D11_BUFFER_DESC bd;
		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned int) * numIndices;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA data;
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = indices;
		device.device->CreateBuffer(&bd, &data, &indexBuffer);
		bd.ByteWidth = vertexSizeInBytes * numVertices;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		data.pSysMem = vertices;
		device.device->CreateBuffer(&bd, &data, &vertexBuffer);
		indicesSize = numIndices;
		strides = vertexSizeInBytes;
	}

	void init(std::vector<STATIC_VERTEX> vertices, std::vector<unsigned int> indices, DXCore& device)
	{
		init(&vertices[0], sizeof(STATIC_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}

	void init(std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices, DXCore& device)
	{
		init(&vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size(), device);
	}

	void draw(DXCore& devicecontext) {
		UINT offsets = 0;
		devicecontext.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		devicecontext.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
		devicecontext.devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		devicecontext.devicecontext->DrawIndexed(indicesSize, 0, 0);
	}
};

class Triangle {
	Vertex vertices[3];
	ID3D11Buffer* vertexBuffer; // pointer to vertex buffer that holds triangle's vertex data for rendering

public:
	void init(DXCore core) {
		// set position and colour of vertices
		vertices[0].position = Vec3(0, 1.0f, 0);
		vertices[0].colour = Colour(0, 1.0f, 0,255);
		vertices[1].position = Vec3(-1.0f, -1.0f, 0);
		vertices[1].colour = Colour(1.0f, 0, 0,255);
		vertices[2].position = Vec3(1.0f, -1.0f, 0);
		vertices[2].colour = Colour(0, 0, 1.0f,255);

		D3D11_BUFFER_DESC bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0; // CPU do not need to access buffer after initialising
		bd.MiscFlags = 0;
		bd.ByteWidth = sizeof(Vertex) * 3; //N=3
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		// upload data to buffer
		D3D11_SUBRESOURCE_DATA uploadData; 
		uploadData.pSysMem = vertices; // pointer to array of vertices
		uploadData.SysMemPitch = 0; // unused 
		uploadData.SysMemSlicePitch = 0;
		core.device->CreateBuffer(&bd, &uploadData, &vertexBuffer); // Creates the vertex buffer on the GPU using the buffer description (bd) and the initial data (uploadData).
	}

	void draw(DXCore core) {
		UINT offsets;
		offsets = 0; 
		UINT strides = sizeof(Vertex);
		core.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // GPU interpretion: Treats every three vertices as an independent triangle.
		core.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets); // Binds the vertex buffer to the Input Assembler Stage of the GPU pipeline
		core.devicecontext->Draw(3, 0); // get commands sent to GPU
	}
};


class Plane {
public:

	 DXCore* core;
	 GamesEngineeringBase::Timer timer;
	 float time = 0;
	Mesh mesh;


	//shader shader;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		// help function
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;

		//Frame frame = 0;
		//frame.fromVector(n);
		// SHADING FRAME IS FROM FIRST LECTURE

		v.tangent = Vec3(0,0,0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}


	void init(DXCore& core) {
		std::vector<STATIC_VERTEX> vertices;
		vertices.push_back(addVertex(Vec3(-1.5, 0, -1.5), Vec3(0, 1, 0), 0, 0));
		vertices.push_back(addVertex(Vec3(1.5, 0, -1.5), Vec3(0, 1, 0), 1, 0));
		vertices.push_back(addVertex(Vec3(-1.5, 0, 1.5), Vec3(0, 1, 0), 0, 1));
		vertices.push_back(addVertex(Vec3(1.5, 0, 1.5), Vec3(0, 1, 0), 1, 1));
		std::vector<unsigned int> indices;
		indices.push_back(2); indices.push_back(1); indices.push_back(0);
		indices.push_back(1); indices.push_back(2); indices.push_back(3);
		
		mesh.init(vertices,indices,core);
	}


	void draw(shader* shaders, DXCore * core) {
	
		mesh.draw(*core);
	}
};

class Cube {
public:
	Mesh mesh;
	DXCore* core;
	std::vector<STATIC_VERTEX> vertices;
	std::vector<unsigned int> indices;
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		// help function
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void init(DXCore& core) {
		
		Vec3 p0 = Vec3(-1.0f, -1.0f, -1.0f);
		Vec3 p1 = Vec3(1.0f, -1.0f, -1.0f);
		Vec3 p2 = Vec3(1.0f, 1.0f, -1.0f);
		Vec3 p3 = Vec3(-1.0f, 1.0f, -1.0f);
		Vec3 p4 = Vec3(-1.0f, -1.0f, 1.0f);
		Vec3 p5 = Vec3(1.0f, -1.0f, 1.0f);
		Vec3 p6 = Vec3(1.0f, 1.0f, 1.0f);
		Vec3 p7 = Vec3(-1.0f, 1.0f, 1.0f);

		vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));


		vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));

		vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
		vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
		vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
		vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));

		
		indices.push_back(0); indices.push_back(1); indices.push_back(2);
		indices.push_back(0); indices.push_back(2); indices.push_back(3);
		indices.push_back(4); indices.push_back(5); indices.push_back(6);
		indices.push_back(4); indices.push_back(6); indices.push_back(7);
		indices.push_back(8); indices.push_back(9); indices.push_back(10);
		indices.push_back(8); indices.push_back(10); indices.push_back(11);
		indices.push_back(12); indices.push_back(13); indices.push_back(14);
		indices.push_back(12); indices.push_back(14); indices.push_back(15);
		indices.push_back(16); indices.push_back(17); indices.push_back(18);
		indices.push_back(16); indices.push_back(18); indices.push_back(19);
		indices.push_back(20); indices.push_back(21); indices.push_back(22);
		indices.push_back(20); indices.push_back(22); indices.push_back(23);
		
		mesh.init(vertices, indices, core);
	}

	//void init(Vec3 p0, Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, Vec3 p5, Vec3 p6, Vec3 p7) {
	//	vertices.push_back(addVertex(p0, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 1.0f));
	//	vertices.push_back(addVertex(p1, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 1.0f));
	//	vertices.push_back(addVertex(p2, Vec3(0.0f, 0.0f, -1.0f), 1.0f, 0.0f));
	//	vertices.push_back(addVertex(p3, Vec3(0.0f, 0.0f, -1.0f), 0.0f, 0.0f));

	//	vertices.push_back(addVertex(p5, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 1.0f));
	//	vertices.push_back(addVertex(p4, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f));
	//	vertices.push_back(addVertex(p7, Vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.0f));
	//	vertices.push_back(addVertex(p6, Vec3(0.0f, 0.0f, 1.0f), 0.0f, 0.0f));

	//	vertices.push_back(addVertex(p4, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
	//	vertices.push_back(addVertex(p0, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
	//	vertices.push_back(addVertex(p3, Vec3(-1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
	//	vertices.push_back(addVertex(p7, Vec3(-1.0f, 0.0f, 0.0f), 0.0f, 0.0f));


	//	vertices.push_back(addVertex(p1, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f));
	//	vertices.push_back(addVertex(p5, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 1.0f));
	//	vertices.push_back(addVertex(p6, Vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.0f));
	//	vertices.push_back(addVertex(p2, Vec3(1.0f, 0.0f, 0.0f), 0.0f, 0.0f));

	//	vertices.push_back(addVertex(p3, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 1.0f));
	//	vertices.push_back(addVertex(p2, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 1.0f));
	//	vertices.push_back(addVertex(p6, Vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.0f));
	//	vertices.push_back(addVertex(p7, Vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f));

	//	vertices.push_back(addVertex(p4, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 1.0f));
	//	vertices.push_back(addVertex(p5, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 1.0f));
	//	vertices.push_back(addVertex(p1, Vec3(0.0f, -1.0f, 0.0f), 1.0f, 0.0f));
	//	vertices.push_back(addVertex(p0, Vec3(0.0f, -1.0f, 0.0f), 0.0f, 0.0f));


	//	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	//	indices.push_back(0); indices.push_back(2); indices.push_back(3);
	//	indices.push_back(4); indices.push_back(5); indices.push_back(6);
	//	indices.push_back(4); indices.push_back(6); indices.push_back(7);
	//	indices.push_back(8); indices.push_back(9); indices.push_back(10);
	//	indices.push_back(8); indices.push_back(10); indices.push_back(11);
	//	indices.push_back(12); indices.push_back(13); indices.push_back(14);
	//	indices.push_back(12); indices.push_back(14); indices.push_back(15);
	//	indices.push_back(16); indices.push_back(17); indices.push_back(18);
	//	indices.push_back(16); indices.push_back(18); indices.push_back(19);
	//	indices.push_back(20); indices.push_back(21); indices.push_back(22);
	//	indices.push_back(20); indices.push_back(22); indices.push_back(23);

	//	mesh.init(vertices, indices, core);
	//}
	
	void updateWorld(Matrix w,shader shad,DXCore core) {
		// submit world matrix to vertex shader
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		shad.apply(&core); // apply adjusted buffer immediately
	}

	void draw(DXCore* core) {

		mesh.draw(*core);
	}
};
class Sphere {
public:
	Mesh mesh;
	// make sphere huge enough to create a sky!
	STATIC_VERTEX addVertex(Vec3 p, Vec3 n, float tu, float tv)
	{
		// help function
		STATIC_VERTEX v;
		v.pos = p;
		v.normal = n;
		v.tangent = Vec3(0, 0, 0); // For now
		v.tu = tu;
		v.tv = tv;
		return v;
	}

	void init(DXCore& core, int rings, int segments, float radius) {

		
		std::vector<STATIC_VERTEX> vertices;
		std::vector<unsigned int> indices;
		// create sphere vertices
		for (int lat = 0; lat <= rings; lat++) {
			float theta = lat * M_PI / rings;
			float sinTheta = sinf(theta);
			float cosTheta = cosf(theta);
			for (int lon = 0; lon <= segments; lon++) {
				float phi = lon * 2.0f * M_PI / segments;
				float sinPhi = sinf(phi);
				float cosPhi = cosf(phi);
				Vec3 position(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi);
				Vec3 normal = position.normalise();
				float tu = 1.0f - (float)lon / segments;
				float tv = 1.0f - (float)lat / rings;
				vertices.push_back(addVertex(position, normal, tu, tv));
			}
		}
		// create indices vertices
		for (int lat = 0; lat < rings; lat++)
		{
			for (int lon = 0; lon < segments; lon++)
			{
				int current = lat * (segments + 1) + lon;
				int next = current + segments + 1;
				indices.push_back(current);
				indices.push_back(next);
				indices.push_back(current + 1);
				indices.push_back(current + 1);
				indices.push_back(next);
				indices.push_back(next + 1);
			}
		}

		mesh.init(vertices, indices,core);
	}
	void draw(DXCore core) {
		mesh.draw(core);
	}
};

class Meshes {
	// all complex meshes
public:
	std::vector<Mesh> meshes;
	Mesh mesh;
	DXCore core;
	std::vector<std::string> textureFilenames;


	void init(std::string filename,DXCore& core, TextureManager* textures) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			textureFilenames.push_back(gemmeshes[i].material.find("diffuse").getValue());
			textures->load(&core, gemmeshes[i].material.find("diffuse").getValue());
			mesh.init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(mesh);
		}


	}


	void initWithoutTexture(std::string filename, DXCore& core) {
		GEMLoader::GEMModelLoader loader;
		std::vector<GEMLoader::GEMMesh> gemmeshes;
		loader.load(filename, gemmeshes);
		for (int i = 0; i < gemmeshes.size(); i++) {
			Mesh mesh;
			std::vector<STATIC_VERTEX> vertices;
			for (int j = 0; j < gemmeshes[i].verticesStatic.size(); j++) {
				STATIC_VERTEX v;
				memcpy(&v, &gemmeshes[i].verticesStatic[j], sizeof(STATIC_VERTEX));
				vertices.push_back(v);
			}
			mesh.init(vertices, gemmeshes[i].indices, core);
			meshes.push_back(mesh);
		}
	}
	void updateWorld(Matrix w, shader shad, DXCore core) {
		// submit world matrix to vertex shader
		shad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &w); // adjust buffer
		shad.apply(&core); // apply adjusted buffer immediately
	}

	void draw(DXCore* core) {
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i].draw(*core);
		}

	}


	void drawTexture(DXCore* core, shader shad, TextureManager* textureM) {
		for (int i = 0; i < meshes.size(); i++)
		{
			shad.updateShaderPS(core, "tex", textureM->find(textureFilenames[i]));
			meshes[i].draw(*core);
		}

	}
};

// update constant anhd draw byuffer

class Cylnder {

	void init() {

	}
};

//class MeshAnimation {
//public:
//	ID3D11Buffer* indexBuffer;
//	ID3D11Buffer* vertexBuffer;
//	int indicesSize;
//	UINT strides;
//
//	void init(void* vertices, int vertexSizeInBytes, int numVertices, unsigned int* indices, int numIndices, DXCore& device) {
//		D3D11_BUFFER_DESC bd;
//		memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));
//		bd.Usage = D3D11_USAGE_DEFAULT;
//		bd.ByteWidth = sizeof(unsigned int) * numIndices;
//		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		D3D11_SUBRESOURCE_DATA data;
//		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
//		data.pSysMem = indices;
//		device.device->CreateBuffer(&bd, &data, &indexBuffer);
//		bd.ByteWidth = vertexSizeInBytes * numVertices;
//		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		data.pSysMem = vertices;
//		device.device->CreateBuffer(&bd, &data, &vertexBuffer);
//		indicesSize = numIndices;
//		strides = vertexSizeInBytes;
//	}
//
//	void init(std::vector<ANIMATED_VERTEX> vertices, std::vector<unsigned int> indices, DXCore& core)
//	{
//		init(&vertices[0], sizeof(ANIMATED_VERTEX), vertices.size(), &indices[0], indices.size(), core);
//	}
//
//	void draw(DXCore& devicecontext) {
//		UINT offsets = 0;
//		devicecontext.devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		devicecontext.devicecontext->IASetVertexBuffers(0, 1, &vertexBuffer, &strides, &offsets);
//		devicecontext.devicecontext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//		devicecontext.devicecontext->DrawIndexed(indicesSize, 0, 0);
//	}
//};
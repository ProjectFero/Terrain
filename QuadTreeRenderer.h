//=====================================================================================
//QuadTree.h
//=====================================================================================
#ifndef __QUADTREERENDERER_H
#define __QUADTREERENDERER_H
//================================================================================================================
//================================================================================================================
//Windows includes
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
using namespace std;
//================================================================================================================
//================================================================================================================
//My Includes
#include "QuadTree.h"
#include "Camera.h"
#include "Material.h"
#include "DirectionalLight.h"
//================================================================================================================
//================================================================================================================
class QuadTreeRenderer
{
	QuadTree* m_quadtree;
	D3D* m_d3d;
	XMFLOAT4X4 mWorld;
	Material mMat;

	ID3D11ShaderResourceView* mLayerMapArraySRV;
	ID3D11ShaderResourceView* mBlendMapSRV;
	ID3D11ShaderResourceView* mHeightMapSRV;
	ID3D11ShaderResourceView* mNormalMapSRV;
	ID3D11ShaderResourceView* mShadowMapSRV;
public:
	QuadTreeRenderer(D3D* d3d, QuadTree* qt,
			string layerMapFilename0,
			string layerMapFilename1,
			string layerMapFilename2,
			string layerMapFilename3,
			string layerMapFilename4,
			string blendMapFilename,
			string normalMapFilename);

	void SetWorld(XMFLOAT4X4 world) { mWorld = world; }

	Material GetMaterial() { return mMat; }

	/*
	void RenderWithReflection(D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	void RenderWithReflection(D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	*/

	//Work on the terrain lighting and better dynamic texturing

	/*void RenderWithReflection(D3D* d3d, TerrainShader* ts, ReflectionShader* rs, Camera* camera, XMMATRIX reflection, XMFLOAT4 clipPlane, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	void RenderWithReflection(D3D* d3d, TerrainShader* ts, Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	void Render(D3D* d3d, TerrainShader* ts, Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);*/

	void Render(D3D* d3d, TerrainShader* ts);//, bool rReflection);

	ID3D11ShaderResourceView* GetLayerMapArraySRV() { return mLayerMapArraySRV; }
	ID3D11ShaderResourceView* GetBlendMapSRV() { return mBlendMapSRV; }
	ID3D11ShaderResourceView* GetHeightMapSRV() { return mHeightMapSRV; }
	ID3D11ShaderResourceView* GetNormalMapSRV() { return mNormalMapSRV; }
	ID3D11ShaderResourceView* GetShadowMapSRV() { return mShadowMapSRV; }

	void BuildLayerMapArraySRV(string layerMapFilename0,
			string layerMapFilename1,
			string layerMapFilename2,
			string layerMapFilename3,
			string layerMapFilename4);
	void BuildBlendMapSRV(string blendMapFilename);
	void BuildHeightmapSRV();
	void BuildNormalMapSRV(string normalMapFilename);
	void BuildShadowMapSRV();

private:
	/*
	void RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	void RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	*/

	/*void RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, ReflectionShader* rs, Camera* camera, XMMATRIX reflection, XMFLOAT4 clipPlane, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	void RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);
	void Render(QNode* node, D3D* d3d, TerrainShader* ts, Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
		float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3]);*/

	void Render(QNode* node, D3D* d3d, TerrainShader* ts);//, bool rReflection);

	void GenerateMesh();
	void GenerateMesh(QNode* node);
	void GenerateMeshNT(QNode* node);
	void GenerateMeshTessellation(QNode* node);
	void GenerateMeshTessellation10(QNode* node);

	void RenderMeshBuffers(QNode* node);
};
//================================================================================================================
//================================================================================================================
#endif//__QUADTREERENDERER_H

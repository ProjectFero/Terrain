#include "QuadTreeRenderer.h"
#include "CUtility.h"
//================================================================================================================
QuadTreeRenderer::QuadTreeRenderer(D3D* d3d
,	QuadTree* qt
,   string layerMapFilename0
,	string layerMapFilename1
,	string layerMapFilename2
,	string layerMapFilename3
,	string layerMapFilename4
,	string blendMapFilename
,	string normalMapFilename
)
:	m_d3d(d3d), m_quadtree(qt)
{
	//Load the world matrix
	XMStoreFloat4x4(&mWorld, XMMatrixIdentity());

	//Create some material for the terrain
	mMat.mAmbient  = XMFLOAT4(0.5f, 0.6f, 0.5f, 1.0f);
	mMat.mDiffuse  = XMFLOAT4(0.5f, 0.6f, 0.5f, 1.0f);
	mMat.mSpecular = XMFLOAT4(0.0f, 0.0f, 0.0f, 64.0f);
	mMat.mReflect  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

	//Create the mesh
	GenerateMesh();

	//Build the Shader Resource Views
	BuildLayerMapArraySRV(layerMapFilename0,
						  layerMapFilename1,
						  layerMapFilename2,
						  layerMapFilename3,
						  layerMapFilename4);
	BuildBlendMapSRV(blendMapFilename);
	BuildHeightmapSRV();
	BuildNormalMapSRV(normalMapFilename);

}
//================================================================================================================
void QuadTreeRenderer::Render(D3D* d3d, TerrainShader* ts)
{
	Render(m_quadtree->GetNodes(), d3d, ts);
}
/*
void QuadTreeRenderer::Render(D3D* d3d, TerrainShader* ts, Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
							  float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	Render(m_quadtree->GetNodes(), d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(D3D* d3d, TerrainShader* ts, Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
											float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	RenderWithReflection(m_quadtree->GetNodes(), d3d, ts, camera, reflection, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(D3D* d3d, TerrainShader* ts, ReflectionShader* rs, Camera* camera, XMMATRIX reflection, XMFLOAT4 clipPlane, float minDist, float maxDist, float minTess, float maxTess,
						float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	RenderWithReflection(m_quadtree->GetNodes(), d3d, ts, rs, camera, reflection, clipPlane, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
}*/
//================================================================================================================
/*
void QuadTreeRenderer::RenderWithReflection(D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
						float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	RenderWithReflection(m_quadtree->GetNodes(), d3d, ts, ws, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
											float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	RenderWithReflection(m_quadtree->GetNodes(), d3d, ts, ws, camera, reflection, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
	float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	if (node->type == LEAF)
	{
		RenderMeshBuffers(node);

		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			//csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
			ts->Render11(m_d3d, node->m_index_count, camera, reflection, mWorld, minDist, maxDist, minTess, maxTess, m_quadtree->TerrainScale(),
				pFogStart, pFogRange, pFogColor, (1 / m_quadtree->MapSize()), (1 / m_quadtree->MapSize()), m_quadtree->GetCellSpacing(),
				mLayerMapArraySRV, mBlendMapSRV, mHeightMapSRV, mMat, mDirLights);

			//Render the water reflection for this node
			ws->RenderReflection11(m_d3d, node->m_index_count);
		}
	}
	else
	{
		//Render the children
		RenderWithReflection(node->children[0], d3d, ts, ws, camera, reflection, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		RenderWithReflection(node->children[1], d3d, ts, ws, camera, reflection, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		RenderWithReflection(node->children[2], d3d, ts, ws, camera, reflection, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		RenderWithReflection(node->children[3], d3d, ts, ws, camera, reflection, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
	}
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, WaterShader* ws, D3Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
	float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	if (node->type == LEAF)
	{
		RenderMeshBuffers(node);

		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			//csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
			ts->Render11(m_d3d, node->m_index_count, camera, mWorld, minDist, maxDist, minTess, maxTess, m_quadtree->TerrainScale(),
				pFogStart, pFogRange, pFogColor, (1 / m_quadtree->MapSize()), (1 / m_quadtree->MapSize()), m_quadtree->GetCellSpacing(),
				mLayerMapArraySRV, mBlendMapSRV, mHeightMapSRV, mMat, mDirLights);

			//Render the water reflection for this node
			ws->RenderReflection11(m_d3d, node->m_index_count);
		}
	}
	else
	{
		//Render the children
		RenderWithReflection(node->children[0], d3d, ts, ws, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		RenderWithReflection(node->children[1], d3d, ts, ws, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		RenderWithReflection(node->children[2], d3d, ts, ws, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		RenderWithReflection(node->children[3], d3d, ts, ws, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
	}
}
*/
//================================================================================================================
void QuadTreeRenderer::Render(QNode* node, D3D* d3d, TerrainShader* ts)
{
	if (node->type == LEAF)
	{
		RenderMeshBuffers(node);

		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			//csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
			ts->Render11(m_d3d, node->m_index_count);

			//if (rReflection)
			//{
			//	//rs->Render11(m_d3d, node->m_index_count);
			//	//cps->Render11(m_d3d, node->m_index_count);
			//}
		}
	}
	else
	{
		//Render the children
		Render(node->children[0], d3d, ts);
		Render(node->children[1], d3d, ts);
		Render(node->children[2], d3d, ts);
		Render(node->children[3], d3d, ts);
	}
}
//================================================================================================================
/*void QuadTreeRenderer::Render(QNode* node, D3D* d3d, TerrainShader* ts, Camera* camera, float minDist, float maxDist, float minTess, float maxTess,
	float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	if (node->type == LEAF)
	{
		RenderMeshBuffers(node);

		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			//csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
			ts->Render11(m_d3d, node->m_index_count, camera, mWorld, minDist, maxDist, minTess, maxTess, m_quadtree->TerrainScale(),
				pFogStart, pFogRange, pFogColor, (1 / m_quadtree->MapSize()), (1 / m_quadtree->MapSize()), m_quadtree->GetCellSpacing(),
				mLayerMapArraySRV, mBlendMapSRV, mHeightMapSRV, mMat, mDirLights);
		}
	}
	else
	{
		//Render the children
		Render(node->children[0], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[1], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[2], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[3], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
	}
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, Camera* camera, XMMATRIX reflection, float minDist, float maxDist, float minTess, float maxTess,
	float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	if (node->type == LEAF)
	{
		RenderMeshBuffers(node);

		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			//csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
			ts->Render11(m_d3d, node->m_index_count);
			//ts->Render11(m_d3d, node->m_index_count, camera, reflection, mWorld, minDist, maxDist, minTess, maxTess, m_quadtree->TerrainScale(),
			//	pFogStart, pFogRange, pFogColor, (1 / m_quadtree->MapSize()), (1 / m_quadtree->MapSize()), m_quadtree->GetCellSpacing(),
			//	mLayerMapArraySRV, mBlendMapSRV, mHeightMapSRV, mMat, mDirLights);
		}
	}
	else
	{
		//Render the children
		Render(node->children[0], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[1], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[2], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[3], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
	}
}
//================================================================================================================
void QuadTreeRenderer::RenderWithReflection(QNode* node, D3D* d3d, TerrainShader* ts, ReflectionShader* rs, Camera* camera, XMMATRIX reflection, XMFLOAT4 clipPlane, float minDist, float maxDist, float minTess, float maxTess,
										float pFogStart, float pFogRange, XMVECTOR pFogColor, DirectionalLight mDirLights[3])
{
	if (node->type == LEAF)
	{
		RenderMeshBuffers(node);

		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			//csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
		if (node->nodeID == 0)
		{
			ts->Render11(m_d3d, node->m_index_count, camera, reflection, mWorld, minDist, maxDist, minTess, maxTess, m_quadtree->TerrainScale(),
				pFogStart, pFogRange, pFogColor, (1 / m_quadtree->MapSize()), (1 / m_quadtree->MapSize()), m_quadtree->GetCellSpacing(),
				mLayerMapArraySRV, mBlendMapSRV, mHeightMapSRV, mMat, mDirLights);
		}

			//Render the reflection shader
			//rs->SetWorld(XMLoadFloat4x4(&mWorld));
			//rs->SetView(reflection);
			//rs->SetProjection(camera->Proj());
			//rs->SetColorTexture(mLayerMapArraySRV);
			//rs->SetNormalTexture(mBlendMapSRV);
			//rs->SetLightDirection(mDirLights[0].mDirection);
			//rs->SetLightDiffuseColor(mDirLights[0].mDiffuseColor);
			//rs->SetColorTexBrightness(2.0f);
			//rs->SetClipPlane(clipPlane);
			//rs->Render11(m_d3d, node->m_index_count);
		}
	}
	else
	{
		//Render the children
		Render(node->children[0], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[1], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[2], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
		Render(node->children[3], d3d, ts, camera, minDist, maxDist, minTess, maxTess, pFogStart, pFogRange, pFogColor, mDirLights);
	}
}*/
//================================================================================================================
void QuadTreeRenderer::RenderMeshBuffers(QNode* node)
{
	/*if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
	{
		if (m_quadtree->Tessellate())
		{
		//	int tessFactor = tess_factor;

		//	unsigned int stride;
		//	unsigned int offset;

		//	//Set the vertex buffer stride and offset
		//	stride = sizeof(Vertex);
		//	offset = 0;

		//	m_d3d->GetDevice10()->IASetVertexBuffers(0, 1, &node->m_vertex_buffer10, &stride, &offset);

		//	//Set the index buffer to active in the input assembler so it can be rendered
		//	m_d3d->GetDevice10()->IASetIndexBuffer(/*node->m_index_bufferTess10[tessFactor]*m_index_buffer[tessFactor], DXGI_FORMAT_R32_UINT, 0);

		//	//Set the type of primitive that should be rendered from this vertex buffer
		//	m_d3d->GetDevice10()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
		else
		{
			unsigned int stride;
			unsigned int offset;

			//Set the vertex buffer stride and offset
			stride = sizeof(VertexTexture);
			offset = 0;

			m_d3d->GetDevice10()->IASetVertexBuffers(0, 1, &node->m_vertex_buffer10, &stride, &offset);

			//Set the index buffer to active in the input assembler so it can be rendered
			m_d3d->GetDevice10()->IASetIndexBuffer(node->m_index_buffer10, DXGI_FORMAT_R32_UINT, 0);

			//Set the type of primitive that should be rendered from this vertex buffer
			m_d3d->GetDevice10()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		}
	}
	else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
	{*/
		unsigned int stride;
		unsigned int offset;

		//Set the vertex buffer stride and offset
		stride = sizeof(VertexTextureBoundsY);
		offset = 0;

		m_d3d->GetDeviceContext()->IASetVertexBuffers(0, 1, &node->m_vertex_buffer11, &stride, &offset);

		//Set the index buffer to active in the input assembler so it can be rendered
		m_d3d->GetDeviceContext()->IASetIndexBuffer(node->m_index_buffer11, DXGI_FORMAT_R32_UINT, 0);

		if (m_quadtree->Tessellate())
		{
			//Set the type of primitive that should be rendered from this vertex buffer
			m_d3d->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		}
		else
		{
			//Set the type of primitive that should be rendered from this vertex buffer
			m_d3d->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		}
	//}
}
//================================================================================================================
void QuadTreeRenderer::GenerateMesh()
{
	//vector<QNode*> nodes = m_quadtree->GetNodes();
	//for (int i = 0; i < nodes.size(); i++)
	//{
	//	if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
	//	{
	//		//if (m_tessellate)
	//		//	GenerateMeshTessellation10(m_nodes[i]);
	//		//else
	//		GenerateMeshNT(nodes[i]);
	//	}
	//	else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
	//	{
	//		if (m_quadtree->Tessellate())
	//			GenerateMeshTessellation(nodes[i]);
	//		else
	//			GenerateMeshNT(nodes[i]);
	//	}
	//}

	GenerateMesh(m_quadtree->GetNodes());
}
//================================================================================================================
void QuadTreeRenderer::GenerateMesh(QNode* node)
{
	if (node == 0) return;

	//if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
	//{
	//	//if (m_tessellate)
	//	//	GenerateMeshTessellation10(m_nodes[i]);
	//	//else
	//	GenerateMeshNT(node);
	//}
	//else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
	//{
	//	if (m_quadtree->Tessellate())
	//		GenerateMeshTessellation(node);
	//	else
	//		GenerateMeshNT(node);
	//}

	if (m_quadtree->Tessellate())
		GenerateMeshTessellation(node);
	else
		GenerateMeshNT(node);

	//Progress through the children
	GenerateMesh(node->children[0]);
	GenerateMesh(node->children[1]);
	GenerateMesh(node->children[2]);
	GenerateMesh(node->children[3]);
}
//================================================================================================================
void QuadTreeRenderer::GenerateMeshNT(QNode* node)
{
	if (node == 0) return;

	if (node->type == LEAF)
	{
		int LeafWidth = m_quadtree->LeafWidth();
		int TerrScale = m_quadtree->TerrainScale();
		int MapSize   = m_quadtree->MapSize();

		int vert_count = (LeafWidth) * (LeafWidth);
		node->vertices = new VertexTextureBoundsY[vert_count];
		
		float red = 1, green = 1, blue = 1;
		int index = 0;
		int height = 0;
		int terrScale = TerrScale;//Nice Big size is 8
		// Center the grid in model space
		float halfWidth = ((float)LeafWidth - 1.0f) / 2.0f;
		float halfLength = ((float)LeafWidth - 1.0f) / 2.0f;

		#pragma region "Load Vertices"
		for (int z = (int)node->boundingCoord[0].z; z <= (int)node->boundingCoord[2].z; z++)
		{
			for (int x = (int)node->boundingCoord[0].x; x <= (int)node->boundingCoord[1].x; x++)
			{
				if (m_quadtree->UsingHeight())
				{
					if (x < MapSize && z < MapSize)
					{
						if (m_quadtree->GetMapExt() == RAW)
							height = m_quadtree->SampleHeight(x + (z * MapSize + 1));
						else
							height = m_quadtree->SampleHeight(x, z);
					}
				}

				//node->vertices[index] = new VertexTexture();

				node->vertices[index].position = D3DXVECTOR3(((x - halfWidth) * terrScale), height, ((z - halfLength) * terrScale));

				node->vertices[index].tex = D3DXVECTOR2(x * (1.0f / (MapSize - 1)), z * (1.0f / (MapSize - 1)));

				//node->vertices[index].type = VType::TEXTURE;
				//switch (node->vertices[index].type)
				//{
				//	case VType::COLOR:
				//	{
				//		VertexColor* vc = (VertexColor*)(&node->vertices[index]);
				//		vc->color = D3DXVECTOR4(red, green, blue, 1.0f);
				//	}
				//	break;
				//	case VType::TEXTURE:
				//	{
				//		VertexTexture* vt = (VertexTexture*)(&node->vertices[index]);
				//		vt->tex.x = vt->position.x * (1.0f / (MapSize - 1));
				//		vt->tex.y = vt->position.z * (1.0f / (MapSize - 1));
				//	}
				//	break;
				//	default: break;//Nothing
				//}

				index++;
			}
		}
		#pragma endregion

		#pragma region "Load Indices"
		index = 0;
		int index_count = ((LeafWidth) * 2) * (LeafWidth - 1) + (LeafWidth - 2);
		node->m_index_count = index_count;
		node->indices = new unsigned long[index_count];
		for (int z = 0; z < LeafWidth - 1; z++)
		{
			//Even rows move left to right, odd rows right to left
			if (z % 2 == 0)
			{
				//Even row
				int x;
				for (x = 0; x < LeafWidth; x++)
				{
					node->indices[index++] = x + (z * LeafWidth);
					node->indices[index++] = x + (z * LeafWidth) + LeafWidth;
				}
				//Insert degenerate vertex if this isn't the last row
				if (z != LeafWidth - 2)
				{
					node->indices[index++] = --x + (z * LeafWidth);
				}
			}
			else
			{
				//Odd row
				int x;
				for (x = LeafWidth - 1; x >= 0; x--)
				{
					node->indices[index++] = x + (z * LeafWidth);
					node->indices[index++] = x + (z * LeafWidth) + LeafWidth;
				}
				//Insert degenerate vertex if this isn't the last row
				if (z != LeafWidth - 2)
				{
					node->indices[index++] = ++x + (z * LeafWidth);
				}
			}
		}
		#pragma endregion

		#pragma region "Create Buffers"
		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
		{
			HRESULT result;
			D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			D3D10_SUBRESOURCE_DATA vertexData, indexData;

			//Setup the description of the static vertex buffer
			vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(VertexTextureBoundsY) * vert_count;
			vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			//vertexBufferDesc.StructureByteStride = 0;

			//Give the subresource a pointer to the vertex data
			vertexData.pSysMem = (node->vertices);
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			//Create the vertex buffer
			result = m_d3d->GetDevice10()->CreateBuffer(&vertexBufferDesc, &vertexData, &node->m_vertex_buffer10);
			if (FAILED(result)) return;

			//Setup the description of the static index buffer
			indexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
			indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
			indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			indexBufferDesc.MiscFlags = 0;
			//indexBufferDesc.StructureByteStride = 0;

			//Give the subresource a pointer to the index data
			indexData.pSysMem = node->indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			//Create the index buffer
			result = m_d3d->GetDevice10()->CreateBuffer(&indexBufferDesc, &indexData, &node->m_index_buffer10);
			if (FAILED(result)) return;

			//Release the arrays new that the buffers have been created and loaded
			delete[] node->vertices;
			node->vertices = 0;

			delete[] node->indices;
			node->indices = 0;
		}
		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
		{
			HRESULT result;
			D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
			D3D11_SUBRESOURCE_DATA vertexData, indexData;

			//Setup the description of the static vertex buffer
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.ByteWidth = sizeof(VertexTextureBoundsY) * vert_count;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			//Give the subresource a pointer to the vertex data
			vertexData.pSysMem = node->vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			//Create the vertex buffer
			result = m_d3d->GetDevice11()->CreateBuffer(&vertexBufferDesc, &vertexData, & node->m_vertex_buffer11);
			if (FAILED(result)) return;

			//Setup the description of the static index buffer
			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			//Give the subresource a pointer to the index data
			indexData.pSysMem = node->indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			//Create the index buffer
			result = m_d3d->GetDevice11()->CreateBuffer(&indexBufferDesc, &indexData, & node->m_index_buffer11);
			if (FAILED(result)) return;

			//Release the arrays new that the buffers have been created and loaded
			delete[] node->vertices;
			node->vertices = 0;

			delete[] node->indices;
			node->indices = 0;
		}
		#pragma endregion
	}
}
//================================================================================================================
void QuadTreeRenderer::GenerateMeshTessellation(QNode* node)
{
	if (node == 0) return;

	//Can Only do Tessellation in DirectX 11
	//if (m_d3d->GetEngineOptions()->m_d3dVersion != DIRECTX11)
	//	return;

	if (node->type == LEAF)
	{
		int LeafWidth = m_quadtree->LeafWidth();
		int TerrScale = m_quadtree->TerrainScale();
		int MapSize   = m_quadtree->MapSize();

		int vert_count = 4;
		node->vertices = new VertexTextureBoundsY[vert_count];
		
		int r;
		float red = 1, green = 1, blue = 1;
		int index = 0;
		
		int terrScale = TerrScale;//Nice Big size is 8
		// Center the grid in model space
		float halfWidth = ((float)LeafWidth - 1.0f) / 2.0f;
		float halfLength = ((float)LeafWidth - 1.0f) / 2.0f;

		#pragma region "Load Vertices"
		//node->vertices[0] = new VertexTexture();
		//node->vertices[1] = new VertexTexture();
		//node->vertices[2] = new VertexTexture();
		//node->vertices[3] = new VertexTexture();

		//Looks like I only need four vertices to pass to the tessellation shader
		node->vertices[0].position = D3DXVECTOR3((((int)node->boundingCoord[0].x - halfWidth) * terrScale), (int)node->boundingCoord[0].y, (((int)node->boundingCoord[0].z - halfLength) * terrScale));
		node->vertices[1].position = D3DXVECTOR3((((int)node->boundingCoord[1].x - halfWidth) * terrScale), (int)node->boundingCoord[1].y, (((int)node->boundingCoord[1].z - halfLength) * terrScale));
		node->vertices[2].position = D3DXVECTOR3((((int)node->boundingCoord[2].x - halfWidth) * terrScale), (int)node->boundingCoord[2].y, (((int)node->boundingCoord[2].z - halfLength) * terrScale));
		node->vertices[3].position = D3DXVECTOR3((((int)node->boundingCoord[3].x - halfWidth) * terrScale), (int)node->boundingCoord[3].y, (((int)node->boundingCoord[3].z - halfLength) * terrScale));

		for (int i = 0; i < 4; i++)
		{
			node->vertices[i].tex = D3DXVECTOR2(node->boundingCoord[i].x * (1.0f / (MapSize - 1)), node->boundingCoord[i].z * (1.0f / (MapSize - 1)));
			//node->vertices[i]->type = VType::TEXTURE;
			//switch (node->vertices[i]->type)
			//{
			//	case VType::COLOR:
			//	{
			//		VertexColor* vc = (VertexColor*)(&node->vertices[i]);
			//		vc->color = D3DXVECTOR4(red, green, blue, 1.0f);
			//	}
			//	break;
			//	case VType::TEXTURE:
			//	{
			//		VertexTexture* vt = (VertexTexture*)(&node->vertices[i]);
			//		vt->tex.x = vt->position.x * (1.0f / (MapSize - 1));
			//		vt->tex.y = vt->position.z * (1.0f / (MapSize - 1));
			//	}
			//	break;
			//	default: break;//Nothing
			//}
		}

		UINT patchID = (node->boundingCoord[0].z * MapSize) + node->boundingCoord[0].x;
		XMFLOAT2 patchBoundsY0 = m_quadtree->GetPatchBoundsY(patchID);
		patchID = (node->boundingCoord[1].z * MapSize) + node->boundingCoord[1].x;
		XMFLOAT2 patchBoundsY1 = m_quadtree->GetPatchBoundsY(patchID);
		patchID = (node->boundingCoord[2].z * MapSize) + node->boundingCoord[2].x;
		XMFLOAT2 patchBoundsY2 = m_quadtree->GetPatchBoundsY(patchID);
		patchID = (node->boundingCoord[3].z * MapSize) + node->boundingCoord[3].x;
		XMFLOAT2 patchBoundsY3 = m_quadtree->GetPatchBoundsY(patchID);

		//Need to get the the correct boundsY
		node->vertices[0].boundsY = D3DXVECTOR2(patchBoundsY0.x, patchBoundsY0.y);
		node->vertices[1].boundsY = D3DXVECTOR2(patchBoundsY1.x, patchBoundsY1.y);
		node->vertices[2].boundsY = D3DXVECTOR2(patchBoundsY2.x, patchBoundsY2.y);
		node->vertices[3].boundsY = D3DXVECTOR2(patchBoundsY3.x, patchBoundsY3.y);
		#pragma endregion

		#pragma region "Load Indices"
		int index_count = 4;
		node->m_index_count = index_count;
		node->indices = new unsigned long[index_count];
		node->indices[0] = 1;
		node->indices[1] = 0;
		node->indices[2] = 3;
		node->indices[3] = 2;
		#pragma endregion

		#pragma region "Create Buffers"
		HRESULT result;
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;

		//Setup the description of the static vertex buffer
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VertexTextureBoundsY) * vert_count;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		//Give the subresource a pointer to the vertex data
		vertexData.pSysMem = node->vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		//Create the vertex buffer
		result = m_d3d->GetDevice11()->CreateBuffer(&vertexBufferDesc, &vertexData, & node->m_vertex_buffer11);
		if (FAILED(result)) return;

		//Setup the description of the static index buffer
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		//Give the subresource a pointer to the index data
		indexData.pSysMem = node->indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		//Create the index buffer
		result = m_d3d->GetDevice11()->CreateBuffer(&indexBufferDesc, &indexData, & node->m_index_buffer11);
		if (FAILED(result)) return;

		//Release the arrays new that the buffers have been created and loaded
		//delete[] node->vertices;
		//node->vertices = 0;

		delete[] node->indices;
		node->indices = 0;
		#pragma endregion
	}
}
//================================================================================================================
void QuadTreeRenderer::GenerateMeshTessellation10(QNode* node)
{
	if (node == 0) return;

	if (node->type == LEAF)
	{
		int LeafWidth = m_quadtree->LeafWidth();
		int TerrScale = m_quadtree->TerrainScale();
		int MapSize   = m_quadtree->MapSize();

		//Generate the vertex buffer
		int vert_count = (LeafWidth) * (LeafWidth);
		node->vertices = new VertexTextureBoundsY[vert_count];
		
		float red = 1, green = 1, blue = 1;
		int index = 0;
		int height = 0;
		int terrScale = TerrScale;//Nice Big size is 8
		// Center the grid in model space
		float halfWidth = ((float)LeafWidth - 1.0f) / 2.0f;
		float halfLength = ((float)LeafWidth - 1.0f) / 2.0f;//9999999999.0f
		D3DXVECTOR3 vec_min(-9999999999.0f, -9999999999.0f, -9999999999.0f);
		D3DXVECTOR3 vec_max(9999999999.0f, 9999999999.0f, 9999999999.0f);

		float xMax = FLT_MAX;//9999999999.0f;
		float xMin = FLT_MIN;//-9999999999.0f;
		float zMax = FLT_MAX;//9999999999.0f;
		float zMin = FLT_MIN;//-9999999999.0f;
		float heightMax = FLT_MAX;//9999999999.0f;
		float heightMin = FLT_MIN;//-9999999999.0f;

		#pragma region "Load Vertices"
		for (int z = (int)node->boundingCoord[0].z; z <= (int)node->boundingCoord[2].z; z++)
		{
			for (int x = (int)node->boundingCoord[0].x; x <= (int)node->boundingCoord[1].x; x++)
			{
				if (m_quadtree->UsingHeight())
				{
					if (x < MapSize && z < MapSize)
					{
						if (m_quadtree->GetMapExt() == RAW)
							height = m_quadtree->SampleHeight(x + (z * MapSize + 1));
						else
							height = m_quadtree->SampleHeight(x, z);
					}
				}

				height = MathHelper::Max<float>(height, heightMax);
				height = MathHelper::Min<float>(height, heightMin);

				xMax = MathHelper::Max<float>(((x - halfWidth) * terrScale), xMax);
				xMin = MathHelper::Min<float>(((x - halfWidth) * terrScale), xMin);

				zMax = MathHelper::Max<float>(((z - halfLength) * terrScale), zMax);
				zMin = MathHelper::Min<float>(((z - halfLength) * terrScale), zMin);

				//node->vertices[index] = new VertexTexture();

				node->vertices[index].position = D3DXVECTOR3(((x - halfWidth) * terrScale), height, ((z - halfLength) * terrScale));

				node->vertices[index].tex = D3DXVECTOR2(x * (1.0f / (MapSize - 1)), z * (1.0f / (MapSize - 1)));

				//Min(node->vertices[index].position, vec_min, vec_min);
				//Max(node->vertices[index].position, vec_max, vec_max);
				//D3DXVec3Minimize(&node->bounds._bmin, &node->bounds._bmin, &node->vertices[index].position);
				//D3DXVec3Maximize(&node->bounds._bmax, &node->bounds._bmax, &node->vertices[index].position);

				
				//node->vertices[index]->type = VType::TEXTURE;
				//switch (node->vertices[index]->type)
				//{
				//	case VType::COLOR:
				//	{
				//		VertexColor* vc = (VertexColor*)(&node->vertices[index]);
				//		vc->color = D3DXVECTOR4(red, green, blue, 1.0f);
				//	}
				//	break;
				//	case VType::TEXTURE:
				//	{
				//		VertexTexture* vt = (VertexTexture*)(&node->vertices[index]);
				//		vt->tex.x = vt->position.x * (1.0f / (MapSize - 1));
				//		vt->tex.y = vt->position.z * (1.0f / (MapSize - 1));
				//	}
				//	break;
				//	default: break;//Nothing
				//}

				UINT patchID = z * MapSize + x;
				XMFLOAT2 patchBoundsY = m_quadtree->GetPatchBoundsY(patchID);
				node->vertices[index].boundsY = D3DXVECTOR2(patchBoundsY.x, patchBoundsY.y);

				index++;
			}
		}
		#pragma endregion

		#pragma region "Create Vertex Buffer"
		HRESULT result;
		D3D10_BUFFER_DESC vertexBufferDesc;
		D3D10_SUBRESOURCE_DATA vertexData;
		//Setup the description of the static vertex buffer
		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VertexTextureBoundsY) * vert_count;
		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		//vertexBufferDesc.StructureByteStride = 0;

		//Give the subresource a pointer to the vertex data
		vertexData.pSysMem = (node->vertices);
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		//Create the vertex buffer
		result = m_d3d->GetDevice10()->CreateBuffer(&vertexBufferDesc, &vertexData, &node->m_vertex_buffer10);
		if (FAILED(result)) return;
		#pragma endregion
	}
}
//================================================================================================================
void QuadTreeRenderer::BuildHeightmapSRV()
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = m_quadtree->MapSize();
	texDesc.Height = m_quadtree->MapSize();
    texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format    = DXGI_FORMAT_R16_FLOAT;
	texDesc.SampleDesc.Count   = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	vector<float> hv = m_quadtree->GetHeightmapVec();
	// HALF is defined in xnamath.h, for storing 16-bit float.
	std::vector<HALF> hmap(hv.size());
	std::transform(hv.begin(), hv.end(), hmap.begin(), XMConvertFloatToHalf);
	
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &hmap[0];
    data.SysMemPitch = m_quadtree->MapSize() * sizeof(HALF);
    data.SysMemSlicePitch = 0;

	ID3D11Texture2D* hmapTex = 0;
	m_d3d->GetDevice11()->CreateTexture2D(&texDesc, &data, &hmapTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;
	m_d3d->GetDevice11()->CreateShaderResourceView(hmapTex, &srvDesc, &mHeightMapSRV);

	// SRV saves reference.
	SAFE_RELEASE(hmapTex);
}
//================================================================================================================
void QuadTreeRenderer::BuildLayerMapArraySRV(string layerMapFilename0,
			string layerMapFilename1,
			string layerMapFilename2,
			string layerMapFilename3,
			string layerMapFilename4)
{
	vector<string> layerFilenames;
	layerFilenames.push_back(layerMapFilename0);
	layerFilenames.push_back(layerMapFilename1);
	layerFilenames.push_back(layerMapFilename2);
	layerFilenames.push_back(layerMapFilename3);
	layerFilenames.push_back(layerMapFilename4);
	mLayerMapArraySRV = d3dHelper::CreateTexture2DArraySRV(m_d3d->GetDevice11(), 
				m_d3d->GetDeviceContext(), layerFilenames);
}
//================================================================================================================
void QuadTreeRenderer::BuildBlendMapSRV(string blendMapFilename)
{
	D3DX11CreateShaderResourceViewFromFile(
		m_d3d->GetDevice11(), blendMapFilename.c_str(), 0, 0, &mBlendMapSRV, 0);
}
//================================================================================================================
void QuadTreeRenderer::BuildNormalMapSRV(string normalMapFilename)
{
	D3DX11CreateShaderResourceViewFromFile(
		m_d3d->GetDevice11(), normalMapFilename.c_str(), 0, 0, &mNormalMapSRV, 0);
}
//================================================================================================================
void QuadTreeRenderer::BuildShadowMapSRV()
{

}
//================================================================================================================
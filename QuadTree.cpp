#include "stdafx.h"
#include "QuadTree.h"
#include "Convert.h"
//-------------------------------------------------------------------------------------
QuadTree::QuadTree(float heightScale, HeightExt ext, bool tessellate) : m_heightmapName("")
{
	//Loads a default QuadTree
	//4*4 w/ 16x16 node size
	m_mapExt = ext;
	m_heightScale = heightScale;
	m_cellSpacing = 0.5f;
	m_tessellate = tessellate;
	m_gridSize = 16;
	m_leafWidth = 5;
	m_terrScale = 2;
	LoadQuadTree();
}
//-------------------------------------------------------------------------------------
QuadTree::QuadTree(int MapSize, float heightScale, HeightExt ext, bool tessellate, int terrScale) : m_heightmapName("")
{
	//Loads a default QuadTree
	//4*4 w/ GridSizexGridSize node size
	m_mapExt = ext;
	m_heightScale = heightScale;
	m_cellSpacing = 0.5f;
	m_tessellate = tessellate;
	m_gridSize = MapSize;
	m_leafWidth = 5;
	m_terrScale = terrScale;
	LoadQuadTree();
}
//-------------------------------------------------------------------------------------
QuadTree::QuadTree(int MapSize, int leafWidth, float heightScale, HeightExt ext, bool tessellate, int terrScale) : m_heightmapName("")
{
	//Loads a default QuadTree
	//leafWidth*leafWidth w/ GridSizexGridSize node size
	m_mapExt = ext;
	m_heightScale = heightScale;
	m_cellSpacing = 0.5f;
	m_tessellate = tessellate;
	m_gridSize = MapSize;
	m_leafWidth = leafWidth + 1;
	m_terrScale = terrScale;
	LoadQuadTree();
}
//-------------------------------------------------------------------------------------
QuadTree::QuadTree(string heightmap, int MapSize, float cellSpacing, float heightScale, HeightExt ext, bool tessellate, int terrScale) : m_heightmapName(heightmap)
{
	m_mapExt = ext;
	m_heightScale = heightScale;
	m_cellSpacing = cellSpacing;
	m_tessellate = tessellate;
	m_leafWidth = 5;
	m_gridSize = MapSize;
	m_terrScale = terrScale;
	LoadQuadTree();
}
//-------------------------------------------------------------------------------------
QuadTree::QuadTree(string heightmap, int MapSize, int leafWidth, float cellSpacing, float heightScale, HeightExt ext, bool tessellate, int terrScale) : m_heightmapName(heightmap)
{
	m_mapExt = ext;
	m_heightScale = heightScale;
	m_cellSpacing = cellSpacing;
	m_tessellate = tessellate;
	m_leafWidth = leafWidth + 1;
	m_gridSize = MapSize;
	m_terrScale = terrScale;
	LoadQuadTree();
}
//-------------------------------------------------------------------------------------
QuadTree::QuadTree(const QuadTree& qtree)
{

}
//-------------------------------------------------------------------------------------
QuadTree::~QuadTree()
{

}
//-------------------------------------------------------------------------------------
void QuadTree::LoadQuadTree()
{
	if (m_heightmapName != "")
		LoadMap(m_heightmapName);
	else
		LoadMap();

	InitializeTree();
	CreateTree();
	Print();
}
//-------------------------------------------------------------------------------------
void QuadTree::LoadMap(string heightmap)
{
	if (m_mapExt == RAW)
	{
		m_heightmap = new Heightmap(heightmap, m_mapExt, m_gridSize, m_gridSize, m_cellSpacing, m_heightScale, m_leafWidth);
		m_heightmap->Smooth();
	}
	else
		m_heightmap = new Heightmap(heightmap, m_mapExt, 0, 0, m_cellSpacing, m_heightScale, m_leafWidth);

	//m_heightmap->LoadElevation(heightmap);
	//m_heightmap->Normalize();

	m_gridSize = m_heightmap->Height();

	int index = 0;
	float height = 0;
	
	mPatchBoundsY.resize((m_gridSize+1) * (m_gridSize+1));

	vertices = new VertexTextureBoundsY*[m_gridSize+1];
	for (int i = 0; i < m_gridSize+1; i++)
		vertices[i] = new VertexTextureBoundsY[m_gridSize+1];

	for (int z = 0; z < m_gridSize+1; z++)
	{
		for (int x = 0; x < m_gridSize+1; x++)
		{
			index = (z * m_gridSize) + x;

			if (x < m_gridSize && z < m_gridSize)
			{
				if (m_mapExt == RAW)
					height = m_heightmap->SampleHeight(index);
				else
					height = m_heightmap->SampleHeight(x, z);
			}

			vertices[x][z].position = D3DXVECTOR3(x, z, height);

			vertices[x][z].tex = D3DXVECTOR2(x * (1.0f / m_gridSize), z * (1.0f / m_gridSize));

			//Build the heightmap vector
			if (x < m_gridSize && z < m_gridSize)
				heightmap_vec.push_back(height);

			//Calculate the patch bounds min/max height
			/*UINT x0 = x * m_leafWidth;
			UINT x1 = (x + 1) * m_leafWidth;
			UINT y0 = z * m_leafWidth;
			UINT y1 = (z + 1) * m_leafWidth;

			float minY = MathHelper::Infinity;
			float maxY = -MathHelper::Infinity;

			bool canBreak = false;
			for (UINT i = y0; i <= y1; i++)
			{
				for (UINT j = x0; j <= x1; j++)
				{
					UINT k = i * m_gridSize + i;

					if (i < m_gridSize && j < m_gridSize)
					{
						minY = MathHelper::Min(minY, m_heightmap->SampleHeight(j, i));
						maxY = MathHelper::Max(maxY, m_heightmap->SampleHeight(j, i));
					}
				}

				if (canBreak) break;
			}*/

			UINT patchID = z * (m_gridSize) + x;
			mPatchBoundsY[patchID] = XMFLOAT2(height, height);
		}
	}

	m_useHeight = true;
}
//-------------------------------------------------------------------------------------
void QuadTree::LoadMap()
{
	int index = 0;
	
	vertices = new VertexTextureBoundsY*[m_gridSize+1];
	for (int i = 0; i < m_gridSize+1; i++)
		vertices[i] = new VertexTextureBoundsY[m_gridSize+1];

	mPatchBoundsY.resize((m_gridSize+1) * (m_gridSize+1));

	for (int z = 0; z < m_gridSize+1; z++)
	{
		for (int x = 0; x < m_gridSize+1; x++)
		{
			//index = x + (z * m_gridSize + 1);

			vertices[x][z].position = D3DXVECTOR3(x,z,0);
			
			vertices[x][z].tex = D3DXVECTOR2(x * (1.0f / m_gridSize), z * (1.0f / m_gridSize));

			//Build the heightmap vector
			if (x < m_gridSize && z < m_gridSize)
				heightmap_vec.push_back(0);

			UINT patchID = z * (m_gridSize) + x;
			mPatchBoundsY[patchID] = XMFLOAT2(0, 0);

			//vertices[x][z].type = VType::TEXTURE;

			//switch (vertices[x][z].type)
			//{
			//	case VType::COLOR:
			//	{
			//		VertexColor* vc = (VertexColor*)(&vertices[x][z]);
			//		vc->color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			//	}
			//	break;
			//	case VType::TEXTURE:
			//	{
			//		VertexTexture* vt = (VertexTexture*)(&vertices[x][z]);
			//		vt->tex.x = 1.0f / m_gridSize;
			//		vt->tex.y = 1.0f / m_gridSize;
			//	}
			//	break;
			//	default: break;//Nothing
			//}
		}
	}

	m_useHeight = false;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//void QuadTree::Render(D3D* d3d, ColorShaderClass* csc, TerrainShader* ts, D3Camera* camera, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
//{
//	for (int i = 0; i < m_nodes.size(); i++)
//	//parallel_for (size_t(0), m_nodes.size(), [&](size_t i)
//	{
//		QNode* node = m_nodes[i];
//		if (node->type == LEAF)
//		{
//			int tessFactor = 0;
//
//			//if (m_tessellate && m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
//			//{
//			//	D3DXVECTOR3 vd = camera->g_camera->getPosition();
//			//	//D3DXVECTOR3 vd = cam->GetPosition();
//			//	tessFactor = TessFactor(vd, node);
//			//	if (tessFactor > 6)
//			//		tessFactor = 6;
//			//}
//			
//			RenderMeshBuffers(node, tessFactor);
//
//			if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
//			{
//				//if (m_tessellate)
//				//{
//				//	csc->Render(d3d, m_index_count[tessFactor]/*node->m_index_countTess[tessFactor]*/, worldMatrix, viewMatrix, projectionMatrix);
//				//}
//				//else
//					csc->Render(d3d, node->m_index_count, worldMatrix, viewMatrix, projectionMatrix);
//			}
//			else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
//			{
//				ts->SetShaderParameters11(m_d3d->GetDeviceContext(), camera, mWorld);
//				ts->Render11(m_d3d, node->m_index_count, camera, mWorld, 0);
//			}
//		}
//	}
//}
//-------------------------------------------------------------------------------------
int QuadTree::GetNodeIndexCount(QNode* node)
{
	if (node->type == LEAF)
	{
		return node->m_index_count;
	}
	/*for (int i = 0; i < m_nodes.size(); i++)
	{
		if (m_nodes[i]->type == LEAF)
		{
			return m_nodes[i]->m_index_count;
		}
	}*/


}
//-------------------------------------------------------------------------------------
//QNode* QuadTree::GetNodes(int i)
//{
//	return m_nodes;//[i];
//}
//-------------------------------------------------------------------------------------
int QuadTree::GetSize()
{
	return m_nodeCount;//m_nodes.size();
}
//-------------------------------------------------------------------------------------
void QuadTree::Print()
{
	out.open("QuadTreeNodeLog.txt", ios::out);

	/*for (int i = 0; i < m_nodes.size(); i++)
	{
		PrintNode(m_nodes[i]);
	}*/

	PrintNode(m_nodes);

	out.close();
}
//-------------------------------------------------------------------------------------
void QuadTree::InitializeTree()
{
	m_totalTreeID = 0;
	m_totalLeaves = (m_gridSize / (m_leafWidth - 1)) * (m_gridSize / (m_leafWidth - 1));
	m_nodeCount = numberOfNodes(m_totalLeaves, m_leafWidth - 1);
}
//-------------------------------------------------------------------------------------
void QuadTree::CreateTree()
{
	D3DXVECTOR3 RootBounds[4];

	RootBounds[0].x = 0;
	RootBounds[0].y = 0;
	RootBounds[0].z = 0;

	RootBounds[1].x = m_gridSize;
	RootBounds[1].y = 0;
	RootBounds[1].z = 0;

	RootBounds[2].x = 0;
	RootBounds[2].y = 0;
	RootBounds[2].z = m_gridSize;

	RootBounds[3].x = m_gridSize;
	RootBounds[3].y = 0;
	RootBounds[3].z = m_gridSize;

	CreateNode(m_nodes, RootBounds, 0, 0);

	//reverse(m_nodes.begin(), m_nodes.end());
}
//-------------------------------------------------------------------------------------
int QuadTree::numberOfNodes(int leaves, int leafWidth)
{
	int ctr = 0, val = 0;

	while (val == 0)
	{
		ctr += leaves;
		leaves /= leafWidth;

		if (leaves == 0)
			break;

		if (leaves == 1)
			val = 1;
	}

	ctr++;

	return ctr;
}
//-------------------------------------------------------------------------------------
void QuadTree::CreateNode(QNode*& node, D3DXVECTOR3 bounds[4], int parentID, int nodeID)
{
	node = new QNode();
	node->children[0] = 0;
	node->children[1] = 0;
	node->children[2] = 0;
	node->children[3] = 0;

	float xDiff = bounds[0].x - bounds[1].x;
	float zDiff = bounds[0].z - bounds[2].z;

	//Find the width and height of the node
	int NodeWidth = (int)abs(xDiff);
	int NodeHeight = (int)abs(zDiff);

	QNodeType type;

	if (NodeWidth == m_leafWidth - 1)
	{
		type = LEAF;
	}
	else
	{
		type = NODE;
	}

	//QNode* node = new QNode();

	node->type = type;
	node->nodeID = nodeID;
	node->parentID = parentID;

	int bounds0X = (int)bounds[0].x;
	int bounds0Z = (int)bounds[0].z;

	int bounds1X = (int)bounds[1].x;
	int bounds1Z = (int)bounds[1].z;

	int bounds2X = (int)bounds[2].x;
	int bounds2Z = (int)bounds[2].z;

	int bounds3X = (int)bounds[3].x;
	int bounds3Z = (int)bounds[3].z;

	//Need to get the bounding coord for this node
	/*node->boundingCoord[0].x = vertices[bounds0X][bounds0Z].position.x;
	node->boundingCoord[0].y = 0;
	node->boundingCoord[0].z = vertices[bounds0X][bounds0Z].position.z;

	node->boundingCoord[1].x = vertices[bounds1X][bounds1Z].position.x;
	node->boundingCoord[1].y = 0;
	node->boundingCoord[1].z = vertices[bounds1X][bounds1Z].position.z;

	node->boundingCoord[2].x = vertices[bounds2X][bounds2Z].position.x;
	node->boundingCoord[2].y = 0;
	node->boundingCoord[2].z = vertices[bounds2X][bounds2Z].position.z;

	node->boundingCoord[3].x = vertices[bounds3X][bounds3Z].position.x;
	node->boundingCoord[3].y = 0;
	node->boundingCoord[3].z = vertices[bounds3X][bounds3Z].position.z;*/

	float height0 = 0;
	float height1 = 0;
	float height2 = 0;
	float height3 = 0;

	if (m_useHeight)
	{
		/*
			if (m_quadtree->GetMapExt() == RAW)
				height = m_quadtree->SampleHeight(x + (z * MapSize + 1));
			else
				height = m_quadtree->SampleHeight(x, z);
		*/
		if (bounds0X < m_gridSize && bounds0Z < m_gridSize) {(GetMapExt() == RAW) ? height0 = m_heightmap->SampleHeight((bounds0Z * MapSize()) + bounds0X) : height0 = m_heightmap->SampleHeight(bounds0X, bounds0Z);}
		if (bounds1X < m_gridSize && bounds1Z < m_gridSize) {(GetMapExt() == RAW) ? height1 = m_heightmap->SampleHeight((bounds1Z * MapSize()) + bounds1X) : height1 = m_heightmap->SampleHeight(bounds1X, bounds1Z);}
		if (bounds2X < m_gridSize && bounds2Z < m_gridSize) {(GetMapExt() == RAW) ? height2 = m_heightmap->SampleHeight((bounds2Z * MapSize()) + bounds2X) : height2 = m_heightmap->SampleHeight(bounds2X, bounds2Z);}
		if (bounds3X < m_gridSize && bounds3Z < m_gridSize) {(GetMapExt() == RAW) ? height3 = m_heightmap->SampleHeight((bounds3Z * MapSize()) + bounds3X) : height3 = m_heightmap->SampleHeight(bounds3X, bounds3Z);}
	}

	//Need to get the bounding coord for this node
	node->boundingCoord[0].x = bounds0X;
	node->boundingCoord[0].y = height0;
	node->boundingCoord[0].z = bounds0Z;

	node->boundingCoord[1].x = bounds1X;
	node->boundingCoord[1].y = height1;
	node->boundingCoord[1].z = bounds1Z;

	node->boundingCoord[2].x = bounds2X;
	node->boundingCoord[2].y = height2;
	node->boundingCoord[2].z = bounds2Z;

	node->boundingCoord[3].x = bounds3X;
	node->boundingCoord[3].y = height3;
	node->boundingCoord[3].z = bounds3Z;

	if (type == LEAF)
	{
		//m_nodes.push_back( node );
		return;
	}
	else
	{
		#pragma region "Child Node 1"
		//======================================================================================================================
		//Child Node 1
		m_totalTreeID++;
		node->branches[0] = m_totalTreeID;
		D3DXVECTOR3 ChildBounds1[4];

		//Top-Left
		ChildBounds1[0].x = bounds[0].x;
		ChildBounds1[0].y = 0;
		ChildBounds1[0].z = bounds[0].z;

		//Top-Right
		ChildBounds1[1].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds1[1].y = 0;
		ChildBounds1[1].z = bounds[1].z;

		//Bottom-Left
		ChildBounds1[2].x = bounds[2].x;
		ChildBounds1[2].y = 0;
		ChildBounds1[2].z = bounds[0].z + abs((bounds[0].z - bounds[2].z) / 2);

		//Bottom-Right
		ChildBounds1[3].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds1[3].y = 0;
		ChildBounds1[3].z = bounds[0].z + abs((bounds[0].z - bounds[2].z) / 2);
		
		CreateNode(node->children[0], ChildBounds1, nodeID, m_totalTreeID);
		//======================================================================================================================
		#pragma endregion

		#pragma region "Child Node 2"
		//======================================================================================================================
		//Child Node 2
		m_totalTreeID++;
		node->branches[1] = m_totalTreeID;
		D3DXVECTOR3 ChildBounds2[4];

		//Top-Left
		ChildBounds2[0].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds2[0].y = 0;
		ChildBounds2[0].z = bounds[1].z;

		//Top-Right
		ChildBounds2[1].x = bounds[1].x;
		ChildBounds2[1].y = 0;
		ChildBounds2[1].z = bounds[1].z;

		//Bottom-Left
		ChildBounds2[2].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds2[2].y = 0;
		ChildBounds2[2].z = bounds[1].z + abs((bounds[1].z - bounds[3].z) / 2);

		//Bottom-Right
		ChildBounds2[3].x = bounds[1].x;
		ChildBounds2[3].y = 0;
		ChildBounds2[3].z = bounds[1].z + abs((bounds[1].z - bounds[3].z) / 2);
		
		CreateNode(node->children[1], ChildBounds2, nodeID, m_totalTreeID);
		//======================================================================================================================
		#pragma endregion

		#pragma region "Child Node 3"
		//======================================================================================================================
		//Child Node 3
		m_totalTreeID++;
		node->branches[2] = m_totalTreeID;
		D3DXVECTOR3 ChildBounds3[4];

		//Top-Left
		ChildBounds3[0].x = bounds[0].x;
		ChildBounds3[0].y = 0;
		ChildBounds3[0].z = bounds[0].z + abs((bounds[0].z - bounds[2].z) / 2);

		//Top-Right
		ChildBounds3[1].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds3[1].y = 0;
		ChildBounds3[1].z = bounds[0].z + abs((bounds[0].z - bounds[2].z) / 2);

		//Bottom-Left
		ChildBounds3[2].x = bounds[2].x;
		ChildBounds3[2].y = 0;
		ChildBounds3[2].z = bounds[2].z;

		//Bottom-Right
		ChildBounds3[3].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds3[3].y = 0;
		ChildBounds3[3].z = bounds[3].z;
		
		CreateNode(node->children[2], ChildBounds3, nodeID, m_totalTreeID);
		//======================================================================================================================
		#pragma endregion

		#pragma region "Child Node 4"
		//======================================================================================================================
		//Child Node 4
		m_totalTreeID++;
		node->branches[3] = m_totalTreeID;
		D3DXVECTOR3 ChildBounds4[4];

		//Top-Left
		ChildBounds4[0].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds4[0].y = 0;
		ChildBounds4[0].z = bounds[1].z + abs((bounds[1].z - bounds[3].z) / 2);

		//Top-Right
		ChildBounds4[1].x = bounds[3].x;
		ChildBounds4[1].y = 0;
		ChildBounds4[1].z = bounds[1].z + abs((bounds[1].z - bounds[3].z) / 2);//-1

		//Bottom-Left
		ChildBounds4[2].x = bounds[0].x + abs((bounds[0].x - bounds[1].x) / 2);
		ChildBounds4[2].y = 0;
		ChildBounds4[2].z = bounds[3].z;

		//Bottom-Right
		ChildBounds4[3].x = bounds[3].x;
		ChildBounds4[3].y = 0;
		ChildBounds4[3].z = bounds[3].z;
		
		CreateNode(node->children[3], ChildBounds4, nodeID, m_totalTreeID);
		//======================================================================================================================
		#pragma endregion

		//m_nodes.push_back( node );
	}
}
//-------------------------------------------------------------------------------------
QNode* QuadTree::LocateChildNode(int id)
{
	/*for (int i = 0; i < m_nodes.size(); i++)
	{
		if (m_nodes[i]->nodeID == id)
		{
			return m_nodes[i];
		}
	}*/

	return LocateChildNode(m_nodes, id);
}

QNode* QuadTree::LocateChildNode(QNode* node, int id)
{
	//Node does not exist
	if (node == 0) return 0;

	if (node->nodeID == id)
		return m_nodes;

	//Test child one
	QNode* n1 = LocateChildNode(node->children[0], id);
	
	//We found our node
	if (n1 != 0) return n1;

	//Test child two
	QNode* n2 = LocateChildNode(node->children[1], id);
	
	//We found our node
	if (n2 != 0) return n2;

	//Test child three
	QNode* n3 = LocateChildNode(node->children[2], id);
	
	//We found our node
	if (n3 != 0) return n3;

	//Test child four
	QNode* n4 = LocateChildNode(node->children[3], id);

	//We found our node
	if (n4 != 0) return n4;

	//Found Nothing
	return 0;
}
//-------------------------------------------------------------------------------------
QNode* QuadTree::LocateLeaf(QNode* node)
{
	QNodeType type = node->type;

	if (type == LEAF)
	{
		return node;
	}
	else
	{
		QNode* child = LocateChildNode(node->branches[0]);

		return LocateLeaf(child);
	}
}
//-------------------------------------------------------------------------------------
/*void QuadTree::ShutdownMeshBuffers()
{
	/*for (int i = 0; i < m_nodes.size(); i++)
	{
		QNode* node = m_nodes[i];
		if (node->type == LEAF)
		{
			if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
			{
				if (node->m_vertex_buffer10)
				{
					node->m_vertex_buffer10->Release();
					node->m_vertex_buffer10 = 0;
				}

				if (node->m_index_buffer10)
				{
					node->m_index_buffer10->Release();
					node->m_index_buffer10 = 0;
				}
			}
			else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
			{
				if (node->m_vertex_buffer11)
				{
					node->m_vertex_buffer11->Release();
					node->m_vertex_buffer11 = 0;
				}

				if (node->m_index_buffer11)
				{
					node->m_index_buffer11->Release();
					node->m_index_buffer11 = 0;
				}
			}
		}
	}
}*/
//-------------------------------------------------------------------------------------
//void QuadTree::RenderMeshBuffers(QNode* node, int tess_factor)
//{
//	if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
//	{
//		if (m_tessellate)
//		{
//			int tessFactor = tess_factor;
//
//			unsigned int stride;
//			unsigned int offset;
//
//			//Set the vertex buffer stride and offset
//			stride = sizeof(Vertex);
//			offset = 0;
//
//			m_d3d->GetDevice10()->IASetVertexBuffers(0, 1, &node->m_vertex_buffer10, &stride, &offset);
//
//			//Set the index buffer to active in the input assembler so it can be rendered
//			m_d3d->GetDevice10()->IASetIndexBuffer(/*node->m_index_bufferTess10[tessFactor]*/m_index_buffer[tessFactor], DXGI_FORMAT_R32_UINT, 0);
//
//			//Set the type of primitive that should be rendered from this vertex buffer
//			m_d3d->GetDevice10()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//		}
//		else
//		{
//			unsigned int stride;
//			unsigned int offset;
//
//			//Set the vertex buffer stride and offset
//			stride = sizeof(Vertex);
//			offset = 0;
//
//			m_d3d->GetDevice10()->IASetVertexBuffers(0, 1, &node->m_vertex_buffer10, &stride, &offset);
//
//			//Set the index buffer to active in the input assembler so it can be rendered
//			m_d3d->GetDevice10()->IASetIndexBuffer(node->m_index_buffer10, DXGI_FORMAT_R32_UINT, 0);
//
//			//Set the type of primitive that should be rendered from this vertex buffer
//			m_d3d->GetDevice10()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//		}
//	}
//	else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
//	{
//		unsigned int stride;
//		unsigned int offset;
//
//		//Set the vertex buffer stride and offset
//		stride = sizeof(Vertex);
//		offset = 0;
//
//		m_d3d->GetDeviceContext()->IASetVertexBuffers(0, 1, &node->m_vertex_buffer11, &stride, &offset);
//
//		//Set the index buffer to active in the input assembler so it can be rendered
//		m_d3d->GetDeviceContext()->IASetIndexBuffer(node->m_index_buffer11, DXGI_FORMAT_R32_UINT, 0);
//
//		if (m_tessellate)
//		{
//			//Set the type of primitive that should be rendered from this vertex buffer
//			m_d3d->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
//		}
//		else
//		{
//			//Set the type of primitive that should be rendered from this vertex buffer
//			m_d3d->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//		}
//	}
//}
//-------------------------------------------------------------------------------------
//void QuadTree::GenerateMesh()
//{
//	for (int i = 0; i < m_nodes.size(); i++)
//	{
//		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
//		{
//			//if (m_tessellate)
//			//	GenerateMeshTessellation10(m_nodes[i]);
//			//else
//			GenerateMeshNT(m_nodes[i]);
//		}
//		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
//		{
//			if (m_tessellate)
//				GenerateMeshTessellation(m_nodes[i]);
//			else
//				GenerateMeshNT(m_nodes[i]);
//		}
//	}
//}
////-------------------------------------------------------------------------------------
//void QuadTree::GenerateMeshNT(QNode* node)
//{
//	if (node->type == LEAF)
//	{
//		int vert_count = (m_leafWidth) * (m_leafWidth);
//		node->vertices = new Vertex[vert_count];
//		
//		float red = 1, green = 1, blue = 1;
//		int index = 0;
//		int height = 0;
//		int terrScale = m_terrScale;//Nice Big size is 8
//		// Center the grid in model space
//		float halfWidth = ((float)m_leafWidth - 1.0f) / 2.0f;
//		float halfLength = ((float)m_leafWidth - 1.0f) / 2.0f;
//
//		#pragma region "Load Vertices"
//		for (int z = (int)node->boundingCoord[0].z; z <= (int)node->boundingCoord[2].z; z++)
//		{
//			for (int x = (int)node->boundingCoord[0].x; x <= (int)node->boundingCoord[1].x; x++)
//			{
//				if (m_useHeight)
//				{
//					if (x < m_gridSize && z < m_gridSize)
//						height = m_heightmap->SampleHeight(x, z);
//				}
//
//				node->vertices[index].position = D3DXVECTOR3(((x - halfWidth) * terrScale), height, ((z - halfLength) * terrScale));
//
//				switch (node->vertices[index].type)
//				{
//					case VType::COLOR:
//					{
//						VertexColor* vc = (VertexColor*)(&node->vertices[index]);
//						vc->color = D3DXVECTOR4(red, green, blue, 1.0f);
//					}
//					break;
//					case VType::TEXTURE:
//					{
//						VertexTexture* vt = (VertexTexture*)(&node->vertices[index]);
//						vt->tex.x = 1.0f;
//						vt->tex.y = 1.0f;
//					}
//					break;
//					default: break;//Nothing
//				}
//
//				index++;
//			}
//		}
//		#pragma endregion
//
//		#pragma region "Load Indices"
//		index = 0;
//		int index_count = ((m_leafWidth) * 2) * (m_leafWidth - 1) + (m_leafWidth - 2);
//		node->m_index_count = index_count;
//		node->indices = new unsigned long[index_count];
//		for (int z = 0; z < m_leafWidth - 1; z++)
//		{
//			//Even rows move left to right, odd rows right to left
//			if (z % 2 == 0)
//			{
//				//Even row
//				int x;
//				for (x = 0; x < m_leafWidth; x++)
//				{
//					node->indices[index++] = x + (z * m_leafWidth);
//					node->indices[index++] = x + (z * m_leafWidth) + m_leafWidth;
//				}
//				//Insert degenerate vertex if this isn't the last row
//				if (z != m_leafWidth - 2)
//				{
//					node->indices[index++] = --x + (z * m_leafWidth);
//				}
//			}
//			else
//			{
//				//Odd row
//				int x;
//				for (x = m_leafWidth - 1; x >= 0; x--)
//				{
//					node->indices[index++] = x + (z * m_leafWidth);
//					node->indices[index++] = x + (z * m_leafWidth) + m_leafWidth;
//				}
//				//Insert degenerate vertex if this isn't the last row
//				if (z != m_leafWidth - 2)
//				{
//					node->indices[index++] = ++x + (z * m_leafWidth);
//				}
//			}
//		}
//		#pragma endregion
//
//		#pragma region "Create Buffers"
//		if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX10)
//		{
//			HRESULT result;
//			D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//			D3D10_SUBRESOURCE_DATA vertexData, indexData;
//
//			//Setup the description of the static vertex buffer
//			vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
//			vertexBufferDesc.ByteWidth = sizeof(Vertex) * vert_count;
//			vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
//			vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
//			vertexBufferDesc.MiscFlags = 0;
//			//vertexBufferDesc.StructureByteStride = 0;
//
//			//Give the subresource a pointer to the vertex data
//			vertexData.pSysMem = node->vertices;
//			vertexData.SysMemPitch = 0;
//			vertexData.SysMemSlicePitch = 0;
//
//			//Create the vertex buffer
//			result = m_d3d->GetDevice10()->CreateBuffer(&vertexBufferDesc, &vertexData, &node->m_vertex_buffer10);
//			if (FAILED(result)) return;
//
//			//Setup the description of the static index buffer
//			indexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
//			indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
//			indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
//			indexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
//			indexBufferDesc.MiscFlags = 0;
//			//indexBufferDesc.StructureByteStride = 0;
//
//			//Give the subresource a pointer to the index data
//			indexData.pSysMem = node->indices;
//			indexData.SysMemPitch = 0;
//			indexData.SysMemSlicePitch = 0;
//
//			//Create the index buffer
//			result = m_d3d->GetDevice10()->CreateBuffer(&indexBufferDesc, &indexData, &node->m_index_buffer10);
//			if (FAILED(result)) return;
//
//			//Release the arrays new that the buffers have been created and loaded
//			delete[] node->vertices;
//			node->vertices = 0;
//
//			delete[] node->indices;
//			node->indices = 0;
//		}
//		else if (m_d3d->GetEngineOptions()->m_d3dVersion == DIRECTX11)
//		{
//			HRESULT result;
//			D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//			D3D11_SUBRESOURCE_DATA vertexData, indexData;
//
//			//Setup the description of the static vertex buffer
//			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//			vertexBufferDesc.ByteWidth = sizeof(Vertex) * vert_count;
//			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//			vertexBufferDesc.MiscFlags = 0;
//			vertexBufferDesc.StructureByteStride = 0;
//
//			//Give the subresource a pointer to the vertex data
//			vertexData.pSysMem = node->vertices;
//			vertexData.SysMemPitch = 0;
//			vertexData.SysMemSlicePitch = 0;
//
//			//Create the vertex buffer
//			result = m_d3d->GetDevice11()->CreateBuffer(&vertexBufferDesc, &vertexData, & node->m_vertex_buffer11);
//			if (FAILED(result)) return;
//
//			//Setup the description of the static index buffer
//			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//			indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
//			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//			indexBufferDesc.MiscFlags = 0;
//			indexBufferDesc.StructureByteStride = 0;
//
//			//Give the subresource a pointer to the index data
//			indexData.pSysMem = node->indices;
//			indexData.SysMemPitch = 0;
//			indexData.SysMemSlicePitch = 0;
//
//			//Create the index buffer
//			result = m_d3d->GetDevice11()->CreateBuffer(&indexBufferDesc, &indexData, & node->m_index_buffer11);
//			if (FAILED(result)) return;
//
//			//Release the arrays new that the buffers have been created and loaded
//			delete[] node->vertices;
//			node->vertices = 0;
//
//			delete[] node->indices;
//			node->indices = 0;
//		}
//		#pragma endregion
//	}
//}
////-------------------------------------------------------------------------------------
//void QuadTree::GenerateMeshTessellation(QNode* node)
//{
//	//Can Only do Tessellation in DirectX 11
//	if (m_d3d->GetEngineOptions()->m_d3dVersion != DIRECTX11)
//		return;
//
//	if (node->type == LEAF)
//	{
//		int vert_count = 4;
//		node->vertices = new Vertex[vert_count];
//		
//		int r;
//		float red = 1, green = 1, blue = 1;
//		int index = 0;
//		int height = 0;
//		int terrScale = m_terrScale;//Nice Big size is 8
//		// Center the grid in model space
//		float halfWidth = ((float)m_leafWidth - 1.0f) / 2.0f;
//		float halfLength = ((float)m_leafWidth - 1.0f) / 2.0f;
//
//		#pragma region "Load Vertices"
//		//Looks like I only need four vertices to pass to the tessellation shader
//		node->vertices[0].position = D3DXVECTOR3((((int)node->boundingCoord[0].x - halfWidth) * terrScale), height, (((int)node->boundingCoord[0].z - halfLength) * terrScale));
//		node->vertices[1].position = D3DXVECTOR3((((int)node->boundingCoord[1].x - halfWidth) * terrScale), height, (((int)node->boundingCoord[1].z - halfLength) * terrScale));
//		node->vertices[2].position = D3DXVECTOR3((((int)node->boundingCoord[2].x - halfWidth) * terrScale), height, (((int)node->boundingCoord[2].z - halfLength) * terrScale));
//		node->vertices[3].position = D3DXVECTOR3((((int)node->boundingCoord[3].x - halfWidth) * terrScale), height, (((int)node->boundingCoord[3].z - halfLength) * terrScale));
//
//		for (int i = 0; i < 4; i++)
//		{
//			switch (node->vertices[i].type)
//			{
//				case VType::COLOR:
//				{
//					VertexColor* vc = (VertexColor*)(&node->vertices[i]);
//					vc->color = D3DXVECTOR4(red, green, blue, 1.0f);
//				}
//				break;
//				case VType::TEXTURE:
//				{
//					VertexTexture* vt = (VertexTexture*)(&node->vertices[i]);
//					vt->tex.x = 1.0f;
//					vt->tex.y = 1.0f;
//				}
//				break;
//				default: break;//Nothing
//			}
//		}
//
//		//Need to get the the correct boundsY
//		node->vertices[0].boundsY = D3DXVECTOR2(0, 0);
//		node->vertices[1].boundsY = D3DXVECTOR2(0, 0);
//		node->vertices[2].boundsY = D3DXVECTOR2(0, 0);
//		node->vertices[3].boundsY = D3DXVECTOR2(0, 0);
//		#pragma endregion
//
//		#pragma region "Load Indices"
//		int index_count = 4;
//		node->m_index_count = index_count;
//		node->indices = new unsigned long[index_count];
//		node->indices[0] = 1;
//		node->indices[1] = 0;
//		node->indices[2] = 3;
//		node->indices[3] = 2;
//		#pragma endregion
//
//		#pragma region "Create Buffers"
//		HRESULT result;
//		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//		D3D11_SUBRESOURCE_DATA vertexData, indexData;
//
//		//Setup the description of the static vertex buffer
//		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//		vertexBufferDesc.ByteWidth = sizeof(Vertex) * vert_count;
//		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		vertexBufferDesc.MiscFlags = 0;
//		vertexBufferDesc.StructureByteStride = 0;
//
//		//Give the subresource a pointer to the vertex data
//		vertexData.pSysMem = node->vertices;
//		vertexData.SysMemPitch = 0;
//		vertexData.SysMemSlicePitch = 0;
//
//		//Create the vertex buffer
//		result = m_d3d->GetDevice11()->CreateBuffer(&vertexBufferDesc, &vertexData, & node->m_vertex_buffer11);
//		if (FAILED(result)) return;
//
//		//Setup the description of the static index buffer
//		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//		indexBufferDesc.ByteWidth = sizeof(unsigned long) * index_count;
//		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//		indexBufferDesc.MiscFlags = 0;
//		indexBufferDesc.StructureByteStride = 0;
//
//		//Give the subresource a pointer to the index data
//		indexData.pSysMem = node->indices;
//		indexData.SysMemPitch = 0;
//		indexData.SysMemSlicePitch = 0;
//
//		//Create the index buffer
//		result = m_d3d->GetDevice11()->CreateBuffer(&indexBufferDesc, &indexData, & node->m_index_buffer11);
//		if (FAILED(result)) return;
//
//		//Release the arrays new that the buffers have been created and loaded
//		delete[] node->vertices;
//		node->vertices = 0;
//
//		delete[] node->indices;
//		node->indices = 0;
//		#pragma endregion
//	}
//}
////-------------------------------------------------------------------------------------
//static void Max(D3DXVECTOR3 left, D3DXVECTOR3 right, D3DXVECTOR3& result)
//{
//    result.x = (left.x > right.x) ? left.x : right.x;
//    result.y = (left.y > right.y) ? left.y : right.y;
//    result.z = (left.z > right.z) ? left.z : right.z;
//}
////-------------------------------------------------------------------------------------
//void Max(float left, float right, float& result)
//{
//	result = (left > right) ? left : right;
//}
////-------------------------------------------------------------------------------------
//static void Min(D3DXVECTOR3 left, D3DXVECTOR3 right, D3DXVECTOR3& result)
//{
//    result.x = (left.x < right.x) ? left.x : right.x;
//    result.y = (left.y < right.y) ? left.y : right.y;
//    result.z = (left.z < right.z) ? left.z : right.z;
//}
////-------------------------------------------------------------------------------------
//void Min(float left, float right, float& result)
//{
//	result = (left < right) ? left : right;
//}
////-------------------------------------------------------------------------------------
//void QuadTree::GenerateMeshTessellation10(QNode* node)
//{
//	if (node->type == LEAF)
//	{
//		//Generate the vertex buffer
//		int vert_count = (m_leafWidth) * (m_leafWidth);
//		node->vertices = new Vertex[vert_count];
//		
//		float red = 1, green = 1, blue = 1;
//		int index = 0;
//		int height = 0;
//		int terrScale = m_terrScale;//Nice Big size is 8
//		// Center the grid in model space
//		float halfWidth = ((float)m_leafWidth - 1.0f) / 2.0f;
//		float halfLength = ((float)m_leafWidth - 1.0f) / 2.0f;//9999999999.0f
//		D3DXVECTOR3 vec_min(-9999999999.0f, -9999999999.0f, -9999999999.0f);
//		D3DXVECTOR3 vec_max(9999999999.0f, 9999999999.0f, 9999999999.0f);
//
//		float xMax = FLT_MAX;//9999999999.0f;
//		float xMin = FLT_MIN;//-9999999999.0f;
//		float zMax = FLT_MAX;//9999999999.0f;
//		float zMin = FLT_MIN;//-9999999999.0f;
//		float heightMax = FLT_MAX;//9999999999.0f;
//		float heightMin = FLT_MIN;//-9999999999.0f;
//
//		#pragma region "Load Vertices"
//		for (int z = (int)node->boundingCoord[0].z; z <= (int)node->boundingCoord[2].z; z++)
//		{
//			for (int x = (int)node->boundingCoord[0].x; x <= (int)node->boundingCoord[1].x; x++)
//			{
//				if (m_useHeight)
//				{
//					if (x < m_gridSize && z < m_gridSize)
//						height = m_heightmap->SampleHeight(x, z);
//				}
//
//				Max(height, heightMax, heightMax);
//				Min(height, heightMin, heightMin);
//
//				Max(((x - halfWidth) * terrScale), xMax, xMax);
//				Min(((x - halfWidth) * terrScale), xMin, xMin);
//
//				Max(((z - halfLength) * terrScale), zMax, zMax);
//				Min(((z - halfLength) * terrScale), zMin, zMin);
//
//				node->vertices[index].position = D3DXVECTOR3(((x - halfWidth) * terrScale), height, ((z - halfLength) * terrScale));
//
//				//Min(node->vertices[index].position, vec_min, vec_min);
//				//Max(node->vertices[index].position, vec_max, vec_max);
//				//D3DXVec3Minimize(&node->bounds._bmin, &node->bounds._bmin, &node->vertices[index].position);
//				//D3DXVec3Maximize(&node->bounds._bmax, &node->bounds._bmax, &node->vertices[index].position);
//
//				switch (node->vertices[index].type)
//				{
//					case VType::COLOR:
//					{
//						VertexColor* vc = (VertexColor*)(&node->vertices[index]);
//						vc->color = D3DXVECTOR4(red, green, blue, 1.0f);
//					}
//					break;
//					case VType::TEXTURE:
//					{
//						VertexTexture* vt = (VertexTexture*)(&node->vertices[index]);
//						vt->tex.x = 1.0f;
//						vt->tex.y = 1.0f;
//					}
//					break;
//					default: break;//Nothing
//				}
//
//				index++;
//			}
//		}
//		#pragma endregion
//
//		#pragma region "Create Vertex Buffer"
//		HRESULT result;
//		D3D10_BUFFER_DESC vertexBufferDesc;
//		D3D10_SUBRESOURCE_DATA vertexData;
//		//Setup the description of the static vertex buffer
//		vertexBufferDesc.Usage = D3D10_USAGE_DYNAMIC;
//		vertexBufferDesc.ByteWidth = sizeof(Vertex) * vert_count;
//		vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
//		vertexBufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
//		vertexBufferDesc.MiscFlags = 0;
//		//vertexBufferDesc.StructureByteStride = 0;
//
//		//Give the subresource a pointer to the vertex data
//		vertexData.pSysMem = node->vertices;
//		vertexData.SysMemPitch = 0;
//		vertexData.SysMemSlicePitch = 0;
//
//		//Create the vertex buffer
//		result = m_d3d->GetDevice10()->CreateBuffer(&vertexBufferDesc, &vertexData, &node->m_vertex_buffer10);
//		if (FAILED(result)) return;
//		#pragma endregion
//	}
//}
//-------------------------------------------------------------------------------------
void QuadTree::PrintNode(QNode* node)
{
	if (node == 0) return;

	BetterString str;

	str = "********************************************************\n";
	QNode* childTest = LocateChildNode(node->branches[0]);
	if (node->nodeID == 0)
		str.push("*Root*\n");
	else
	{
		if (childTest != 0)
			str.push("*Node*\n");
		else
			str.push("*Leaf*\n");
	}
	str.push("Node ID: ");
	str.push(Convert::ConvertIntToString(node->nodeID));
	str.push("\n");
	str.push("Node Bounding Coord: ");
	str.push("\n");
	PrintCoord(node, str);
	
	QNode* ChildNode1 = LocateChildNode(node->branches[0]);
	QNode* ChildNode2 = LocateChildNode(node->branches[1]);
	QNode* ChildNode3 = LocateChildNode(node->branches[2]);
	QNode* ChildNode4 = LocateChildNode(node->branches[3]);

	if (ChildNode1 != 0)
	{
		str.push("Node Children: ");
		str.push("\n");

		str.push("Child 1: ");
		str.push("\n");
		PrintCoord(ChildNode1, str);

		str.push("Child 2: ");
		str.push("\n");
		PrintCoord(ChildNode2, str);

		str.push("Child 3: ");
		str.push("\n");
		PrintCoord(ChildNode3, str);

		str.push("Child 4: ");
		str.push("\n");
		PrintCoord(ChildNode4, str);
	}

	out << str;

	//Look at the children nodes
	PrintNode(node->children[0]);
	PrintNode(node->children[1]);
	PrintNode(node->children[2]);
	PrintNode(node->children[3]);
}
//-------------------------------------------------------------------------------------
void QuadTree::PrintCoord(QNode* node, BetterString& bs)
{
	bs.push("Coord1: ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[0].x));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[0].y));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[0].z));
	bs.push("\n");
	
	bs.push("Coord2: ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[1].x));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[1].y));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[1].z));
	bs.push("\n");

	bs.push("Coord3: ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[2].x));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[2].y));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[2].z));
	bs.push("\n");

	bs.push("Coord4: ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[3].x));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[3].y));
	bs.push(", ");
	bs.push(Convert::ConvertIntToString(node->boundingCoord[3].z));
	bs.push("\n");
}
//-------------------------------------------------------------------------------------

//=====================================================================================
//QuadTree.h
//=====================================================================================
#ifndef __QUADTREE_H
#define __QUADTREE_H
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//Windows includes
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
using namespace std;
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//My Includes
//#include "ZCamera.h"
//#include "Camera3D.h"
#include "QNode.h"
#include "D3D.h"
#include "Heightmap.h"
#include "BetterString.h"
#include "TerrainShader.h"
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//Leave the QuadTree class as a template for loading the basic QuadTree
//Create a QuadTreeRender class that will have all the Direct3D rendering stuff for the QuadTree
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
class QuadTree
{
public:
	QuadTree(float heightScale = 2, HeightExt ext = HENONE, bool tessellate = true);
	QuadTree(int MapSize, float heightScale = 2, HeightExt ext = HENONE, bool tessellate = true, int terrScale = 2);
	QuadTree(int MapSize, int leafWidth, float heightScale = 2, HeightExt ext = HENONE, bool tessellate = true, int terrScale = 2);
	QuadTree(string heightmap, int MapSize = 0, float cellSpacing = 0.5f, float heightScale = 2, HeightExt ext = HENONE, bool tessellate = true, int terrScale = 2);
	QuadTree(string heightmap, int MapSize = 0, int leafWidth = 64, float cellSpacing = 0.5f, float heightScale = 2, HeightExt ext = HENONE, bool tessellate = true, int terrScale = 2);

	QuadTree(const QuadTree& qtree);
	~QuadTree();

	//Load elevation for the map
	void LoadMap(string heightmap);
	void LoadMap();
	void Print();
	//int GetNodeIndexCount();
	int GetNodeIndexCount(QNode* node);
	//Iteration stuff
	//QNode* GetNodes(int i);
	int GetSize();
	
	vector<float> GetHeightmapVec() { return heightmap_vec; }
	XMFLOAT2 GetPatchBoundsY(UINT patchID) { return mPatchBoundsY[patchID]; }
	int SampleHeight(int index) { return m_heightmap->SampleHeight(index); }
	int SampleHeight(int x, int z) { return m_heightmap->SampleHeight(x, z); }
	
	bool Tessellate() { return m_tessellate; }
	//vector<QNode*> GetNodes() { return m_nodes; }
	QNode* GetNodes() { return m_nodes; }
	int TotalLeaves() { return m_totalLeaves; }
	int LeafWidth() { return m_leafWidth; }
	int MapSize() { return m_gridSize; }
	int TerrainScale() { return m_terrScale; }
	bool UsingHeight() { return m_useHeight; }
	HeightExt GetMapExt() { return m_mapExt; }
	float GetCellSpacing() { return m_cellSpacing; }

private:
	void LoadQuadTree();
	//Initialize the basic components of the QuadTree
	void InitializeTree();
	//Create the QuadTree by calling a recursive function
	//that will create the tree
	void CreateTree();
	int numberOfNodes(int leaves, int leafWidth);
	void CreateNode(QNode*& node, D3DXVECTOR3 bounds[4], int parentID, int nodeID);
	QNode* LocateChildNode(int id);
	QNode* LocateChildNode(QNode* node, int id);
	//Need to find the first available leaf node
	QNode* LocateLeaf(QNode* node);
	void PrintNode(QNode* node);
	void PrintCoord(QNode* node, BetterString& bs);

	
	//Variables
	int m_totalLeaves;
	int m_leafWidth;//Needs to be one more then it should be
	int m_gridSize;
	int m_totalTreeID;
	int m_nodeCount;
	BoundingBox bounds;
	int m_terrScale;
	HeightExt m_mapExt;
	float m_heightScale;
	float m_cellSpacing;
	string m_heightmapName;
	VertexTextureBoundsY** vertices;
	unsigned long* indices;
	ofstream out;
	Heightmap* m_heightmap;
	bool m_useHeight;
	bool m_tessellate;

	//D3D* m_d3d;

	//ID3D11Buffer* m_vertex_buffer;
	//ID3D10Buffer* m_index_buffer[7];
	//unsigned long m_index_count[7];

	//vector<Color> m_leafColors;

	vector<float> heightmap_vec;
	vector<XMFLOAT2> mPatchBoundsY;
	//vector<QNode*> m_nodes;
	QNode* m_nodes;
};
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
#endif//__QUADTREE_H
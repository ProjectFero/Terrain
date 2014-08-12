//=====================================================================================
//QNode.h
//=====================================================================================
#ifndef __QNODE_H
#define __QNODE_H
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
#include "Vertex.h"
#include "QNodeType.h"
#include "D3D.h"
//class Vertex;
//-------------------------------------------------------------------------------------
class QNode
{
public:
	QNode* children[4];

	//ID's of the children
	int branches[4];

	//Bounding Coordinates for the node
	D3DXVECTOR3 boundingCoord[4];

	D3DXVECTOR2 boundsY;

	//BoundingBox* bounds;

	//id of the node
	int nodeID;

	//id of the parent's node
	int parentID;

	//Is this a leaf node or a regular node
	QNodeType type;

	//D3D* m_d3d;

	int m_vertex_count;
	int m_index_count;

	//6 Tess stages
	int m_index_countTess[7];
	
	int NumFaces;

	VertexTextureBoundsY* vertices;
	unsigned long* indices;

	ID3D11Buffer* m_vertex_buffer11;
	ID3D11Buffer* m_index_buffer11;

	ID3D10Buffer* m_vertex_buffer10;
	ID3D10Buffer* m_index_buffer10;

	//There are 6 Tessellation stages
	ID3D10Buffer* m_index_bufferTess10[7];

	QNode() : vertices(0), indices(0), m_vertex_buffer11(0), m_index_buffer11(0)
	{}
};
//-------------------------------------------------------------------------------------
#endif//__QNODE_H
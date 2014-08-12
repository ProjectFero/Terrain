//=====================================================================================
//Heightmap.h
//=====================================================================================
#ifndef __HEIGHTMAP_H
#define __HEIGHTMAP_H
//===================================================================================================================
#include "D3D.h"
//===================================================================================================================
struct HEIGHT_DATA
{
	float x;
	float y;
	float z;
};
//===================================================================================================================
enum HeightExt
{
	HENONE, BMP, RAW
};
//===================================================================================================================
class Heightmap
{
public:
	Heightmap(string heightmap, HeightExt ext, int width, int height, float cellSpacing, float heightScale, int leafSize);
	Heightmap(const Heightmap& o);
	~Heightmap();

	float SampleHeight(int x, int z);

	float SampleHeight(int index);

	bool LoadElevation(string heightmap);
	//This will help the heightmap not look to spikey, (DONT KNOW IF NEED)
	void Normalize();

	//This is used by the data loaded in by RAW
	float GetHeight(float x, float z);
	void Smooth();
	float Average(int x, int z);
	bool InBounds(int x, int z);

	int Width() { return m_heightmap_width; }
	int Height() { return m_heightmap_height; }

private:
	HeightExt m_ext;//What extention is the heightmap

	float m_cellSpacing;
	float m_height_scale;
	int m_heightmap_width;
	int m_heightmap_height;
	
	float** m_height_data;

	//This will load a .raw heightmap format
	vector<float> m_heightmap;

	//HEIGHT_DATA* m_height_data;
};
//===================================================================================================================
#endif//__HEIGHTMAP_H
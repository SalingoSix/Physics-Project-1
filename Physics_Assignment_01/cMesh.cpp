#include "cMesh.h"

#include <glm/glm.hpp>		// cross product (I hope)

cMesh::cMesh()
{
	this->numberOfVertices = 0;
	this->numberOfTriangles = 0;
	//
	this->maxExtent = 0.0f;
	this->scaleForUnitBBox = 1.0f;
	return;
}


cMesh::~cMesh()
{

	return;
}

void cMesh::CalculateExtents(void)
{
	// Assume 1st vertex is both max and min
	this->minXYZ.x = this->pVertices[0].x;
	this->minXYZ.y = this->pVertices[0].y;
	this->minXYZ.z = this->pVertices[0].z;
	this->maxXYZ.x = this->pVertices[0].x;
	this->maxXYZ.y = this->pVertices[0].y;
	this->maxXYZ.z = this->pVertices[0].z;

	for (int index = 0; index != this->numberOfVertices; index++)
	{
		if (this->pVertices[index].x < this->minXYZ.x)
		{
			this->minXYZ.x = this->pVertices[index].x;
		}
		if (this->pVertices[index].x > this->maxXYZ.x)
		{
			this->maxXYZ.x = this->pVertices[index].x;
		}
		// y...
		if (this->pVertices[index].y < this->minXYZ.y)
		{
			this->minXYZ.y = this->pVertices[index].y;
		}
		if (this->pVertices[index].y > this->maxXYZ.y)
		{
			this->maxXYZ.y = this->pVertices[index].y;
		}
		// z...
		if (this->pVertices[index].z < this->minXYZ.z)
		{
			this->minXYZ.z = this->pVertices[index].z;
		}
		if (this->pVertices[index].z > this->maxXYZ.z)
		{
			this->maxXYZ.z = this->pVertices[index].z;
		}

	}//for ( int index...

	this->maxExtentXYZ.x = this->maxXYZ.x - this->minXYZ.x;
	this->maxExtentXYZ.y = this->maxXYZ.y - this->minXYZ.y;
	this->maxExtentXYZ.z = this->maxXYZ.z - this->minXYZ.z;

	this->maxExtent = this->maxExtentXYZ.x;
	if (this->maxExtent < this->maxExtentXYZ.y)
	{	// Y is bigger
		this->maxExtent = this->maxExtentXYZ.y;
	}
	if (this->maxExtent < this->maxExtentXYZ.z)
	{	// Z is bigger
		this->maxExtent = this->maxExtentXYZ.z;
	}
	//
	this->scaleForUnitBBox = 1.0f / this->maxExtent;

	return;
}
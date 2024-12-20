#pragma once

#include "objfilemodel.h"

//Not needed due to objfilemodel -----------
//struct Vertex //Quad drawing
//{
//	XMFLOAT3 Pos;
//	XMFLOAT4 Color;
//	XMFLOAT2 UV;
//	XMFLOAT3 Norm;
//}; ---------------------------------------

class Mesh
{
public:

	//Vertex data and indices
	//Buffers - If change objfilemodel down the line
	//ID3D11Buffer* pVBuffer = NULL; //Vertex buffer
	//ID3D11Buffer* pIBuffer = NULL; // Index buffer

	//One model(mesh) per object
	ObjFileModel* model;

	Mesh(ObjFileModel* obj);
	void Clean();
};


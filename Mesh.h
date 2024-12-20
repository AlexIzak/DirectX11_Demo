#pragma once

#include "objfilemodel.h"

class Mesh
{
public:

	//Vertex data and indices
	//Buffers - If change objfilemodel down the line
	//ID3D11Buffer* pVBuffer = NULL; //Vertex buffer
	//ID3D11Buffer* pIBuffer = NULL; // Index buffer

	//One model(mesh) per object
	ObjFileModel* model;
	//ObjFileModel* modelSkybox;

	Mesh(ObjFileModel* obj);
	void Clean();
};


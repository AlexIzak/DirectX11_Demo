#pragma once
#include "Material.h"
#include "Mesh.h"

class GameObject;

class MeshRenderer
{

public:

	/*CBUFFER0 cbuffer;*/
	Mesh* mesh;
	Material* material;
	GameObject* gameObject;

	//MeshRenderer(ObjFileModel* obj, ID3D11ShaderResourceView* texture, ID3D11Device* device, GameObject* go);
	MeshRenderer(ObjFileModel* obj, Material* mat, GameObject* go);

	void Draw();
	//void Clean();
};


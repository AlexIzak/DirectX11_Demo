#pragma once
#include "Material.h"
#include "Mesh.h"

class GameObject;

class MeshRenderer
{

public:

	Mesh* mesh;
	Material* material;
	GameObject* gameObject;

	MeshRenderer(ObjFileModel* obj, Material* mat, GameObject* go);

	void Draw();
	
};


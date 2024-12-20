#pragma once

#include "Transform.h"
#include "MeshRenderer.h"

class GameObject
{
public:

	Transform transform;

	MeshRenderer* meshRenderer;

	//bool isReflective = true;

	string shape;
	string texture;

	GameObject();
	//GameObject(const char* meshFile, const wchar_t* matFile, ID3D11Device* device, ID3D11DeviceContext* deviceContext, bool reflective);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	void Clean();
};


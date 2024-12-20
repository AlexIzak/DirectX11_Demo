#include "GameObject.h"

GameObject::GameObject()
{
}

//GameObject::GameObject(const char* meshFile, const wchar_t* matFile, ID3D11Device* device, ID3D11DeviceContext* deviceContext, bool reflective = false)
//{
//	meshRenderer = new MeshRenderer(meshFile, matFile, device, deviceContext, this);
//	isReflective = reflective;
//}

void GameObject::SetPosition(float x, float y, float z)
{
	transform.pos = { x, y, z };
}

void GameObject::SetRotation(float x, float y, float z)
{
	transform.rot = { x, y, z };
}

void GameObject::SetScale(float x, float y, float z)
{
	transform.scl = { x, y, z };
}

void GameObject::Clean()
{
	//meshRenderer->Clean();
	delete meshRenderer;
}

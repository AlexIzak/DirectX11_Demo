#include "MeshRenderer.h"

//MeshRenderer::MeshRenderer(ObjFileModel* obj, ID3D11ShaderResourceView* texture, ID3D11Device* device, GameObject* go)
//{
//	mesh = new Mesh(obj);
//	material = new Material(texture, device);
//	gameObject = go;
//}

MeshRenderer::MeshRenderer(ObjFileModel* obj, Material* mat, GameObject* go)
{
	mesh = new Mesh(obj);
	material = mat;
	gameObject = go;
}

void MeshRenderer::Draw()
{
	mesh->model->Draw();
}

//void MeshRenderer::Clean()
//{
//	if (material)
//		material->Clean();
//	//delete mesh->model;
//}
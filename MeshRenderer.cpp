#include "MeshRenderer.h"

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
#include "Mesh.h"

Mesh::Mesh(ObjFileModel* obj)
{
	model = obj;
}

void Mesh::Clean()
{
	delete model;
}

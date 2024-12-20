#pragma once

#include <DirectXMath.h>

using namespace DirectX;

struct Transform
{
	XMFLOAT3 pos{ 0,0,1 }; // Position
	XMFLOAT3 rot{ 0,0,0 }; // Rotation
	XMFLOAT3 scl{ 1,1,1 }; // Scale

	XMMATRIX GetWorldMatrix()
	{
		//Transform matrices
		XMMATRIX translation = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX rotation = XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z);
		XMMATRIX scale = XMMatrixScaling(scl.x, scl.y, scl.z);
		XMMATRIX world = scale * rotation * translation;
		return world;
	}

	XMFLOAT3 GetForwardVector()
	{
		XMFLOAT3 forward{ (float)sin(rot.y) * (float)sin(rot.x),
						(float)cos(rot.x),
						(float)cos(rot.y) * (float)sin(rot.x) }; //Control rotation
		return forward;

	}

	XMFLOAT3 GetRightVector()
	{
		XMFLOAT3 right{ (float)cos(rot.y),
						0,
						(float)-sin(rot.y) }; //Control rotation
		return right;

	}
};

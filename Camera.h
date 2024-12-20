#pragma once

#include <DirectXMath.h>
#include "Transform.h"

using namespace DirectX;

struct Camera
{
	//float x = 0, y = 0, z = 0;
	//float pitch = XM_PIDIV2, yaw = 0;

	Transform transform;

	Camera()
	{
		transform.rot.x = XM_PIDIV2;
	}

	XMMATRIX GetViewMatrix()
	{
		//View
		XMVECTOR eyePos = XMLoadFloat3(&transform.pos);
		//XMVECTOR lookAt{ 0,0,1 }; //World forward
		XMFLOAT3 forward = transform.GetForwardVector();
		XMVECTOR lookTo = XMLoadFloat3(&forward);
		XMVECTOR camUp{ 0,1,0 }; //World up
		//view = XMMatrixLookAtLH(eyePos, lookAt, camUp);
		XMMATRIX view = XMMatrixLookToLH(eyePos, lookTo, camUp);
		return view;
	}
};

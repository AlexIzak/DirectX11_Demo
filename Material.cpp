#include "Material.h"

Material::Material(ID3D11ShaderResourceView* texture, ID3D11Device* device)
{
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBUFFER0);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(device->CreateBuffer(&cbd, NULL, &pCBuffer)))
	{
		OutputDebugString(L"Failed to create constant buffer");
		return;
	}

	/*cbd.ByteWidth = sizeof(CBufferSkybox);
	if (FAILED(device->CreateBuffer(&cbd, NULL, &pSkyboxCBuffer)))
	{
		OutputDebugString(L"Failed to create skybox constant buffer");
		return;
	}*/

	pTexture = texture;
}

void Material::Clean()
{
	if (pVS) pVS->Release();
	if (pPS) pPS->Release();
	if (pLayout) pLayout->Release();
	//if (pTexture) pTexture->Release();
	//TODO - Put this in texture class
	if (pSampler) pSampler->Release();
	if (pCBuffer) pCBuffer->Release();
}
#pragma once

#include <d3d11.h>
#include <d3d11shader.h>
#include <DirectXMath.h>
#include <WICTextureLoader.h>

using namespace DirectX;

#define MAX_POINT_LIGHTS 8

struct PointLight
{
	XMVECTOR position = { 0,0,0,1 }; // 16 bytes
	XMVECTOR colour = { 1,1,1,1 }; // 16 bytes

	float strength = 10; // 4 bytes
	BOOL enabled = false; // 4 bytes
	float padding[2]; // 8 bytes
	// Total = 48 bytes
};

struct CBUFFER0
{
	XMMATRIX WVP; //64 bytes World-View-Projection matrix
	//The 64 comes from each row being 16 bytes
	//and 4 rows in total - 4 * 16 = 64 bytes
	//Each '4' is a 4 byte float value
	// [4,4,4,4]
	// [4,4,4,4]
	// [4,4,4,4]
	// [4,4,4,4]
	//XMMATRIX is a strictly alignet type for SIMD hardware
	XMMATRIX WV; // Reflecting environment
	XMVECTOR ambientLightCol; //16 bytes
	XMVECTOR directionalLightDir; //16 bytes
	XMVECTOR directionalLightCol; //16 bytes

	PointLight pointLights[MAX_POINT_LIGHTS]; //48 * 8 = 384 bytes
};

class Material
{
public:

	//Const buffer
	ID3D11Buffer* pCBuffer = NULL;

	CBUFFER0 cbuffer;

	BOOL isReflective;

	//Shaders and shader inputs
	ID3D11VertexShader* pVS = NULL; //the vertex shader
	ID3D11PixelShader* pPS = NULL; //the pixel shader
	ID3D11InputLayout* pLayout = NULL; //the pointer to the input layout
	ID3D11ShaderReflection* vShaderReflection = NULL;

	//Textures
	ID3D11ShaderResourceView* pTexture = NULL; // Texture
	ID3D11SamplerState* pSampler = NULL; // Sampler

	Material(ID3D11ShaderResourceView* texture, ID3D11Device* device);
	void Clean();
};


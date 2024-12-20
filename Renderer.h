#pragma once

#include <iostream>
#include "ReadData.h"
//Must before #include <DirectXMath.h> - allows direct access of DirectX Math Matrices at cost of performance
//#define _XM_NO_INTRINSICS_
//#define XM_NO_ALIGNMENTS // removes some optimisations
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
//Font
#include <SpriteBatch.h>
#include <SpriteFont.h>
//Model Loading
#include "objfilemodel.h"
//Skybox
#include <DDSTextureLoader.h>

#include "InputHandling.h"
#include "GameObject.h"


using namespace DirectX;

#pragma comment (lib, "d3d11.lib")

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  600

class GameObject;

class Renderer
{
public:
	//Variables ----------------------------------------------------------------
	HINSTANCE hInst = NULL; //Handle to this instance
	HWND hWnd = NULL; //Handle to our created window
	const wchar_t* windowName = L"AGP Project - Alex"; //Wide char array

	//Variables for d3d
	IDXGISwapChain* swapChain = NULL; // the pointer to the swap chain interface
	ID3D11Device* device = NULL; // the pointer to our Direct3D device interface
	ID3D11DeviceContext* deviceContext = NULL; // the pointer to our Direct3D device context
	ID3D11RenderTargetView* backBuffer = NULL; // a view to access our back buffer

	//Shader Pipeline
	//ID3D11VertexShader* pVS = NULL; //the vertex shader
	//ID3D11PixelShader* pPS = NULL; //the pixel shader
	//ID3D11InputLayout* pLayout = NULL; //the pointer to the input layout
	//ID3D11ShaderReflection* vShaderReflection = NULL;

	//Using objfilemodel so we dont need hardcoded vertices
	//Vertex vertices[8] =
	//{
	//	//           x      y     z               r     g      b     a              u     v               
	//	//Front
	//	{XMFLOAT3{-0.5f, -0.5f, -0.5f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 1.0f}, XMFLOAT3{-0.5773f, -0.5773f, -0.5773f}}, //Front BotLeft
	//	{XMFLOAT3{-0.5f,  0.5f, -0.5f}, XMFLOAT4{0.0f, 1.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f}, XMFLOAT3{-0.5773f,  0.5773f, -0.5773f}}, //Front TopLeft
	//	{XMFLOAT3{ 0.5f,  0.5f, -0.5f}, XMFLOAT4{0.0f, 0.0f, 1.0f, 1.0f}, XMFLOAT2{1.0f, 0.0f}, XMFLOAT3{ 0.5773f,  0.5773f, -0.5773f}}, //Front TopRight
	//	{XMFLOAT3{ 0.5f, -0.5f, -0.5f}, XMFLOAT4{1.0f, 1.0f, 1.0f, 1.0f}, XMFLOAT2{1.0f, 1.0f}, XMFLOAT3{ 0.5773f, -0.5773f, -0.5773f}}, //Front BotRight

	//	//Back
	//	{XMFLOAT3{-0.5f, -0.5f,  0.5f}, XMFLOAT4{0.0f, 1.0f, 1.0f, 1.0f}, XMFLOAT2{0.0f, 1.0f}, XMFLOAT3{-0.5773f, -0.5773f,  0.5773f}}, //Back BotLeft
	//	{XMFLOAT3{-0.5f,  0.5f,  0.5f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{0.0f, 0.0f}, XMFLOAT3{-0.5773f,  0.5773f,  0.5773f}}, //Back TopLeft
	//	{XMFLOAT3{ 0.5f,  0.5f,  0.5f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{1.0f, 0.0f}, XMFLOAT3{ 0.5773f,  0.5773f,  0.5773f}}, //Back TopRight
	//	{XMFLOAT3{ 0.5f, -0.5f,  0.5f}, XMFLOAT4{1.0f, 0.0f, 0.0f, 1.0f}, XMFLOAT2{1.0f, 1.0f}, XMFLOAT3{ 0.5773f, -0.5773f,  0.5773f}}, //Back BotRight
	//};

	ID3D11Buffer* pIBuffer = NULL; // Index buffer
	ID3D11DepthStencilView* ZBuffer = NULL; //the pointer to our depth buffer

	//Font

	unique_ptr<SpriteBatch> spriteBatch;
	vector<SpriteFont*> spriteFonts;

	ID3D11BlendState* pAlphaBlendStateEnable = NULL;
	ID3D11BlendState* pAlphaBlendStateDisable = NULL;

	//For Transparency - backface culling
	ID3D11RasterizerState* pRasterizer;

	//Lighting

	XMVECTOR ambientLightColour = { 0.1, 0.1, 0.1 };
	XMVECTOR directionalLightShinesFrom = { 0.2788f, 0.7063f, 0.6506f };
	XMVECTOR directionalLightColour = { 0.85, 0.85, 0.85 };

	PointLight pointLights[MAX_POINT_LIGHTS];

	//Model Loading

	ObjFileModel* modelSkybox;

	//Skybox
	ID3D11RasterizerState* pRasterSolid = NULL;
	ID3D11RasterizerState* pRasterSkybox = NULL;
	ID3D11DepthStencilState* pDepthWriteSolid = NULL;
	ID3D11DepthStencilState* pDepthWriteSkybox = NULL;
	ID3D11Buffer* pSkyboxCBuffer = NULL;
	ID3D11ShaderResourceView* pSkyboxTexture = NULL;
	ID3D11VertexShader* pVSSkybox = NULL;
	ID3D11PixelShader* pPSSkybox = NULL;
	ID3D11InputLayout* pLayoutSkybox = NULL;

	struct CBufferSkybox
	{
		XMMATRIX WVP; //64 bytes
	};

private:
	//Mesh Renderers
	MeshRenderer* mRend;
	vector<MeshRenderer*> mRList;

public:
	
	HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow);

	//D3D
	HRESULT InitD3D(vector<GameObject*> objList);
	void CleanD3D();

	//Console Allocation
	void OpenConsole();

	//Back Buffer
	void RenderFrame(Camera& g_cam);

	//Shader Pipeline
	HRESULT InitPipeline(vector<GameObject*> objList);
	void InitGraphics(vector<GameObject*> objList);

	void CreateMaterial(ID3D11ShaderResourceView* texture, string name);

	//Skybox
	HRESULT LoadVertexShader(LPCWSTR filename, LPCSTR entrypoint, ID3D11VertexShader** vs, ID3D11InputLayout** il, Material* material);
	HRESULT LoadPixelShader(LPCWSTR filename, LPCSTR entrypoint, ID3D11PixelShader** ps);

	void DrawSkybox(Camera g_cam);

	HWND GetWindow() { return hWnd; };
};


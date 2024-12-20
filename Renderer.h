#pragma once

//#include <Windows.h>
//#include <d3d11.h>
#include <iostream>
#include "ReadData.h"
//Must before #include <DirectXMath.h> - allows direct access of DirectX Math Matrices at cost of performance
//#define _XM_NO_INTRINSICS_
//#define XM_NO_ALIGNMENTS // removes some optimisations
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
//Texturing
//#include <WICTextureLoader.h>
//Font
//#include "text2D.h"
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

//#define MAX_POINT_LIGHTS 8

//Not needed due to objfilemodel -----------
//struct Vertex //Quad drawing
//{
//	XMFLOAT3 Pos;
//	XMFLOAT4 Color;
//	XMFLOAT2 UV;
//	XMFLOAT3 Norm;
//}; ---------------------------------------

//struct PointLight
//{
//	XMVECTOR position = { 0,0,0,1 }; // 16 bytes
//	XMVECTOR colour = { 1,1,1,1 }; // 16 bytes
//
//	float strength = 10; // 4 bytes
//	BOOL enabled = false; // 4 bytes
//	float padding[2]; // 8 bytes
//	// Total = 48 bytes
//};

//struct CBUFFER0
//{
//	XMMATRIX WVP; //64 bytes World-View-Projection matrix
//	//The 64 comes from each row being 16 bytes
//	//and 4 rows in total - 4 * 16 = 64 bytes
//	//Each '4' is a 4 byte float value
//	// [4,4,4,4]
//	// [4,4,4,4]
//	// [4,4,4,4]
//	// [4,4,4,4]
//	//XMMATRIX is a strictly alignet type for SIMD hardware
//	XMMATRIX WV; // Reflecting environment
//	XMVECTOR ambientLightCol; //16 bytes
//	XMVECTOR directionalLightDir; //16 bytes
//	XMVECTOR directionalLightCol; //16 bytes
//
//	//XMVECTOR pointLightPos; //16 bytes
//	//XMVECTOR pointLightCol; //16 bytes
//	//float pointLightStrength; //4 bytes
//
//	PointLight pointLights[MAX_POINT_LIGHTS]; //48 * 8 = 384 bytes
//};

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

	//ID3D11Buffer* pVBuffer = NULL; //Vertex buffer
	//ID3D11Buffer* pCBuffer = NULL; // Constant buffer
	ID3D11Buffer* pIBuffer = NULL; // Index buffer
	ID3D11DepthStencilView* ZBuffer = NULL; //the pointer to our depth buffer

	//Texturing
	//ID3D11ShaderResourceView* pTexture = NULL; // Texture
	//ID3D11SamplerState* pSampler = NULL; // Sampler

	//Font
	//Text2D* pText;

	/*ID3D11ShaderResourceView* textTexture;
	SpriteBatch* spriteBatch;*/

	unique_ptr<SpriteBatch> spriteBatch;
	//unique_ptr<SpriteFont> spriteFont;
	vector<SpriteFont*> spriteFonts;

	ID3D11BlendState* pAlphaBlendStateEnable = NULL;
	ID3D11BlendState* pAlphaBlendStateDisable = NULL;

	//For Transparency - backface culling
	ID3D11RasterizerState* pRasterizer;

	//Lighting
	//XMVECTOR ambientLightColour = Colors::AliceBlue;
	XMVECTOR ambientLightColour = { 0.1, 0.1, 0.1 };
	XMVECTOR directionalLightShinesFrom = { 0.2788f, 0.7063f, 0.6506f };
	//XMVECTOR directionalLightColour = { 0.96f, 0.8f, 0.75f, 1.0f };
	//XMVECTOR directionalLightColour = Colors::Crimson;
	XMVECTOR directionalLightColour = { 0.85, 0.85, 0.85 };

	//XMVECTOR pointLightPosition = { 0, 1.0f, -1 };
	//XMVECTOR pointLightColour = { 0.85f, 0.85f, 1 };
	//float pointLightStrength = 15;

	PointLight pointLights[MAX_POINT_LIGHTS];

	//Model Loading
	//ObjFileModel* model;
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

	//InputHandling* input;

private:
	//Mesh Renderers
	MeshRenderer* mRend;
	vector<MeshRenderer*> mRList;

public:
	//Function Prototypes --------------------------------------------------------------------------------------------------------
	HRESULT InitWindow(HINSTANCE instanceHandle, int nCmdShow);

	//D3D
	HRESULT InitD3D(vector<GameObject*> objList);
	void CleanD3D();

	//Console Allocation
	void OpenConsole();

	//Back Buffer
	void RenderFrame(Camera& g_cam);//, vector<GameObject*> objList);//, Transform cube1);

	//Shader Pipeline
	HRESULT InitPipeline(vector<GameObject*> objList);
	void InitGraphics(vector<GameObject*> objList);

	void CreateMaterial(ID3D11ShaderResourceView* texture, string name);

	//Skybox
	HRESULT LoadVertexShader(LPCWSTR filename, LPCSTR entrypoint, ID3D11VertexShader** vs, ID3D11InputLayout** il, Material* material);
	HRESULT LoadPixelShader(LPCWSTR filename, LPCSTR entrypoint, ID3D11PixelShader** ps);

	void DrawSkybox(Camera g_cam);

	HWND GetWindow() { return hWnd; };
	//void SetWindow(HWND hwnd) { hWnd = hwnd; };
	//----------------------------------------------------------------------------------

};


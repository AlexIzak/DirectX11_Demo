#pragma once

#include <iostream>
#include <map>
//#include <d3d11.h>
//#include <d3d11shader.h>
//#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include "objfilemodel.h"
#include <SpriteFont.h>
#include "Material.h"

using namespace std;

//class Material;

class AssetManager
{
public:

	//Store textures in a map - thats it
	static map<string, ID3D11ShaderResourceView*> textureLib;

	//Store objs in a map - thats it
	static map<string, ObjFileModel*> objLib;

	//Store fonts in a map - thats it
	static map<string, SpriteFont*> fontLib;

	//Store materials in a map - thats it
	static map<string, Material*> matLib;

	//Loads necessary assests into respective maps
	static void InitialiseLibraries(ID3D11Device* device, ID3D11DeviceContext* deviceContext);

	//Getters
	//TODO - Add error handling if something invalid is passed - try get
	static ID3D11ShaderResourceView* GetTexture(string filename);
	static ObjFileModel* GetMesh(string filename);
	static SpriteFont* GetFont(string filename);
	static Material* GetMaterial(string filename);

	//Load assets
	static void LoadWICTexture(const wchar_t* filename, string key, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	static void LoadDDSTexture(const wchar_t* filename, string key, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	static void LoadMesh(const char* filename, string key, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	static void LoadFont(const wchar_t* filename, string key, ID3D11Device* device);
	static void LoadMaterial(string key, Material* mat);

	static void Unload();
};


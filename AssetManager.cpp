#include "AssetManager.h"

map<string, ID3D11ShaderResourceView*> AssetManager::textureLib;
map<string, ObjFileModel*> AssetManager::objLib;
map<string, SpriteFont*> AssetManager::fontLib;
map<string, Material*> AssetManager::matLib;

void AssetManager::InitialiseLibraries(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	//Textures
	LoadWICTexture(L"Assets/Textures/Box.bmp", "Wood", device, deviceContext);
	LoadWICTexture(L"Assets/Textures/treadplate.png", "Metal", device, deviceContext);

	//Objects
	LoadMesh("Assets/Objects/cube.obj", "Cube", device, deviceContext);
	LoadMesh("Assets/Objects/Sphere.obj", "Sphere", device, deviceContext);

	//Fonts
	LoadFont(L"Assets/Fonts/comic_sans_ms_16.spritefont", "ComicSans", device);
	LoadFont(L"Assets/Fonts/arial_16.spritefont", "Arial", device);
	LoadFont(L"Assets/Fonts/calibri_16.spritefont", "Calibri", device);
}

ID3D11ShaderResourceView* AssetManager::GetTexture(string filename)
{
	//Trying to add error checking/data validation in case of wrong filename
	try
	{
		return textureLib[filename];

		//throw(filename);
	}
	catch (const std::exception&)
	{
		//TODO - Load a default texture/font/mesh/material on exception thrown and then display error msg
		cout << "Texture not found. Provided filename invalid. Filename = " << filename;
	}
}

ObjFileModel* AssetManager::GetMesh(string filename)
{
	try
	{
		return objLib[filename];

		//throw(filename);
	}
	catch (const std::exception&)
	{
		cout << "Mesh not found. Provided filename invalid. Filename = " << filename;
	}
}

SpriteFont* AssetManager::GetFont(string filename)
{
	try
	{
		return fontLib[filename];

		//throw(filename);
	}
	catch (const std::exception&)
	{
		cout << "Font not found. Provided filename invalid. Filename = " << filename;
	}
}

Material* AssetManager::GetMaterial(string filename)
{
	try
	{
		return matLib[filename];

		throw(filename);
	}
	catch (string file)
	{
		cout << "Material not found. Provided filename invalid. Filename = " << file;
	}
}

void AssetManager::LoadWICTexture(const wchar_t* filename, string key, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* texture;
	CreateWICTextureFromFile(device, deviceContext, filename, NULL, &texture);
	textureLib.emplace(key, texture);
}

void AssetManager::LoadDDSTexture(const wchar_t* filename, string key, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ID3D11ShaderResourceView* texture;
	CreateDDSTextureFromFile(device, deviceContext, filename, NULL, &texture);
	textureLib.emplace(key, texture);
}

void AssetManager::LoadMesh(const char* filename, string key, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	ObjFileModel* model;
	model = new ObjFileModel{ (char*)filename, device, deviceContext };
	objLib.emplace(key, model);
}

void AssetManager::LoadFont(const wchar_t* filename, string key, ID3D11Device* device)
{
	SpriteFont* font;
	font = new SpriteFont(device, filename);
	fontLib.emplace(key, font);
}

void AssetManager::LoadMaterial(string key, Material* mat)
{
	matLib.emplace(key, mat);
}

void AssetManager::Unload()
{
	for (auto var : textureLib)
	{
		var.second->Release();
	}
	textureLib.clear();

	for (auto var : objLib)
	{
		delete var.second;
	}
	objLib.clear();

	for (auto var : fontLib)
	{
		delete var.second;
	}
	fontLib.clear();

	for (auto var : matLib)
	{
		var.second->Clean();
		delete var.second;
	}
	matLib.clear();
}

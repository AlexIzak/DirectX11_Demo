#include "Renderer.h"

#include "AssetManager.h"

HRESULT Renderer::InitWindow(HINSTANCE instanceHandle, int nCmdShow)
{
	hInst = instanceHandle; //Store our app handle (memory location)

	//First we need to register our window class
	//This is how windows stores properties for windows that we want to create
	WNDCLASSEX wc = {}; // " = {}" sets all values to 0. Can also be achieved with "ZeroMemory(&wc, sizeof(WNDCLASSEX))" macro
	//fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = InputHandling::WindowProc; //Our window procedure function, make sure it handles window creation or calls DefWindowProc(...)
	wc.hInstance = instanceHandle; //Give our app's handle
	wc.lpszClassName = L"WindowClass1"; //Windows will store our window class under this name
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW; //Background colour of the win32 app (not needed for D3D)
	//Register class with above struct. If it fails, if block will execute
	if (!RegisterClassEx(&wc))
	{
		return E_FAIL; //Return fail code to be picked up by Failed(...) macro
	}

	//Adjust the window dimensions so that the top window bar is not taking pixels away from our app

	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - SCREEN_WIDTH / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - SCREEN_HEIGHT / 2;

	RECT wr = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + SCREEN_WIDTH;
	wr.bottom = wr.top + SCREEN_HEIGHT;

	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	//Create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		L"WindowClass1", //Name of our window class
		windowName, //Title of the window
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU, // Window style with no resizing and maximising
		//WS_OVERLAPPEDWINDOW, // Alternative window style that allows resizing
		//CW_USEDEFAULT, 0,
		//100, //x-position of the window
		//100, //y-position of the window
		wr.left,
		wr.top,
		wr.right - wr.left, //Width of the window
		wr.bottom - wr.top, //Height of the window
		NULL, // No parent window
		NULL, //No menus
		hInst,
		//instanceHandle, //Application handle
		//NULL
		this); //Used with multiple windows

	if (hWnd == NULL) return E_FAIL;

	//Display the window on the screen
	ShowWindow(hWnd, nCmdShow);

	return S_OK;
}

HRESULT Renderer::InitD3D(vector<GameObject*> objList)
{
	//Create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd = {};
	//Fill the swap chain desription struct
	scd.BufferCount = 1; // One back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 32-bit color
	scd.BufferDesc.Width = SCREEN_WIDTH; // Set the back buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT; // Set the back buffer height
	scd.BufferDesc.RefreshRate.Numerator = 60; // 60 FPS
	scd.BufferDesc.RefreshRate.Denominator = 1; // 60/1 = 60 FPS
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Intended swapchain use
	scd.OutputWindow = hWnd; //Window to use
	scd.SampleDesc.Count = 1; // Number of samples for AA
	scd.Windowed = TRUE; // Windowed/Full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Allow full-screen switching


	HRESULT hr;
	//Create a swap chain, device and device context from the scd
	hr = D3D11CreateDeviceAndSwapChain(NULL, //Use default graphics adapter
		D3D_DRIVER_TYPE_HARDWARE, // Use hardware acceleration, can also use software or WARP renderers
		NULL, // Used for software driver types
		D3D11_CREATE_DEVICE_DEBUG, //Flags can be OR'd together. We are enabling debug for better warnings and errors
		NULL, // Direct3D feature levels. NULL will use D3D11.0 or older
		NULL, // Size of array passed to above member - NULL since we passed NULL
		D3D11_SDK_VERSION, // Always set to this
		&scd, // Pointer to our swap chain description
		&swapChain, // Pointer to our swap chain COM object
		&device, // Pointer to our device
		NULL, // Out param - will be set to chosen feature level
		&deviceContext); // Pointer to our immediate device context

	if (FAILED(hr)) return hr;

	//Get the address of the back buffer
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	//Get the back buffer from the swap chain
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);
	if (FAILED(hr)) return hr;

	//Use the back buffer address to create the render target view
	hr = device->CreateRenderTargetView(pBackBufferTexture, NULL, &backBuffer);
	if (FAILED(hr)) return hr;

	pBackBufferTexture->Release(); // This is no longer needed

	//Z-Buffer texture description
	D3D11_TEXTURE2D_DESC tex2dDesc = { 0 };
	tex2dDesc.Width = SCREEN_WIDTH;
	tex2dDesc.Height = SCREEN_HEIGHT;
	tex2dDesc.ArraySize = 1;
	tex2dDesc.MipLevels = 1;
	tex2dDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex2dDesc.SampleDesc.Count = scd.SampleDesc.Count; //Same sample count as swap chain
	tex2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex2dDesc.Usage = D3D11_USAGE_DEFAULT;

	//Z-Buffer texture
	ID3D11Texture2D* zBufferTexture;
	hr = device->CreateTexture2D(&tex2dDesc, NULL, &zBufferTexture);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Z-Buffer Texture");
		return E_FAIL;
	}

	//Create the depth buffer view
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	dsvDesc.Format = tex2dDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = device->CreateDepthStencilView(zBufferTexture, &dsvDesc, &ZBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth stencil view");
		return E_FAIL;
	}
	zBufferTexture->Release();

	//Set the back buffer as the current render target
	deviceContext->OMSetRenderTargets(1, &backBuffer, ZBuffer);

	//Define and set the viewport
	D3D11_VIEWPORT viewport = {  };
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	deviceContext->RSSetViewports(1, &viewport);

	//Font - only needed for text2d.h - I use sprite batch so commented for now -------------------------
	/*D3D11_BLEND_DESC bd1 = { 0 };
	bd1.RenderTarget[0].BlendEnable = TRUE;
	bd1.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd1.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd1.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd1.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd1.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd1.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd1.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd1.IndependentBlendEnable = FALSE;
	bd1.AlphaToCoverageEnable = FALSE;
	device->CreateBlendState(&bd1, &pAlphaBlendStateEnable);

	D3D11_BLEND_DESC bd2 = { 0 };
	bd2.RenderTarget[0].BlendEnable = TRUE;
	bd2.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd2.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd2.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd2.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd2.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd2.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd2.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bd2.IndependentBlendEnable = FALSE;
	bd2.AlphaToCoverageEnable = FALSE;
	device->CreateBlendState(&bd2, &pAlphaBlendStateDisable);------------------------------------------*/ 

	//Skybox
	//Back-face culling
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	
	hr = device->CreateRasterizerState(&rsDesc, &pRasterSolid);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create rasterizer state");
		return hr;
	}

	//Front-face culling
	rsDesc.CullMode = D3D11_CULL_FRONT;

	hr = device->CreateRasterizerState(&rsDesc, &pRasterSkybox);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create rasterizer state - skybox");
		return hr;
	}

	//Depth Writing enabled
	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	hr = device->CreateDepthStencilState(&dsDesc, &pDepthWriteSolid);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Depth Stencil state");
		return hr;
	}

	//Depth Writing disabled
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	hr = device->CreateDepthStencilState(&dsDesc, &pDepthWriteSkybox);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create Depth Stencil state - skybox");
		return hr;
	}

	InitGraphics(objList);
	InitPipeline(objList);

	return S_OK;
}

void Renderer::CleanD3D()
{
	if (backBuffer) backBuffer->Release();
	if (swapChain) swapChain->Release();
	if (device) device->Release();
	if (deviceContext) deviceContext->Release();

	if (ZBuffer) ZBuffer->Release();

	//Font

	//if (pAlphaBlendStateEnable) pAlphaBlendStateEnable->Release(); //Sprite Batch does this automatically
	//if (pAlphaBlendStateDisable) pAlphaBlendStateDisable->Release();
	if (pRasterizer) pRasterizer->Release();

	//TODO - Move skybox into the asset manager
	delete modelSkybox;

	//Skybox
	if (pRasterSolid) pRasterSolid->Release();
	if (pRasterSkybox) pRasterSkybox->Release();
	if (pDepthWriteSolid) pDepthWriteSolid->Release();
	if (pDepthWriteSkybox) pDepthWriteSkybox->Release();
	if (pSkyboxCBuffer) pSkyboxCBuffer->Release();
	if (pSkyboxTexture) pSkyboxTexture->Release();
	if (pVSSkybox) pVSSkybox->Release();
	if (pPSSkybox) pPSSkybox->Release();
	if (pLayoutSkybox) pLayoutSkybox->Release();

	AssetManager::Unload();
}

void Renderer::OpenConsole()
{
	if (AllocConsole())
	{
		//Read from console --------------------------  Method 1 --------------------
		/*wchar_t c[7];
		memset(c, 0, sizeof(c));
		DWORD n;
		ReadConsole(GetStdHandle(STD_INPUT_HANDLE), &c, 5, &n, 0);*/
		//Will have a \r and \n, "return" and "new line" if entire input fit in buffer
		//----------------------------------------------------------------------------

		//--------------------  Better Solution --------------------------------------
		//Redirecting input, output nad errors to allocated console
		FILE* fp = nullptr;
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		std::ios::sync_with_stdio(true);

		std::cout << "Hello Side Console!" << std::endl;
	}
}

void Renderer::RenderFrame(Camera& cam)
{
	//Clear back buffer with desired colour
	deviceContext->ClearRenderTargetView(backBuffer, Colors::Red);
	deviceContext->ClearDepthStencilView(ZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawSkybox(cam);

	//Simple movement - TESTING
	static float fakeTime = 0;
	fakeTime += 0.0001f;
	/*obj.transform.pos.x = sin(fakeTime);
	obj.transform.rot.x = sin(fakeTime);
	obj.transform.rot.y = sin(fakeTime);
	obj.transform.pos.z += 0.0001f;*/
	//obj.SetPosition(sin(fakeTime), 0.0f, sin(fakeTime));
	//obj.SetRotation(sin(fakeTime), sin(fakeTime), 0.0f);

	for (size_t i = 0; i < mRList.size(); i++)
	{
		auto mRend = mRList[i];

		deviceContext->VSSetShader(mRend->material->pVS, 0, 0);
		deviceContext->PSSetShader(mRend->material->pPS, 0, 0);


		//Select which vertex buffer, index buffer and primitive to use - PER MESH!
		//UINT stride = sizeof(Vertex);
		UINT stride = sizeof(ObjFileModel::MODEL_POS_COL_TEX_NORM_VERTEX);
		UINT offset = 0;
		//deviceContext->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);
		deviceContext->IASetVertexBuffers(0, 1, &mRend->mesh->model->pVertexBuffer, &stride, &offset);

		deviceContext->IASetIndexBuffer(pIBuffer, DXGI_FORMAT_R32_UINT, 0);

		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Set texture and sampler
		deviceContext->PSSetSamplers(0, 1, &mRend->material->pSampler);
		deviceContext->PSSetShaderResources(0, 1, &mRend->material->pTexture);
		deviceContext->PSSetShaderResources(1, 1, &pSkyboxTexture);

		//Matrices
		XMMATRIX world, view, projection;
		//Projection
		projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100);
		//View
		view = cam.GetViewMatrix();

		////World matrix and constant buffer values - PER OBJECT!!

		world = mRend->gameObject->transform.GetWorldMatrix();
		mRend->material->cbuffer.WVP = world * view * projection;
		mRend->material->cbuffer.WV = world * view;

		//Lighting
		//Ambient light
		mRend->material->cbuffer.ambientLightCol = ambientLightColour;
		//Directional light
		mRend->material->cbuffer.directionalLightCol = directionalLightColour;
		XMMATRIX transpose = XMMatrixTranspose(world); //Transpose rotations
		mRend->material->cbuffer.directionalLightDir = XMVector3Transform(directionalLightShinesFrom, transpose);

		//Point Light

		for (size_t i = 0; i < MAX_POINT_LIGHTS; ++i)
		{
			mRend->material->cbuffer.pointLights[i].enabled = pointLights[i].enabled;

			if (!pointLights[i].enabled)
				continue; //Skip disabled lights

			XMMATRIX inverse = XMMatrixInverse(nullptr, world);

			mRend->material->cbuffer.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
			mRend->material->cbuffer.pointLights[i].colour = pointLights[i].colour;
			mRend->material->cbuffer.pointLights[i].strength = pointLights[i].strength;
		}

		//Update constant buffer
		deviceContext->UpdateSubresource(mRend->material->pCBuffer, 0, 0, &mRend->material->cbuffer, 0, 0);
		deviceContext->VSSetConstantBuffers(0, 1, &mRend->material->pCBuffer);

		//Draw the 3D object - 36 (vertices)
		//g_devCon->DrawIndexed(36, 0, 0);

		mRend->Draw();
	}

	//Delta Time
	Time::CalculateDelta();

	//Font
	spriteBatch->Begin();
	//Timer
	//TODO - Create a wrapper class that takes a float and int to trim decimal places off the float
	wstring timer = L"Time : ";
	timer.append(to_wstring(((int)(fakeTime * 100)) / 100.f).substr(0, 4));
	//TODO - Extract this into a function to avoid repetition
	spriteFonts[0]->DrawString(spriteBatch.get(), timer.c_str(), XMFLOAT2(0, 0), Colors::White, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0, 1.0));

	wstring fps = L"FPS : ";
	fps.append(to_wstring(1 / Time::GetDeltaTime()).substr(0, 3));
	spriteFonts[1]->DrawString(spriteBatch.get(), fps.c_str(), XMFLOAT2(0, 20), Colors::Red, 0.0f, XMFLOAT2(0, 0), XMFLOAT2(1.0, 1.0));
	spriteBatch->End();

	//Flip the back and front buffers around. Display on screen
	swapChain->Present(0, 0);
}

HRESULT Renderer::InitPipeline(vector<GameObject*> objList)
{
	//Material shaders

	//Load normal shaders if not reflective material

	Material* material = AssetManager::GetMaterial("Wood_mat");
	LoadVertexShader(L"Compiled Shaders/VertexShader.cso", "main", &material->pVS, &material->pLayout, material);
	LoadPixelShader(L"Compiled Shaders/PixelShader.cso", "main", &material->pPS);

	//Load reflective shaders if reflective material

	material = nullptr;
	material = AssetManager::GetMaterial("Metal_mat");
	LoadVertexShader(L"Compiled Shaders/ReflectiveVShader.cso", "main", &material->pVS, &material->pLayout, material);
	LoadPixelShader(L"Compiled Shaders/ReflectivePShader.cso", "main", &material->pPS);


	//Skybox shaders
	LoadVertexShader(L"Compiled Shaders/SkyboxVShader.cso", "main", &pVSSkybox, &pLayoutSkybox, material);
	LoadPixelShader(L"Compiled Shaders/SkyboxPShader.cso", "main", &pPSSkybox);

	return S_OK;
}

void Renderer::InitGraphics(vector<GameObject*> objList)
{
	//Using objfilemodel so this not needed for now -------------------------------------------------------------------------------
	//Create the vertex buffer
	//D3D11_BUFFER_DESC bd = { 0 };
	//bd.Usage = D3D11_USAGE_DYNAMIC; //Dynamic allows CPU-write and GPU-read
	////bd.ByteWidth = sizeof(Vertex); //Size of buffer - sizeof vertex * num of vertices
	//bd.ByteWidth = sizeof(ObjFileModel::MODEL_POS_COL_TEX_NORM_VERTEX) ;//vertices); //Alternative case for simplicity
	//bd.BindFlags = D3D11_BIND_VERTEX_BUFFER; //Use as vertex buffer
	//bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Allow CPU to write in buffer
	//device->CreateBuffer(&bd, NULL, &pVBuffer); // Create the buffer

	//if (pVBuffer == 0) //Used to avoid intellisense error - not needed if line above is error handled properly (CreateBuffer())
	//{
	//	return;
	//}

	//Copy the vertices into the buffer
	//D3D11_MAPPED_SUBRESOURCE ms;
	//deviceContext->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); //Map the buffer

	//memcpy(ms.pData, model->vertices/*vertices*/, sizeof(model->vertices));//vertices)); // Copy the data to the buffer

	//deviceContext->Unmap(pVBuffer, NULL);
	//Using objfilemodel so this not needed -------------------------------------------------------------------------------------

	//Create constant buffer
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBufferSkybox);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(device->CreateBuffer(&cbd, NULL, &pSkyboxCBuffer)))
	{
		OutputDebugString(L"Failed to create skybox constant buffer");
		return;
	}

	//Create an index buffer
	unsigned int indices[] = { /*front*/ 0,1,2,2,3,0, /*back*/ 7,6,5,5,4,7, /*left*/ 4,5,1,1,0,4,
		/*right*/ 3,2,6,6,7,3, /*top*/ 1,5,6,6,2,1, /*bottom*/ 4,0,3,3,7,4 };

	//Fill in a buffer description
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(indices);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//Define the resource data
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = indices;

	//Create the buffer with the device
	if (FAILED(device->CreateBuffer(&bufferDesc, &initData, &pIBuffer)))
	{
		OutputDebugString(L"Failed to create index buffer");
		return;
	}
	
	//Asset Initialisation
	AssetManager::InitialiseLibraries(device, deviceContext);

	//Making a material with required texture
	CreateMaterial(AssetManager::GetTexture("Wood"), "Wood_mat");
	CreateMaterial(AssetManager::GetTexture("Metal"), "Metal_mat");

	//Mesh Renderer init
	for (auto o : objList)
	{
		ObjFileModel* obj = AssetManager::GetMesh(o->shape);
		o->meshRenderer = new MeshRenderer(obj, AssetManager::GetMaterial(o->texture), o);
		mRend = o->meshRenderer;
		mRList.push_back(mRend);
	}

	//Font
	spriteBatch = make_unique<SpriteBatch>(deviceContext);
	
	SpriteFont* timerFont = AssetManager::GetFont("Arial");
	SpriteFont* fpsFont = AssetManager::GetFont("Calibri");

	spriteFonts.push_back(timerFont);
	spriteFonts.push_back(fpsFont);

	//Rasterizer state for disabling backface culling
	//TODO Transparent object
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.FillMode = D3D11_FILL_SOLID;
	//rsDesc.FillMode = D3D11_FILL_WIREFRAME;
	device->CreateRasterizerState(&rsDesc, &pRasterizer);
	deviceContext->RSSetState(pRasterizer);

	//Models

	modelSkybox = new ObjFileModel{ (char*)"Assets/Objects/cube.obj", device, deviceContext };

	//Skybox
	//CreateDDSTextureFromFile(device, deviceContext, L"TestCubeMap.dds", NULL, &pSkyboxTexture); //Easier to test reflection
	CreateDDSTextureFromFile(device, deviceContext, L"Assets/Textures/WindowsXP.dds", NULL, &pSkyboxTexture);
}

void Renderer::CreateMaterial(ID3D11ShaderResourceView* texture, string name)
{
	Material* material = new Material(texture, device);

	//Sampler creation
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//TODO - Sampler in texture class - make the new class
	device->CreateSamplerState(&samplerDesc, &material->pSampler);

	AssetManager::LoadMaterial(name, material);
}

HRESULT Renderer::LoadVertexShader(LPCWSTR filename, LPCSTR entrypoint, ID3D11VertexShader** vs, ID3D11InputLayout** il, Material* material)
{
	// Load and compile the vertex and pixel shaders
	HRESULT result;

	auto vertexShaderBytecode = DX::ReadData(filename);

	//Encapsulate both shaders into shader objects
	result = device->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, vs);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create vertex shader");
		return result;
	}

	//Set the shader objects
	deviceContext->VSSetShader(*vs, 0, 0);

	D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&material->vShaderReflection);
	
	D3D11_SHADER_DESC desc;
	
	material->vShaderReflection->GetDesc(&desc);

	D3D11_SIGNATURE_PARAMETER_DESC* signatureParamDescriptions = new _D3D11_SIGNATURE_PARAMETER_DESC[desc.InputParameters]{ 0 };
	for (UINT i = 0; i < desc.InputParameters; i++)
	{
		material->vShaderReflection->GetInputParameterDesc(i, &signatureParamDescriptions[i]);
	}

	//Signature param desc makes as follows: float4 = 15, float3 = 7, float2 = 3, float[1] = 1. These are bitmasks
	//To put toghether input layout from shader info it's something like this
	D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[desc.InputParameters]{ 0 };
	for (size_t i = 0; i < desc.InputParameters; i++)
	{
		ied[i].SemanticName = signatureParamDescriptions[i].SemanticName;
		ied[i].SemanticIndex = signatureParamDescriptions[i].SemanticIndex;
		if (signatureParamDescriptions[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
		{
			switch (signatureParamDescriptions[i].Mask) //Not the most efficient solution
			{
			case 1:		ied[i].Format = DXGI_FORMAT_R32_FLOAT;				break; //float1
			case 3:		ied[i].Format = DXGI_FORMAT_R32G32_FLOAT;			break; //float2
			case 7:		ied[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;		break; //float3
			case 15:	ied[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;		break; //float4
			default: break; //I'm sorry
			}
		}//The above only covers ---x, --yx, -zyx, wzyx. It may be possible for a mask to be -yx- or yx-- or zyx- (6, 12, 14).
		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}

	result = device->CreateInputLayout(ied, desc.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), il);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create input layout");
		return result;
	}

	deviceContext->IASetInputLayout(*il);

	delete[] signatureParamDescriptions;
	delete[] ied;

	return S_OK;
}

HRESULT Renderer::LoadPixelShader(LPCWSTR filename, LPCSTR entrypoint, ID3D11PixelShader** ps)
{
	// Load and compile the vertex and pixel shaders
	HRESULT result;

	auto pixelShaderBytecode = DX::ReadData(filename);

	//Encapsulate both shadersinto shader objects
	result = device->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, ps);
	if (FAILED(result))
	{
		OutputDebugString(L"Failed to create pixel shader");
		return result;
	}

	//Set the shader objects

	deviceContext->PSSetShader(*ps, 0, 0);

	return S_OK;;
}

void Renderer::DrawSkybox(Camera g_cam)
{
	//Front-face culling and disable depth write
	deviceContext->OMSetDepthStencilState(pDepthWriteSkybox, 1);
	deviceContext->RSSetState(pRasterSkybox);

	//Set skybox shaders
	deviceContext->VSSetShader(pVSSkybox, 0, 0);
	deviceContext->PSSetShader(pPSSkybox, 0, 0);
	deviceContext->IASetInputLayout(pLayoutSkybox);

	//Constant buffer data
	CBufferSkybox cbuf;
	XMMATRIX translation, projection, view;

	translation = XMMatrixTranslation(g_cam.transform.pos.x, g_cam.transform.pos.y, g_cam.transform.pos.z);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100);
	view = g_cam.GetViewMatrix();
	cbuf.WVP = translation * view * projection;
	deviceContext->UpdateSubresource(pSkyboxCBuffer, 0, 0, &cbuf, 0, 0);

	//Set shader resources
	deviceContext->VSSetConstantBuffers(0, 1, &pSkyboxCBuffer);
	
	Material* material = AssetManager::GetMaterial("Wood_mat");
	deviceContext->PSSetSamplers(0, 1, &material->pSampler);
	deviceContext->PSSetShaderResources(0, 1, &pSkyboxTexture);

	modelSkybox->Draw();

	//Back-face culling and enable depth write
	deviceContext->OMSetDepthStencilState(pDepthWriteSolid, 1);
	deviceContext->RSSetState(pRasterSolid);

	//Set standard shaders
	
	deviceContext->VSSetShader(material->pVS, 0, 0);
	deviceContext->PSSetShader(material->pPS, 0, 0);
	deviceContext->IASetInputLayout(material->pLayout);
}

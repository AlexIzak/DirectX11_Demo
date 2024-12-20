#include "Renderer.h"

// Global Variables ----------------------------------------------------------------

Camera g_cam;

GameObject cube;
GameObject sphere;
GameObject plane;

Renderer renderer;
//----------------------------------------------------------------------------------------------------------------------------

//Function Prototypes --------------------------------------------------------------------------------------------------------

////Scene Init
void InitScene();

vector<GameObject*> GetListOfObjects();

//----------------------------------------------------------------------------------

//Program Entry Point
INT WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	renderer.OpenConsole();

	if (FAILED(renderer.InitWindow(hInstance, nCmdShow)))
	{
		MessageBox(NULL, L"Failed to create window", L"Critical Error!", MB_ICONERROR | MB_OK);
	}

	if (FAILED(renderer.InitD3D(GetListOfObjects())))
	{
		MessageBox(NULL, L"Unable to create swapchain and device.", L"Critical Error", MB_ICONERROR | MB_OK);
	}

	//Used to hold windows event messages
	MSG msg;

	//Mouse set up
	InputHandling::mouse.SetWindow(renderer.GetWindow());
	InputHandling::mouse.SetMode(Mouse::MODE_RELATIVE);
	//Mouse::Get().SetWindow(renderer.GetWindow());
	//Mouse::Get().SetMode(Mouse::MODE_RELATIVE);

	InitScene();
	//Enter the main loop
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			//Translate certain messages into correct format, namely key presses
			TranslateMessage(&msg);

			//Send the message of the loop if a quit message is detected
			DispatchMessage(&msg);

			//Break out of the loop if a quit message is detected
			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			//Game code here
			InputHandling::HandleInput(g_cam);
			renderer.RenderFrame(g_cam);//, GetListOfObjects());
		}
	}

	renderer.CleanD3D();

	return 0;
}

void InitScene()
{
	g_cam.transform.pos = { 0, 0.0f, 0.0f };
	//Object 1
	cube.SetPosition(0.0f, 0.0f, 1.5f);
	//go.transform.pos = { 0, 0.0f, 1.5f };
	//cube.SetRotation(0.0f, XMConvertToRadians(45), 0.0f);
	//go.transform.rot = { 0, XMConvertToRadians(45), 0 };

	sphere.SetPosition(10.0f, 0.0f, 0.0f);

	plane.SetPosition(0.0f, -10.0f, 0.0f);
	plane.SetScale(50.0f, 1.0f, 50.0f);


	//printf("%f %f %f\n", cube.transform.pos.x, cube.transform.pos.y, cube.transform.pos.z);
	//printf("%f %f %f", plane.transform.pos.x, plane.transform.pos.y, plane.transform.pos.z);

	//Object 2
	//cube2.pos = { 2.f, 0.0f, 6.0f };
	//cube2.rot = { 0, XMConvertToRadians(45), 0 };

	//Lights
	renderer.pointLights[0] = { XMVECTOR{ 1.5f, 0, -1}, Colors::Red, 20, true };
	renderer.pointLights[1] = { XMVECTOR{ -1.5f, 0, -1}, Colors::Blue, 20, true };
}

vector<GameObject*> GetListOfObjects()
{
	vector<GameObject*> gos;

	
	cube.shape = "Cube";
	cube.texture = "Wood_mat";
	
	sphere.shape = "Sphere";
	sphere.texture = "Metal_mat";
	
	plane.shape = "Cube";
	plane.texture = "Wood_mat";
	

	gos.push_back(&cube);
	gos.push_back(&plane);
	gos.push_back(&sphere);

	return gos;
}

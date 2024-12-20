#pragma once

#include "Renderer.h"

class Game
{
public:
	Camera g_cam;

	GameObject cube;
	GameObject plane;

	Renderer renderer;


private:
	////Scene Init
	void InitScene();

	vector<GameObject*> GetListOfObjects();

	void PlayGame();

	void CleanGame();
};


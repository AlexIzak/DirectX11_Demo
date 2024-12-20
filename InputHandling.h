#pragma once

#include <Windows.h>
#include <d3d11.h>

//Input
#include <Keyboard.h>
#include <Mouse.h>

#include "Camera.h"

#include "Time.h"

#ifdef MOVEMENT3D
if (moveForward) { g_cam.x += speed * sin(g_cam.yaw) * sin(g_cam.pitch); /**/ g_cam.y += speed * cos(g_cam.pitch); /**/ g_cam.z += speed * cos(g_cam.yaw) * sin(g_cam.pitch); }
if (moveBack) { g_cam.x -= speed * sin(g_cam.yaw) * sin(g_cam.pitch); /**/ g_cam.y -= speed * cos(g_cam.pitch); /**/ g_cam.z -= speed * cos(g_cam.yaw) * sin(g_cam.pitch); }

if (moveRight) { g_cam.x += speed * cos(g_cam.yaw);		g_cam.z += speed * (-sin(g_cam.yaw)); }
if (moveLeft) { g_cam.x -= speed * cos(g_cam.yaw);		g_cam.z -= speed * (-sin(g_cam.yaw)); }
#endif // MOVEMENT3D


class InputHandling
{
public:
	//Movement
	static float speed;

	//Input
	static Keyboard keyboard;
	static Keyboard::KeyboardStateTracker kbTracker;

	static Mouse mouse;
	static Mouse::ButtonStateTracker msTracker;

	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

	//Input
	static void HandleInput(Camera& cam);
};


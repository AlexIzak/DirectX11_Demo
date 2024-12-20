#include "InputHandling.h"

Mouse InputHandling::mouse;
Keyboard InputHandling::keyboard;
float InputHandling::speed = 1.0f;

LRESULT InputHandling::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//This message is sent when the user closes the window
		//Depending on your handling of your application windows, you may not need this
		//In this case, if a window is destroyed (when ESC is pressed below),
		//we also tell the application to quit entirely by posting a quit message
	case WM_DESTROY:
	{
		//Send a quit message to the app
		PostQuitMessage(0);
		return 0;
	}

	//Keyboard Inputs
	case WM_ACTIVATE:
	case WM_ACTIVATEAPP:
	case WM_INPUT:
		Keyboard::ProcessMessage(message, wParam, lParam);
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	case WM_SYSKEYDOWN:
		if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		{
			//This is where you'd implement the classic ALT+ENTER hotkey for fullscreen toggle
		}
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(message, wParam, lParam);
		break;

	case WM_MOUSEACTIVATE:
		//This will ignore mouse clicks that regain focus on the window
		//Good practice to prevent player "misinputs" when they click into the window
		return MA_ACTIVATEANDEAT;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(message, wParam, lParam);
		break;

	default:
		//Let windows handle everything else with default handling
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

Keyboard::KeyboardStateTracker InputHandling::kbTracker;
Mouse::ButtonStateTracker InputHandling::msTracker;

void InputHandling::HandleInput(Camera& cam)
{
	//Keyboard Input
	auto kbState = InputHandling::keyboard.GetState();
	kbTracker.Update(kbState);

	if (kbTracker.pressed.Escape) PostQuitMessage(0);

	XMVECTOR camPos = XMLoadFloat3(&cam.transform.pos);
	XMFLOAT3 fw = cam.transform.GetForwardVector();
	XMVECTOR forward = XMLoadFloat3(&fw);
	XMFLOAT3 r = cam.transform.GetRightVector();
	XMVECTOR right = XMLoadFloat3(&r);

	if (kbTracker.lastState.W) 
		XMStoreFloat3(&cam.transform.pos, camPos + forward * speed * Time::GetDeltaTime());
	if (kbTracker.lastState.S)
		XMStoreFloat3(&cam.transform.pos, camPos - forward * speed * Time::GetDeltaTime());
	if (kbTracker.lastState.A)
		XMStoreFloat3(&cam.transform.pos, camPos - right * speed * Time::GetDeltaTime());
	if (kbTracker.lastState.D)
		XMStoreFloat3(&cam.transform.pos, camPos + right * speed * Time::GetDeltaTime());

	//Getting key states - Example for future
	if (kbTracker.pressed.Space)
	{
		//Space pressed this frame
	}
	if (kbTracker.released.Space)
	{
		//Space released this frame
	}
	if (kbTracker.lastState.Space)
	{
		//Space currently held
	}
	//----------------------------------------

	//Mouse Input
	auto msState = Mouse::Get().GetState();
	msTracker.Update(msState);

	float sensitivity = XM_2PI * 0.025f;
	cam.transform.rot.y += (float)(msState.x) * sensitivity * Time::GetDeltaTime();
	cam.transform.rot.x += (float)(msState.y) * sensitivity * Time::GetDeltaTime();
	//printf("Yaw:%f, Pitch:%f\n", cam.transform.rot.y, cam.transform.rot.x); //Testing

	//if (msTracker.leftButton == Mouse::ButtonStateTracker::PRESSED) //Can also be HELD, RELEASED or UP
	//{ //Reset camera
	//	cam.x = 0;
	//	cam.y = 0;
	//	cam.z = 0;
	//	cam.yaw = 0;
	//	cam.pitch = XM_PIDIV2;
	//}
}

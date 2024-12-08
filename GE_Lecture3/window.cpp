#define WINDOW_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define WINDOW_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp)) //? not sure
#include "window.h"
#include <iostream>
#include<memory.h>

Window* window; //point to instance window inside WndProc

// Process window messages (mouse, keypress) to control application
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_DESTROY:
	{
		// trigged when user press x or close window, will exit application
		PostQuitMessage(0);
		exit(0);
		return 0;
	}
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		exit(0);
		return 0;
	}
	// Handle left, right and middle mouse button
	case WM_KEYDOWN:
	{
		window->keys[(unsigned int)wParam] = true; // use global pointer
		return 0;
	}
	case WM_KEYUP:
	{
		window->keys[(unsigned int)wParam] = false;
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
		window->mouseButtons[0] = true; // use -> to tell complier this variable is from window class. when mouse is pressed, make bool to true 
		return 0;
	}
	case WM_LBUTTONUP:
	{
		window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));
		window->mouseButtons[0] = false;
		return 0;
	}
	case WM_MOUSEMOVE:
	{
		window->updateMouse(WINDOW_GET_X_LPARAM(lParam), WINDOW_GET_Y_LPARAM(lParam));// update mouse position
		return 0;
		
	}
	default:
	{
		// SEND EVENT OTHERWISE
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	}
}




void Window::init(int window_width, int window_height, const std::string window_name) {
	// initialise window
	// try triangle
	WNDCLASSEX wc; // define window class: style, cursor, icon, background, winproc
	hinstance = GetModuleHandle(NULL);
	name = window_name;
	/*
	CS_HREDRAW and CS_VREDRAW: Redraw the entire window if the width or height changes.
	CS_OWNDC: Allocates a unique device context (DC) for the window.
	*/
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc; // Sets the window procedure (WndProc) function that processes messages sent to the window, such as keyboard or mouse input.
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance; //  The handle of the application instance.
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Sets the cursor to the default arrow.
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); //Sets the background color to black.
	wc.lpszMenuName = NULL; //No menu is associated with the window
	std::wstring wname = std::wstring(name.begin(), name.end());
	wc.lpszClassName = wname.c_str();// Sets the name of the window class.
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc); // register window class


	width = window_width;
	height = window_height;
	style = WS_OVERLAPPEDWINDOW | WS_VISIBLE; // a resizable window that is visible immediately

	// create actual window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, wname.c_str(), wname.c_str(), style, window_x, window_y, width, height, NULL, NULL, hinstance, this);
	memset(keys, 0, 256 * sizeof(char)); // clear array tracking any key states
	window = this; // make global pointer refer to current instance
}

// process pending messages to window
void Window::processMessages() {

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG)); // Clears the msg structure.
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		// Checks for messages and removes them from queue
		TranslateMessage(&msg); // prepare keyboard input for processing
		DispatchMessage(&msg); // sends message to Wndproc
	}
	POINT mousePos;
	GetCursorPos(&mousePos); // Get current mouse position
	ScreenToClient(hwnd, &mousePos); // Convert to client space
}


/*
Global Flow:
The program sets up a window with Window::init().
The WndProc function handles user input (keyboard and mouse) and system events.
The Window::processMessages() ensures the application processes user inputs and keeps running.
Event Handling:
Keyboard and mouse events are processed, updating the Window instance's state (e.g., keys, mouseButtons).
Closing the Application:
When the user presses close (X), WM_CLOSE or WM_DESTROY ends the application.

*/

//void Window::updateMouse() {
//	mouseDiffx = mousex - mousedx;
//	mouseDiffy = mousey - mousedy;
//	mousedx = mousex;
//	mousedy = mousey;
//}
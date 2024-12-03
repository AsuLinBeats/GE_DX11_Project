#pragma once
#include<windows.h> 
#include<string>
// only need string in header file.
class Window {
public:
	HWND hwnd; // handle, like pointer, where we store our window
	HINSTANCE hinstance; // handle to instance of window
	std::string name;
	DWORD style;

	int width;
	int height;

	int window_x;
	int window_y;

	bool keys[256];
	int mousex;
	int mousey;
	int mousedx;
	int mousedy;
	bool mouseButtons[3];

	void init(int window_width, int window_height, const std::string window_name);

	void processMessages();

	void updateMouse(int x, int y) {
		mousex = x;
		mousey = y;
	}
};
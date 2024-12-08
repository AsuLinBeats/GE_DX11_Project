#pragma once
#include<windows.h> 
#include<string>
#include"GraphicMath.h"
#include"camera.h"
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

    int mouseDiffx;
    int mouseDiffy;

    POINT currentMousePos;

    POINT lastMousePos = { 0,0 };
    bool firstMouseCapture = true;
    // previously defined in lecture
    void init(int window_width, int window_height, const std::string window_name);

    void processMessages();

    void updateMouse(int x, int y) {
        mousex = x;
        mousey = y;
    }


    void ClipCursorToWindow(HWND hwnd) {
        //  lock mouse cursor inside the window
        // get the window area
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        // convert coor to screen
        POINT topLeft = { clientRect.left, clientRect.top };
        POINT bottomRight = { clientRect.right, clientRect.bottom };
        ClientToScreen(hwnd, &topLeft);
        ClientToScreen(hwnd, &bottomRight);
        // Create a RECT restrict area of window
        RECT clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
        // Restrict the cursor to this rectangle
        ClipCursor(&clipRect);
    }

    void CenterCursor(HWND hwnd) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        // Convert client center to screen coordinates
        POINT center;
        center.x = (clientRect.right - clientRect.left) / 2;
        center.y = (clientRect.bottom - clientRect.top) / 2;
        ClientToScreen(hwnd, &center);
        SetCursorPos(center.x, center.y);
    }

   

    void capture(Vec3 &from, Vec3 &object, Vec3 & up, float &mouseSensitivity, Camera &camera) {
        // capture movement of cursor and change camera parameters with it
        GetCursorPos(&currentMousePos);
        ScreenToClient(hwnd, &currentMousePos);
        float mouseDX = 0.0f;
        float mouseDY = 0.0f;

        if (!firstMouseCapture)
        {
            mouseDX = (float)(currentMousePos.x - lastMousePos.x);
            mouseDY = (float)(currentMousePos.y - lastMousePos.y);
        }
        else
        {
            firstMouseCapture = false;
        }
        lastMousePos = currentMousePos;

        Vec3 forward = (object - from).normalise();
        Vec3 right = (up.Cross(forward).normalise());
        Vec3 cameraUp = forward.Cross(right);

        if (mouseDY != 0.0f) {
            float pitch = mouseDY * mouseSensitivity;
            //Matrix pitchRotation = Matrix::RotationAxis(right, -pitch);
            Matrix pitchRotation = Matrix::rotationX(-pitch);
            forward = pitchRotation.toVec3(forward).normalise();

            // Recalculate up and right
            right = (up.Cross(forward)).normalise();
            up = forward.Cross(right);
            object = from + forward;
        }


        if (mouseDX != 0.0f) {
            float yaw = mouseDX * mouseSensitivity;
            Matrix yawRotation = Matrix::rotationY(yaw);
            // Rotate forward vector
            forward = yawRotation.toVec3(forward).normalise();
            // Recalculate right and up vectors
            right = (up.Cross(forward)).normalise();
            up = forward.Cross(right);
            // Update object (lookAt point) so camera looks along the forward direction
            object = from + forward;
        }
        
      //  CenterCursor(hwnd);
        camera.setPostion(from);
        camera.setRotation(object);
        ClipCursorToWindow(hwnd);
    }


   
};
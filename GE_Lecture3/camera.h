#pragma once
#include "GraphicMath.h"
#include"GamesEngineeringBase.h"
class Camera {
public:
	Vec3 position;
	Vec3 rotation;

	Vec3 up;
	Matrix cp;
	Matrix pp;
	float fov;
	float nearPlane;
	float farPlane;
	// do transformation inside the camera
	Vec3 forward;
	Vec3 right;
	Vec3 upLocal; // local
	Vec3 upWorld;

	POINT currentMousePos;
	POINT lastMousePos;
	bool firstMouseCapture = true;
	void init() {
		
	}

	Camera(float _fov, float _nearPlane, float _farPlane) {
		fov = _fov;
		nearPlane = _nearPlane;
		farPlane = _farPlane;
		rotation.x = 0;
		rotation.y = 0;
		rotation.z = 0;
		upWorld = Vec3(0, 1, 0);
	}

	void setPostion(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void setPostion(Vec3& v) {
		position = v;
	}

	void setRotation(float p, float y, float r) {
		rotation.x = p;
		rotation.y = y;
		rotation.z = r;
	}

	void setRotation(Vec3& v) {
		rotation = v;
	}

	void captureInput(HWND hwnd, float mouseSensitivity) {
		GetCursorPos(&currentMousePos);
		ScreenToClient(hwnd, &currentMousePos);

		float mouseDX = 0.0f;
		float mouseDY = 0.0f;

		if (!firstMouseCapture) {
			mouseDX = (float)(currentMousePos.x - lastMousePos.x);
			mouseDY = (float)(currentMousePos.y - lastMousePos.y);
		}
		else {
			firstMouseCapture = false;
		}

		lastMousePos = currentMousePos;

		// Use the camera's processMouse method to adjust pitch and yaw
		if (mouseDX != 0.0f || mouseDY != 0.0f) {
			processMouse(mouseDX, mouseDY, mouseSensitivity);
		}

		// centre cursor
		// CenterCursor(hwnd);
		// ClipCursorToWindow(hwnd);
	}

	void updateVectors() {
		float cp = cosf(rotation.x);
		float sp = sinf(rotation.x);
		float cy = cosf(rotation.y);
		float sy = sinf(rotation.y);

		// Forward vector (assuming rotation.x about X axis, rotation.y about Y axis)
		forward = Vec3(cy * cp, sp, sy * cp);
		forward = forward.normalise();

		// Right vector
		right = forward.Cross(upWorld).normalise();

		// Up vector
		upLocal = right.Cross(forward).normalise();
	}
	void resetCamera() {
		position = Vec3(11, 5, 11);
		rotation.x = 0.f;
		rotation.y = 0.f;
		rotation.z = 0.f;
	}
	// Process input (e.g., movement) before updating matrices
	void processInput(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool reset, float speed, float dt) {
		if (reset) {
			resetCamera();
		}

		// Move camera
		if (moveForward) position += forward * speed * dt;
		if (moveBackward) position -= forward * speed * dt;
		if (moveRight) position += right * speed * dt;
		if (moveLeft) position -= right * speed * dt;
	}

	// Apply mouse input to adjust rotation.y/rotation.x
	void processMouse(float deltaX, float deltaY, float mouseSensitivity) {
		rotation.y -= deltaX * mouseSensitivity;
		rotation.x -= deltaY * mouseSensitivity; 

		// Constrain rotation.x to avoid flipping
		if (rotation.x > 1.5f) rotation.x = 1.5f;   // ~85 degrees
		if (rotation.x < -1.5f) rotation.x = -1.5f;
	}

	Matrix getViewMatrix() {
		updateVectors();
		Vec3 to = position + forward;
		return Matrix().lookAt(position, to, upLocal); // FIX THE BUG OF CAMERA BEFORE
	}

	Matrix getProjectionMatrix(float aspectRatio) {
		return Matrix().PerPro(aspectRatio, 1.0f, fov, farPlane, nearPlane);
	}

	Matrix updateCameraMat() {
		// apply new lookat matrix
		Matrix lookAtMatrix = cp.lookAt(position, rotation, up);
		return lookAtMatrix;
	}



	Matrix updateProjectionMat() {
		Matrix perspProjMatrix = pp.PerPro(1.f, 1.f, fov, farPlane, nearPlane);
		return perspProjMatrix;
	}


};
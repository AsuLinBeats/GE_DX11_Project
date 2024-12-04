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
	void init() {
		
	}

	Camera(float _fov, float _nearPlane, float _farPlane) {
		fov = _fov;
		nearPlane = _nearPlane;
		farPlane = _farPlane;
		up.y = 1;
	}
	Camera() {
		up.y = 1;
	}

	void setPostion(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void setPostion(Vec3& v) {
		position.x = v.x;
		position.y = v.y;
		position.z = v.z;
	}

	void setRotation(float p, float y, float r) {
		rotation.x = p;
		rotation.y = y;
		rotation.z = r;
	}

	void setRotation(Vec3& v) {
		rotation.x = v.x;
		rotation.y = v.y;
		rotation.z = v.z;
	}

	void moveX(float& speed) {
		// change position, i.e, position
		position.x+=speed;
	}

	void moveY(float& speed) {
		// change position, i.e, position
		position.y += speed;
	}

	void moveZ(float& speed) {
		// change position, i.e, position
		position.z += speed;
	}
	//Vec3 moveY(float& speed) {
	//	// change position, i.e, position
	//	return Vec3(position.x, position.y+=speed, position.z);
	//}

	void rotationX(float& speed) {
		// change rotation, i.e the direction for player
		//改变rotation,也就是摄像机再看哪里,根据鼠标前后和上下有俩结果,写两个,X应该是上下
		rotation.x += speed;
	}


	void rotationY(float& speed) {
		// change rotation, i.e the direction for player
		//改变rotation,也就是摄像机再看哪里,根据鼠标前后和上下有俩结果,写两个,X应该是上下
		rotation.y += speed;
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
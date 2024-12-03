#pragma once
#include "GraphicMath.h"

class camera {
	Vec3 position;
	Vec3 rotation;

	void init() {
		
	}

	void setPostion(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void setRotation(float p, float y, float r) {
		rotation.x = p;
		rotation.y = y;
		rotation.z = r;
	}

};
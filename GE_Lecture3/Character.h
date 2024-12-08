#pragma once
#include "GamesEngineeringBase.h"
#include "GraphicMath.h"
#include"Collider.h"



//class Character {
//	Vec3 position;
//	int currentBlood;
//	int maxBlood;
//	int currentAmmo;
//	int maxAmmo;
//	Vec3 wPosition;
//	Vec3 rotation;
//	float speed;
//	float sightrange;
//	void move() {
//		// 对于玩家而言,这个与相机移动完全一致
//	};
//
//	void rotation() {
//
//	};
//
//	void init(int blood, int ammo) {
//		// init properties and load meshes
//		maxBlood = blood;
//		currentBlood = maxBlood;
//
//		maxAmmo = ammo;
//		currentAmmo = maxAmmo;
//	};
//
//	void spawn() {
//
//	};
//
//	void reload() {
//		// suppose we have infinite magazines
//		currentAmmo = maxAmmo;
//	};
//
//	void update() {
//		// apply changes and draw 
//	};
//	
//	bool collider() {
//
//	}
//};
//

class Bullet {
	float damage;
	float speed;
};

class Player {
	Vec3 position;
	float blood;
	float maxBlood;
	Vec3 direction;
	float speed;
	bool isAttacking;
	bool isDead;
	float attackGap;
	float attackRange;
	AnimationInstance solider;
	AABB collider;
	void Hurt(float attack) {
		if (blood > 0) {
			blood -= attack;
		}
		else {
			blood = 0; // to avoid minus blood
			isDead = true;
		}
	}

	void Heal(float amount) {
		// natural heal:
		if (!isDead) {
			blood += amount;
			if (blood >= maxBlood) {
				blood = maxBlood; // maxblood
			}
		}
	}

	void movePlayer(Camera& cam, float dt) {

		position = cam.position;
	}

	void addCollider() {
		collider.reset();// reset first to avoid conflict

	}


};

class Enemy{
public:
	Vec3 position;
	float blood;
	float maxBlood;
	Vec3 direction;
	float speed;
	bool isAttacking;
	bool isDead;
	float attackGap;
	float attackRange;
	AnimationInstance dina;

	Enemy(float _maxblood) : maxBlood(_maxblood), blood(maxBlood), position(1, 0, 1), direction(1, 0, 0), speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {};

	void Hurt(float attack) {
		if (blood > 0) {
			blood -= attack;
		}
		else {
			blood = 0; // to avoid minus blood
			isDead = true;
		}
	}

	void Heal(float amount) {
		// natural heal:
		if (!isDead) {
			blood += amount;
			if (blood >= maxBlood) {
				blood = maxBlood; // maxblood
			}
		}
	}

	void moveNormal(float dt, float distance) {
		// patrol when not detect enemy
		if (isDead) {
			return;
		}
		position += direction * speed * dt;
		dina.update("Run", dt);
		// then followed by shading code
	}

	void deadRender() {
		if (isDead || blood <= 0) {
			// TODO use shade code to shade dead animation
		}
	}

	//void attackTarget(Vec3 targetPosition, float& targetHealth, float dt) {
	// 
	// // has some bugs.
	//	if (isDead || attackGap > 0) {
	//		attackGap -= dt; // wait
	//		return;
	//	}

	//	Vec3 toTarget = targetPosition - position;
	//	float distance = toTarget.Length();

	//	if (distance <= attackRange) {
	//		isAttacking = true;
	//		dina.update("Attack", dt);
	//		targetHealth -= 20.0f;
	//		attackGap = 2.0f; // 2 seconds cooldown
	//		isAttacking = false; // set false after attacking
	//	}
	//	else {
	//		isAttacking = false;
	//	}
	//}
};
#pragma once
#include"Collider.h"
#include "GamesEngineeringBase.h"
#include "GraphicMath.h"
#include"mesh.h"
#include"anime.h"
#include"camera.h"
#include"Support_tools.h"
class Bullet {
	// reserve bullet class for futhre rendering, but the attack detection is not rely on bullet, it only is checked in enemy class.
public:
	Vec3 pos;   // Current position of the bullet
	Vec3 dir;  // Direction the bullet is traveling
	float speed;     // Speed of the bullet
	bool isActive;   // Whether the bullet is active (e.g., in flight)
	float damage;
	Cube bull;
	Vec3 prevPos;
	// Enemy dinasour;
	Bullet(Vec3 _pos, Vec3 _dir, float _spd, float _damage) {
		pos = _pos;
		dir = _dir;
		speed = _spd;
		damage = _damage;
		dir.normalise();// ensure consistent move
	}
	Bullet() {};
	void Attack() {
		isActive = false;
	}

	void update(float dt, float& hitDist) {
		if (!isActive) return;
		prevPos = pos;
		// Move the bullet
		pos += dir * speed * dt;

		// set bullet to false so it will be detected and deleted in player's update
		if (pos.Length() > 100.0f) {
			isActive = false;
		}

	}
};


class Enemy {
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
	float damageTimer; // count collision time to avoid consistent hurt
	AnimationInstance dina;
	Collider coll;
	Enemy(float _maxblood) : maxBlood(_maxblood), blood(maxBlood), position(1, 0, 1), direction(1, 0, 0), speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {};
	Enemy() : position(0, 0, 0), maxBlood(100.0f), blood(100.0f), direction(1, 0, 0),
		speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {}

	void init(DXCore core, TextureManager& textureManager) {
		dina.initTexture("TRex.gem", core, &textureManager);
		dina.update("Idle", 0.f);
		//collider.vertices = dina.vertices;
		//collider.update();
	}

	void Hurt(float attack) {
		if (!isDead) {
			blood -= attack;
			if (blood <= 0) {
				blood = 0;
				isDead = true;
			}
		}
	}


	void moveNormal(float dt, float moveDistance, shader animationTextureShad, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
		// Check if enemy is dead
		if (blood <= 0) {
			isDead = true;
		}

		// Patrol logic (only if alive)
		if (!isDead) {
			position += direction * speed * dt;

			// Toggle direction after moving the set distance
			float traveledDistance = 0.f;
			traveledDistance += speed * dt;
			if (traveledDistance > moveDistance) {
				traveledDistance = 0.f;
				direction = Vec3(-direction.x, direction.y, -direction.z);
			}
		}
		if (isDead) {
			draw("death", animationTextureShad, dt, dxcore, resultMatrix, textureManager);
		}
		else {
			draw("Run", animationTextureShad, dt, dxcore, resultMatrix, textureManager);
		}
	}


	void draw(std::string name, shader animationTextureShadG, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager & textureManager) {
		dina.update(name, dt);
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		Matrix w1;
		//animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dina.matrices);
		w1 = Matrix::worldTrans(Vec3(0.9, 0.9, 0.9), Vec3(0, 0, 0), Vec3(-10, 0, 0));
		animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShadG.apply(&dxcore);
		dina.drawTexture(&dxcore, animationTextureShadG, &textureManager);
	}
	
};

	class Player {
	public:
		Vec3 position;
		float blood;
		float maxBlood;
		Vec3 direction;
		float speed;
		bool isAttacking;
		bool isDead;
		float attackGap;
		float attackCooldown;
		int ammo;
		int maxAmmo;
		std::vector<Bullet> bullets;
		AnimationInstance Uzi;
		Collider coll;
		Enemy enemy;
		Player(float _maxblood, float _maxAmmo, Vec3 _position, Vec3 _direction, float _speed, float _attackGap) {

			maxBlood = _maxblood;
			position = _position;
			direction = _direction;
			speed = _speed;
			attackGap = _attackGap;
			maxAmmo = _maxAmmo;

		}
		void init(DXCore core, TextureManager& textureManager) {
			Uzi.initTexture("Uzi.gem", core, &textureManager);
			ammo = maxAmmo;
			blood = maxBlood;
			isDead = false;
			isAttacking = false;
			attackCooldown = 0.f;
		}
		void Hurt(float attack) {
			if (!isDead) {
				blood -= attack;
				if (blood <= 0) {
					blood = 0;
					isDead = true;
				}
			}
		}


		void movePlayer(Camera& cam, float dt) {
			position = cam.position;
			direction = cam.rotation;

		}


		void reload(float dt, float reloadDuration) {
			// suppose the reserve ammo is infinite
			static float reloadTime = 0.f;
			reloadTime += dt;
			if (reloadTime >= reloadDuration) {
				ammo = maxAmmo;
				reloadTime = 0.f; // reset reloadTime after reloading
			}

		}

		void shoot(float dt, float reloadDuration, float flySpeed) {
			if (attackCooldown <= 0.0f && !isDead && ammo > 0) {
				float damage = 10.f;
				bullets.emplace_back(position, direction, flySpeed, damage);// Create a new bullet at the player position, direction is same with player
				coll.SetRay(position, direction); // this is collision of bullet, it will check if there is ammo

				ammo -= 1;
				attackCooldown = attackGap; // reset timer
			}
			if (ammo <= 0) {
				// reload automatically when no ammos
				reload(dt, reloadDuration);

			}
		}

		void update(float dt, float hitDistance, float reloadDuration) {
			// Update the attack cooldown
			if (attackCooldown > 0.0f) {
				attackCooldown -= dt;
				if (attackCooldown < 0.0f)
					attackCooldown = 0.0f;
			}
			coll.SetSphere2(position, 1); // set collider for player

			coll.SetSphere1(enemy.position, 1);// collider for enemy, set 15 for now

			if (coll.RaySphere()) {
				// collision btw enemy and bullet
				enemy.Hurt(20.f); // set 20 for now
				/*std::ostringstream logStream;
				logStream << "colliderrrrrrrrr raysphere " << "\n";
				DebugLog(logStream.str());*/
			}
			if (coll.SphereS()) {
				// collision with player
				Hurt(20.f);
				enemy.Hurt(20.f);
				/*std::ostringstream logStream;
				logStream << "colliderrrrrrrrr sphere " << "\n";
				DebugLog(logStream.str());*/
			}

			// Update bullets
			for (auto& b : bullets) {
				b.update(dt, hitDistance);
			}

			if (ammo <= 0) {
				// reload automatically when no ammos
				reload(dt, reloadDuration);
			}
			//
			// TODO REMEMBER UNCOMMIT WHEN FINISH
			//std::ostringstream logStream;
			//logStream << "Ammo: " << ammo << "|" << maxAmmo << "\n";
			//DebugLog(logStream.str());

			//// Remove inactive bullets
			bullets.erase(std::remove_if(bullets.begin(), bullets.end(), // 
				[](const Bullet& b) { return !b.isActive; }), // move bullet marked as dead to the end
				bullets.end());
		}

		void draw(shader& animationTextureShadG, float& dt, DXCore& dxcore, TextureManager& textureManager, bool& mouseRightPressed, Matrix& wn1) {
			animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", Uzi.matrices);
			animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &wn1);
			animationTextureShadG.apply(&dxcore);
			Uzi.drawTexture(&dxcore, animationTextureShadG, &textureManager);

		}
	};
	
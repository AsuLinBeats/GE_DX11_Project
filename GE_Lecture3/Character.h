#pragma once
#include"Collider.h"
#include "GamesEngineeringBase.h"
#include "GraphicMath.h"
#include"mesh.h"
#include"anime.h"
#include"camera.h"
#include"Support_tools.h"
class Bullet {
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
		// the check of collision for bullet will be implemented in enemy class to avoid sequence problem
		//if (checkCollision(dinasour.collider, hitDist)) {
		//	isActive = false;
		//}
	}

	bool checkCollision(const AABB& enemyBox, float& hitDist) {
		// Create a ray from prevPos to pos, so it will be a ray from player's position and moving towards player's direction while shooting
		Vec3 segmentDir = pos - prevPos;
		float segmentLength = segmentDir.Length();
		if (segmentLength == 0.0f) return false; // Bullet didn't move when being created

		segmentDir.normalise();
		Ray r(prevPos, segmentDir);

		float t;
		if (r.rayAABB(enemyBox, t)) {
			// t is distance along ray where intersection occurs.
			// Check if intersection is within the segment of prevPos to pos, which is each tiny part of ray
			if (t <= segmentLength) {
				hitDist = t;
				return true;
			}
		}
		return false;
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
	AABB collider;
	Player(float _maxblood, float _maxAmmo,Vec3 _position, Vec3 _direction, float _speed, float _attackGap) {

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

	void Heal(float amount) {
		// natural heal:
		if (!isDead) {
			blood += amount;
			if (blood >= maxBlood) {
				blood = maxBlood; 
			}
		}
	}

	//void movePlayer(Camera& cam, float dt, const std::vector<Enemy>& enemies) {
	// TODO ADJUST THIS CODE AND INSERT TO ENEMY'S COLLIDER TO MAKE PLAYER UNABLE TO MOVE.
	//	Vec3 oldPosition = position;

	//	// Move according to camera
	//	position = cam.position;
	//	direction = cam.rotation;
	//	collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);

	//	// Check collision with enemies
	//	for (const auto& enemy : enemies) {
	//		if (!enemy.isDead && collider.intersects(enemy.collider)) {
	//			// Collision detected, revert to old position
	//			position = oldPosition;
	//			collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);

	//			// Stop processing further collisions since we already reverted
	//			break;
	//		}
	//	}
	//}

	//void movePlayer(Camera& cam, float dt, Enemy enemy) {
	//	// a simple example for testing
	//	Vec3 oldPosition = position;

	//	// Move according to camera
	//	position = cam.position;
	//	direction = cam.rotation;
	//	collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);

	//	// Check collision with enemies

	//	if (!enemy.isDead && collider.intersects(enemy.collider)) {
	//		// Collision detected, revert to old position
	//		position = oldPosition;
	//		collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);


	//	}
	//
	//}

	void movePlayer(Camera& cam, float dt) {
		position = cam.position;
		direction = cam.rotation;
		// collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);
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

		// Update bullets
		for (auto& b : bullets) {
			b.update(dt,hitDistance);
		}

		if (ammo <= 0) {
			// reload automatically when no ammos
			reload(dt, reloadDuration);

		}
		// TODO REMEMBER UNCOMMIT WHEN FINISH
		//std::ostringstream logStream;
		//logStream << "Ammo: " << ammo << "|" << maxAmmo << "\n";
		//DebugLog(logStream.str());

		// Remove inactive bullets
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

	//void drawAmmo() {
	// TODO CURRENTLY DO NOT CONSIDER DRAW AMMO MODEL
	//	// ammo amount remainder
	//	// it will be beautiful if i can render it at the left bottom of the screen
	//	std::ostringstream logStream;
	//	logStream << "Ammo: " << ammo << "|" << maxAmmo << "\n";
	//	DebugLog(logStream.str());
	//}
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
	AABB collider;
	Enemy(float _maxblood) : maxBlood(_maxblood), blood(maxBlood), position(1, 0, 1), direction(1, 0, 0), speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {};
	Enemy() : position(0, 0, 0), maxBlood(100.0f), blood(100.0f), direction(1, 0, 0),
		speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {}

	void init(DXCore core, TextureManager& textureManager) {
		dina.initTexture("TRex.gem", core, &textureManager);
		//collider.vertices = dina.vertices;
		//collider.update();
	}

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
		// TODO CAN ADD SOME EFFECTS LIKE TWINKLE GREEN WHEN HEALING, just change shader can do that I think
	}

	void moveNormal(float dt, float moveDistance, shader animationTextureShad, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager,Player& player, Bullet& bullet) {
		// patrol when not detect enemy
		float traveledDistance = 0.f; // track the distance dinasour moving
		if (isDead) {
			return;
		}
		
		position += direction * speed * dt;
		traveledDistance += speed * dt;

		if (traveledDistance > moveDistance) {
			traveledDistance = 0.f;
			direction = Vec3(-direction.x, direction.y, -direction.z); // make dinasour turn over
		}
		collider.update(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), position);
		checkCollider(bullet);
		checkColliderWithPlayer(player, dt);
		if (isDead) {
			draw("death", animationTextureShad, dt, dxcore, resultMatrix, textureManager);
		}
		
		draw("Run", animationTextureShad, dt, dxcore, resultMatrix, textureManager);
	}

	void checkCollider(Bullet& bullet) {
		// collision between enemy and bullet
		// check bullet situation
		if (bullet.isActive) {
			float hitDist;
			if (bullet.checkCollision(collider, hitDist)) { // access result of collider from bullet
				Hurt(bullet.damage);
				bullet.isActive = false; // for insurance, make sure inactive is false
				std::ostringstream logStream;
				logStream << "enemy collisionnnnnnnnnnnnnnnnnnnnn with bullet!!!" << "\n";
				DebugLog(logStream.str());
			}
		}
	}

	void checkColliderWithPlayer(Player& player, float dt) {
		// collision between enemy and player
		damageTimer += dt;
		// If player's collider intersects with enemy's collider, cause damage once per second
		if (collider.intersects(player.collider)) {
			// Only cause damage if damageTimer > 1 second
			if (damageTimer >= 1.0f) {
				player.Hurt(15.0f); // Enemy causing damage to player
				damageTimer = 0.0f; // reset timer
				Hurt(20.f);
				std::ostringstream logStream;
				logStream << "enemy collisionnnnnnnnnnnnnnnnnnnnn with player" << "\n";
				DebugLog(logStream.str());
			}
		}
	}

	void draw(std::string name, shader animationTextureShadG, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
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

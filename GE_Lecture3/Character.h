#pragma once
#include"Collider.h"
#include "GamesEngineeringBase.h"
#include "GraphicMath.h"
#include"mesh.h"
#include"anime.h"
#include"camera.h"
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
		// the check of collision for bullet will be implemented in enemy class.
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


	//void render(shader& bulletShad, DXCore& dxcore, Matrix& vp) {
	//  // Acturally, the bullets don't require model, or just using a simple cube or sphere can represent it
	//	if (!isActive) return;
	//	// it looks like texture of bullet is loaded in weapons. here probably render a basic cube to represent bullet.
	//	// Render the bullet using its position
	//	Matrix worldMatrix = Matrix::worldTrans(Vec3(0.1f, 0.1f, 0.1f), Vec3(0, 0, 0), pos);
	//	bulletShad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &worldMatrix);
	//	bulletShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);
	//	bulletShad.apply(&dxcore);
	//}


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
	AnimationInstance solider;
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
		solider.initTexture("Soldier1.gem", core, &textureManager);
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

	void Dead(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
		if (isDead || blood <= 0) {
			// TODO use shade code to shade dead animation
			solider.update("death from the front", dt);
			draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);
			// TODO Actually, here can change the camera to behind the player and add black/white filter to make it more real.
			// Game over, break the game
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

	void update(float dt, float hitDistance) {
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

		// Remove inactive bullets
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), // 
			[](const Bullet& b) { return !b.isActive; }), // move bullet marked as dead to the end
			bullets.end());
	}

	void checkBulletsCollision(Player& player) {

		// If we want enemy to handle it, we can do it here:
		for (auto& b : player.bullets) {
			if (b.isActive) {
				float hitDist;
				if (b.checkCollision(collider, hitDist)) {
					Hurt(b.damage);
					b.isActive = false;
				}
			}
		}
	}

	void draw(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		solider.update("walking", dt);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", solider.matrices);
		Matrix w1;
		w1 = Matrix::worldTrans(Vec3(0.02, 0.02, 0.02), Vec3(0, 0, 0), Vec3(-4, 0, 0));
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.apply(&dxcore);
		solider.drawTexture(&dxcore, animationTextureShad, &textureManager);
	}

	//void drawAmmo() {
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
		dina.update("Run", dt);
		position += direction * speed * dt;
		traveledDistance += speed * dt;

		if (traveledDistance > moveDistance) {
			traveledDistance = 0.f;
			direction = Vec3(-direction.x, direction.y, -direction.z); // make dinasour turn over
		}
		collider.update(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), position); // update collider position
		checkCollider(bullet); // check if collider with bullet 
		checkColliderWithPlayer(player); // check with player
		// then followed by shading code
		draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);
	}

	void Dead(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
		if (isDead || blood <= 0) {
			// TODO use shade code to shade dead animation
			dina.update("Dead", dt);
			draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);
			

		}
	}

	void checkCollider(Player& player) {
		for (auto& b : player.bullets) {
			// check bullet situation
			if (b.isActive) {
				float hitDist;
				if (b.checkCollision(collider, hitDist)) { // access result of collider from bullet
					Hurt(b.damage);
					b.isActive = false; // for insurance, make sure inactive is false
				}
			}
		}
	}

	void checkCollider(Bullet& bullet) {

			// check bullet situation
			if (bullet.isActive) {
				float hitDist;
				if (bullet.checkCollision(collider, hitDist)) { // access result of collider from bullet
					Hurt(bullet.damage);
					bullet.isActive = false; // for insurance, make sure inactive is false
				}
			}
		
	}


	void checkColliderWithPlayer(Player& player) {
		// If player's collider intersects with enemy's collider, cause damage once per second
		if (collider.intersects(player.collider)) {
			// Only cause damage if damageTimer > 1 second
			if (damageTimer >= 1.0f) {
				player.Hurt(5.0f); // Enemy causing damage to player
				damageTimer = 0.0f; // reset timer
			}
		}
	}

	void draw(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
		Matrix w1;
		dina.update("Run", dt);
		//animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dina.matrices);
		w1 = Matrix::worldTrans(Vec3(0.9, 0.9, 0.9), Vec3(0, 0, 0), Vec3(-10, 0, 0));
		animationTextureShad.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
		animationTextureShad.apply(&dxcore);
		dina.drawTexture(&dxcore, animationTextureShad, &textureManager);
	}
};

// TODO SPLIT FILES
//class Bullet {
//public:
//    Vec3 pos;   // Current position of the bullet
//    Vec3 dir;   // Direction the bullet is traveling
//    float speed; // Speed of the bullet
//    bool isActive; // Whether the bullet is active (e.g., in flight)
//    float damage;
//    Cube bull;
//    Vec3 prevPos;
//
//    Bullet(const Vec3& _pos, const Vec3& _dir, float _spd, float _damage);
//    Bullet();
//    void Attack();
//    void update(float dt, float& hitDist);
//    bool checkCollision(const AABB& enemyBox, float& hitDist);
//    void render(shader& bulletShad, DXCore& dxcore, Matrix& vp);
//};
//
//class Player {
//public:
//    Vec3 position;
//    float blood;
//    float maxBlood;
//    Vec3 direction;
//    float speed;
//    bool isAttacking;
//    bool isDead;
//    float attackGap;
//    float attackCooldown;
//    int ammo;
//    int maxAmmo;
//    std::vector<Bullet> bullets;
//    AnimationInstance solider;
//    AABB collider;
//
//    Player(float _maxblood, float _maxAmmo, Vec3 _position, Vec3 _direction, float _speed, float _attackGap);
//    void init(DXCore core, TextureManager& textureManager);
//    void Hurt(float attack);
//    void Heal(float amount);
//    void movePlayer(Camera& cam, float dt);
//    void reload(float dt, float reloadDuration);
//    void Dead(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager);
//    void shoot(float dt, float reloadDuration, float flySpeed);
//    void update(float dt, float hitDistance);
//    void checkBulletsCollision(Player& player);
//    void draw(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager);
//};
//
//class Enemy {
//public:
//    Vec3 position;
//    float blood;
//    float maxBlood;
//    Vec3 direction;
//    float speed;
//    bool isAttacking;
//    bool isDead;
//    float attackGap;
//    float attackRange;
//    float damageTimer;
//    AnimationInstance dina;
//    AABB collider;
//
//    Enemy(float _maxblood);
//    Enemy();
//    void init(DXCore core, TextureManager& textureManager);
//    void Hurt(float attack);
//    void Heal(float amount);
//    void moveNormal(float dt, float moveDistance, shader animationTextureShad, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager, Player& player, Bullet& bullet);
//    void Dead(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager);
//    void checkCollider(Player& player);
//    void checkCollider(Bullet& bullet);
//    void checkColliderWithPlayer(Player& player);
//    void draw(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager);
//};
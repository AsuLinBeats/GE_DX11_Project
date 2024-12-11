//#include"player.h"
//#include"Enemy.h"
//#include"Collider.h"
//#include "GamesEngineeringBase.h"
//#include "GraphicMath.h"
//#include"mesh.h"
//#include"anime.h"
//#include"camera.h"
//#include"Support_tools.h"
//
//
//Player::Player(float _maxblood, float _maxAmmo, Vec3 _position, Vec3 _direction, float _speed, float _attackGap) {
//
//	maxBlood = _maxblood;
//	position = _position;
//	direction = _direction;
//	speed = _speed;
//	attackGap = _attackGap;
//	maxAmmo = _maxAmmo;
//
//}
//void Player::init(DXCore core, TextureManager& textureManager) {
//	Uzi.initTexture("Soldier1.gem", core, &textureManager);
//	ammo = maxAmmo;
//	blood = maxBlood;
//	isDead = false;
//	isAttacking = false;
//	attackCooldown = 0.f;
//}
//void Player::Hurt(float attack) {
//	if (!isDead) {
//		blood -= attack;
//		if (blood <= 0) {
//			blood = 0;
//			isDead = true;
//		}
//	}
//}
//
//void Player::Heal(float amount) {
//	// natural heal:
//	if (!isDead) {
//		blood += amount;
//		if (blood >= maxBlood) {
//			blood = maxBlood;
//		}
//	}
//}
//
//void Player::movePlayer(Camera& cam, float dt, const std::vector<Enemy>& enemies) {
//	Vec3 oldPosition = position;
//
//	// Move according to camera
//	position = cam.position;
//	direction = cam.rotation;
//	collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);
//
//	// Check collision with enemies
//	for (const auto& enemy : enemies) {
//		if (!enemy.isDead && collider.intersects(enemy.collider)) {
//			// Collision detected, revert to old position
//			position = oldPosition;
//			collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);
//
//			// Stop processing further collisions since we already reverted
//			break;
//		}
//	}
//}
//
//void Player::movePlayer(Camera& cam, float dt, Enemy enemy) {
//	// a simple example for testing
//	Vec3 oldPosition = position;
//
//	// Move according to camera
//	position = cam.position;
//	direction = cam.rotation;
//	collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);
//
//	// Check collision with enemies
//
//	if (!enemy.isDead && collider.intersects(enemy.collider)) {
//		// Collision detected, revert to old position
//		position = oldPosition;
//		collider.update(Vec3(1, 1, 1), Vec3(0.0f, 0.0f, 0.0f), position);
//	}
//}
//
//
//
//
//void Player::reload(float dt, float reloadDuration) {
//	// suppose the reserve ammo is infinite
//	static float reloadTime = 0.f;
//	reloadTime += dt;
//	if (reloadTime >= reloadDuration) {
//		ammo = maxAmmo;
//		reloadTime = 0.f; // reset reloadTime after reloading
//	}
//
//}
//
//void Player::Dead(shader animationTextureShadG, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager, Vec3 weaponWorldPos) {
//	if (isDead || blood <= 0) {
//		// TODO use shade code to shade dead animation
//		Uzi.update("death from the front", dt);
//		draw("Armature|08 Fire", animationTextureShadG, dt, dxcore, weaponWorldPos, textureManager, resultMatrix);
//		// TODO Actually, here can change the camera to behind the player and render solider and add black/white filter to make it more real.
//		// Game over, break the game
//	}
//}
//
//void Player::shoot(float dt, float reloadDuration, float flySpeed) {
//	if (attackCooldown <= 0.0f && !isDead && ammo > 0) {
//		float damage = 10.f;
//		bullets.emplace_back(position, direction, flySpeed, damage);// Create a new bullet at the player position, direction is same with player
//		ammo -= 1;
//		attackCooldown = attackGap; // reset timer
//	}
//	if (ammo <= 0) {
//		// reload automatically when no ammos
//		reload(dt, reloadDuration);
//	}
//}
//
//void Player::update(float dt, float hitDistance) {
//	// Update the attack cooldown
//	if (attackCooldown > 0.0f) {
//		attackCooldown -= dt;
//		if (attackCooldown < 0.0f)
//			attackCooldown = 0.0f;
//	}
//
//	// Update bullets
//	for (auto& b : bullets) {
//		b.update(dt, hitDistance);
//	}
//
//	// Remove inactive bullets
//	bullets.erase(std::remove_if(bullets.begin(), bullets.end(), // 
//		[](const Bullet& b) { return !b.isActive; }), // move bullet marked as dead to the end
//		bullets.end());
//}
//
//void Player::checkBulletsCollision(Player& player) {
//
//	// If we want enemy to handle it, we can do it here:
//	for (auto& b : player.bullets) {
//		if (b.isActive) {
//			float hitDist;
//			if (b.checkCollision(collider, hitDist)) {
//				Hurt(b.damage);
//				b.isActive = false;
//			}
//		}
//	}
//}
//
//void Player::draw(std::string name, shader animationTextureShadG, float dt, DXCore dxcore, Vec3 weaponWorldPos, TextureManager& textureManager, Matrix& resultMatrix) {
//	animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
//	Matrix w1;
//	Matrix wn1;
//	Uzi.update(name, dt);
//	//animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
//	animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", Uzi.matrices);
//	wn1 = Matrix::worldTrans(Vec3(0.1, 0.1, 0.1), Vec3(M_PI / 2, 0, M_PI / 2), weaponWorldPos);
//	animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &wn1);
//	animationTextureShadG.apply(&dxcore);
//	Uzi.drawTexture(&dxcore, animationTextureShadG, &textureManager);
//
//}
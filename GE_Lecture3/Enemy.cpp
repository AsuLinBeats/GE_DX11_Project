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
//Enemy::Enemy(float _maxblood) : maxBlood(_maxblood), blood(maxBlood), position(1, 0, 1), direction(1, 0, 0), speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {};
//Enemy::Enemy() : position(0, 0, 0), maxBlood(100.0f), blood(100.0f), direction(1, 0, 0),
//	speed(3.f), isAttacking(false), isDead(false), attackGap(1.f), attackRange(1.5f) {}
//
//void Enemy::init(DXCore core, TextureManager& textureManager) {
//	dina.initTexture("TRex.gem", core, &textureManager);
//	//collider.vertices = dina.vertices;
//	//collider.update();
//}
//
//void Enemy::Hurt(float attack) {
//	if (blood > 0) {
//		blood -= attack;
//	}
//	else {
//		blood = 0; // to avoid minus blood
//		isDead = true;
//	}
//}
//
//void Enemy::Heal(float amount) {
//	// natural heal:
//	if (!isDead) {
//		blood += amount;
//		if (blood >= maxBlood) {
//			blood = maxBlood; // maxblood
//		}
//	}
//	// TODO CAN ADD SOME EFFECTS LIKE TWINKLE GREEN WHEN HEALING, just change shader can do that I think
//}
//
//void Enemy::moveNormal(float dt, float moveDistance, shader animationTextureShad, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager, Player& player, Bullet& bullet) {
//	// patrol when not detect enemy
//	float traveledDistance = 0.f; // track the distance dinasour moving
//	if (isDead) {
//		return;
//	}
//	dina.update("Run", dt);
//	position += direction * speed * dt;
//	traveledDistance += speed * dt;
//
//	if (traveledDistance > moveDistance) {
//		traveledDistance = 0.f;
//		direction = Vec3(-direction.x, direction.y, -direction.z); // make dinasour turn over
//	}
//	collider.update(Vec3(0.9f, 0.9f, 0.9f), Vec3(0, 0, 0), position);
//	checkBulletsCollision(player);
//	checkColliderWithPlayer(player, dt);
//	draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);
//}
//
//void Enemy::Dead(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
//	if (isDead || blood <= 0) {
//		// TODO use shade code to shade dead animation
//		dina.update("Dead", dt);
//		draw(animationTextureShad, dt, dxcore, resultMatrix, textureManager);
//
//
//	}
//}
//
////void checkCollider(Player& player) {
////	for (auto& b : player.bullets) {
////		// check bullet situation
////		if (b.isActive) {
////			float hitDist;
////			if (b.checkCollision(collider, hitDist)) { // access result of collider from bullet
////				Hurt(b.damage);
////				b.isActive = false; // for insurance, make sure inactive is false
//
////			}
////		}
////	}
////}
//void Enemy::checkBulletsCollision(Player& player) {
//	for (auto& b : player.bullets) {
//		if (b.isActive) {
//			float hitDist;
//			if (b.checkCollision(collider, hitDist)) {
//				Hurt(b.damage);
//				b.isActive = false;
//				DebugLog("Enemy collided with a bullet!");
//			}
//		}
//	}
//}
//
//
//void Enemy::checkColliderWithPlayer(Player& player, float dt) {
//	damageTimer += dt;
//	// If player's collider intersects with enemy's collider, cause damage once per second
//	if (collider.intersects(player.collider)) {
//		// Only cause damage if damageTimer > 1 second
//		if (damageTimer >= 1.0f) {
//			player.Hurt(5.0f); // Enemy causing damage to player
//			damageTimer = 0.0f; // reset timer
//
//			std::ostringstream logStream;
//			logStream << "enemy collisionnnnnnnnnnnnnnnnnnnnn with player" << "\n";
//			DebugLog(logStream.str());
//		}
//	}
//}
//
//void Enemy::draw(shader animationTextureShadG, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager) {
//	animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "VP", &resultMatrix);
//	Matrix w1;
//	dina.update("Run", dt);
//	//animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
//	animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "bones", dina.matrices);
//	w1 = Matrix::worldTrans(Vec3(0.9, 0.9, 0.9), Vec3(0, 0, 0), Vec3(-10, 0, 0));
//	animationTextureShadG.updateConstantVS("Animated", "animatedMeshBuffer", "W", &w1);
//	animationTextureShadG.apply(&dxcore);
//	dina.drawTexture(&dxcore, animationTextureShadG, &textureManager);
//
//}

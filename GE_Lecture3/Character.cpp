#include"Character.h"
//
//Bullet::Bullet(const Vec3& _pos, const Vec3& _dir, float _spd, float _damage)
//    : pos(_pos), dir(_dir.normalise()), speed(_spd), damage(_damage), isActive(true), prevPos(_pos) {}
//
//Bullet::Bullet() : pos(0, 0, 0), dir(0, 0, 0), speed(0), damage(0), isActive(false), prevPos(0, 0, 0) {}
//
//void Bullet::Attack() {
//    isActive = false;
//}
//
//void Bullet::update(float dt, float& hitDist) {
//    if (!isActive) return;
//
//    prevPos = pos;
//    pos += dir * speed * dt;
//
//    if (pos.Length() > 100.0f) {
//        isActive = false;
//    }
//}
//
//bool Bullet::checkCollision(const AABB& enemyBox, float& hitDist) {
//    Vec3 segmentDir = pos - prevPos;
//    float segmentLength = segmentDir.Length();
//    if (segmentLength == 0.0f) return false;
//
//    segmentDir = segmentDir.normalise();
//    Ray r(prevPos, segmentDir);
//
//    float t;
//    if (r.rayAABB(enemyBox, t) && t <= segmentLength) {
//        hitDist = t;
//        return true;
//    }
//    return false;
//}
//
//void Bullet::render(shader& bulletShad, DXCore& dxcore, Matrix& vp) {
//    if (!isActive) return;
//
//    Matrix worldMatrix = Matrix::worldTrans(Vec3(0.1f, 0.1f, 0.1f), Vec3(0, 0, 0), pos);
//    bulletShad.updateConstantVS("StaticModel", "staticMeshBuffer", "W", &worldMatrix);
//    bulletShad.updateConstantVS("StaticModel", "staticMeshBuffer", "VP", &vp);
//    bulletShad.apply(&dxcore);
//}
//
//// Player class implementations
//Player::Player(float _maxblood, float _maxAmmo, Vec3 _position, Vec3 _direction, float _speed, float _attackGap)
//    : position(_position), direction(_direction), speed(_speed), maxBlood(_maxblood), blood(_maxblood),
//    attackGap(_attackGap), maxAmmo(_maxAmmo), ammo(_maxAmmo), isDead(false), isAttacking(false), attackCooldown(0.f) {}
//
//void Player::init(DXCore core, TextureManager& textureManager) {
//    solider.initTexture("Soldier1.gem", core, &textureManager);
//    ammo = maxAmmo;
//    blood = maxBlood;
//    isDead = false;
//    isAttacking = false;
//    attackCooldown = 0.f;
//}
//
//void Player::Hurt(float attack) {
//    if (!isDead) {
//        blood -= attack;
//        if (blood <= 0) {
//            blood = 0;
//            isDead = true;
//        }
//    }
//}
//
//void Player::Heal(float amount) {
//    // natural heal:
//		if (!isDead) {
//			blood += amount;
//			if (blood >= maxBlood) {
//				blood = maxBlood; 
//			}
//		} 
//}

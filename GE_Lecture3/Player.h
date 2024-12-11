//#pragma once
//#include "GraphicMath.h"
//#include "Collider.h"
//#include "camera.h"
//#include "anime.h"
//#include "Character.h"
//#include <vector>
//
//// Forward declaration of Enemy to avoid circular include
//class Enemy;
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
//    AnimationInstance Uzi;
//    AABB collider;
//
//    Player(float _maxblood, float _maxAmmo, Vec3 _position, Vec3 _direction, float _speed, float _attackGap);
//
//    void init(DXCore core, TextureManager& textureManager);
//    void Hurt(float attack);
//    void Heal(float amount);
//
//    void movePlayer(Camera& cam, float dt, const std::vector<Enemy>& enemies);
//
//    void movePlayer(Camera& cam, float dt, Enemy enemy);
//
//    void reload(float dt, float reloadDuration);
//    void Dead(shader animationTextureShadG, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager, Vec3 weaponWorldPos);
//    void shoot(float dt, float reloadDuration, float flySpeed);
//    void update(float dt, float hitDistance);
//    void checkBulletsCollision(Player& player);
//    void draw(std::string name, shader animationTextureShadG, float dt, DXCore dxcore, Vec3 weaponWorldPos, TextureManager& textureManager, Matrix& resultMatrix);
//};




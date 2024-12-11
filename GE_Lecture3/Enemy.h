//#pragma once
//#include "GraphicMath.h"
//#include "Collider.h"
//#include "anime.h"
//#include "Character.h"
//#include <vector>
//
//
//class Player;
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
//
//    void init(DXCore core, TextureManager& textureManager);
//    void Hurt(float attack);
//    void Heal(float amount);
//
//    void moveNormal(float dt, float moveDistance, shader animationTextureShad, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager, Player& player, Bullet& bullet);
//    void Dead(shader animationTextureShad, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager);
//    void checkBulletsCollision(Player& player);
//    void checkColliderWithPlayer(Player& player, float dt);
//    void draw(shader animationTextureShadG, float dt, DXCore dxcore, Matrix resultMatrix, TextureManager& textureManager);
//};



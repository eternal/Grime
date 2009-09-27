#pragma once
#include "entity.h"
#include "Enemy.h"

class Projectile :
    public entity
{
public:
    Projectile(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IPhysxManager* manager, SPhysxAndNodePair* pair,  core::array<Projectile*>* objectArray, core::array<video::ITexture*> explosionTextures, core::array<Enemy*>* enemyArray);
    ~Projectile(void);
    
    scene::ISceneManager* smgr;
    irrklang::ISoundEngine* soundEngine;
    IPhysxManager* physxManager;
    SPhysxAndNodePair* pair;
    core::array<Projectile*>* projectileObjects;
    core::array<video::ITexture*> explosionTextures;
    core::array<Enemy*>* enemyArray;
    
    f32 radius;
    s32 power;
    
    void Update(s32 time);
    void DamageTargets(vector3df pos, f32 radius, s32 power);
    
    bool active;
};

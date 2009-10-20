#pragma once
#include "entity.h"
#include "Enemy.h"
#include "Block.h"

class Projectile :
    public Entity
{
public:
    Projectile(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IPhysxManager* manager, SPhysxAndNodePair* pair,  core::array<Projectile*>* objectArray, core::array<video::ITexture*> explosionTextures, core::array<Enemy*>* enemyArray, core::array<SPhysxAndNodePair*>* clutterArray, core::array<Block*>* blockArray);
    ~Projectile(void);
    
    scene::ISceneManager* smgr;
    irrklang::ISoundEngine* soundEngine;
    IPhysxManager* physxManager;
    SPhysxAndNodePair* pair;
    core::array<Projectile*>* projectileObjects;
    core::array<video::ITexture*> explosionTextures;
    core::array<Enemy*>* enemyArray;
    core::array<SPhysxAndNodePair*>* clutterArray;
    core::array<Block*>* blockArray;
    
    f32 radius;
    s32 power;
    s32 timeElapsed;
    
    ISound* projectileFlight;
    
    void Update(s32 time);
    void DamageTargets(vector3df pos, f32 radius, s32 power);
    void Detonate();
    
    bool active;
};

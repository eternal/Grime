//=============================================================================
//  Main game class - TO BE IMPLEMENTED
//  Author: Douglas Cook
//=============================================================================


#pragma once
#include <irrlicht.h>
#include <irrKlang.h>
#include <IrrPhysx.h>
#include <cstdlib>
#include "EffectHandler.h"
#include "Player.h"
#include "Enemy.h"
#include "Block.h"
#include "Projectile.h"
#include "SpawnManager.h"
#include "LODSceneNode.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class Game
{
public:
    Game(IrrlichtDevice* device, ISoundEngine* soundEngine, IPhysxManager* physxManager, EffectHandler* effect);
    ~Game(void);
    
    ISceneManager* smgr;
    ISoundEngine* soundEngine;
    IPhysxManager* physxManager;
    EffectHandler* effect;
    gui::IGUIEnvironment* guienv;
    Player* player;
    core::array<Enemy*> enemyObjects;
    core::array<Block*> blockObjects;
    core::array<Projectile*> projectileObjects;
    SpawnManager* spawnManager;
    SPhysxAndCameraPair* cameraPair;
    ISceneNode* roomnode;
    IMesh* room;
    
    s32 cleanupTimer;
    
    core::array<video::ITexture*> explosionTextures;
    core::array<video::ITexture*> impactTextures;
    
    
    vector3df startPosition;
    bool blockFinalToggle;
    
    void CreateExplosion(vector3df position);
    void CreateImpactEffect(vector3df position, vector3df normal);
    void CreateMuzzleFlash();
    void CreateCamera();
    void LoadLevel();
    
    void Update(s32 time);
    
    s32 GetRandom(s32 upper);
    void RestartLevel();
    void WeaponFire();
    SPhysxAndNodePair* CreateSphere(const core::vector3df& pos, f32 radius, f32 density, core::vector3df* initialVelocity);
    void ConvertBlocks();
    
    void CleanupArrays();
    core::array<Enemy*> RebuildEnemies();
};


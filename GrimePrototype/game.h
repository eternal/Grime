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
    
    IrrlichtDevice* device;
    ISceneManager* smgr;
    ISoundEngine* soundEngine;
    IPhysxManager* physxManager;
    EffectHandler* effect;
    gui::IGUIEnvironment* guienv;
    Player* player;
    core::array<Enemy*> enemyObjects;
    core::array<Block*> blockObjects;
    core::array<Projectile*> projectileObjects;
    core::array<SPhysxAndNodePair*> clutterObjects;
    
    SpawnManager* spawnManager;
    SPhysxAndCameraPair* cameraPair;
    ISceneNode* roomnode;
    IMesh* room;
    
    s32 cleanupTimer;
    s32 updateTimer;
    s32 endTimer;
    s32 endPhase;
    
    bool restart;
    
    core::array<video::ITexture*> explosionTextures;
    core::array<video::ITexture*> impactTextures;
    
    bool gameOver;
    bool newGameSelected;
    
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
    void WeaponFire(s32 weapon);
    SPhysxAndNodePair* CreateSphere(const core::vector3df& pos, f32 radius, f32 density, core::vector3df* initialVelocity);
    void ConvertBlocks();
    
    void CleanupArrays();
    core::array<Enemy*> RebuildEnemies();
    core::array<Projectile*> RebuildProjectiles();
    void WeaponCloseRaycast(core::line3df line);
    void ClearEnemies();
    void AddClutter(IMesh* mesh, vector3df position, vector3df scale);
    void SpawnClutter();
};


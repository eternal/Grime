#pragma once
#include "Enemy.h"
#include "Cockroach.h"
#include "Beetle.h"
#include "Rat.h"
#include "Spider.h"
#include "Player.h"
#include "Block.h"

enum ENEMY_TYPES{
    ENEMY_COCKROACH,
    ENEMY_SPIDER,
    ENEMY_BEETLE,
    ENEMY_RAT,
    NUMBER_OF_ENEMIES
};

class SpawnManager
{
public:
    SpawnManager(ISceneManager* smgr, ISoundEngine* soundEngine, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects, core::array<Block*>* blockArray, Player* player);
    ~SpawnManager(void);
    
    ISceneManager* smgr;
    IPhysxManager* physxManager;
    core::array<Enemy*>* enemyObjects;
    core::array<Block*>* blockArray;
    ISoundEngine* soundEngine;
    Player* player;
    
    IAnimatedMesh* cockroachMesh;
    IAnimatedMesh* spiderMesh;
    IAnimatedMesh* ratMesh;
    IAnimatedMesh* beetleMesh;
    
    vector3df positionOne;
    vector3df positionTwo;
    vector3df positionThree;
    
    s32 timeBetweenSpawns;
    s32 currentTimer;
    s32 minuteTimer;
    
    s32 spawnTimer;
    
    s32 waveTimer;
    
    s32 cooldownTimer;
    
    s32 phase;
    
    bool spawnsActive;
    bool onCooldown;
    
    void Update( s32 time );
    void SpawnWave();
    void SpawnCockroach(vector3df position);
    void SpawnBeetle(vector3df position);
    void SpawnSpider(vector3df position);
    void SpawnRat(vector3df position);
    
    void Spawn(s32 type, vector3df position);
    
    s32 getRandom( s32 upper );
    
    void DIRTYMESHFIX();
    void SpawnWaveSpiders();
    void SpawnWaveBeetles();
    
    vector3df RandomPoint();
    void FinalWave();
};

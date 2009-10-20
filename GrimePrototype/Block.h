#pragma once
#include "entity.h"
#include "Enemy.h"

class Block : public Entity
{
public:
    Block(scene::ISceneManager* smgr, irrklang::ISoundEngine* soundEngine, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects, core::array<Block*>* blockObjects);
    ~Block(void);
    void Update(s32 time);
    void ConvertToStatic();
    void ConvertToDynamic();
    
    bool active;
    
    core::array<Block*>* blockObjects;
    irrklang::ISoundEngine* soundEngine;
    SPhysxAndBlockPair* pair;
    ISceneManager* smgr;
    IPhysxManager* physxManager;
    IMesh* cubeMesh;
};

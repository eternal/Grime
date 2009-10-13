#pragma once
#include "entity.h"
#include "Enemy.h"

class Block : public Entity
{
public:
    Block(scene::ISceneManager* smgr, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects, core::array<Block*>* blockObjects);
    ~Block(void);
    void Update(s32 time);
    void ConvertToStatic();
    void ConvertToDynamic();
    
    core::array<Block*>* blockObjects;
    SPhysxAndBlockPair* pair;
    ISceneManager* smgr;
    IPhysxManager* physxManager;
    IMesh* cubeMesh;
};

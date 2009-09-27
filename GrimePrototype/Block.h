#pragma once
#include "entity.h"
#include "Enemy.h"

class Block : public Entity
{
public:
    Block(scene::ISceneManager* smgr, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects);
    ~Block(void);
    
    SPhysxAndNodePair* pair;
};

#pragma once
#include "enemy.h"

class Spider :
    public Enemy
{
public:
    Spider(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position = vector3df(-501.0f,100.0f,-230.0f));
    ~Spider(void);
    
    u32 circleDirection;
    
    void Update(s32 time);

};

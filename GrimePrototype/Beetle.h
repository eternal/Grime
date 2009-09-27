#pragma once
#include "enemy.h"

class Beetle :
    public Enemy
{
public:
    Beetle(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position = vector3df(-501.0f,100.0f,-230.0f));
    ~Beetle(void);
};

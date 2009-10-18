#pragma once
#include "Enemy.h"
//#include "LODSceneNode.h"

class Cockroach : public Enemy
{
public:
    Cockroach(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position = vector3df(-501.0f,100.0f,-230.0f));
    ~Cockroach(void);
    
    IMeshSceneNode* node;    
    
    void Update(s32 time);
    void FaceTarget();
};

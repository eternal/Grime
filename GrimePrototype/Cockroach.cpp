#include "Cockroach.h"

Cockroach::Cockroach(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine,  IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position) : Enemy(sceneManager, soundEngine, mesh,manager,objectArray,player,position)
{
    strength = 1;
    health = 2;
    speed = 1.2f;
}

Cockroach::~Cockroach(void)
{
}

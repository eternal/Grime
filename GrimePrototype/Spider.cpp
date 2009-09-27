#include "Spider.h"

Spider::Spider( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/ ) : Enemy(sceneManager, soundEngine, mesh, manager, objectArray, player, position)
{
    strength = 2;
    health = 3;
    speed = 1.2f;
}
Spider::~Spider(void)
{
}

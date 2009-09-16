#include "Player.h"

Player::Player(scene::ISceneManager* sceneManager, IAnimatedMesh* mesh, IPhysxManager* manager, SPhysxAndCameraPair* cameraPair,  core::array<SPhysxAndNodePair*>* objectArray, EffectHandler* effect)
{
    //not much needs to be stored for player, just set references and pair
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->objects = objectArray;
    this->pair = cameraPair;
    
    health = 100;
}

Player::~Player(void)
    {
        
    }
void Player::Update(s32 time) 
{
    
}
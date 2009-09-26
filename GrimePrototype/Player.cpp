#include "Player.h"
#include <iostream>

Player::Player(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, SPhysxAndCameraPair* cameraPair,  core::array<SPhysxAndNodePair*>* objectArray, EffectHandler* effect)
{
    //not much needs to be stored for player, just set references and pair
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->soundEngine = soundEngine;
    this->objects = objectArray;
    this->pair = cameraPair;
    vector3df pos;
    this->pair->PhysxObject->getPosition(pos);
    this->sound = soundEngine->play3D("media/sounds/Run1.wav", pos, true, false, true);
    health = 100;
    currentWeapon = WEAPON_PISTOL;
    //pair->gun = smgr->addMeshSceneNode(smgr->getMesh("media/RPG1.obj")->getMesh(0), pair->SceneNode);
    SetWeapon(currentWeapon);
}

Player::~Player(void)
{
    sound->stop();
    sound->drop();   
}
void Player::SetWeapon(u32 weapon)
{
    if (pair->gun)
    {
        //if node already exists (99% of times) remove it as it will be replaced
        pair->gun->remove();
    }
    if (weapon == WEAPON_CLOSE)
    {
        //pair->gun = smgr->addMeshSceneNode(smgr->getMesh("media/gun3.obj")->getMesh(0));
        pair->gun = smgr->addMeshSceneNode(smgr->getMesh("media/gun3.obj")->getMesh(0), pair->SceneNode);
        pair->gun->setPosition(core::vector3df(4.5f,-10.2f,10.0f));
        pair->gun->setRotation(core::vector3df(35.0f,90.0,90.0f));
        pair->gun->setScale(core::vector3df(0.5f,0.5f,0.3f));
        pair->gun->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);  
    }
    else if (weapon == WEAPON_PISTOL)
    {
        pair->gun = smgr->addMeshSceneNode(smgr->getMesh("media/weapon.obj")->getMesh(0), pair->SceneNode);
        pair->gun->setPosition(core::vector3df(1.5f,-3.2f,1.0f));
        pair->gun->setRotation(core::vector3df(0.0f,180.0,0.0f));
        pair->gun->setScale(core::vector3df(0.5f,0.5f,0.5f));
        pair->gun->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 
    }
    else if (weapon == WEAPON_RPG)
    {
        pair->gun = smgr->addMeshSceneNode(smgr->getMesh("media/RPG1.obj")->getMesh(0), pair->SceneNode);
        pair->gun->setPosition(vector3df(4.5f,-27.5f,-10.0f));
        pair->gun->setRotation(core::vector3df(0.0f,0.0,0.0f));
        pair->gun->setScale(core::vector3df(1.0f,1.0f,0.7f));
        pair->gun->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);   
    }
}
void Player::WeaponSelect(s32 delta) 
{
    std::cout << delta << std::endl;
    if (delta > 0)
    {
        int tempDelta = delta;
        while (tempDelta > 0)
        {
            currentWeapon++;
            tempDelta--;
            if (currentWeapon == NUMBER_OF_WEAPONS)
            {
                currentWeapon = 0;
            }
        }
    }
    else 
    {
        s32 reverseDelta = -delta;
        while (reverseDelta > 0)
        {   
            reverseDelta--;
            if (currentWeapon == 0)
            {
                currentWeapon = NUMBER_OF_WEAPONS - 1;
            }
            else {
                currentWeapon--;
            }
        }
        
    }    
    std::cout << currentWeapon << std::endl;
    this->SetWeapon(currentWeapon);
}
void Player::Update(s32 time) 
{
    
    vector3df pos, rot;
    pair->PhysxObject->getPosition(pos);
    pair->PhysxObject->getRotation(rot);
    soundEngine->setListenerPosition(pos, rot);
    sound->setPosition(pos);
}
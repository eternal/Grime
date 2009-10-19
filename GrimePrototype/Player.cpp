#include "Player.h"
#include <iostream>

Player::Player(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IPhysxManager* manager, SPhysxAndCameraPair* cameraPair, EffectHandler* effect)
{
    //not much needs to be stored for player, just set references and pair
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->soundEngine = soundEngine;
    this->pair = cameraPair;
    ratKilled = false;
    gameComplete = false;
    vector3df pos;
    this->pair->PhysxObject->getPosition(pos);
    this->sound = soundEngine->play3D("media/sounds/Run1.wav", pos, true, true, true);
    health = 100;
    blockAmmunitionTimer = 0;
    damagedTimer = 0;
    webTimer = 0;
    knockbackChargeupTimer = 0;
    knockbackChargeup = false;
    
    currentWeapon = WEAPON_PISTOL;
    for (u32 i = 0; i < NUMBER_OF_WEAPONS; i++)
    {
        weaponCooldown[i] = 0;
    }
    
    for (u32 i =0; i < NUMBER_OF_WEAPONS; i++)
    {
        weaponAmmunition[i] = 0;
    }
    weaponAmmunition[WEAPON_BLOCKGUN] = 25;
    
    //preload meshes
    smgr->getMesh("media/gun3.obj");
    smgr->getMesh("media/RPG1.obj");
    smgr->getMesh("media/weapon.obj");
    
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
    if (weapon == WEAPON_BLOCKGUN)
    {
        pair->gun = smgr->addMeshSceneNode(smgr->getMesh("media/gun3.obj")->getMesh(0), pair->SceneNode);
        pair->gun->setPosition(core::vector3df(4.5f,-10.2f,10.0f));
        pair->gun->setRotation(core::vector3df(35.0f,90.0,90.0f));
        pair->gun->setScale(core::vector3df(0.5f,0.5f,0.3f));
        pair->gun->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);  
        pair->gun->setVisible(false);
    }
    else if (weapon == WEAPON_CLOSE)
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
        pair->gun->setPosition(core::vector3df(2.2f,-3.2f,1.2f));
        pair->gun->setRotation(core::vector3df(0.0f,180.0,0.0f));
        pair->gun->setScale(core::vector3df(0.5f,0.5f,0.5f));
        pair->gun->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
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
void Player::WeaponSelect(f32 delta) 
{
    if (delta > 0.0f)
    {
        f32 tempDelta = delta;
        while (tempDelta > 0.0f)
        {
            currentWeapon++;
            tempDelta--;
            if (currentWeapon == NUMBER_OF_WEAPONS - 1)
            {
                currentWeapon = 0;
            }
        }
    }
    else 
    {
        f32 reverseDelta = -delta;
        while (reverseDelta > 0.0f)
        {   
            reverseDelta--;
            if (currentWeapon == 0)
            {
                currentWeapon = NUMBER_OF_WEAPONS - 2;
            }
            else 
            {
                currentWeapon--;
            }
        }
        
    }    
    this->SetWeapon(currentWeapon);
}
int Player::GetWeapon()
{
    return currentWeapon;
}
bool Player::CurrentWeaponOnCooldown() 
{
    if (this->weaponCooldown[currentWeapon] <= 0.0f)
    {
        return false;
    }
    else 
    {
        return true;
    }
}
void Player::AddCoolDown() 
{
    if (this->currentWeapon == WEAPON_PISTOL)
    {
        this->weaponCooldown[WEAPON_PISTOL] = 250;
    }
    else if (this->currentWeapon == WEAPON_RPG)
    {
        this->weaponCooldown[WEAPON_RPG] = 3000;
    }
    else if (this->currentWeapon == WEAPON_CLOSE)
    {
        this->weaponCooldown[WEAPON_CLOSE] = 10000;
    }
    else if (this->currentWeapon == WEAPON_BLOCKGUN)
    {
        this->weaponCooldown[WEAPON_BLOCKGUN] = 1000;
    }
}
void Player::AddCoolDown(s32 weapon) 
{
    if (weapon == WEAPON_PISTOL)
    {
        this->weaponCooldown[WEAPON_PISTOL] = 250;
    }
    else if (weapon == WEAPON_RPG)
    {
        this->weaponCooldown[WEAPON_RPG] = 3000;
    }
    else if (weapon == WEAPON_CLOSE)
    {
        this->weaponCooldown[WEAPON_CLOSE] = 10000;
    }
    else if (weapon == WEAPON_BLOCKGUN)
    {
        this->weaponCooldown[WEAPON_BLOCKGUN] = 1000;
    }
}
s32 Player::CurrentCooldown()
{
    return weaponCooldown[currentWeapon];
}

void Player::Jump()
{
    if (this->pair->camera)
    {
        // Perform a raycast to find the objects we just shot at
        core::line3df line;
        line.start = pair->camera->getPosition();
        line.end = line.start;
        line.end.Y = -2000.0f;
        //access the physics engine to find the intersection point
        core::array<SRaycastHitData> rayArray = physxMan->raycastAllRigidObjects(line);
        for (u32 i = 0; i < rayArray.size(); ++i) 
        {
            SRaycastHitData ray = rayArray[i];
            if (ray.Object->getType() == EOT_TRIANGLE_MESH || ray.Object->getType() == EOT_BOX)
            {
                f32 dis = (ray.HitPosition - line.start).getLength();
                if (dis <= 25)
                {
                    pair->PhysxObject->setLinearVelocity(core::vector3df(0,70,0));
                }
            }
        }
    }
}
void Player::Update(s32 time) 
{
    if (knockbackChargeup)
    {
        knockbackChargeupTimer += time;
        std::cout << knockbackChargeupTimer << std::endl;
    }
    for (u32 i = 0; i < NUMBER_OF_WEAPONS; i++)
    {
        if (weaponCooldown[i] > 0.0f)
        {
            weaponCooldown[i] -= time;
        }
    }
    
    blockAmmunitionTimer += time;
    if (blockAmmunitionTimer >= 5000)
    {
        weaponAmmunition[WEAPON_BLOCKGUN]++;
        blockAmmunitionTimer = 0;
    }
    
    vector3df pos, rot;
    pair->PhysxObject->getPosition(pos);
    pair->PhysxObject->getRotation(rot);
    soundEngine->setListenerPosition(pos, rot);
    sound->setPosition(pos);
}
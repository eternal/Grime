//=============================================================================
//  Simple player class
//  Author: Douglas Cook
//=============================================================================

#pragma once
#include "entity.h"
#include "XEffects.h"
#include <irrlicht.h>
#include <iostream>
#include <IrrPhysx.h>
#include "PairStructs.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace IrrPhysx;

enum WEAPONS 
{
    WEAPON_PISTOL,
    WEAPON_RPG,
    WEAPON_CLOSE,
    WEAPON_BLOCKGUN,
    NUMBER_OF_WEAPONS
};


class Player : public Entity
{
    public:
        Player(scene::ISceneManager* smgr, irrklang::ISoundEngine* soundEngine, IPhysxManager* manager, SPhysxAndCameraPair* cameraPair, EffectHandler* effects);
        ~Player(void);
        
        //update is called every frame; perform game logic
        void Update(s32 time);
        void SetWeapon(u32 weapon);
        void WeaponSelect(f32 delta);
        int GetWeapon();
        bool CurrentWeaponOnCooldown();
        void AddCoolDown();
        s32 CurrentCooldown();
        void Jump();

        //store local references to required data        
        scene::ISceneManager* smgr;
        IPhysxManager* physxMan;
        scene::IAnimatedMeshSceneNode* node;
        irrklang::ISoundEngine* soundEngine;
        SPhysxAndCameraPair* pair; 
        
        irrklang::ISound* sound;
        s32 currentWeapon;   
        s32 weaponCooldown[NUMBER_OF_WEAPONS];
        
        s32 health;
};

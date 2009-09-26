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
//#include "IrrPhysx/ShapeCreation.h"
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
    NUMBER_OF_WEAPONS
};


class Player : public entity
{
    public:
        Player(scene::ISceneManager* smgr, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, SPhysxAndCameraPair* cameraPair, core::array<SPhysxAndNodePair*>* objectArray, EffectHandler* effects);
        ~Player(void);
        
        //update is called every frame; perform game logic
        void Update(s32 time);
        void SetWeapon(u32 weapon);
        void WeaponSelect(s32 delta);
        
        //store local references to required data        
        scene::ISceneManager* smgr;
        IPhysxManager* physxMan;
        scene::IAnimatedMeshSceneNode* node;
        irrklang::ISoundEngine* soundEngine;
        SPhysxAndCameraPair* pair;
        core::array<SPhysxAndNodePair*>* objects;    
        
        irrklang::ISound* sound;
        s32 currentWeapon;   
        
        s32 health;
};

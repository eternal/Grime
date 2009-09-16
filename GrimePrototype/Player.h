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
#include "IrrPhysx/ShapeCreation.h"
#include "CameraPhysics.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class Player : public entity
{
    public:
        Player(scene::ISceneManager* smgr, IAnimatedMesh* mesh, IPhysxManager* manager, SPhysxAndCameraPair* cameraPair, core::array<SPhysxAndNodePair*>* objectArray, EffectHandler* effects);
        ~Player(void);
        
        //update is called every frame; perform game logic
        void Update(s32 time);
        
        //store local references to required data        
        scene::ISceneManager* smgr;
        IPhysxManager* physxMan;
        scene::IAnimatedMeshSceneNode* node;
        SPhysxAndCameraPair* pair;
        core::array<SPhysxAndNodePair*>* objects;       
        
        s32 health;
};

//=============================================================================
//  Enemy class
//  Author: Douglas Cook
//=============================================================================

#pragma once
#include "entity.h"
#include "Player.h"
#include <irrlicht.h>
#include <IrrPhysx.h>
#include "IrrPhysx/ShapeCreation.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

class Enemy : public entity
{
    public:
        Enemy(void);
        Enemy(scene::ISceneManager* sceneManager, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<SPhysxAndNodePair*>* objectArray, Player* player, vector3df position = vector3df(-501.0f,100.0f,-230.0f));
        ~Enemy(void);
        
        //physics pair
        SPhysxAndNodePair* pair;
        //required references
        scene::ISceneManager* smgr;
        IPhysxManager* physxMan;
        scene::IAnimatedMeshSceneNode* node;
        
        core::array<SPhysxAndNodePair*>* objects;        
        
        Player* target;
        
        bool active;
        bool attackPhase;
        bool attackPhaseActive;
        void FaceTarget();
        void CheckPhase();
        
        s32 attackTimer;
        
        void Update(s32 time);
};

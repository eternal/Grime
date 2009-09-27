//=============================================================================
//  Enemy class
//  Author: Douglas Cook
//=============================================================================

#pragma once
#include "entity.h"
#include "Player.h"
#include <irrlicht.h>
#include <IrrPhysx.h>
#include <irrKlang.h>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace IrrPhysx;

class Enemy : public Entity
{
    public:
        Enemy(void);
        Enemy(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position = vector3df(-501.0f,100.0f,-230.0f));
        ~Enemy(void);
        
        //physics pair
        SPhysxAndNodePair* pair;
        //required references
        scene::ISceneManager* smgr;
        irrklang::ISoundEngine* soundEngine;
        IPhysxManager* physxMan;
        scene::IAnimatedMeshSceneNode* node;
        
        core::array<Enemy*>* enemyArray;        
        ISound* sound;
        
        Player* target;
        u16 strength;
        f32 speed;
        f32 health;
        
        bool active;
        bool attackPhase;
        bool attackPhaseActive;
        bool soundReset;
        
        virtual void FaceTarget();
        void CheckPhase();
        
        s32 attackTimer;
        f32 soundResetTimer;
        u32 soundWalkCurrentPosition;
        
        virtual void Update(s32 time);
        bool IsStillAlive();
};

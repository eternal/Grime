//=============================================================================
//  Base class - TO BE IMPLEMENTED
//  Author: Douglas Cook
//=============================================================================

#pragma once
#include <irrlicht.h>
#include <IrrPhysx.h>
#include "PairStructs.h"
//#include "IrrPhysx/ShapeCreation.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace IrrPhysx;

class Entity
    {
    public:
        Entity(void);
        Entity(scene::ISceneManager* smgr, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<SPhysxAndNodePair*>* objectArray);
        ~Entity(void);
        
        //required references
        scene::ISceneManager* smgr;
        IPhysxManager* physxMan;
        scene::IAnimatedMeshSceneNode* node;
        
        //store reference to other objects for debug purposes
        core::array<SPhysxAndNodePair*>* objects;        

        
    };

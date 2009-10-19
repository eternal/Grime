//=============================================================================
//  Struct to hold physics object for camera and camera node
//  Author: Douglas Cook
//=============================================================================
#pragma once
#include <irrlicht.h>
#include <IrrPhysx.h>
#include <irrKlang.h>
#include "irrKlangSceneNode.h"
#include <iostream>

using namespace irr;
using namespace IrrPhysx;

struct SPhysxAndNodePair {

    SPhysxAndNodePair() {
        PhysxObject = NULL;
        SceneNode = NULL;
    }
    void updateTransformation() 
    {
            core::vector3df vec(0,0,0);
            // Update the node's position to that of the physx object

            if (PhysxObject)
            {
                PhysxObject->getPosition(vec);
                SceneNode->setPosition(vec);            
            }

    }
    void updateRotation() 
    {
        core::vector3df vec(0,0,0);
        // Update the node's position to that of the physx object

        if (PhysxObject)
        {
            PhysxObject->getRotation(vec);
            SceneNode->setRotation(vec);            
        }

    }
    IPhysxObject* PhysxObject;
    scene::ISceneNode* SceneNode;

};
struct SPhysxAndNodePairCloth {

    SPhysxAndNodePairCloth() {
        PhysxObject = NULL;
        SceneNode = NULL;
    }

    void updateTransformation() {
        if (PhysxObject->getType() == EOT_CLOTH) { // If it's a cloth we just need to update it (for tearing purposes)
            ((IClothPhysxObject*)PhysxObject)->update();
        } else { // Otherwise we update its transformation
            core::vector3df vec;
            // Update the node's position to that of the physx object
            PhysxObject->getPosition(vec);
            SceneNode->setPosition(vec);
        }
    }

    IPhysxObject* PhysxObject;
    scene::ISceneNode* SceneNode;

};
//struct SPhysxAndNodePairAudible : public SPhysxAndNodePair {
//    SPhysxAndNodePairAudible() : SPhysxAndNodePair() { SoundNode = NULL; }
//    
//    CIrrKlangSceneNode* SoundNode;
//    
//    void updateTransformation() {
//        if (PhysxObject && SceneNode) {
//            core::vector3df vec;
//            // Update the node's position to that of the physx object
//            PhysxObject->getPosition(vec);
//            SceneNode->setPosition(vec);
//            if (SoundNode) 
//            {
//                SoundNode->setPosition(vec);
//            }
//        }
//    }
//    
//};
struct SPhysxAndBlockPair : public SPhysxAndNodePair {
    //get all the details from the normal pair
    SPhysxAndBlockPair() : SPhysxAndNodePair() { }
    
    core::vector3df blockOffset;
    
    //called every update
    void updateTransformation() {
        if (PhysxObject && SceneNode) {
            core::vector3df currentPhysxPos;
            PhysxObject->getPosition(currentPhysxPos);
            SceneNode->setPosition(currentPhysxPos + blockOffset);
        }
    }
};

struct SPhysxAndCameraPair : public SPhysxAndNodePair {
    //get all the details from the normal pair
    SPhysxAndCameraPair() : SPhysxAndNodePair() { gun = NULL; }
    core::vector3df PreviousPosition;
    core::vector3df CameraOffset;
    
    ISceneNode* gun;
    ICameraSceneNode* camera;

    //called every update
    void updateTransformation() {
        if (PhysxObject && SceneNode) {
            core::vector3df currentPhysxPos;
            PhysxObject->getPosition(currentPhysxPos);
            core::vector3df currentCameraPos = SceneNode->getPosition() - CameraOffset;
            core::vector3df newPos = currentPhysxPos + (currentCameraPos - PreviousPosition);
            PhysxObject->setPosition(newPos);
            SceneNode->setPosition(newPos + CameraOffset);
            PreviousPosition = newPos;
        }
    }
};
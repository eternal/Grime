//=============================================================================
//  Struct to hold physics object for camera and camera node
//  Author: Douglas Cook
//=============================================================================

#pragma once
struct SPhysxAndCameraPair : public SPhysxAndNodePair {
    //get all the details from the normal pair
    SPhysxAndCameraPair() : SPhysxAndNodePair() {}
    core::vector3df PreviousPosition;
    core::vector3df CameraOffset;

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
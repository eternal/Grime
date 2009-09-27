#include "entity.h"
Entity::Entity() {

}

//////////////////////////////////////////////////////////////////////////
//TEMPORARY CODE
Entity::Entity(scene::ISceneManager* sceneManager, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<SPhysxAndNodePair*>* objectArray)
    {
        vector3df pos = vector3df(0.0f,100.0f,0.0f);
        vector3df rot = vector3df(0.0f,0.0f,0.0f);
        vector3df scale = vector3df(2.0f,2.0f,2.0f);
        vector3df pos2 = vector3df(15.0f, 0.0f, 0.0f);
        vector3df rot2 = vector3df(-90.0f,0.0f, 0.0f);
        
        this->smgr = sceneManager;
        this->physxMan = manager;
        this->objects = objectArray;
        SPhysxAndNodePair* pair = new SPhysxAndNodePair;
        IPhysxMesh* pmesh = physxMan->createConvexMesh(mesh->getMeshBuffer(0), scale);

        pair->PhysxObject = physxMan->createConvexMeshObject(pmesh, pos, rot, 10);


        pair->SceneNode = sceneManager->addAnimatedMeshSceneNode(mesh, NULL, -1, pos, rot, scale);
        //pair->SceneNode->setMaterialTexture(0, driver->getTexture("media/dwarf2.JPG"));
        pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
        objects->push_back(pair);
    }

Entity::~Entity(void)
    {
    }

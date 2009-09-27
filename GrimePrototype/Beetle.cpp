#include "Beetle.h"

Beetle::Beetle( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/ )
{
    //set position and scale data
    vector3df pos = position;
    vector3df rot = vector3df(0.0f,0.0f,0.0f);
    vector3df scale = vector3df(0.5f,0.5f,0.5f);
    //set references
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->soundEngine = soundEngine;
    this->enemyArray = objectArray;
    this->target = player;
    this->active = true;
    this->health = 1;
    this->speed = 1.0f;
    this->strength = 1;
    //instantiate pair
    pair = new SPhysxAndNodePair;
    attackPhase = false;
    attackPhaseActive = false;
    soundReset = false;
    soundResetTimer = 0.0f;
    soundWalkCurrentPosition = 0;
    vector3df physxRot = rot;
    //physxRot.X -= 90.0f;
    //physxRot.Y -= 45.0f;

    //add scene node to game
    this->node = sceneManager->addAnimatedMeshSceneNode(mesh, NULL, -1, pos, rot, scale);
    IPhysxMesh* pmesh = physxMan->createConvexMesh(this->node->getMesh()->getMeshBuffer(0), scale);
    pair->PhysxObject = physxMan->createConvexMeshObject(pmesh, pos, physxRot, 1000.0f);
    pair->SceneNode = node;

    //avoid rolling around
    pair->PhysxObject->setAngularDamping(1000.0f);
    //pair->SoundNode = new CIrrKlangSceneNode(soundEngine, node, smgr, 1);
    //pair->SoundNode->setSoundFileName("media/sounds/Bang1.wav");
    // pair->SoundNode->setPlayOnceMode();
    // pair->SoundNode->setMinMaxSoundDistance(1.0f);

    u32 randNum = (rand() % 300) + 1;
    sound = soundEngine->play3D("media/sounds/InsectWalk1.wav", this->pair->SceneNode->getAbsolutePosition(), true, false, true);
    sound->setMinDistance(100.0f);
    sound->setMaxDistance(500.0f);
    sound->setPlayPosition(randNum);

    this->node->setAnimationSpeed(25.0f);
    //set walking frame loop

    attackTimer = 0;
    //since mesh was scaled, normalise normals
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
   
    strength = 3;
    health = 3;
    speed = 1.0f;
 
    this->node->setFrameLoop(19,34);
    
}
Beetle::~Beetle(void)
{

}

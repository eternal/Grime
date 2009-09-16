#include "Enemy.h"
//possibly add randomising constructor later
Enemy::Enemy(void)
{
}
//normal constructor
Enemy::Enemy(scene::ISceneManager* sceneManager, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<SPhysxAndNodePair*>* objectArray, Player* player, vector3df position)
    {
    //set position and scale data
    vector3df pos = position;
    vector3df rot = vector3df(0.0f,0.0f,0.0f);
    vector3df scale = vector3df(2.0f,2.0f,2.0f);
    //set references
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->objects = objectArray;
    this->target = player;
    this->active = true;
    //instantiate pair
    pair = new SPhysxAndNodePair;
    attackPhase = false;
    attackPhaseActive = false;
    
    IPhysxMesh* pmesh = physxMan->createConvexMesh(mesh->getMeshBuffer(0), scale);
    pair->PhysxObject = physxMan->createConvexMeshObject(pmesh, pos, rot, 1000.0f);
    //avoid rolling around
    pair->PhysxObject->setAngularDamping(1000.0f);
    //add scene node to game
    this->node = sceneManager->addAnimatedMeshSceneNode(mesh, NULL, -1, pos, rot, scale);
    pair->SceneNode = node;
    
    this->node->setAnimationSpeed(25.0f);
    //set walking frame loop
    this->node->setFrameLoop(10,22);
    
    //since mesh was scaled, normalise normals
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
}

Enemy::~Enemy(void)
{
}

void Enemy::FaceTarget() {
    vector3df targetPos, nodePos, selfPos;
    //grab positions from physics objects
    target->pair->PhysxObject->getPosition(targetPos);
    this->pair->PhysxObject->getPosition(selfPos);
    //find vector self->target
    nodePos = targetPos - selfPos;
    vector3df selfRot;
    selfRot.Y = atan(nodePos.Z/nodePos.X) * (180.0f / irr::core::PI);
    if((targetPos.X - selfRot.X) > 0) {
        selfRot.Y = 90 - selfRot.Y;
        } else if((targetPos.X - selfPos.X) < 0) {
            selfRot.Y = -90 - selfRot.Y;
        }
    selfRot.Y += 180;
    this->pair->PhysxObject->setRotation(selfRot);
    this->pair->SceneNode->setRotation(selfRot);
}

void Enemy::CheckPhase() 
{
    if (attackPhase)
    {
        if (attackPhaseActive)
        {
            
        }
        else
        {
            this->node->setFrameLoop(23,77);
            attackPhaseActive = true;
        }
    }
    else
    {
        if (attackPhaseActive)
        {
            this->node->setFrameLoop(10,22);
            attackPhaseActive = false;
        }
    }
}

void Enemy::Update(s32 time) 
{
    if (active)
    {
        //if target exists
        if (target)
        {
            FaceTarget();
            //TODO: UNIFY VARIABLES FOR READABILITY
            vector3df direction, position, playerPos, resultant;
            f32 distanceToTarget;
            //get position of player and target
            this->pair->PhysxObject->getPosition(position);
            this->target->pair->PhysxObject->getPosition(playerPos);
            //get vector this->player
            direction = playerPos - position;   
            //get magnitude
            distanceToTarget = direction.getLength();
            //std::cout << distanceToTarget << std::endl;
            if (distanceToTarget < 50)
            {
                attackPhase = true;
            }
            else {
                attackPhase = false;
            }
            CheckPhase();
            //normalise vector
            direction.normalize();
            //normalise with respect to time elapsed since last update
            direction = direction * (time/10.0f);
            //find and apply the change
            resultant = position + direction;
            //check for floating point errors that were appearing            
            if (!(_isnan(resultant.X)))
            {
                this->pair->PhysxObject->setPosition(resultant);
                this->pair->updateTransformation();
            }
        }
    }

}
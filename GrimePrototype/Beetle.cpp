#include "Beetle.h"

Beetle::Beetle( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, core::array<Block*>* blockArray, Player* player, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/ )
{
    /*Bug
        Standing - 11
        Dead - 10
        Standing>Walk - 10-18
        Walk cycle - 19>34
        attack cycle - 40-60*/
        
    //set position and scale data
    vector3df pos = position;
    vector3df rot = vector3df(0.0f,0.0f,0.0f);
    vector3df scale = vector3df(0.5f,0.5f,0.5f);
    //set references
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->soundEngine = soundEngine;
    this->enemyArray = objectArray;
    this->blockArray = blockArray;
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
   
    strength = 6;
    health = 3;
    speed = 1.0f;
 
    this->node->setFrameLoop(19,34);
    
    this->blockDestroyTimer = 0;
}

Beetle::Beetle( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/ )
{

}
Beetle::~Beetle(void)
{

}

void Beetle::Update(s32 time)
{
    if (active)
    {    
        if (this->IsStillAlive() && this->pair->PhysxObject && this->pair->SceneNode)
        {
            try 
            {
                this->pair->updateTransformation();  

                sound->setPosition(this->pair->SceneNode->getAbsolutePosition());

                // pair->SoundNode->setLoopingStreamMode();
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
                    
                    //raycast for block in front
                    line3df ray;
                    ray.start = position + (direction.normalize() * 10.0f);
                    ray.end = playerPos;
                    vector3df intersect;
                    IPhysxObject* closestObject = physxMan->raycastClosestObject(ray, &intersect);
                    if (closestObject->getType() == EOT_BOX)
                    {
                        f32 distanceToBlock = (position - intersect).getLength();
                        std::cout << "Distance: " << distanceToBlock << std::endl;
                        if (distanceToBlock < 75)
                        {
                            blockDestroyTimer += time;
                            std::cout << blockDestroyTimer << std::endl;
                        }
                        if (blockDestroyTimer >= 4000)
                        {
                            for (u32 i = 0; i < blockArray->size(); i++)
                            {
                                if (closestObject == (*blockArray)[i]->pair->PhysxObject)
                                {
                                    vector3df directionToBlock = (position - intersect).normalize();
                                    directionToBlock.Y = 0;
                                    this->pair->PhysxObject->setPosition(position + directionToBlock * 50.0f);
                                    this->pair->PhysxObject->setLinearVelocity(directionToBlock * 5.0f);
                                    this->pair->updateTransformation();
                                    this->pair->PhysxObject->getPosition(position);
                                    
                                    Block* block = (*blockArray)[i];
                                    block->active = false;
                                    block->pair->SceneNode->remove();
                                    physxMan->removePhysxObject(block->pair->PhysxObject);
                                    blockArray->erase(i);
                                    delete block;
                                    break;
                                }
                            }
                            blockDestroyTimer = 0;
                        }
                    }
                    
                    //std::cout << distanceToTarget << std::endl;
                    if (distanceToTarget < 50)
                    {
                        attackPhase = true;
                    }
                    else 
                    {
                        attackPhase = false;
                    }
                    if (soundReset)
                    {
                        soundResetTimer += time;
                    }
                    if (distanceToTarget < 30)
                    {
                        attackTimer += time;
                    }
                    if (attackTimer >= 1000)
                    {
                        //    pair->SoundNode->stop();
                        soundWalkCurrentPosition = sound->getPlayPosition();
                        sound->stop();
                        sound->drop();
                        sound = soundEngine->play3D("media/sounds/Bite1.wav",this->pair->SceneNode->getAbsolutePosition(), false, true, true);
                        sound->setMinDistance(100.0f);
                        sound->setMaxDistance(1000.0f);
                        sound->setIsPaused(false);
                        soundReset = true;
                        //     pair->SoundNode->setPlayOnceMode();
                        u32 critChance = (rand() % 4) + 1;
                        if (critChance == 1)
                        {
                            target->health -= strength * 2;
                        }
                        else {
                            target->health -= strength;
                        } 
                        attackTimer = 0;

                    }
                    if (soundResetTimer >= 300)
                    {
                        sound->stop();
                        sound->drop();
                        sound = soundEngine->play3D("media/sounds/InsectWalk1.wav", this->pair->SceneNode->getAbsolutePosition(), true, false, true);
                        sound->setPlayPosition(soundWalkCurrentPosition);
                        sound->setMinDistance(100.0f);
                        sound->setMaxDistance(1000.0f);
                        soundResetTimer = 0;
                        soundReset = false;
                    }
                    CheckPhase();
                    //normalise vector
                    direction.normalize();
                    //normalise with respect to time elapsed since last update
                    direction = direction * (time/10.0f) * speed;
                    direction.Y = 0; //no flying mr bug
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
            catch (...)
            {
                std::cout << "exception" << std::endl;
                std::cout << "Enemy: " << &pair << std::endl;
                active = false;
                try 
                {
                    this->pair->SceneNode->setVisible(false);
                    this->pair->SceneNode->remove();
                }
                catch (...)
                {
                    std::cout << "Recovery failed: Mesh still in Scene Graph" << std::endl;
                }
                try 
                {
                    physxMan->removePhysxObject(this->pair->PhysxObject);
                }
                catch (...)
                {
                    std::cout << "Recovery failed: Mesh still in Physics Graph" << std::endl;
                }

            }

        }
        else 
        {                    
            try 
            {   
                for (u32 i = 0; i < enemyArray->size(); ++i)
                {
                    Enemy* enemy = (*enemyArray)[i];
                    if (enemy->pair == this->pair)
                    {
                        enemyArray->erase(i);
                        break;
                    }                   
                }
                physxMan->removePhysxObject(this->pair->PhysxObject);
                this->pair->SceneNode->setVisible(false);
                this->pair->SceneNode->remove();
            }
            catch (...)
            {
                std::cout << "another exception" << std::endl;
                std::cout << "Enemy: " << &pair << std::endl; 
            }
            active = false;
        } 
    }
}
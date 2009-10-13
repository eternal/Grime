#include "Spider.h"

Spider::Spider( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/ )
{
    //    Spider
    //    Dead -  10
    //    Standing - 1
    //    walk cycle - 11-21
    //    attack cycle - 22-77
    //set position and scale data
    vector3df pos = position;
    vector3df rot = vector3df(0.0f,0.0f,0.0f);
    vector3df scale = vector3df(3.0f,3.0f,3.0f);
    //set references
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->soundEngine = soundEngine;
    this->enemyArray = objectArray;
    this->target = player;
    this->active = true;

    //instantiate pair
    pair = new SPhysxAndNodePair;
    attackPhase = false;
    attackPhaseActive = false;
    soundReset = false;
    soundResetTimer = 0.0f;
    soundWalkCurrentPosition = 0;

    IPhysxMesh* pmesh = physxMan->createConvexMesh(mesh->getMeshBuffer(0), scale);
    pair->PhysxObject = physxMan->createConvexMeshObject(pmesh, pos, rot, 1000.0f);
    //avoid rolling around
    pair->PhysxObject->setAngularDamping(1000.0f);
    //add scene node to game
    this->node = sceneManager->addAnimatedMeshSceneNode(mesh, NULL, -1, pos, rot, scale);
    pair->SceneNode = node;

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
    this->node->setFrameLoop(11,21);

    attackTimer = 0;
    //since mesh was scaled, normalise normals
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

    circleDirection = (rand() % 2);
    
    strength = 2;
    health = 3;
    speed = 1.2f;
}
Spider::~Spider(void)
{
}

void Spider::Update(s32 time)
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
                    //std::cout << distanceToTarget << std::endl;
                    if (distanceToTarget < 600)
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
                    if (distanceToTarget < 350)
                    {
                        line3df ray;
                        ray.start = position + (direction.normalize() * 10.0f);
                        ray.end = playerPos;
                        IPhysxObject* closestObject = physxMan->raycastClosestObject(ray);
                        if (closestObject->getType() == EOT_SPHERE)
                        {
                            attackTimer += time;
                        }
                        else
                        {
                            //sidestrafe
                            vector3df lookAt = direction;
                            if (circleDirection == 0)
                            {
                                lookAt.rotateXZBy(90);
                            }
                            else 
                            {
                                lookAt.rotateXZBy(-90);
                            }
                            lookAt.Y = 0;
                            vector3df resul = position + lookAt.normalize() * speed;
                            if (!(_isnan(resul.X)))
                                if (!(_isnan(resul.Y)))
                                    if (!(_isnan(resul.Z)))
                                        pair->PhysxObject->setPosition(resul);
                            //std::cout << resul.X << " " << resul.Y << " " << resul.Z << std::endl;           
                            pair->updateTransformation();
                        }
                    }
                    if (attackTimer >= 2000)
                    {
                        //    pair->SoundNode->stop();
                        soundWalkCurrentPosition = sound->getPlayPosition();
                        sound->stop();
                        sound->drop();
                        //sound->roll();
                        sound = soundEngine->play3D("media/sounds/Bite1.wav",this->pair->SceneNode->getAbsolutePosition(), false, true, true);
                        sound->setMinDistance(100.0f);
                        sound->setMaxDistance(1000.0f);
                        sound->setIsPaused(false);
                        soundReset = true;
                        //pair->SoundNode->setPlayOnceMode();
                        target->health -= strength;
                        //attack logic
                        target->pair->PhysxObject->setLinearVelocity((-direction.normalize()) * 100.0f);
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
                    resultant = position + direction * speed;
                    //check for floating point errors that were appearing     
                    if (distanceToTarget > 300)
                    {
                        if (!(_isnan(resultant.X)))
                        {
                            this->pair->PhysxObject->setPosition(resultant);
                            //std::cout << resultant.X << " " << resultant.Y << " " << resultant.Z << std::endl;
                            this->pair->updateTransformation();
                        }
                    }
                }
            }
            catch (...)
            {
                std::cout << "exception lol" << std::endl;
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
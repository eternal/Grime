#include "Rat.h"

Rat::Rat(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/ )
{
    
    /*Rat
        Standing - 1-11
        Initial walk - 12-28
        Walk cycle - 28-39
        Attack cycle - (42-54)*/
    //set position and scale data
    vector3df pos = position;
    vector3df rot = vector3df(0.0f,0.0f,0.0f);
    vector3df scale = vector3df(1.8f,1.8f,1.8f);
    //set references
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
    immuneToBlockCrush = true;

    IPhysxMesh* pmesh = physxMan->createConvexMesh(mesh->getMeshBuffer(1), scale);
    pair->PhysxObject = physxMan->createConvexMeshObject(pmesh, pos, rot, 30000000.0f);
    //avoid rolling around
    pair->PhysxObject->setAngularDamping(1000.0f);
    pair->PhysxObject->setLinearDamping(400.0f);
    pair->PhysxObject->setUserData(&immuneToBlockCrush);
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
    this->node->setFrameLoop(28,39);

    attackTimer = 0;
    //since mesh was scaled, normalise normals
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    strength = 20;
    health = 50;
    speed = 1.0f;
}
Rat::~Rat(void)
{
}

void Rat::CheckPhase() 
{
    if (attackPhase)
    {
        if (attackPhaseActive)
        {

        }
        else
        {
            this->node->setFrameLoop(42,54);
            attackPhaseActive = true;
        }
    }
    else
    {
        if (attackPhaseActive)
        {
            this->node->setFrameLoop(28,39);
            attackPhaseActive = false;
        }
    }
}

void Rat::Update( s32 time )
{
    if (active)
    {    
        if (this->IsStillAlive() && this->pair->PhysxObject && this->pair->SceneNode)
        {
            try 
            {
                vector3df pos;
                this->pair->PhysxObject->getPosition(pos);
                pos.Y = 20.0f;
                this->pair->PhysxObject->setPosition(pos);
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
                    
                    if (distanceToTarget < 200)
                    {
                        //std::cout << distanceToTarget << std::endl;
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
                    if (distanceToTarget < 110)
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
                        target->health -= strength; 
                        target->damagedTimer = 510;
                        direction.Y += 50.0f;
                        target->pair->PhysxObject->setLinearVelocity(direction.normalize() * 200.0f);
                        target->pair->updateTransformation();
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
                    if (distanceToTarget >= 100.0f)
                    {
                        if (!(_isnan(resultant.X)))
                        {
                            this->pair->PhysxObject->setPosition(resultant);
                            this->pair->updateTransformation();
                        }
                    }
                }
            }
            catch (...)
            {
                std::cerr << "exception lol" << std::endl;
                std::cerr << "Enemy: " << &pair << std::endl;
                active = false;
                try 
                {
                    this->pair->SceneNode->setVisible(false);
                    this->pair->SceneNode->remove();
                }
                catch (...)
                {
                    std::cerr << "Recovery failed: Mesh still in Scene Graph" << std::endl;
                }
                try 
                {
                    physxMan->removePhysxObject(this->pair->PhysxObject);
                }
                catch (...)
                {
                    std::cerr << "Recovery failed: Mesh still in Physics Graph" << std::endl;
                }

            }

        }
        else 
        { 
            if (!(this->IsStillAlive()))
            {
                this->target->ratKilled = true;
            }
                               
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
                std::cerr << "another exception" << std::endl;
                std::cerr << "Enemy: " << &pair << std::endl; 
            }
            active = false;
        } 
    }
}
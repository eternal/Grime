#include "Cockroach.h"

Cockroach::Cockroach(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine,  IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, vector3df position)
{
    //set position and scale data
    vector3df pos = position;
    vector3df rot = vector3df(0.0f,0.0f,0.0f);
    vector3df scale = vector3df(0.2f,0.2f,0.2f);
    //vector3df scale = vector3df(1.0f,1.0f,1.0f);
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
    physxRot.X -= 90.0f;
    physxRot.Y -= 45.0f;
    
    this->node = sceneManager->addMeshSceneNode(mesh, NULL, -1, pos, rot, scale);
    IPhysxMesh* pmesh = physxMan->createConvexMesh(this->node->getMesh()->getMeshBuffer(2), scale);
    //IPhysxMesh* pmesh = physxMan->createConvexMesh(this->node->getMesh()->getMeshBuffer(0), scale);
    pair->PhysxObject = physxMan->createConvexMeshObject(pmesh, pos, physxRot, 1000.0f);
    //pair->PhysxObject = physxMan->createBoxObject(pos,physxRot, scale * 10, 1000.0f);
    pair->SceneNode = node;
    
    pair->PhysxObject->setRotation(physxRot);
    pair->SceneNode->setRotation(physxRot);
    
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

//    this->node->setJointMode(EJUOR_NONE);
    //set walking frame loop
//    this->node->setFrameLoop(1,1);

    attackTimer = 0;
    //since mesh was scaled, normalise normals
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

    strength = 3;
    health = 1;
    speed = 1.3f;
}

Cockroach::~Cockroach(void)
{
}

void Cockroach::FaceTarget()
{
    vector3df targetPos, nodePos, selfPos;
    //grab positions from physics objects
    target->pair->PhysxObject->getPosition(targetPos);
    this->pair->PhysxObject->getPosition(selfPos);
    //find vector self->target
    nodePos = selfPos - targetPos;
    vector3df selfRot = nodePos.getHorizontalAngle();
    selfRot.X = -90.0f;
    selfRot.Z = 0.0f;
    this->pair->PhysxObject->setRotation(selfRot);
    this->pair->SceneNode->setRotation(selfRot);
}

void Cockroach::Update(s32 time)
{
    if (active)
    {    
        if (this->IsStillAlive() && this->pair->PhysxObject && this->pair->SceneNode)
        {
            try 
            {
                //this->node->setFrameLoop(1,1);
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
                    if (distanceToTarget < 70)
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
                    if (distanceToTarget < 60)
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
                    //CheckPhase();
                    //normalise vector
                    direction.normalize();
                    //normalise with respect to time elapsed since last update
                    direction = direction * (time/10.0f) * speed;
                    direction.Y = 0; //no flying mr bug
                    //find and apply the change
                    resultant = position + direction;
                    //check for floating point errors that were appearing    
                    if (!(distanceToTarget <= 56.0f))
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
#include "Rat.h"

Rat::Rat(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, core::array<video::ITexture*>* explosionTextures, vector3df position /*= vector3df(-501.0f,100.0f,-230.0f)*/)
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
    previousPosition = position;
    distanceSinceFootstep = 0;
    //set references
    //set references
    this->smgr = sceneManager;
    this->physxMan = manager;
    this->soundEngine = soundEngine;
    this->enemyArray = objectArray;
    this->target = player;
    this->mesh = mesh;
    this->active = true;
    this->health = 1;
    this->speed = 1.0f;
    this->strength = 1;
    this->endingExplosionTimer = 0;
    this->explosionPhase = 0;
    this->explosionTextures = explosionTextures;
    //instantiate pair
    pair = new SPhysxAndNodePair;
    attackPhase = false;
    attackPhaseActive = false;
    soundReset = false;
    soundResetTimer = 0.0f;
    soundWalkCurrentPosition = 0;
    immuneToBlockCrush = true;
    
    roarTimer = (rand() % 2000) + 750;

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
    
    sound = soundEngine->play3D("media/sounds/Bite1.wav",this->pair->SceneNode->getAbsolutePosition(), false, true, true);

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

s32 Rat::GetRandom(s32 upper) 
{
    return (rand() % upper) + 1;
}

void Rat::CreateExplosion( vector3df position, bool massive )
{
    u32 soundSelect = (rand() % 5);
    if (soundSelect == 0)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom1.wav");
    }
    else if (soundSelect == 1)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom2.wav");
    }
    else if (soundSelect == 2)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom3.wav");
    }
    else if (soundSelect == 3)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom4.wav");
    }
    else
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom5.wav");
    }
    scene::ISceneNodeAnimator* anim = NULL;
    // create animation for explosion
    anim = smgr->createTextureAnimator((*explosionTextures), 100, false);
    scene::IBillboardSceneNode* bill;
    if (massive)
    {
        bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(600,600), position);
        
        soundEngine->play2D("media/sounds/weapons/rpg/Boom1.wav");
        soundEngine->play2D("media/sounds/weapons/rpg/Boom2.wav");
        soundEngine->play2D("media/sounds/weapons/rpg/Boom3.wav");
        soundEngine->play2D("media/sounds/weapons/rpg/Boom4.wav");
        soundEngine->play2D("media/sounds/weapons/rpg/Boom5.wav");
    }
    else 
    {
        bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(60,60), position);
    }
    // Setup the material
    bill->setMaterialFlag(video::EMF_LIGHTING, false);
    bill->setMaterialTexture(0, (*explosionTextures)[0]);
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animator
    bill->addAnimator(anim);
    anim->drop();
    // create deletion animator to automatically remove the billboard
    anim = smgr->createDeleteAnimator(100*10);

    bill->addAnimator(anim);
    anim->drop();
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
                if (roarTimer > 0)
                {
                    roarTimer -= time;
                }
                else
                {
                    u32 soundSelect = (rand() % 4);
                    if (soundSelect == 0)
                    {
                        soundEngine->play2D("media/sounds/rat/Roar1.wav");
                    }
                    else if (soundSelect == 1)
                    {
                        soundEngine->play2D("media/sounds/rat/Roar2.wav");
                    }
                    else if (soundSelect == 2)
                    {
                        soundEngine->play2D("media/sounds/rat/Roar3.wav");
                    }
                    else 
                    {
                        soundEngine->play2D("media/sounds/rat/Roar4.wav");
                    }
                    roarTimer = (rand() % 6000) + 750;
                }
                vector3df pos;
                this->pair->PhysxObject->getPosition(pos);
                pos.Y = 10.0f;
                
                distanceSinceFootstep += (pos - previousPosition).getLength();
                if (distanceSinceFootstep >= 45.0f)
                {
                    u32 soundSelect = (rand() % 7);
                    if (soundSelect == 0)
                    {
                        soundEngine->play2D("media/sounds/rat/1.wav");
                    }
                    else if (soundSelect == 1)
                    {
                        soundEngine->play2D("media/sounds/rat/2.wav");
                    }
                    else if (soundSelect == 2)
                    {
                        soundEngine->play2D("media/sounds/rat/3.wav");
                    }
                    else if (soundSelect == 3)
                    {
                        soundEngine->play2D("media/sounds/rat/4.wav");
                    }
                    else if (soundSelect == 4)
                    {
                        soundEngine->play2D("media/sounds/rat/5.wav");
                    }
                    else if (soundSelect == 5)
                    {
                        soundEngine->play2D("media/sounds/rat/6.wav");
                    }
                    else
                    {
                        soundEngine->play2D("media/sounds/rat/7.wav");
                    }   
                    distanceSinceFootstep = 0;
                }
                previousPosition = pos;
                
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
                if (this->target->ratKilled == false)
                {
                    soundEngine->play2D("media/sounds/rat/RoarFinal.ogg");
                }
                this->target->ratKilled = true;
            }
            endingExplosionTimer += time;
            if (endingExplosionTimer >= 250 )
            {
                IMeshBuffer* meshBuffer = mesh->getMeshBuffer(1);
                S3DVertex* S3vertices = reinterpret_cast<S3DVertex*>(meshBuffer->getVertices());
                s32 vertexChosen = GetRandom(meshBuffer->getVertexCount());
                std::cout << vertexChosen << std::endl;
                vector3df pos = S3vertices[vertexChosen].Pos + this->pair->SceneNode->getPosition();
                pos.Y += GetRandom(75);
                CreateExplosion(pos, false); 
                endingExplosionTimer = 0;   
                explosionPhase++;            
            }
            
            if (explosionPhase == 20)
            {
                CreateExplosion(pair->SceneNode->getPosition(), true);
                this->target->gameComplete = true;
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
}
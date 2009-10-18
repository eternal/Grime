#include "Game.h"
//#define WEAPONDEBUG 1

Game::Game(IrrlichtDevice* device, ISoundEngine* soundEngine, IPhysxManager* physxManager, EffectHandler* effect)
{
    blockFinalToggle = false;
    cleanupTimer = 0;
    updateTimer = 0;
    this->smgr = device->getSceneManager();
    this->soundEngine = soundEngine;
    this->physxManager = physxManager;
    this->effect = effect;
    this->guienv = device->getGUIEnvironment();
}

Game::~Game(void)
{
}

// Creates a sphere at the specified position, of the specified size and with the specified intial velocity (useful for throwing it)
SPhysxAndNodePair* Game::CreateSphere(const core::vector3df& pos, f32 radius, f32 density, core::vector3df* initialVelocity) 
{

    SPhysxAndNodePair* pair = new SPhysxAndNodePair;
    pair->PhysxObject = physxManager->createSphereObject(pos, core::vector3df(0,0,0), radius, density, initialVelocity);
    pair->SceneNode = smgr->addSphereSceneNode(radius, 12, 0, -1, pos, core::vector3df(0, 0, 0), core::vector3df(1.0f, 1.0f, 1.0f));
    //pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 
    return pair;
}

void Game::CreateCamera() 
{
    //WASD Keymap
    SKeyMap* keyMap = new SKeyMap[5];
    keyMap[0].Action = EKA_MOVE_FORWARD;
    keyMap[0].KeyCode = KEY_KEY_W;
    keyMap[1].Action = EKA_MOVE_BACKWARD;
    keyMap[1].KeyCode = KEY_KEY_S;
    keyMap[2].Action = EKA_STRAFE_LEFT;
    keyMap[2].KeyCode = KEY_KEY_A;
    keyMap[3].Action = EKA_STRAFE_RIGHT;
    keyMap[3].KeyCode = KEY_KEY_D;
    keyMap[4].Action = EKA_JUMP_UP;
    keyMap[4].KeyCode = KEY_SPACE;

    //replace the pair if it exists (shouldnt happen)
    if (cameraPair) delete cameraPair;
    cameraPair = new SPhysxAndCameraPair;
    //add the irrlicht fps camera scene node, modify the keymap to play with WASD
    cameraPair->camera = smgr->addCameraSceneNodeFPS(NULL, 50, 0.1f, -1, keyMap, 4, true);
    cameraPair->camera->setFOV(PI / 2.0f);
    cameraPair->camera->setPosition(startPosition);
    cameraPair->camera->setTarget(core::vector3df(0.0f,32.0f,5.0f));
    cameraPair->SceneNode = cameraPair->camera;
    vector3df physxStartPos = startPosition;
    physxStartPos.Y += 100.0f;
    cameraPair->PhysxObject = physxManager->createSphereObject(startPosition, core::vector3df(0,0,0), 15.0f, 90000.0f);
    cameraPair->PhysxObject->setAngularDamping(1000.0f); // Stops the sphere from rolling
    cameraPair->PreviousPosition = startPosition;
    cameraPair->CameraOffset = core::vector3df(0,10,0);
    cameraPair->camera->setFarValue(4000.0f);
    smgr->setActiveCamera(cameraPair->camera);
}

void Game::CreateExplosion( vector3df position )
{
    scene::ISceneNodeAnimator* anim = NULL;
    // create animation for explosion
    anim = smgr->createTextureAnimator(explosionTextures, 100, false);
    // create explosion billboard
    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(60,60), position);
    // Setup the material
    bill->setMaterialFlag(video::EMF_LIGHTING, false);
    bill->setMaterialTexture(0, explosionTextures[0]);
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animator
    bill->addAnimator(anim);
    anim->drop();
    // create deletion animator to automatically remove the billboard
    anim = smgr->createDeleteAnimator(100*10);

    bill->addAnimator(anim);
    anim->drop();
}

void Game::CreateImpactEffect( vector3df position, vector3df normal )
{
    core::vector3df scale(50,50,1);
    // create texture animation for the nodes
    scene::ISceneNodeAnimator* textureAnim = smgr->createTextureAnimator(impactTextures, 25, false);
    // create deletion animator to automatically remove the nodes
    scene::ISceneNodeAnimator* deleteAnim = smgr->createDeleteAnimator(25*6);

    // create impact quad
    scene::IMeshSceneNode* node = smgr->addMeshSceneNode(smgr->getMesh("media/quad.ms3d"));
    node->setPosition(position + normal * 0.25f);
    node->setScale(scale);
    // Rotate the quad to match that of the specified normal
    core::quaternion quat; 
    quat.rotationFromTo(core::vector3df(0,0,1), normal); 
    core::matrix4 mat; 
    quat.getMatrix(mat,vector3df(0,0,0)); 
    node->setRotation(mat.getRotationDegrees());
    // Set up the material
    node->setMaterialFlag(video::EMF_LIGHTING, false);
    node->setMaterialTexture(0, impactTextures[0]);
    node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animators
    node->addAnimator(textureAnim);
    node->addAnimator(deleteAnim);

    // create impact billboard
    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(NULL, core::dimension2d<f32>(scale.X,scale.Y), position);
    // Set up the material
    bill->setMaterialFlag(video::EMF_LIGHTING, false);
    bill->setMaterialTexture(0, impactTextures[0]);
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animators
    bill->addAnimator(textureAnim);
    bill->addAnimator(deleteAnim);
    textureAnim->drop();
    deleteAnim->drop();
}

void Game::CreateMuzzleFlash()
{
    vector3df position(5.0f,-3.5f,20.0f);
    scene::ISceneNodeAnimator* anim = NULL;
    // create animation for explosion
    anim = smgr->createTextureAnimator(explosionTextures, 10, false);
    // create explosion billboard

    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(smgr->getActiveCamera(), core::dimension2d<f32>(10,10), position);
    // Setup the material
    bill->setMaterialFlag(video::EMF_LIGHTING, false);
    bill->setMaterialTexture(0, explosionTextures[0]);
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animator
    bill->addAnimator(anim);
    anim->drop();
    // create deletion animator to automatically remove the billboard
    anim = smgr->createDeleteAnimator(10*10);

    bill->addAnimator(anim);
    anim->drop();
}
void Game::LoadLevel()
{    
    startPosition = vector3df(0.0f,30.1f,0.0f);
    this->CreateCamera();
    player = new Player(smgr, soundEngine, physxManager, cameraPair, effect);
    spawnManager = new SpawnManager(smgr, soundEngine, physxManager, &enemyObjects, &blockObjects, player);
    // Preload texture animators
    // just for prototype only
    c8 tmp[64];
    explosionTextures.clear();
    impactTextures.clear();
    for (s32 i = 1 ; i <= 10 ; ++i) 
    {
        sprintf_s(tmp, "media/explosion/%02d.jpg", i);
        explosionTextures.push_back(smgr->getVideoDriver()->getTexture(tmp));
    }
    for (s32 i = 1 ; i <= 6 ; ++i) 
    {
        sprintf_s(tmp, "media/impact/%02d.jpg", i);
        impactTextures.push_back(smgr->getVideoDriver()->getTexture(tmp));
    }
    //set gravity to a ridiculous amount due to scale
    core::vector3df gravity = vector3df(0.0f, -98.1f, 0.0f);
    physxManager->setGravity(gravity);

    //TODO: FIX PHYSICS MESHING BUG, BANDAID FIX BELOW BY SENDING FIRST SPAWN OFF MAP
    spawnManager->DIRTYMESHFIX();

    //set black clear colour
    effect->setClearColour(SColor(255, 0, 0, 0));
    effect->setAmbientColor(SColor(255, 32, 32, 32));

    //TODO: REDO LIGHTING, THIS WILL DO FOR PROTOTYPE
    ILightSceneNode* light = smgr->addLightSceneNode(0,vector3df(97, 150, 23),SColorf(0.3f, 0.3f, 0.3f, 0.5f),1600.0f);
    light->setLightType(ELT_POINT);
    ILightSceneNode* light2 = smgr->addLightSceneNode(0,vector3df(1229, 150, 26),SColorf(0.3f, 0.3f, 0.3f, 0.5f),1600.0f);
    light2->setLightType(ELT_POINT);
    ILightSceneNode* light3 = smgr->addLightSceneNode(0,vector3df(-1250, 150, 186),SColorf(0.3f, 0.3f, 0.3f, 0.5f),2000.0f);
    light3->setLightType(ELT_POINT);
    //check which shader language to use

//    const stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";

    effect->addPostProcessingEffectFromFile(core::stringc("media/shaders/BloomP.hlsl"));
    effect->addPostProcessingEffectFromFile(core::stringc("media/shaders/Toon.hlsl"));
    this->RebuildEnemies();
    this->RestartLevel();
}
void Game::Update( s32 time )
{
    if (player->health <= 0) 
    {
        this->RestartLevel();
    }
    cleanupTimer += time;
    if (cleanupTimer >= 15000)
    {
        this->CleanupArrays();
        cleanupTimer = 0;
    }
    cameraPair->updateTransformation();
    player->Update(time);
    spawnManager->Update(time);
    for (u32 i = 0; i < enemyObjects.size(); i++) 
    {
        try 
        {
            enemyObjects[i]->Update(time);
        }
        catch (...)
        {
            enemyObjects = this->RebuildEnemies();
            std::cerr << "Exception Handled in Game::Update. Enemies array rebuilt" << std::endl;
        }
    }
    for (u32 i = 0; i < projectileObjects.size(); i++)
    {
        try 
        {
            projectileObjects[i]->Update(time);
        }
        catch (...)
        {
            this->CleanupArrays();
            std::cerr << "Exception handled, arrays cleaned: enemies, projectiles" << std::endl;
        }
        
    }
    for (u32 i = 0; i < blockObjects.size(); ++i)
    {
        blockObjects[i]->Update(time);
    }
    
}

void Game::RestartLevel() {
    for (u32 i = 0; i < enemyObjects.size(); i++)
    {
        Enemy* enemy = enemyObjects[i];
        physxManager->removePhysxObject(enemy->pair->PhysxObject);
        enemy->pair->SceneNode->remove();
        delete enemy;
    }
    for (u32 i = 0; i < blockObjects.size(); i++)
    {
        Block* block = blockObjects[i];
        physxManager->removePhysxObject(block->pair->PhysxObject);
        block->pair->SceneNode->remove();
        delete block;
    }
    enemyObjects.clear();
    blockObjects.clear();

    player->pair->PhysxObject->setPosition(startPosition);
    player->pair->updateTransformation();

    spawnManager->timeBetweenSpawns = 2500;
    player->health = 100;
}
void Game::WeaponCloseRaycast(core::line3df line)
{
    core::array<SRaycastHitData> rayArray = physxManager->raycastAllRigidObjects(line);
    for (u32 i = 0; i < rayArray.size(); i++)
    {
        SRaycastHitData ray = rayArray[i];
        if (ray.Object->getType() != EOT_SPHERE && ray.Object->getType() != EOT_TRIANGLE_MESH)
        {   
            vector3df currentVelocity;
            ray.Object->getLinearVelocity(currentVelocity);
            ray.Object->setLinearVelocity(currentVelocity + ((line.end - line.start).normalize() * 600.0f));
        }
    }
}

void Game::WeaponFire(s32 weapon)
{
    if ((player->weaponCooldown[weapon]) <= 0)
    {
        player->AddCoolDown(weapon);
        if (weapon == WEAPON_BLOCKGUN)
        {
            if (player->weaponAmmunition[WEAPON_BLOCKGUN] > 0)
            {
                Block* block = new Block(smgr, physxManager, &enemyObjects, &blockObjects);
                blockObjects.push_back(block);
            }
            player->weaponAmmunition[WEAPON_BLOCKGUN]--;
        }
        else if (player->GetWeapon() == WEAPON_PISTOL)
        {
            // Perform a raycast to find the objects we just shot at
            core::line3df line;
            line.start = cameraPair->camera->getPosition();
            //put the end of the line off in the distance
            line.end = line.start + (cameraPair->camera->getTarget() - line.start).normalize() * 5000.0f;
            //access the physics engine to find the intersection point
            core::array<SRaycastHitData> rayArray = physxManager->raycastAllRigidObjects(line);
            //core::array<SRaycastHitData> filteredRayArray;
            SRaycastHitData closestObject;
            if (rayArray.size() > 0)
            {
                closestObject = rayArray[0];
                for (u32 i = 0; i < rayArray.size(); ++i) 
                {
                    SRaycastHitData ray = rayArray[i];
                    if (ray.Object->getType() != EOT_SPHERE)
                    {
                        f32 dis = (ray.HitPosition - line.start).getLength();
                        f32 dis2 = (closestObject.HitPosition - line.start).getLength();
    #ifdef WEAPONDEBUG
                        std::cout << "Test Distance: " << dis << std::endl;
                        std::cout << "Current Closest: " << dis2 << std::endl;
    #endif
                        if (dis < dis2) 
                        {
                            closestObject = ray;
                        }
                    }
                }
            }
    #ifdef WEAPONDEBUG            
            std::cout << "===========" << std::endl;
    #endif            
            IPhysxObject* objectHit = closestObject.Object;
            //IPhysxObject* objectHit = physxManager->raycastClosestObject(line, &intersection, &normal);
            if (objectHit) 
            {
                //check for collisions with any of the types of bounding boxes the enemies use
                //TO REFACTOR, PLACE IN GAME CLASS
                if (objectHit->getType() == EOT_CONVEX_MESH || objectHit->getType() == EOT_BOX || objectHit->getType() == EOT_SPHERE) 
                {
                    for (u32 i = 0 ; i < enemyObjects.size() ; ++i) 
                    {  // check it against the objects in our array to see if it matches
                        if (enemyObjects[i]->pair->PhysxObject == objectHit) 
                        {
                            Enemy* enemy = enemyObjects[i];
                            //add small pushback and texture explosion
                            s32 chance = GetRandom(4);
                            if (chance == 4) // 1/4 chance
                            {
                                enemy->health--;
                            }
                            else
                            {
                                enemy->health -= 0.5;
                            }

                            if (enemy->IsStillAlive())
                            {
                                CreateImpactEffect(closestObject.HitPosition,closestObject.SurfaceNormal);
                                std::cout << enemy->health << " health left" << std::endl;
                            }
                            else
                            {
                                CreateExplosion(closestObject.HitPosition);   
                                //kill enemy
                                //remove from arrays and memory
                                try 
                                {
                                    physxManager->removePhysxObject(enemy->pair->PhysxObject);
                                    enemy->pair->SceneNode->remove();
                                    delete enemy;
                                    enemyObjects.erase(i);  
                                }                       
                                catch (...)
                                {
                                    std::cout << "Automatic removal failure" << std::endl;
                                    try 
                                    {
                                        enemyObjects.erase(i); //preserve arrays
                                        enemy->active = false;
                                        enemy->pair->SceneNode->setVisible(false);
                                        enemy->pair->SceneNode->remove();
                                    }
                                    catch (...)
                                    {
                                        std::cout << "Recovery failure. Adverse effects may be experienced." << std::endl;
                                    }
                                }
                            }
                            break;
                        }
                    }	
                }                    
                //for any other objects except for the camera
                else if (objectHit != cameraPair->PhysxObject) 
                {
                    objectHit->setLinearVelocity(line.getVector().normalize() * 30.0f);
                    CreateImpactEffect(closestObject.HitPosition, closestObject.SurfaceNormal);
                    //physxManager->createExplosion(closestObject.HitPosition, 100.0f, 300000000.0f, 100000000000.0f, 0.2f);
                }
            }
        }
        else if (player->GetWeapon() == WEAPON_RPG)
        {
            core::vector3df t = cameraPair->camera->getPosition();
            core::vector3df vel = cameraPair->camera->getTarget() - cameraPair->camera->getPosition();
            vel.normalize();
            t = t + vel * 20.0f;
            vel *= 800.0f;

            projectileObjects.push_back(new Projectile(smgr,soundEngine,physxManager, CreateSphere(t, 10.0f, 10000.0f, &vel), &projectileObjects, explosionTextures, &enemyObjects));
        }
        else if (player->GetWeapon() == WEAPON_CLOSE)
        {
            //physxManager->createExplosion(player->pair->SceneNode->getAbsolutePosition(), 100.0f, 300000000.0f, 100000000000.0f, 1.0f);
            core::line3df line;
            line.start = cameraPair->camera->getPosition();
            //put the end of the line off in the distance
            line.end = line.start + (cameraPair->camera->getTarget() - line.start).normalize() * 100.0f;
            //access the physics engine to find the intersection point
            vector3df origin = line.end;
            //simulate spray;
            for (s32 i = -50; i < 50; i += 25)
            {
                for (s32 j = -50; j < 50; j += 25)
                {
                    for (s32 k = -50; k < 50; k+= 25)
                    {
                        line.end = origin;
                        line.end.X += i;
                        line.end.Y += j;
                        line.end.Z += k;
                        WeaponCloseRaycast(line);
                    }
                }
            }
        }
    }
}
void Game::WeaponFire()
{
    if (!(player->CurrentWeaponOnCooldown()))
    {
        //player->AddCoolDown();
        CreateMuzzleFlash();
#ifdef WEAPONDEBUG
        std::cout << "Fire" << std::endl;
#endif
        if (player->GetWeapon() == WEAPON_BLOCKGUN)
        {
            WeaponFire(WEAPON_BLOCKGUN);
        }
        else if (player->GetWeapon() == WEAPON_PISTOL)
        {
            WeaponFire(WEAPON_PISTOL);
        }
        else if (player->GetWeapon() == WEAPON_RPG)
        {
            WeaponFire(WEAPON_RPG);
        }
        else if (player->GetWeapon() == WEAPON_CLOSE)
        {
            WeaponFire(WEAPON_CLOSE);
        }
    }
}
void Game::ConvertBlocks() 
{
    //convert blox to physxblocks
    for (u32 i = 0; i < blockObjects.size(); ++i)
    {
        Block* block = blockObjects[i];
        if (blockFinalToggle)
        {
            block->ConvertToDynamic();
        }
        else 
        {
            block->ConvertToStatic();
        }
        
    }
    blockFinalToggle = !blockFinalToggle;
    //spawn rat
    //spawnManager->FinalWave();
}
//simple random number function
s32 Game::GetRandom(s32 upper) 
{
    return (rand() % upper) + 1;
}
core::array<Projectile*> Game::RebuildProjectiles()
{
    core::array<Projectile*> projectileRebuild;
    for (u32 i = 0; i < projectileObjects.size(); i++)
    {
        try
        {
            Projectile* projectile = projectileObjects[i];
            projectile->pair->PhysxObject->getType();
            projectileRebuild.push_back(projectile);
        }
        catch (...)
        {
            std::cout << "Physx object corrupted or missing: cleaning projectile from array" << std::endl;	
        }
    }
    std::cout << "Projectile array rebuilt." << std::endl;
    return projectileRebuild;
}
core::array<Enemy*> Game::RebuildEnemies()
{
    core::array<Enemy*> enemiesRebuild;
    for (u32 i = 0; i < enemyObjects.size(); ++i)
    {
        try
        {
            Enemy* enemy = enemyObjects[i];
            enemy->pair->PhysxObject->getType(); //throwaway function to check enemy works
            enemiesRebuild.push_back(enemy);
        }
        catch (...)
        {
            std::cout << "Physx object corrupted or missing: cleaning enemy from array" << std::endl;
        }
    }
    std::cout << "Enemy array rebuilt." << std::endl;
    return enemiesRebuild;
}

void Game::CleanupArrays()
{
    core::array<Enemy*> enemies;
    enemies = this->RebuildEnemies();
    this->enemyObjects.clear();
    this->enemyObjects = enemies;
    core::array<Projectile*> projectiles;
    projectiles = this->RebuildProjectiles();
    this->projectileObjects.clear();
    this->projectileObjects = projectiles;
}
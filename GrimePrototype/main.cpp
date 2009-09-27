//=============================================================================
//  Grime Prototype main.cpp
//  build 05291057
//=============================================================================
#define DEBUG 1
//#define FULLSCREEN 1
#define NOSPAWN 1
//#define OLDSMGR 1 
#include <irrlicht.h>
#include <iostream>
#include <IrrPhysx.h>
#include <cstdlib>
#include <ctime> 
#include "XEffects.h"
#include <NxPhysics.h>
#include <irrKlang.h>
#include "irrKlangSceneNode.h"
#include "Game.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace IrrPhysx;

//add libraries
#ifdef _MSC_VER
#pragma comment(lib,"Irrlicht.lib")
#pragma comment(lib,"irrKlang.lib")
#endif
#ifndef DEBUG
//get rid of the console window, found on the msdn forums somewhere
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:main")
#endif


//main globals for devices etc
IrrlichtDevice* device = NULL;
video::IVideoDriver* driver = NULL;
scene::ISceneManager* smgr = NULL;
gui::IGUIEnvironment* guienv = NULL;
IPhysxManager* physxManager = NULL;
EffectHandler* effect = NULL;
irrklang::ISoundEngine* soundEngine = NULL;
ISceneNode* roomnode = NULL;
Game* game = NULL;

// Creates a sphere at the specified position, of the specified size and with the specified intial velocity (useful for throwing it)
SPhysxAndNodePair* createSphere(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, const core::vector3df& pos, f32 radius, f32 density, core::vector3df* initialVelocity) {

    SPhysxAndNodePair* pair = new SPhysxAndNodePair;
    pair->PhysxObject = physxManager->createSphereObject(pos, core::vector3df(0,0,0), radius, density, initialVelocity);
    pair->SceneNode = smgr->addSphereSceneNode(radius, 12, 0, -1, pos, core::vector3df(0, 0, 0), core::vector3df(1.0f, 1.0f, 1.0f));
    //pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
    pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 

    return pair;

}
//simple function for adding a camera and creating a physics pair for the camera


//Event reciever class
//Handle keyboard and mouse input
//TODO: Move into own class file
class CEventReceiver : public IEventReceiver 
{
public:
    bool OnEvent(const SEvent& event) {       
        if (event.EventType == EET_KEY_INPUT_EVENT) {
     
            switch (event.KeyInput.Key) {
#ifdef DEBUG   
                case KEY_ADD:   
                    game->spawnManager->timeBetweenSpawns+= 100;
                    break;
                case KEY_SUBTRACT:
                    game->spawnManager->timeBetweenSpawns-= 100;
                    break;
#endif //DEBUG
                case KEY_SPACE:
                    //jump
                    if (game->cameraPair->camera)
                    {
                        // Perform a raycast to find the objects we just shot at
                        core::line3df line;
                        line.start = game->cameraPair->camera->getPosition();
                        line.end = line.start;
                        line.end.Y = -2000.0f;
                        //access the physics engine to find the intersection point
                        core::array<SRaycastHitData> rayArray = physxManager->raycastAllRigidObjects(line);
                        for (u32 i = 0; i < rayArray.size(); ++i) {
                            SRaycastHitData ray = rayArray[i];
                            if (ray.Object->getType() == EOT_TRIANGLE_MESH)
                            {
                                f32 dis = (ray.HitPosition - line.start).getLength();
#ifdef DEBUG                                
                                std::cout << "Distance:" << dis << std::endl;
#endif //DEBUG
                                if (dis <= 25)
                                {
                                    game->cameraPair->PhysxObject->setLinearVelocity(core::vector3df(0,70,0));
                                }
                            }
                        }
                    }

                    break;
            }
            //if the key is held down, return and ignore
            if (event.KeyInput.PressedDown) return false;

            switch (event.KeyInput.Key) {
                case KEY_F9:
                    //screenshot                  
                    driver->writeImageToFile(driver->createScreenShot(),"screenie.jpg",0);
                    break;
#ifdef DEBUG
                //case KEY_KEY_F:
                //    //debug spawn enemy function
                //    SpawnSpider();
                //    std::cout << "Enemy count" << enemyObjects.size() << std::endl;
                //    break;
                case KEY_KEY_U:
                    game->CreateMuzzleFlash();
                break;
#endif //DEBUG
                case KEY_KEY_G: {
                    Block* block = new Block(smgr, physxManager, &game->enemyObjects);
                    game->blockObjects.push_back(block);
                 }
                 break;               
#ifdef DEBUG                
                case KEY_KEY_1: {
                    game->spawnManager->SpawnCockroach(vector3df(0.0f,100.0f,0.0f));
                    }
                    break;
                case KEY_KEY_2: {
                    game->spawnManager->SpawnSpider(vector3df(0.0f,100.0f,0.0f));
                    }
                    break;
                case KEY_KEY_3: {
                    game->spawnManager->SpawnBeetle(vector3df(0.0f,100.0f,0.0f));
                    }
                    break;
                case KEY_KEY_4: {
                    game->spawnManager->SpawnRat(vector3df(0.0f,100.0f,0.0f));
                    }
                    break; 
                case KEY_KEY_8: 
                {
                    game->spawnManager->SpawnSpider(game->spawnManager->positionOne);
                }
                break;
                case KEY_KEY_9: 
                {
                    game->spawnManager->SpawnSpider(game->spawnManager->positionTwo);
                }
                break;
                case KEY_KEY_0: 
                {
                    game->spawnManager->SpawnSpider(game->spawnManager->positionThree);
                }
                break;
                //case KEY_KEY_C:
                //    // DEBUG: Delete all objects
                //    for (u32 i = 0 ; i < objects.size() ; i++) {
                //        SPhysxAndNodePair* pair = objects[i];
                //        physxManager->removePhysxObject(pair->PhysxObject);
                //        pair->SceneNode->remove();
                //        delete pair;
                //        }
                //    for (u32 i = 0; i < enemyObjects.size(); i++)
                //    {
                //        Enemy* enemy = enemyObjects[i];
                //        physxManager->removePhysxObject(enemy->pair->PhysxObject);
                //        enemy->pair->SceneNode->remove();
                //        delete enemy;
                //    }
                //    for (u32 i = 0; i < projectileObjects.size(); i++)
                //    {
                //        Projectile* proj = projectileObjects[i];
                //        physxManager->removePhysxObject(proj->pair->PhysxObject);
                //        proj->pair->SceneNode->remove();
                //        delete proj;
                //    }
                //    enemyObjects.clear();
                //    objects.clear();
                //    projectileObjects.clear();
                //    break;
                case KEY_KEY_I:
                    game->RestartLevel();
                    break;
                case KEY_KEY_V:
                    // Toggle the debug data visibility
                    physxManager->setDebugDataVisible(!physxManager->isDebugDataVisible());
                    break;
                /*case KEY_KEY_P:
                    effect->addEffectToNode(roomnode,(E_EFFECT_TYPE)currentShader);
                    currentShader++;
                    if (currentShader == 7)
                        currentShader = 0;
                    break;
                case KEY_KEY_O:
                    effectToggle = !effectToggle;
                    if (effectToggle) toonPPEffect = effect->addPostProcessingEffectFromFile("Shaders/toon.hlsl");
                    else effect->removePostProcessingEffect(toonPPEffect);
                    break;*/
                //case KEY_KEY_L:
                //    bloomToggle = !bloomToggle;
                //    if (bloomToggle)
                //    {
                //        effectBloom = effect->addPostProcessingEffectFromFile("Shaders/BloomP.hlsl");
                //        //effectBlur1 = effect->addPostProcessingEffectFromFile("Shaders/BlurHP.hlsl");
                //        effectBlur2 = effect->addPostProcessingEffectFromFile("Shaders/BlurVP.hlsl");
                //        toonPPEffect = effect->addPostProcessingEffectFromFile("Shaders/toon.hlsl");
                //        //effectHighPass = effect->addPostProcessingEffectFromFile("Shaders/BrightPass.hlsl");
                //    }
                //    else {
                //        effect->removePostProcessingEffect(effectBloom);
                //        effect->removePostProcessingEffect(effectBlur1);
                //        effect->removePostProcessingEffect(effectBlur2);
                //        //effect->removePostProcessingEffect(effectHighPass);
                //        //effect->removePostProcessingEffect(toonPPEffect);
                //    }
                //    break;
                case KEY_MULTIPLY:
                    game->spawnManager->timeBetweenSpawns = 100;
                    break;
                case KEY_DIVIDE:
                    game->spawnManager->timeBetweenSpawns = 2500;
                    break;
                    
#endif //DEBUG
                case KEY_ESCAPE:
                    //close the device and exit
                    device->closeDevice();
                    break;
            }
        } 
        else if (event.EventType == EET_MOUSE_INPUT_EVENT) {
            bool mouseLeftPressedDown = false;
            bool mouseRightPressedDown = false;
            if (event.MouseInput.isLeftPressed())
            {
                if (!(game->player->CurrentWeaponOnCooldown()))
                {
                    game->CreateMuzzleFlash();
                    game->player->AddCoolDown();
                    std::cout << "Fire" << std::endl;
                    if (game->player->GetWeapon() == WEAPON_BLOCKGUN)
                    {
                        Block* block = new Block(smgr, physxManager, &game->enemyObjects);
                        game->blockObjects.push_back(block);
                    }
                    else if (game->player->GetWeapon() == WEAPON_PISTOL)
                    {
                        // Perform a raycast to find the objects we just shot at
                        core::line3df line;
                        line.start = game->cameraPair->camera->getPosition();
                        //put the end of the line off in the distance
                        line.end = line.start + (game->cameraPair->camera->getTarget() - line.start).normalize() * 5000.0f;
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
                                    std::cout << "Test Distance: " << dis << std::endl;
                                    std::cout << "Current Closest: " << dis2 << std::endl;
                                    if (dis < dis2) {
                                        closestObject = ray;
                                    }
                                }
                            }
                        }
                        std::cout << "===========" << std::endl;
                        IPhysxObject* objectHit = closestObject.Object;
                        //IPhysxObject* objectHit = physxManager->raycastClosestObject(line, &intersection, &normal);
                        if (objectHit) {
                            //check for collisions with any of the types of bounding boxes the enemies use
                            //TO REFACTOR, PLACE IN GAME CLASS
                            if (objectHit->getType() == EOT_CONVEX_MESH || objectHit->getType() == EOT_BOX || objectHit->getType() == EOT_SPHERE) 
                            {
                                for (u32 i = 0 ; i < game->enemyObjects.size() ; ++i) 
                                {  // check it against the objects in our array to see if it matches
                                    if (game->enemyObjects[i]->pair->PhysxObject == objectHit) {
                                        Enemy* enemy = game->enemyObjects[i];
                                        //add small pushback and texture explosion
                                        s32 chance = game->GetRandom(4);
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
                                            game->CreateImpactEffect(closestObject.HitPosition,closestObject.SurfaceNormal);
                                            std::cout << enemy->health << " health left" << std::endl;
                                        }
                                        else
                                        {
                                            game->CreateExplosion(closestObject.HitPosition);   
                                            //kill enemy
                                            //remove from arrays and memory                         
                                            physxManager->removePhysxObject(enemy->pair->PhysxObject);
                                            enemy->pair->SceneNode->remove();
                                            delete enemy;
                                            game->enemyObjects.erase(i);  
                                        }
                                        break;
                                    }
                                }	
                            }  
    #ifdef DEBUG                    
                            //// If it's a sphere we blow it up
                            //else if (objectHit->getType() == EOT_SPHERE) 
                            //{
                            //    for (u32 i = 0 ; i < objects.size() ; ++i) 
                            //    {  // check it against the objects in our array to see if it matches
                            //        if (objects[i]->PhysxObject == objectHit) {
                            //            core::vector3df pos;
                            //            objects[i]->PhysxObject->getPosition(pos);
                            //            createExplosion(pos);
                            //            objectHit->setLinearVelocity(line.getVector().normalize() * 600.0f);
                            //            //physxManager->removePhysxObject(objects[i]->PhysxObject);
                            //            //objects[i]->SceneNode->remove();
                            //            //delete objects[i];
                            //            //objects.erase(i);
                            //            break;
                            //        }
                            //    }	
                            //}
                            //for any other objects except for the camera
                            else if (objectHit != game->cameraPair->PhysxObject) 
                            {
                                objectHit->setLinearVelocity(line.getVector().normalize() * 30.0f);
                                game->CreateImpactEffect(closestObject.HitPosition, closestObject.SurfaceNormal);
                                //physxManager->createExplosion(closestObject.HitPosition, 100.0f, 300000000.0f, 100000000000.0f, 0.2f);
                            }
                        }
                    }
                    else if (game->player->GetWeapon() == WEAPON_RPG)
                    {
                        core::vector3df t = game->cameraPair->camera->getPosition();
                        core::vector3df vel = game->cameraPair->camera->getTarget() - game->cameraPair->camera->getPosition();
                        vel.normalize();
                        t = t + vel * 20.0f;
                        vel*=1000.0f;

                        game->projectileObjects.push_back(new Projectile(smgr,soundEngine,physxManager, createSphere(physxManager, smgr, driver, t, 10.0f, 1000.0f, &vel), &game->projectileObjects, game->explosionTextures, &game->enemyObjects));
                    }
                    else if (game->player->GetWeapon() == WEAPON_CLOSE)
                    {
                        physxManager->createExplosion(game->player->pair->SceneNode->getAbsolutePosition(), 100.0f, 300000000.0f, 100000000000.0f, 1.0f);
                    }
                }
                
            }
            switch (event.MouseInput.Event) {
                case EMIE_MOUSE_WHEEL: {
                    game->player->WeaponSelect(event.MouseInput.Wheel);
                }
                break;
                //single instance effect
                case EMIE_LMOUSE_PRESSED_DOWN: {
                    
                        
#endif //DEBUG                     
                    
                }
                break;
                case EMIE_LMOUSE_LEFT_UP: {
                    mouseLeftPressedDown = false;
                }
                break;
#ifdef DEBUG                
                case EMIE_RMOUSE_PRESSED_DOWN: {
                    mouseRightPressedDown = true;
                    // Throw a sphere from the camera
                    //objects.push_back(createSphereFromCamera(physxManager, smgr, driver, camera, 4, 20.0f));
                }
                break;
                case EMIE_RMOUSE_LEFT_UP: {
                    mouseRightPressedDown = false;
                    break;
                }
#endif // DEBUG
            }          
        } 
        else if (event.EventType == EET_GUI_EVENT) 
        {
            //gui events
            return false;
        }
        return false;
    }
};

int main() {
	video::E_DRIVER_TYPE driverType;
    //driverType = video::EDT_OPENGL; //USING OPENGL EXCLUSIVELY FOR PROTOTYPE DUE TO BETTER IRRLICHT LIGHTING SUPPORT
	driverType = video::EDT_DIRECT3D9;
	//event receiver instance
    CEventReceiver receiver;
    //explicitly choose screen res
    core::dimension2du resolution(1280,800);
    // create device and exit if creation failed
#ifndef FULLSCREEN    
	device = createDevice(driverType, resolution, 32, false, false, false, &receiver);
#else
    device = createDevice(driverType, resolution, 32, true, false, false, &receiver);	
#endif
	if (device == 0)
		return 1; // could not create selected driver.
    
    //instantiate necessary globals
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    soundEngine  = irrklang::createIrrKlangDevice();
    
    //seed random numbers
    srand((unsigned)time(0));
    
    //soundEngine->setSoundVolume(1.0f);
    if (!soundEngine || !driver || !smgr || !guienv)
        return 1; //fatal error
    
	//set screen buffer resolution
	dimension2du ScreenRTT = !driver->getVendorInfo().equals_ignore_case("NVIDIA Corporation") ? dimension2du(1024, 1024) : driver->getScreenSize();

	//effects handler
	effect = new EffectHandler(device, "shaders", ScreenRTT, true);

    // Set some Physx scene description parameters to optimise raycasting
    SSceneDesc sceneDesc;
    // Set the bounding box of the physx "scene"
    sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-2300,-200,-2000), core::vector3df(2000,800,2000));
    sceneDesc.BoundPlanes = true;
    // Y axis up
    sceneDesc.UpAxis = 1;
    //create physx manager for scene details
    physxManager  = createPhysxManager(device, sceneDesc);
    
    game = new Game(smgr, soundEngine, physxManager, effect);
    //create room mesh from asset
	IMesh* room = smgr->getMesh("media/level/kitchen retex final.b3d")->getMesh(0);
	
	//IMesh* room = smgr->getMesh("media/temp.b3d")->getMesh(0);
	//create scene node for mesh and place
	roomnode = smgr->addMeshSceneNode(room,0,-1,vector3df(0,0,0),vector3df(0,0,0));
	//roomnode->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);
	
	//scale to proper size
	vector3df roomScale(1200.0f,1200.0f,1200.0f);
	roomnode->setScale(vector3df(roomScale));
	for (u32 i = 0; i < roomnode->getMaterialCount(); i++) {
	    roomnode->getMaterial(i).Lighting = true;
	}
	
	
	
	//normalize mesh's normals as it has been scaled
	roomnode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    //effect->addShadowToNode(roomnode, EFT_NONE, ESM_BOTH);
    //effect->addNodeToDepthPass(roomnode);
    //effect->addEffectToNode(roomnode,(E_EFFECT_TYPE)0);
    //effect->addEffectToNode(roomnode, (E_EFFECT_TYPE)0);
#ifdef DEBUG	
	scene::ITriangleSelector* selector = 0;
    selector = smgr->createTriangleSelector(room,roomnode);

    printf("%d\n",room->getMeshBufferCount());	
#endif //DEBUG   
    //get the mesh buffers of the mesh and create physics representation
    for (u32 i = 0 ; i < room->getMeshBufferCount(); i++) {
        //first calculate the mesh triangles and make physx object
        //IPhysxMesh* triMesh = physxManager->createTriangleMesh(room->getMeshBuffer(i), vector3df(1.0f,1.0f,1.0f));
        IPhysxMesh* triMesh = physxManager->createTriangleMesh(room->getMeshBuffer(i), roomScale);
        //secondly add the object to the world
        physxManager->createTriangleMeshObject(triMesh,vector3df(0.0f,0.0f,0.0f));
    }
    //remove cursor from view
	device->getCursorControl()->setVisible(false);
#ifdef DEBUG
    video::SMaterial lineMaterial;
    lineMaterial.Lighting = false;
    lineMaterial.Thickness = 2.0f;
    
    video::SMaterial triMaterial;
    triMaterial.Lighting = false;
#endif //DEBUG
    //add crosshair to centre of screen (64x64 image so -32)
    guienv->addImage(driver->getTexture("media/crosshair2.png"), core::position2di(resolution.Width/2-32,resolution.Height/2-32));
    
    //grab bitmap font
    guienv->getSkin()->setFont(guienv->getFont("media/GillSans12.png"));
    gui::IGUIStaticText* levelText = guienv->addStaticText(L"Grime Prototype: Kitchen", core::rect<s32>(5,2,200,200));
    levelText->setOverrideColor(video::SColor(255,255,255,255));
    gui::IGUIStaticText* buildText = guienv->addStaticText(L"Build: 200909170234", core::rect<s32>(5,20,200,200));
    buildText->setOverrideColor(video::SColor(255,255,255,255));  

    gui::IGUIStaticText* generalText = guienv->addStaticText(L"Time Between Spawns: ", core::rect<s32>(5,38,400,200));
    gui::IGUIStaticText* spawns = guienv->addStaticText(L"2500", core::rect<s32>(164,38,400,200));
    generalText->setOverrideColor(video::SColor(255,255,255,255));
    spawns->setOverrideColor(video::SColor(255,255,255,255));
    gui::IGUIStaticText* textHealth = guienv->addStaticText(L"Health: ", core::rect<s32>(5,58,400,200));
    gui::IGUIStaticText* health = guienv->addStaticText(L"100", core::rect<s32>(75,58,400,200));
    textHealth->setOverrideColor(video::SColor(255,255,255,255));
    health->setOverrideColor(video::SColor(255,255,255,255));
    
    gui::IGUIStaticText* textCooldown = guienv->addStaticText(L"Cooldown: ", rect<s32>(5,578,400,800));
    gui::IGUIStaticText* cooldown = guienv->addStaticText(L"Cooldown", rect<s32>(105,578,400,800));
    textCooldown->setOverrideColor(SColor(255,255,255,255));
    cooldown->setOverrideColor(SColor(255,255,255,255));
    
    gui::IGUIStaticText* textPosition = guienv->addStaticText(L"Position: ", rect<s32>(5,598,400,800));
    gui::IGUIStaticText* textPosition2 = guienv->addStaticText(L"Position", rect<s32>(105,598,400,800));
    textPosition->setOverrideColor(SColor(255,255,255,255));
    textPosition2->setOverrideColor(SColor(255,255,255,255));
    
    gui::IGUIStaticText* textPrimitives = guienv->addStaticText(L"Primitives Drawn: ", rect<s32>(5, 618, 400, 800));
    textPrimitives->setOverrideColor(SColor(255,255,255,255));
    
    //set gravity to a ridiculous amount due to scale
    core::vector3df gravity = vector3df(0.0f, -98.1f, 0.0f);
    physxManager->setGravity(gravity);
#ifdef DEBUG    
    std::cout << gravity.X << " " << gravity.Y << " " << gravity.Z << std::endl;
#endif //DEBUG  
    
//TODO: FIX PHYSICS MESHING BUG, BANDAID FIX BELOW BY SENDING FIRST SPAWN OFF MAP
    game->spawnManager->DIRTYMESHFIX();
    
    //set black clear colour
    effect->setClearColour(SColor(255, 0, 0, 0));
    effect->setAmbientColor(SColor(255, 32, 32, 32));
    //smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
    //TODO: REDO LIGHTING, THIS WILL DO FOR PROTOTYPE
    ILightSceneNode* light = smgr->addLightSceneNode(0,vector3df(97, 150, 23),SColorf(0.3f, 0.3f, 0.3f, 0.5f),1600.0f);
    light->setLightType(ELT_POINT);
    ILightSceneNode* light2 = smgr->addLightSceneNode(0,vector3df(1229, 150, 26),SColorf(0.3f, 0.3f, 0.3f, 0.5f),1600.0f);
    light2->setLightType(ELT_POINT);
    ILightSceneNode* light3 = smgr->addLightSceneNode(0,vector3df(-1250, 150, 186),SColorf(0.3f, 0.3f, 0.3f, 0.5f),2000.0f);
    light3->setLightType(ELT_POINT);
    //check which shader language to use
    
	const stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";

	effect->addPostProcessingEffectFromFile(core::stringc("shaders/BloomP.hlsl"));
	effect->addPostProcessingEffectFromFile(core::stringc("shaders/Toon.hlsl"));
    game->RestartLevel();
	
#ifdef SMGRDEBUG
	scene::ISceneNode* selectedSceneNode =0;
	scene::ISceneNode* lastSelectedSceneNode =0;
#endif //SMGRDEBUG
    //set pre-loop data
	int lastFPS = -1;
    s32 lastTime = device->getTimer()->getTime();
	while(device->run())
	{
		if (device->isWindowActive())
		{
		    //find time between renders
            s32 timeNow = device->getTimer()->getTime();
            //std::cout << timeNow << std::endl;
            s32 elapsedTime = timeNow - lastTime;
            lastTime = timeNow;
                    
            if (game->player->health <= 0) {
                game->RestartLevel();
            }
            //simulate physics
		    physxManager->simulate(elapsedTime/1000.0f);
		    //REMEMBER: DO NOT MODIFY PHYSX OBJECTS WHILE SIMULATING
		    physxManager->fetchResults();
            
		    game->Update(elapsedTime);
            //start drawing
			driver->beginScene(true, true, video::SColor(255,200,200,200));
#ifndef OLDSMGR			
			effect->update();
#endif //OLDSMGR		
#ifdef OLDSMGR //not required with the use of XEffects	
			smgr->drawAll();
#endif //OLDSMGR
#ifdef DEBUG //physx debug data to show bounding boxes   
            physxManager->renderDebugData(video::SColor(225,255,255,255));            
#endif // DEBUG
            core::stringw strTime, strHealth, strCooldown, strPosition, strPrimitives = "";
            strTime += game->spawnManager->timeBetweenSpawns;
            strHealth += game->player->health;
            strCooldown += game->player->CurrentCooldown();
            strPosition += "X: "; 
            strPosition += game->cameraPair->camera->getAbsolutePosition().X;
            strPosition += " Y: ";
            strPosition += game->cameraPair->camera->getAbsolutePosition().Y; 
            strPosition += " Z: "; 
            strPosition += game->cameraPair->camera->getAbsolutePosition().Z;
            strPrimitives += "Primitives Count: ";
            strPrimitives += driver->getPrimitiveCountDrawn();
            strPrimitives += " PhysxObjects Count: ";
            strPrimitives += physxManager->getNumPhysxObjects();
            //_itow(player->health, temp2, 10);
            spawns->setText(strTime.c_str());
            health->setText(strHealth.c_str());
            cooldown->setText(strCooldown.c_str());
            textPosition2->setText(strPosition.c_str());
            textPrimitives->setText(strPrimitives.c_str());
            guienv->drawAll();

#ifdef PHYSDEBUG
            //find normal between target and raycasted object
			core::vector3df normal;
            IPhysxObject* objectHit = physxManager->raycastClosestObject(line, &intersection, &normal);
            if (objectHit) {
                // Draw a 3D line showing the normal of the surface hit
                core::vector3df lineEnd = intersection + normal * 10.0f;
                driver->setTransform(video::ETS_WORLD, core::matrix4());
                driver->setMaterial(lineMaterial);
                driver->draw3DLine(intersection, lineEnd, video::SColor(255,0,0,255));
                }
#endif //PHYSDEBUG
#ifdef SMGRDEBUG
            //old smgr code for lighting node that is under cursor; debug purposes only
			selectedSceneNode =
				smgr->getSceneCollisionManager()->getSceneNodeFromCameraBB(camera);

			if (lastSelectedSceneNode)
				lastSelectedSceneNode->setMaterialFlag(video::EMF_LIGHTING, true);

			if (selectedSceneNode == roomnode || selectedSceneNode == bill)
				selectedSceneNode = 0;

			if (selectedSceneNode)
				selectedSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

			lastSelectedSceneNode = selectedSceneNode;
#endif //SMGRDEBUG
            //done rendering
			driver->endScene();

			int fps = driver->getFPS();

			if (lastFPS != fps)
			{
				core::stringw str = L"Grime Prototype [";
				str += driver->getName();
				str += "] FPS:";
				str += fps;

				device->setWindowCaption(str.c_str());
				lastFPS = fps;
			}
		}
		else
			device->yield();
	}
#ifdef DEBUG
	selector->drop();
#endif //DEBUG
	device->drop();
	return 0;
}
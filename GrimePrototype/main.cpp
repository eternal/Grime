//=============================================================================
//  Grime Prototype main.cpp
//  build 05291057
//=============================================================================
#define DEBUG 1
//#define FULLSCREEN 1
//#define NOSPAWN 1
//#define OLDSMGR 1 
#include <irrlicht.h>
#include <iostream>
#include <IrrPhysx.h>
#include <cstdlib>
#include <ctime> 
#include "XEffects.h"
#include "IrrPhysx/ShapeCreation.h"
#include <NxPhysics.h>
#include <irrKlang.h>
#include "irrKlangSceneNode.h"
#include "CameraPhysics.h"
#include "game.h"
#include "entity.h"
#include "Player.h"
#include "Enemy.h"

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;

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
scene::ICameraSceneNode* camera = NULL;
IPhysxManager* physxManager = NULL;
EffectHandler* effect = NULL;
irrklang::ISoundEngine* soundEngine = NULL;
//physx and scenenode pair for the camera to interact (character controller)
SPhysxAndCameraPair* cameraPair = NULL;
ISceneNode* roomnode = NULL;
//stores all non instantiated objects
core::array<SPhysxAndNodePair*> objects;
//texture arrays
//TO REFACTOR INTO GAME CLASS
core::array<video::ITexture*> explosionTextures;
core::array<video::ITexture*> impactTextures;
//default object density as a global
f32 objectDensity = 50;
//globals for player and enemy array
Player* player;
core::array<Enemy*> enemyObjects;
//global for the enemy mesh
//TODO: CLEAN 
IAnimatedMesh* mesh;
s32 lastJumpTime = 0;
s32 timeBetweenSpawns = 2500;
s32 lastSpawn = 0;
SKeyMap* keyMap;
s32 currentShader = 0;
s32 toonPPEffect = 0;
s32 effectBloom = 0;
s32 effectHighPass = 0;
s32 effectBlur1 = 0;
s32 effectBlur2 = 0;
bool effectToggle = false;
bool bloomToggle = false;
s32 currentTime = 0;
vector3df startPosition;


//simple random number function
s32 getRandom(s32 upper) {
    srand((unsigned)time(0));
    return (rand() % upper) + 1;
}

//Temporary spawning function to streamline spawning of enemies
void SpawnSpider() {
    Enemy* enemy = new Enemy(smgr, mesh, physxManager, &objects, player);
    enemyObjects.push_back(enemy);
}

void SpawnSpiderCustom(vector3df position) {
    Enemy* enemy = new Enemy(smgr, mesh, physxManager, &objects, player, position);
    enemyObjects.push_back(enemy);
}

//once the spawn timer ticks over, spawn another
//TEMPORARY CODE
void SpawnSpiderTimer() 
{
    //PROTOTYPE SPWAN POINTS
    vector3df positionOne(-552.9f,28.0f,-339.166f);
    vector3df positionTwo(-195.101f,28.0f,289.715f);
    vector3df positionThree(131.92f,180.1f,-288.47f);
    //fairly self explanatory, 1/5 chance to get 1 spider at a random point, 1/5 chance to get 2 spiders
    // 1/5 chance to get 3 spiders, 2/5 chances for no spawn
    int rand1 = getRandom(5);
    if (rand1 == 1)
    {
        int rand2 = getRandom(3);
        if (rand2 == 1)
        {
            SpawnSpiderCustom(positionOne);
        }
        if (rand2 == 2)
        {
            SpawnSpiderCustom(positionTwo);
        }
        if (rand2 == 3)
        {
            SpawnSpiderCustom(positionThree);
        }
    }
    else if (rand1 == 2)
    {
        int rand2 = getRandom(3);
        if (rand2 == 1)
        {
            SpawnSpiderCustom(positionOne);
            SpawnSpiderCustom(positionTwo);
        }
        if (rand2 == 2)
        {
            SpawnSpiderCustom(positionOne);
            SpawnSpiderCustom(positionThree);
        }
        if (rand2 == 3)
        {
            SpawnSpiderCustom(positionThree);
            SpawnSpiderCustom(positionTwo);
        }
    }
    else if (rand1 == 3)
    {
        SpawnSpiderCustom(positionOne);
        SpawnSpiderCustom(positionTwo);
        SpawnSpiderCustom(positionThree);
    }
}

void RestartLevel() {
    for (u32 i = 0 ; i < objects.size() ; i++) {
        SPhysxAndNodePair* pair = objects[i];
        physxManager->removePhysxObject(pair->PhysxObject);
        pair->SceneNode->remove();
        delete pair;
        }
    for (u32 i = 0; i < enemyObjects.size(); i++)
    {
        Enemy* enemy = enemyObjects[i];
        physxManager->removePhysxObject(enemy->pair->PhysxObject);
        enemy->pair->SceneNode->remove();
        delete enemy;
    }
    enemyObjects.clear();
    objects.clear();
    
    player->pair->PhysxObject->setPosition(startPosition);
    player->pair->updateTransformation();
    
    timeBetweenSpawns = 2500;
    player->health = 100;
}

//TEMPORARY CODE
//TODO: REWRITE
void createExplosion(const core::vector3df& position) {
#ifdef DEBUG
    // Testing physics pushback
    physxManager->createExplosion(position, 100.0f, 500.0f, 200.0f, 1.0f);
#endif
    scene::ISceneNodeAnimator* anim = NULL;
    // create animation for explosion
    anim = smgr->createTextureAnimator(explosionTextures, 100, false);
    // create explosion billboard
    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(60,60), position);
    // Setup the material
    bill->setMaterialFlag(video::EMF_LIGHTING, false);
    bill->setMaterialTexture(0, smgr->getVideoDriver()->getTexture("media/explosion/01.jpg"));
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animator
    bill->addAnimator(anim);
    anim->drop();
    // create deletion animator to automatically remove the billboard
    anim = smgr->createDeleteAnimator(100*10);
    
    bill->addAnimator(anim);
    anim->drop();
}
void createImpactEffect(const core::vector3df& position, const core::vector3df& normal) {

    core::vector3df scale(5,5,1);
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
    node->setMaterialTexture(0, smgr->getVideoDriver()->getTexture("media/impact/01.jpg"));
    node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animators
    node->addAnimator(textureAnim);
    node->addAnimator(deleteAnim);

    // create impact billboard
    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(NULL, core::dimension2d<f32>(scale.X,scale.Y), position);
    // Set up the material
    bill->setMaterialFlag(video::EMF_LIGHTING, false);
    bill->setMaterialTexture(0, smgr->getVideoDriver()->getTexture("media/impact/01.jpg"));
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animators
    bill->addAnimator(textureAnim);
    bill->addAnimator(deleteAnim);
    textureAnim->drop();
    deleteAnim->drop();
}

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
                    timeBetweenSpawns+= 100;
                    break;
                case KEY_SUBTRACT:
                    timeBetweenSpawns-= 100;
                    break;
#endif //DEBUG
                case KEY_SPACE:
                    //jump
                    if (camera)
                    {
                        // Perform a raycast to find the objects we just shot at
                        core::line3df line;
                        line.start = camera->getPosition();
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
                                    cameraPair->PhysxObject->setLinearVelocity(core::vector3df(0,70,0));
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
                case KEY_KEY_F:
                    //debug spawn enemy function
                    SpawnSpider();
                    std::cout << "Enemy count" << enemyObjects.size() << std::endl;
                    break;
#endif //DEBUG
                case KEY_KEY_G: {
                    core::line3df line;
                    line.start = camera->getPosition();
                    //put the end of the line off in the distance
                    line.end = line.start + (camera->getTarget() - line.start).normalize() * 5000.0f;
                    //access the physics engine to find the intersection point
                    core::array<SRaycastHitData> rayArray = physxManager->raycastAllRigidObjects(line);
                    //core::array<SRaycastHitData> filteredRayArray;
                    SRaycastHitData closestObject;
                    closestObject = rayArray[0];
                    for (u32 i = 0; i < rayArray.size(); ++i) {
                        SRaycastHitData ray = rayArray[i];
                        if (ray.Object->getType() == EOT_TRIANGLE_MESH)
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
                    std::cout << "=========" << std::endl;
                    if (closestObject.Object->getType() == EOT_TRIANGLE_MESH) {
                        vector3df scale(1,1,1);
                        vector3df rot(0,0,0);
                        closestObject.HitPosition.Y += scale.Y / 2;
                        
                        vector3df blockPosition = closestObject.HitPosition;
                        //Quantise block positions
                        f32 blockPositionX = round_(blockPosition.X/30) * 30;
                        f32 blockPositionZ = round_(blockPosition.Z/40) * 40;
                        ///TODO FIX Y QUANTISE
                        //f32 blockPositionY = round_(blockPosition.Y/30) * 30;
                        blockPosition.X = blockPositionX;
                        blockPosition.Z = blockPositionZ;
                        vector3df blockPhysicsPosition = blockPosition;
                        blockPhysicsPosition.Y -= scale.Y / 2;
                        blockPhysicsPosition.X += scale.X / 6.25f;
                        SPhysxAndNodePair* pair = new SPhysxAndNodePair;

                        //IMesh* cubeMesh = smgr->getMesh("media/cube.obj");
                        IMesh* cubeMesh = smgr->getMesh("media/block.obj");
                        //pair->SceneNode = smgr->addMeshSceneNode(cubeMesh, 0, -1, temp, vector3df(0,0,0), scale);
                        //pair->PhysxObject = physxManager->createBoxObject(intersection, core::vector3df(0,0,0), scale/2.0f, 30000000.0f, &(vector3df(0,0,0)));                        
                        pair->PhysxObject = physxManager->createTriangleMeshObject(physxManager->createTriangleMesh(cubeMesh->getMeshBuffer(0), scale), blockPhysicsPosition);
                        //pair->SceneNode = smgr->addCubeSceneNode(1, 0, -1, intersection, rot, scale);
                        pair->SceneNode = smgr->addMeshSceneNode(cubeMesh, 0, -1, blockPosition, rot, scale);
                        pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);            
                    }
                 }
                 break;               
#ifdef DEBUG                    
                case KEY_KEY_C:
                    // DEBUG: Delete all objects
                    for (u32 i = 0 ; i < objects.size() ; i++) {
                        SPhysxAndNodePair* pair = objects[i];
                        physxManager->removePhysxObject(pair->PhysxObject);
                        pair->SceneNode->remove();
                        delete pair;
                        }
                    for (u32 i = 0; i < enemyObjects.size(); i++)
                    {
                        Enemy* enemy = enemyObjects[i];
                        physxManager->removePhysxObject(enemy->pair->PhysxObject);
                        enemy->pair->SceneNode->remove();
                        delete enemy;
                    }
                    enemyObjects.clear();
                    objects.clear();
                    break;
                case KEY_KEY_I:
                    RestartLevel();
                    break;
                case KEY_KEY_V:
                    // Toggle the debug data visibility
                    physxManager->setDebugDataVisible(!physxManager->isDebugDataVisible());
                    break;
                case KEY_KEY_P:
                    effect->addEffectToNode(roomnode,(E_EFFECT_TYPE)currentShader);
                    currentShader++;
                    if (currentShader == 7)
                        currentShader = 0;
                    break;
                case KEY_KEY_O:
                    effectToggle = !effectToggle;
                    if (effectToggle) toonPPEffect = effect->addPostProcessingEffectFromFile("Shaders/toon.hlsl");
                    else effect->removePostProcessingEffect(toonPPEffect);
                    break;
                case KEY_KEY_L:
                    bloomToggle = !bloomToggle;
                    if (bloomToggle)
                    {
                        effectBloom = effect->addPostProcessingEffectFromFile("Shaders/BloomP.hlsl");
                        //effectBlur1 = effect->addPostProcessingEffectFromFile("Shaders/BlurHP.hlsl");
                        effectBlur2 = effect->addPostProcessingEffectFromFile("Shaders/BlurVP.hlsl");
                        toonPPEffect = effect->addPostProcessingEffectFromFile("Shaders/toon.hlsl");
                        //effectHighPass = effect->addPostProcessingEffectFromFile("Shaders/BrightPass.hlsl");
                    }
                    else {
                        effect->removePostProcessingEffect(effectBloom);
                        effect->removePostProcessingEffect(effectBlur1);
                        effect->removePostProcessingEffect(effectBlur2);
                        //effect->removePostProcessingEffect(effectHighPass);
                        //effect->removePostProcessingEffect(toonPPEffect);
                    }
                    break;
                case KEY_MULTIPLY:
                    timeBetweenSpawns = 100;
                    break;
                case KEY_DIVIDE:
                    timeBetweenSpawns = 2500;
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
            switch (event.MouseInput.Event) {
                //shoot
                case EMIE_LMOUSE_PRESSED_DOWN: {
                    mouseLeftPressedDown = true;
                    // Perform a raycast to find the objects we just shot at
                    core::line3df line;
                    line.start = camera->getPosition();
                    //put the end of the line off in the distance
                    line.end = line.start + (camera->getTarget() - line.start).normalize() * 5000.0f;
                    //access the physics engine to find the intersection point
                    core::array<SRaycastHitData> rayArray = physxManager->raycastAllRigidObjects(line);
                    //core::array<SRaycastHitData> filteredRayArray;
                    SRaycastHitData closestObject;
                    closestObject = rayArray[0];
                    for (u32 i = 0; i < rayArray.size(); ++i) {
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
                    std::cout << "===========" << std::endl;
                    IPhysxObject* objectHit = closestObject.Object;
                    //IPhysxObject* objectHit = physxManager->raycastClosestObject(line, &intersection, &normal);
                    if (objectHit) {
                        //check for collisions with any of the types of bounding boxes the enemies use
                        //TO REFACTOR, PLACE IN GAME CLASS
                        if (objectHit->getType() == EOT_CONVEX_MESH || objectHit->getType() == EOT_BOX || objectHit->getType() == EOT_SPHERE) 
                        {
                            for (u32 i = 0 ; i < enemyObjects.size() ; ++i) 
                            {  // check it against the objects in our array to see if it matches
                                if (enemyObjects[i]->pair->PhysxObject == objectHit) {
                                    Enemy* enemy = enemyObjects[i];
                                    core::vector3df pos;
                                    enemy->pair->PhysxObject->getPosition(pos);
                                    //add small pushback and texture explosion
                                    createExplosion(pos);           
                                    //remove from arrays and memory                         
                                    physxManager->removePhysxObject(enemy->pair->PhysxObject);
                                    enemy->pair->SceneNode->remove();
                                    delete enemy;
                                    enemyObjects.erase(i);
                                    break;
                                }
                            }	
                        }  
#ifdef DEBUG                    
                        // If it's a sphere we blow it up
                        else if (objectHit->getType() == EOT_SPHERE) 
                        {
                            for (u32 i = 0 ; i < objects.size() ; ++i) 
                            {  // check it against the objects in our array to see if it matches
                                if (objects[i]->PhysxObject == objectHit) {
                                    core::vector3df pos;
                                    objects[i]->PhysxObject->getPosition(pos);
                                    createExplosion(pos);
                                    objectHit->setLinearVelocity(line.getVector().normalize() * 600.0f);
                                    //physxManager->removePhysxObject(objects[i]->PhysxObject);
                                    //objects[i]->SceneNode->remove();
                                    //delete objects[i];
                                    //objects.erase(i);
                                    break;
                                }
                            }	
                        }
                        //for any other objects except for the camera
                        else if (objectHit != cameraPair->PhysxObject) 
                        {
                            objectHit->setLinearVelocity(line.getVector().normalize() * 30.0f);
                            createImpactEffect(closestObject.HitPosition, closestObject.SurfaceNormal);
                            //physxManager->createExplosion(closestObject.HitPosition, 30000.0f, 10000000.0f, 10000000.0f, 1.0f);
                        }
                        
#endif //DEBUG                     
                    }
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
                    objects.push_back(createSphereFromCamera(physxManager, smgr, driver, camera, 4, 20.0f));
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

//simple function for adding a camera and creating a physics pair for the camera
void CreateCamera() {
    //WASD Keymap
    keyMap = new SKeyMap[5];
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
    //core::vector3df pos(75, 15, -10);
    //SWITCHED from custom camera to internal for stability for prototype
    //add the irrlicht fps camera scene node, modify the keymap to play with WASD
    camera = smgr->addCameraSceneNodeFPS(NULL, 50, 0.1f, -1, keyMap, 4, true);
    camera->setPosition(startPosition);
    camera->setTarget(core::vector3df(0.0f,20.0f,2.0f));
    //fill pair
    cameraPair->SceneNode = camera;
    vector3df physxStartPos = startPosition;
    physxStartPos.Y += 100.0f;
    cameraPair->PhysxObject = physxManager->createSphereObject(startPosition, core::vector3df(0,0,0), 15.0f, 10000.0f);
    cameraPair->PhysxObject->setAngularDamping(1000.0f); // Stops the sphere from rolling
    cameraPair->PreviousPosition = startPosition;
    cameraPair->CameraOffset = core::vector3df(0,10,0);
    
    // on screen gun
    // TODO: REFACTOR THIS AND ABOVE INTO CAMERA CLASS
    // NOTE: UNSKINNED; UNFINISHED MODEL
    scene::ISceneNode* gun = smgr->addMeshSceneNode(smgr->getMesh("media/weapon.obj")->getMesh(0), camera);
    gun->setPosition(core::vector3df(1.5f,-3.2f,1.0f));
    gun->setRotation(core::vector3df(0.0f,180.0f,0.0f));
    gun->setScale(core::vector3df(0.5f,0.5f,0.5f));
    gun->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);   
}

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
    
    if (!soundEngine || !driver || !smgr || !guienv)
        return 1; //fatal error
    
	//set screen buffer resolution
	dimension2du ScreenRTT = !driver->getVendorInfo().equals_ignore_case("NVIDIA Corporation") ? dimension2du(1024, 1024) : driver->getScreenSize();

	//effects handler
	effect = new EffectHandler(device, "shaders", ScreenRTT, true);

    // Set some Physx scene description parameters to optimise raycasting
    SSceneDesc sceneDesc;
    // Set the bounding box of the physx "scene"
    sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-1200,-200,-1200), core::vector3df(1200,800,1200));
    sceneDesc.BoundPlanes = true;
    // Y axis up
    sceneDesc.UpAxis = 1;
    //create physx manager for scene details
    physxManager  = createPhysxManager(device, sceneDesc);
    //create room mesh from asset
	IMesh* room = smgr->getMesh("media/level/kitchen retex final.b3d")->getMesh(0);
	
	//IMesh* room = smgr->getMesh("media/temp.b3d")->getMesh(0);
	//create scene node for mesh and place
	roomnode = smgr->addMeshSceneNode(room,0,-1,vector3df(0,0,0),vector3df(0,0,0));
	//roomnode->setMaterialType((video::E_MATERIAL_TYPE)newMaterialType1);
	
	//scale to proper size
	vector3df roomScale(600.0f,600.0f,600.0f);
	roomnode->setScale(vector3df(roomScale));
	for (u32 i = 0; i < roomnode->getMaterialCount(); i++) {
	    roomnode->getMaterial(i).Lighting = true;
	}
	
	startPosition = vector3df(0.0f,20.0f,0.0f);
	
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
    wchar_t temp[10];
    _itow(timeBetweenSpawns, temp, 10);

    gui::IGUIStaticText* generalText = guienv->addStaticText(L"Time Between Spawns: ", core::rect<s32>(5,38,400,200));
    gui::IGUIStaticText* spawns = guienv->addStaticText(temp, core::rect<s32>(164,38,400,200));
    generalText->setOverrideColor(video::SColor(255,255,255,255));
    spawns->setOverrideColor(video::SColor(255,255,255,255));
    wchar_t temp2[10];  
    gui::IGUIStaticText* textHealth = guienv->addStaticText(L"Health: ", core::rect<s32>(5,58,400,200));
    gui::IGUIStaticText* health = guienv->addStaticText(L"100", core::rect<s32>(75,58,400,200));
    textHealth->setOverrideColor(video::SColor(255,255,255,255));
    health->setOverrideColor(video::SColor(255,255,255,255));
    // Preload texture animators
    // TODO: CLEAN and use particles instead of animated textures
    // just for prototype only
    c8 tmp[64];
    for (s32 i = 1 ; i <= 10 ; ++i) {
        sprintf_s(tmp, "media/explosion/%02d.jpg", i);
        explosionTextures.push_back(smgr->getVideoDriver()->getTexture(tmp));
    }
    for (s32 i = 1 ; i <= 6 ; ++i) {
        sprintf_s(tmp, "media/impact/%02d.jpg", i);
        impactTextures.push_back(smgr->getVideoDriver()->getTexture(tmp));
    }
    
    //set gravity to a ridiculous amount due to scale
    core::vector3df gravity = vector3df(0.0f, -98.1f, 0.0f);
    physxManager->setGravity(gravity);
#ifdef DEBUG    
    std::cout << gravity.X << " " << gravity.Y << " " << gravity.Z << std::endl;
#endif //DEBUG

    //instantiate enemy mesh
    mesh = smgr->getMesh("media/spideranim.x");
    //mesh = smgr->getMesh("media/Bench1.obj");
    //create custom fps camera with physics object
    CreateCamera();
    smgr->setActiveCamera(camera);
    camera->setPosition(startPosition);
    //camera->setFarValue(500.0f);
    //spawn player and link camera
    player = new Player(smgr, mesh, physxManager, cameraPair, &objects, effect);
    
//TODO: FIX PHYSICS MESHING BUG, BANDAID FIX BELOW BY SENDING FIRST SPAWN OFF MAP
    enemyObjects.push_back( new Enemy(smgr, mesh, physxManager, &objects, NULL, vector3df(0.0f, -150.0f, 0.0f)));
#ifdef DEBUG    
    //temporary testing etc
    //enemyObjects.push_back( new Enemy(smgr, mesh, physxManager, &objects, player, vector3df(0.0f, 306.0f, 0.0f)));
#endif //DEBUG
    
    //set fairly blue clear colour
    effect->setClearColour(SColor(255, 0, 0, 0));
    effect->setAmbientColor(SColor(255, 32, 32, 32));
    //smgr->setAmbientLight(video::SColorf(0.3,0.3,0.3,1));
    //TODO: REDO LIGHTING, THIS WILL DO FOR PROTOTYPE
    ILightSceneNode* light = smgr->addLightSceneNode(0,vector3df(97, 150, 23),SColorf(0.3f, 0.3f, 0.3f, 0.5f),800.0f);
    light->setLightType(ELT_POINT);
    ILightSceneNode* light2 = smgr->addLightSceneNode(0,vector3df(629, 150, 26),SColorf(0.3f, 0.3f, 0.3f, 0.5f),800.0f);
    light2->setLightType(ELT_POINT);
    ILightSceneNode* light3 = smgr->addLightSceneNode(0,vector3df(-650, 150, 186),SColorf(0.3f, 0.3f, 0.3f, 0.5f),1000.0f);
    light3->setLightType(ELT_POINT);
    //check which shader language to use
    
	const stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";
    //effect->addEffectToNode(roomnode, 
    //add bright pass, blurs and bloom
    
    
	//effect->addPostProcessingEffectFromFile(core::stringc("shaders/BrightPass.hlsl"));
	//effect->addPostProcessingEffectFromFile(core::stringc("shaders/BlurHP.hlsl"));
	//effect->addPostProcessingEffectFromFile(core::stringc("shaders/BlurVP.hlsl"));
	effect->addPostProcessingEffectFromFile(core::stringc("shaders/BloomP.hlsl"));
	effect->addPostProcessingEffectFromFile(core::stringc("shaders/Toon.hlsl"));
    RestartLevel();
	
#ifdef SMGRDEBUG
	scene::ISceneNode* selectedSceneNode =0;
	scene::ISceneNode* lastSelectedSceneNode =0;
#endif //SMGRDEBUG
    //set pre-loop data
	int lastFPS = -1;
    s32 lastTime = device->getTimer()->getTime();
    lastJumpTime = lastTime;
	while(device->run())
	{
		if (device->isWindowActive())
		{
		    //find time between renders
            s32 timeNow = device->getTimer()->getTime();
            //std::cout << timeNow << std::endl;
            s32 elapsedTime = timeNow - lastTime;
            lastTime = timeNow;
            currentTime += elapsedTime;
            if (currentTime >= 100) {
                timeBetweenSpawns--;
                currentTime = 0;
            }
#ifndef NOSPAWN                        
            if ((lastTime - lastSpawn) >= timeBetweenSpawns)
            {
                SpawnSpiderTimer();
                lastSpawn = lastTime;
            }
#endif            
            if (player->health <= 0) {
                RestartLevel();
            }
            //simulate physics
		    physxManager->simulate(elapsedTime/1000.0f);
		    //REMEMBER: DO NOT MODIFY PHYSX OBJECTS WHILE SIMULATING
		    physxManager->fetchResults();
		    //update scene objects related to physics
            for (u32 i = 0 ; i < objects.size() ; i++) 
                objects[i]->updateTransformation();
            for (u32 i = 0; i < enemyObjects.size(); i++)
                enemyObjects[i]->pair->updateTransformation();                
            cameraPair->updateTransformation();
            
            //begin update routines
            player->Update(elapsedTime);
		    for (u32 i = 0; i < enemyObjects.size(); i++) {
		        enemyObjects[i]->Update(elapsedTime);
		        //effect->addEffectToNode(enemyObjects[i]->pair->SceneNode,(E_EFFECT_TYPE)5);
		    }
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
            // Render the GUI
            _itow(timeBetweenSpawns, temp, 10);
            _itow(player->health, temp2, 10);
            spawns->setText(temp);
            health->setText(temp2);
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
#ifdef DEBUG
                std::cout << driver->getPrimitiveCountDrawn() << std::endl;
				printf("X: %f, Y: %f, Z: %f\n",camera->getAbsolutePosition().X,camera->getAbsolutePosition().Y,camera->getAbsolutePosition().Z);
#endif //DEBUG		
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
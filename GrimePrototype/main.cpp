//=============================================================================
//  Grime Prototype main.cpp
//  build 200909281238
//=============================================================================
#define DEBUG 1
//#define FULLSCREEN 1
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
Game* game = NULL;

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
                    game->player->Jump();

                    break;
            }
            //if the key is held down, return and ignore
            if (event.KeyInput.PressedDown) return false;

            switch (event.KeyInput.Key) {
                case KEY_KEY_H:
                    game->FinalWave();
                    break;
                case KEY_F9:
                    //screenshot                  
                    driver->writeImageToFile(driver->createScreenShot(),"screenie.jpg",0);
                    break;
#ifdef DEBUG
                case KEY_KEY_U:
                    game->CreateMuzzleFlash();
                break;
                case KEY_KEY_G: {
                    Block* block = new Block(smgr, physxManager, &game->enemyObjects);
                    game->blockObjects.push_back(block);
                 }
                 break;                             
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
                case KEY_KEY_I:
                    game->RestartLevel();
                    break;
                case KEY_KEY_V:
                    // Toggle the debug data visibility
                    physxManager->setDebugDataVisible(!physxManager->isDebugDataVisible());
                    break;
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
            if (event.MouseInput.isLeftPressed())
            {
                game->WeaponFire();
                
            }
            switch (event.MouseInput.Event) {
                case EMIE_MOUSE_WHEEL: {
                    game->player->WeaponSelect(event.MouseInput.Wheel);
                }
                break;
                //single instance effect
                case EMIE_LMOUSE_PRESSED_DOWN: {
      
                }
                break;
                case EMIE_LMOUSE_LEFT_UP: {

                }
                break;
#ifdef DEBUG                
                case EMIE_RMOUSE_PRESSED_DOWN: {

                }
                break;
                case EMIE_RMOUSE_LEFT_UP: {

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

    //remove cursor from view
	device->getCursorControl()->setVisible(false);
	
    //add crosshair to centre of screen (64x64 image so -32)
    guienv->addImage(driver->getTexture("media/crosshair2.png"), core::position2di(resolution.Width/2-32,resolution.Height/2-32));
    
    //grab bitmap font
    guienv->getSkin()->setFont(guienv->getFont("media/GillSans12.png"));
    gui::IGUIStaticText* levelText = guienv->addStaticText(L"Grime Prototype: Kitchen", core::rect<s32>(5,2,200,200));
    levelText->setOverrideColor(video::SColor(255,255,255,255));
    gui::IGUIStaticText* buildText = guienv->addStaticText(L"Build: 200909281238", core::rect<s32>(5,20,200,200));
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
    
    gui::IGUIStaticText* textTime = guienv->addStaticText(L"Time: ", rect<s32>(1100,2,1200,200));
    textTime->setOverrideColor(SColor(255,255,255,255));
    
    //set gravity to a ridiculous amount due to scale
    core::vector3df gravity = vector3df(0.0f, -98.1f, 0.0f);
    physxManager->setGravity(gravity);
    
//TODO: FIX PHYSICS MESHING BUG, BANDAID FIX BELOW BY SENDING FIRST SPAWN OFF MAP
    game->spawnManager->DIRTYMESHFIX();
    
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
    
	const stringc shaderExt = (driver->getDriverType() == EDT_DIRECT3D9) ? ".hlsl" : ".glsl";

	effect->addPostProcessingEffectFromFile(core::stringc("shaders/BloomP.hlsl"));
	effect->addPostProcessingEffectFromFile(core::stringc("shaders/Toon.hlsl"));
    game->RestartLevel();
	
    //set pre-loop data
	int lastFPS = -1;
    s32 lastTime = device->getTimer()->getTime();
	while(device->run())
	{
		if (device->isWindowActive())
		{
		    //find time between renders
            s32 timeNow = device->getTimer()->getTime();
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
	
			effect->update();
	
#ifdef DEBUG //physx debug data to show bounding boxes   
            physxManager->renderDebugData(video::SColor(225,255,255,255));            
#endif // DEBUG
            core::stringw strTime, strHealth, strCooldown, strPosition, strPrimitives, strTotalTime = "";
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
            strTotalTime += "Time: ";
            strTotalTime += game->spawnManager->waveTimer / 1000.0f;
            //_itow(player->health, temp2, 10);
            spawns->setText(strTime.c_str());
            health->setText(strHealth.c_str());
            cooldown->setText(strCooldown.c_str());
            textPosition2->setText(strPosition.c_str());
            textPrimitives->setText(strPrimitives.c_str());
            textTime->setText(strTotalTime.c_str());
            guienv->drawAll();

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
	device->drop();
	return 0;
}
//=============================================================================
//  Grime Prototype main.cpp
//  build 200909291248
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
#include "EventReceiver.h"

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

int main() {
	video::E_DRIVER_TYPE driverType;
	driverType = video::EDT_DIRECT3D9;
	//event receiver instance
    EventReceiver receiver;
    //explicitly choose screen res
    core::dimension2du resolution(1280,800);
    // create device and exit if creation failed
#ifndef FULLSCREEN    
	device = createDevice(driverType, resolution, 32, false, false, false, &receiver);
#else
    device = createDevice(driverType, resolution, 32, true, false, false, receiver);	
#endif
	if (device == 0)
	{
		return 1; // could not create selected driver.
    }
    
    receiver.device = device;
    //instantiate necessary globals
    driver = device->getVideoDriver();
    smgr = device->getSceneManager();
    guienv = device->getGUIEnvironment();
    soundEngine  = irrklang::createIrrKlangDevice();
    
    //seed random numbers
    srand((unsigned)time(0));
    
    if (!soundEngine || !driver || !smgr || !guienv)
    {
        return 2; //fatal error
    }
    
	//set screen buffer resolution
	dimension2du ScreenRTT = !driver->getVendorInfo().equals_ignore_case("NVIDIA Corporation") ? dimension2du(1024, 1024) : driver->getScreenSize();

	//effects handler
	effect = new EffectHandler(device, "shaders", ScreenRTT, true);
    
    if (!effect)
    {
        return 3; //critical error
    }
    
    //remove cursor from view
    device->getCursorControl()->setVisible(false);
    
    // Set some Physx scene description parameters to optimise raycasting
    SSceneDesc sceneDesc;
    // Set the bounding box of the physx "scene"
    sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-2300,-200,-2000), core::vector3df(2000,800,2000));
    sceneDesc.BoundPlanes = true;
    // Y axis up
    sceneDesc.UpAxis = 1;
    //create physx manager for scene details
    physxManager  = createPhysxManager(device, sceneDesc);
    
    if(!physxManager)
    {
        return 4; // physics engine failure
    }
    
    game = new Game(smgr, soundEngine, physxManager, effect);
    receiver.game = game;
    
    //add crosshair to centre of screen (64x64 image so -32)
    guienv->addImage(driver->getTexture("media/crosshair2.png"), core::position2di(resolution.Width/2-32,resolution.Height/2-32));

    //grab bitmap font
    guienv->getSkin()->setFont(guienv->getFont("media/GillSans12.png"));
    guienv->getSkin()->setColor(gui::EGDC_BUTTON_TEXT, SColor(255,255,255,255));
    
    gui::IGUIStaticText* levelText = guienv->addStaticText(L"Grime: Kitchen", core::rect<s32>(5,2,200,200));
    gui::IGUIStaticText* buildText = guienv->addStaticText(L"Build: 200909291248", core::rect<s32>(5,20,200,200));
    gui::IGUIStaticText* textSpawns = guienv->addStaticText(L"Time Between Spawns: ", core::rect<s32>(5,38,400,200));    
    gui::IGUIStaticText* textHealth = guienv->addStaticText(L"Health: ", core::rect<s32>(5,58,400,200));
    gui::IGUIStaticText* textCooldown = guienv->addStaticText(L"Cooldown: ", rect<s32>(5,700,400,800));
    gui::IGUIStaticText* textPosition = guienv->addStaticText(L"Position: ", rect<s32>(5,725,400,800));
    gui::IGUIStaticText* textPrimitives = guienv->addStaticText(L"Primitives Drawn: ", rect<s32>(5, 750, 400, 810));
    gui::IGUIStaticText* textTime = guienv->addStaticText(L"Time: ", rect<s32>(1100,2,1300,200));
    gui::IGUIStaticText* textFPS = guienv->addStaticText(L"FPS: ", rect<s32>(1100,22,1200,200));

    //set pre-loop data
	int lastFPS = -1;
    s32 lastTime = device->getTimer()->getTime();
	while(device->run())
	{
	    //find time between renders
        s32 timeNow = device->getTimer()->getTime();
        s32 elapsedTime = timeNow - lastTime;
        lastTime = timeNow;
        
        //simulate physics
	    physxManager->simulate(elapsedTime/1000.0f);
	    //REMEMBER: DO NOT MODIFY PHYSX OBJECTS WHILE SIMULATING
	    physxManager->fetchResults();
        
	    game->Update(elapsedTime);
        //start drawing
        if (device->isWindowActive())
        {            
			driver->beginScene(true, true, video::SColor(255,200,200,200));
	
			effect->update();
	
#ifdef DEBUG //physx debug data to show bounding boxes   
            physxManager->renderDebugData(video::SColor(225,255,255,255));            
#endif // DEBUG
            
            core::stringw strTime = "Time Between Spawns: "; 
            strTime += game->spawnManager->timeBetweenSpawns;
            core::stringw strHealth = "Health: "; 
            strHealth += game->player->health;
            core::stringw strCooldown = "Cooldown: "; 
            strCooldown += game->player->CurrentCooldown();
            core::stringw strPosition = "X: "; 
            strPosition += game->cameraPair->camera->getAbsolutePosition().X;
            strPosition += " Y: ";
            strPosition += game->cameraPair->camera->getAbsolutePosition().Y; 
            strPosition += " Z: "; 
            strPosition += game->cameraPair->camera->getAbsolutePosition().Z;
            core::stringw strPrimitives = "Primitives Count: ";
            strPrimitives += driver->getPrimitiveCountDrawn();
            strPrimitives += " PhysxObjects Count: ";
            strPrimitives += physxManager->getNumPhysxObjects();
            strPrimitives += " \nEnemy Objects: ";
            strPrimitives += game->enemyObjects.size();
            core::stringw strTotalTime = "Time: ";
            strTotalTime += game->spawnManager->waveTimer / 1000.0f;
            core::stringw strFPS = "FPS: ";
            strFPS += lastFPS;
            textSpawns->setText(strTime.c_str());
            textHealth->setText(strHealth.c_str());
            textCooldown->setText(strCooldown.c_str());
            textPosition->setText(strPosition.c_str());
            textPrimitives->setText(strPrimitives.c_str());
            textTime->setText(strTotalTime.c_str());
            textFPS->setText(strFPS.c_str());
            
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
		{
			device->yield();
	    }
	}
	device->drop();
	return 0;
}
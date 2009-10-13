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
#include <iostream>
#include <ctime> 
#include "XEffects.h"
#include <NxPhysics.h>
#include <irrKlang.h>
#include "irrKlangSceneNode.h"
#include "Game.h"
#include "EventReceiver.h"
#include "StateManager.h"

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
StateManager* stateManager = NULL;

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
    device = createDevice(driverType, resolution, 32, true, false, false, &receiver);	
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
	effect = new EffectHandler(device, "media/shaders", ScreenRTT, true);
    
    if (!effect)
    {
        return 3; //critical error
    }
    
    //remove cursor from view
    device->getCursorControl()->setVisible(false);
    
    // Set some Physx scene description parameters to optimise raycasting
    SSceneDesc sceneDesc;
    // Set the bounding box of the physx "scene"
    sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-2300,-200,-2000), core::vector3df(2000,1500,2000));
    sceneDesc.BoundPlanes = true;
    // Y axis up
    sceneDesc.UpAxis = 1;
    //create physx manager for scene details
    physxManager  = createPhysxManager(device, sceneDesc);
    
    if(!physxManager)
    {
        return 4; // physics engine failure
    }
    smgr->getFileSystem()->addZipFileArchive("media/kitchen.pk3");
    game = new Game(device, soundEngine, physxManager, effect);
    stateManager = new StateManager(device, physxManager, game);
    receiver.game = game;
    receiver.stateManager = stateManager;
    
    stateManager->LoadState(EGS_MENU);
    //set pre-loop data
	int lastFPS = -1;
    s32 lastTime = device->getTimer()->getTime();
	while(device->run())
	{         
	    //find time between renders
        s32 timeNow = device->getTimer()->getTime();
        s32 elapsedTime = timeNow - lastTime;
        lastTime = timeNow;
        
        //s32 realTime = device->getTimer()->getRealTime();
        //simulate physics
	    physxManager->simulate(elapsedTime/1000.0f);
	    //REMEMBER: DO NOT MODIFY PHYSX OBJECTS WHILE SIMULATING
	    physxManager->fetchResults();
        //std::cout << "Time taken to simulate: " <<  device->getTimer()->getRealTime() - realTime << std::endl;
        
        //s32 realTimeUpdates = device->getTimer()->getRealTime();
        stateManager->Update(elapsedTime);
        //std::cout << "Time taken to update: " <<  device->getTimer()->getRealTime() - realTimeUpdates << std::endl;

        //start drawing  
        if (device->isWindowActive())
        { 
			driver->beginScene(true, true, video::SColor(255,200,200,200));
            //s32 realTimeEffect = device->getTimer()->getRealTime();
			effect->update();
            //std::cout << "Time taken to draw: " <<  device->getTimer()->getRealTime() - realTimeEffect << std::endl;

	        //s32 realtimedebug = device->getTimer()->getRealTime();
#ifdef DEBUG //physx debug data to show bounding boxes   
            physxManager->renderDebugData(video::SColor(225,255,255,255));            
#endif // DEBUG
            //std::cout << "Time taken to draw debug: " <<  device->getTimer()->getRealTime() - realtimedebug << std::endl;     
            //s32 realtimegui = device->getTimer()->getRealTime();
            guienv->drawAll();
            //std::cout << "Time taken to draw debug: " <<  device->getTimer()->getRealTime() - realtimegui << std::endl;     
            
            //done rendering
			driver->endScene();
            //std::cout << "Scene render time: " << device->getTimer()->getRealTime() - realTimeEffect << std::endl;
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
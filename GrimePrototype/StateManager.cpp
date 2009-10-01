#include "StateManager.h"


StateManager::StateManager(IrrlichtDevice* device, IPhysxManager* physxManager, Game* game)
{
    this->smgr = device->getSceneManager();
    this->physxManager = physxManager;
    this->guienv = device->getGUIEnvironment();
    this->driver = device->getVideoDriver();
    this->game = game;
    
    resolution = driver->getCurrentRenderTargetSize();
}

StateManager::~StateManager(void)
{
}
void StateManager::Update(s32 time)
{
    switch (currentState)
    {
        case EGS_GAME:
        {
            game->Update(time);
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
            strFPS += driver->getFPS();
            textSpawns->setText(strTime.c_str());
            textHealth->setText(strHealth.c_str());
            textCooldown->setText(strCooldown.c_str());
            textPosition->setText(strPosition.c_str());
            textPrimitives->setText(strPrimitives.c_str());
            textTime->setText(strTotalTime.c_str());
            textFPS->setText(strFPS.c_str());
        }
        break;
        case EGS_MENU:
        {
        }
        break;
    }
    
    
}
void StateManager::LoadState(s32 state) 
{
    smgr->clear();
    guienv->clear();
    switch (state)
    {
        case EGS_MENU:
        {
            levelText = guienv->addStaticText(L"Grime: Kitchen", core::rect<s32>(5,2,200,200));
            this->currentState = EGS_MENU;
            
        }
        break;
        case EGS_GAME:
        {
            //add crosshair to centre of screen (64x64 image so -32)
            guienv->addImage(driver->getTexture("media/crosshair2.png"), core::position2di(resolution.Width/2-32,resolution.Height/2-32));

            //grab bitmap font
            guienv->getSkin()->setFont(guienv->getFont("media/GillSans12.png"));
            guienv->getSkin()->setColor(gui::EGDC_BUTTON_TEXT, SColor(255,255,255,255));
            
            levelText = guienv->addStaticText(L"Grime: Kitchen", core::rect<s32>(5,2,200,200));
            buildText = guienv->addStaticText(L"Build: 200909291248", core::rect<s32>(5,20,200,200));
            textSpawns = guienv->addStaticText(L"Time Between Spawns: ", core::rect<s32>(5,38,400,200));    
            textHealth = guienv->addStaticText(L"Health: ", core::rect<s32>(5,58,400,200));
            textCooldown = guienv->addStaticText(L"Cooldown: ", rect<s32>(5,700,400,800));
            textPosition = guienv->addStaticText(L"Position: ", rect<s32>(5,725,400,800));
            textPrimitives = guienv->addStaticText(L"Primitives Drawn: ", rect<s32>(5, 750, 400, 810));
            textTime = guienv->addStaticText(L"Time: ", rect<s32>(1100,2,1300,200));
            textFPS = guienv->addStaticText(L"FPS: ", rect<s32>(1100,22,1200,200));

            room = smgr->getMesh("media/level/kitchen retex final.b3d")->getMesh(1);
    
            //create scene node for mesh and place
            roomnode = smgr->addMeshSceneNode(room,0,-1,vector3df(0,0,0),vector3df(0,0,0));        
            
            vector3df roomScale(1200.0f,1200.0f,1200.0f);
            roomnode->setScale(vector3df(roomScale));
            for (u32 i = 0; i < roomnode->getMaterialCount(); i++) 
            {
                roomnode->getMaterial(i).Lighting = true;
            }
            
            //normalize mesh's normals as it has been scaled
            roomnode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            
            //get the mesh buffers of the mesh and create physics representation
            for (u32 i = 0 ; i < room->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                //IPhysxMesh* triMesh = physxManager->createTriangleMesh(room->getMeshBuffer(i), vector3df(1.0f,1.0f,1.0f));
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(room->getMeshBuffer(i), roomScale);
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(0.0f,0.0f,0.0f));
            }
            game->LoadLevel();
            this->currentState = EGS_GAME;
        }
        break;
    }
}

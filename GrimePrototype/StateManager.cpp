#include "StateManager.h"

StateManager::StateManager(IrrlichtDevice* device, IPhysxManager* physxManager, Game* game)
{
    this->smgr = device->getSceneManager();
    this->device = device;
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
            core::stringw strHealth = "";
            strHealth += game->player->health;
            
            if (game->gameOver)
            {
                deadOverlay->setColor(SColor(255,255,255,255));
            }
            else
            {
                deadOverlay->setColor(SColor(0,255,255,255));
            }
            
            if (game->player->damagedTimer > 0)
            {
                game->player->damagedTimer -= time;
                if (game->player->damagedTimer <= 0)
                {
                    game->player->damagedTimer = 0;
                }
                f32 opacity = ((f32)game->player->damagedTimer / 510.0f) *255.0f;
                damageOverlay->setColor(SColor((u32)opacity, 255,255,255));
            }
            
            if (game->player->webTimer > 0)
            {
                game->player->webTimer -= time;
                if (game->player->webTimer <= 0)
                {
                    game->player->webTimer =0;
                }
                f32 opacity = ((f32)game->player->webTimer / 510.0f) *255.0f;
                webOverlay->setColor(SColor((u32)opacity, 255,255,255)); 
            }

            if (game->player->weaponCooldown[WEAPON_RPG] <= 0)
            {
                imgRPG->setColor(SColor(255,255,255,255));
            }
            else 
            {
                f32 opacity = 255 - ((((float)game->player->weaponCooldown[WEAPON_RPG]/3000.0f)) * 255);
                imgRPG->setColor(SColor((u32)opacity,255,255,255));
            }
            if (game->player->weaponCooldown[WEAPON_BLOCKGUN] <= 0)
            {
                imgBlock->setColor(SColor(255,255,255,255));
            }
            else 
            {
                f32 opacity = 255 - ((((float)game->player->weaponCooldown[WEAPON_BLOCKGUN]/1000.0f)) * 255);
                imgBlock->setColor(SColor((u32)opacity,255,255,255));
            }
            if (game->player->weaponCooldown[WEAPON_CLOSE] <= 0)
            {
                imgKnockback->setColor(SColor(255,255,255,255));
            }
            else 
            {
                f32 opacity = 255 - ((((float)game->player->weaponCooldown[WEAPON_CLOSE]/10000.0f)) * 255);
                imgKnockback->setColor(SColor((u32)opacity,255,255,255));
            }
            if (game->player->weaponCooldown[WEAPON_PISTOL] <= 0)
            {
                imgAmmo->setColor(SColor(255,255,255,255));
            }
            else 
            {
                f32 opacity = 255 - ((((float)game->player->weaponCooldown[WEAPON_PISTOL]/250.0f)) * 255);
                imgAmmo->setColor(SColor((u32)opacity,255,255,255));
            }
            
            switch (game->player->currentWeapon)
            {
                case WEAPON_PISTOL:
                    crosshair->setImage(driver->getTexture("media/gui/Crosshairs/MG.png"));
                break;
                case WEAPON_CLOSE:
                    crosshair->setImage(driver->getTexture("media/gui/Crosshairs/Knockback.png"));
                break;
                case WEAPON_RPG:
                    crosshair->setImage(driver->getTexture("media/gui/Crosshairs/RPG.png"));
                break;
            }
            
            core::stringw strAmmo = ""; 
            strAmmo += game->player->weaponAmmunition[WEAPON_BLOCKGUN];
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
            u32 minutes = (u32)((game->spawnManager->waveTimer / 1000.0f)/ 60.0f);
            u32 seconds = (u32)(game->spawnManager->waveTimer / 1000.0f) - minutes * 60;
            strTotalTime += minutes;
            strTotalTime += ":";
            if (seconds < 10)
            {
                strTotalTime += "0";
            }
            strTotalTime += seconds;
            //core::stringw strFPS = "FPS: ";
            //strFPS += driver->getFPS();
            core::stringw strNotify = "";
            if (game->spawnManager->onCooldown)
            {
                strNotify += "Next Wave: ";
                strNotify += (60000 - game->spawnManager->cooldownTimer) / 1000;
            }
            
//            textSpawns->setText(strTime.c_str());
            textHealth->setText(strHealth.c_str());
            textAmmo->setText(strAmmo.c_str());
 //           textPosition->setText(strPosition.c_str());
 //           textPrimitives->setText(strPrimitives.c_str());
            textTime->setText(strTotalTime.c_str());
            //textFPS->setText(strFPS.c_str());
            textNotifications->setText(strNotify.c_str());
        }
        break;
        case EGS_MENU:
        {
            if (game->newGameSelected)
            {
                menuNew->setOverrideColor(SColor(255,255,255,255));
                menuExit->setOverrideColor(SColor(255,128,255,255));
            }
            else
            {
                menuExit->setOverrideColor(SColor(255,255,255,255));
                menuNew->setOverrideColor(SColor(255,128,255,255));
            }
        }
        break;
    }
    
    
}
void StateManager::NodeInit(IMeshSceneNode* node, vector3df scale)
{
    for (u32 i = 0; i < node->getMaterialCount(); i++) 
    {
        node->getMaterial(i).Lighting = true;
    }

    //normalize mesh's normals as it has been scaled
    node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    //node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    //node->setAutomaticCulling(EAC_BOX);
    //get the mesh buffers of the mesh and create physics representation
    for (u32 i = 0 ; i < node->getMesh()->getMeshBufferCount(); ++i) 
    {
        //first calculate the mesh triangles and make physx object
        //IPhysxMesh* triMesh = physxManager->createTriangleMesh(room->getMeshBuffer(i), vector3df(1.0f,1.0f,1.0f));
        IPhysxMesh* triMesh = physxManager->createTriangleMesh(node->getMesh()->getMeshBuffer(i), scale);
        //secondly add the object to the world
        physxManager->createTriangleMeshObject(triMesh,vector3df(0.0f,0.0f,0.0f),vector3df(90.0f,0.0f,0.0f));
    }
}
void StateManager::LoadState(s32 state) 
{
    smgr->clear();
    guienv->clear();
    SSceneDesc sceneDesc;
    // Set the bounding box of the physx "scene"
    sceneDesc.MaxBounds = core::aabbox3df(core::vector3df(-2300,-200,-2000), core::vector3df(2000,1500,2000));
    sceneDesc.BoundPlanes = true;
    // Y axis up
    sceneDesc.UpAxis = 1;
    physxManager->resetScene(sceneDesc);
    switch (state)
    {
        case EGS_MENU:
        {
            levelText = guienv->addStaticText(L"Grime: Kitchen", core::rect<s32>(5,2,200,200));
            this->currentState = EGS_MENU;
            
            gui::IGUIImage* backdrop = guienv->addImage(driver->getTexture("media/menu/mainmenu.png"), core::position2di(0,0));
            
            menuNew = guienv->addStaticText(L"New Game", core::rect<s32>(120,300,800,1006));
            menuNew->setOverrideFont(guienv->getFont("media/gui/Font60.png"));
            menuNew->setOverrideColor(SColor(255,128,255,255));
            
            menuExit = guienv->addStaticText(L"Exit", core::rect<s32>(700,300,1000,1006));
            menuExit->setOverrideFont(guienv->getFont("media/gui/Font60.png"));
            menuExit->setOverrideColor(SColor(255,128,255,255));
        }
        break;
        case EGS_GAME:
        {
            //add crosshair to centre of screen (64x64 image so -32)
            driver->getTexture("media/gui/Crosshairs/RPG.png");
            driver->getTexture("media/gui/Crosshairs/Knockback.png");

            crosshair = guienv->addImage(driver->getTexture("media/gui/Crosshairs/MG.png"),core::position2di(resolution.Width/2-75,resolution.Height/2-75));
            crosshair->setColor(SColor(100,255,255,255));
            
            imgBoxCooldowns = guienv->addImage(driver->getTexture("media/gui/CooldownBox.png"), core::position2di(5,700));
            imgBoxHealth = guienv->addImage(driver->getTexture("media/gui/HP-Box.png"), core::position2di(5,601));
            
            imgHealth = guienv->addImage(driver->getTexture("media/gui/Health.png"), core::position2di(12,606));
            imgBlock = guienv->addImage(driver->getTexture("media/gui/Block.png"), core::position2di(12,716));          
            imgKnockback = guienv->addImage(driver->getTexture("media/gui/Knockback.png"), core::position2di(490,716));
            imgRPG = guienv->addImage(driver->getTexture("media/gui/RPG.png"), core::position2di(395,711));
            imgAmmo = guienv->addImage(driver->getTexture("media/gui/Ammo.png"), core::position2di(301,716));
            
            damageOverlay = guienv->addImage(driver->getTexture("media/gui/RED2.png"), core::position2di(-200,-125));
            damageOverlay->setColor(SColor(0,255,255,255));
            
            webOverlay = guienv->addImage(driver->getTexture("media/gui/White.png"), core::position2di(-200,-125));
            webOverlay->setColor(SColor(0,255,255,255));

            deadOverlay = guienv->addImage(driver->getTexture("media/gui/RED.png"), core::position2di(0,0));
            deadOverlay->setColor(SColor(0,255,255,255));
            
            gui::IGUIImage* fadeoutOverlay = guienv->addImage(driver->getTexture("media/gui/BLACK.png"), core::position2di(0,0));  //preload texture
            fadeoutOverlay->setColor(SColor(0,255,255,255));
            
            //grab bitmap font
            guienv->getSkin()->setFont(guienv->getFont("media/GillSans12.png"));
            guienv->getSkin()->setColor(gui::EGDC_BUTTON_TEXT, SColor(255,255,255,255));
            
            //levelText = guienv->addStaticText(L"Grime: Kitchen", core::rect<s32>(5,2,200,200));
            //buildText = guienv->addStaticText(L"Build: 200910181838", core::rect<s32>(5,20,200,200));
            //textSpawns = guienv->addStaticText(L"Time Between Spawns: ", core::rect<s32>(5,38,400,200));
                
            textHealth = guienv->addStaticText(L"Health: ", core::rect<s32>(120,590,400,1006));
            textHealth->setOverrideFont(guienv->getFont("media/gui/Font60.png"));
            //textAmmunition = ;
            
            textAmmo = guienv->addStaticText(L"Ammo: ", rect<s32>(144,690,1200,800));
            textAmmo->setOverrideFont(guienv->getFont("media/gui/Font60.png"));

            //textPosition = guienv->addStaticText(L"", rect<s32>(800,725,1200,800));
            //textPrimitives = guienv->addStaticText(L"Primitives Drawn: ", rect<s32>(800, 750, 1200, 810));
            textTime = guienv->addStaticText(L"Time: ", rect<s32>(600,2,1300,200));
            textTime->setOverrideFont(guienv->getFont("media/gui/Font24.png"));
            textFPS = guienv->addStaticText(L"", rect<s32>(1100,22,1200,200));
            textNotifications = guienv->addStaticText(L"Phase: ", rect<s32>(12,550,412,600));
            textNotifications->setOverrideFont(guienv->getFont("media/gui/Font24.png"));

            //room = smgr->getMesh("media/level/kitchen retex final.b3d")->getMesh(1);
            
            vector3df roomScale(1200.0f,1200.0f,1200.0f);
            vector3df roomTranslate(0.0f,0.0f,0.0f);
            vector3df roomRotate(90.0f,0.0f,0.0f);
            IMesh* meshRoom = smgr->getMesh("media/level/roomplace.b3d")->getMesh(0);
            IMesh* meshFridge = smgr->getMesh("media/level/fridgeattempt.b3d")->getMesh(0);
            IMesh* meshBenchUpper = smgr->getMesh("media/level/kitchenbenchupper.b3d")->getMesh(0);
            IMesh* meshBenchLower = smgr->getMesh("media/level/kitchenbenchlowerattempt.b3d")->getMesh(0);
            IMesh* meshMicrowave = smgr->getMesh("media/level/kitchenmicrowave.b3d")->getMesh(0);
            IMesh* meshMirror = smgr->getMesh("media/level/kitchenmirror.b3d")->getMesh(0);
            IMesh* meshPlant = smgr->getMesh("media/level/kitchenplant.b3d")->getMesh(0);
            IMesh* meshSink = smgr->getMesh("media/level/kitchensink.b3d")->getMesh(0);
            IMesh* meshPantry = smgr->getMesh("media/level/pantry.b3d")->getMesh(0);
            IMesh* meshDoor = smgr->getMesh("media/level/door.b3d")->getMesh(0);

            IMeshSceneNode* nodeRoom = smgr->addMeshSceneNode(meshRoom, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodeFridge = smgr->addMeshSceneNode(meshFridge, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodeBenchUpper = smgr->addMeshSceneNode(meshBenchUpper, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodeBenchLower = smgr->addMeshSceneNode(meshBenchLower, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodeMicrowave = smgr->addMeshSceneNode(meshMicrowave, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodeMirror = smgr->addMeshSceneNode(meshMirror, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodePlant = smgr->addMeshSceneNode(meshPlant, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodeSink = smgr->addMeshSceneNode(meshSink, 0, -1, roomTranslate, roomRotate, roomScale);
            
            vector3df pantryTranslate = vector3df(0.0f,-20.0f,0.0f);
            IMeshSceneNode* nodePantry = smgr->addMeshSceneNode(meshPantry, 0, -1, pantryTranslate, roomRotate, roomScale);
            for (u32 i = 0 ; i < nodePantry->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodePantry->getMesh()->getMeshBuffer(i), roomScale);
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,pantryTranslate,vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodePantry->getMaterialCount(); i++) 
            {
                nodePantry->getMaterial(i).Lighting = true;
            }  
            nodePantry->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);   
            nodePantry->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
            
            vector3df doorTranslate = vector3df(-650.0f,-20.0f,350.0f);
            vector3df doorScale = vector3df(1800,1800,1200);
            IMeshSceneNode* nodeDoor = smgr->addMeshSceneNode(meshDoor, 0, -1, doorTranslate, roomRotate, doorScale);
            for (u32 i = 0 ; i < nodeDoor->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodeDoor->getMesh()->getMeshBuffer(i), doorScale);
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,doorTranslate,vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodeDoor->getMaterialCount(); i++) 
            {
                nodeDoor->getMaterial(i).Lighting = true;
            }
            nodeDoor->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);            
            
            IMeshSceneNode* nodePlantTwo = smgr->addMeshSceneNode(meshPlant, 0, -1, vector3df(-160.0f,0.0f,-10.0f), vector3df(90.0f,0.0f,0.0f), vector3df(100.0f,100.0f,100.0f));
            for (u32 i = 0 ; i < nodePlantTwo->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodePlantTwo->getMesh()->getMeshBuffer(i), vector3df(100.0,100.0f,100.0f));
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(-160.0f,0.0f,-10.0f),vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodePlantTwo->getMaterialCount(); i++) 
            {
                nodePlantTwo->getMaterial(i).Lighting = true;
            }
            nodePlantTwo->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            //nodePlantTwo->setAutomaticCulling(EAC_BOX);
            
            IMeshSceneNode* nodeFridge = smgr->addMeshSceneNode(meshFridge, 0, -1, vector3df(20.0f,0.0f,-100.0f), vector3df(90.0f,0.0f,0.0f), vector3df(130.0f,130.0f,130.0f));
            //IMeshSceneNode* nodeFridge = smgr->addMeshSceneNode(meshFridge, 0, -1, roomTranslate, roomRotate, roomScale);
            //NodeInit(nodeFridge,roomScale);
            for (u32 i = 0 ; i < nodeFridge->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodeFridge->getMesh()->getMeshBuffer(i), vector3df(130.0,130.0f,130.0f));
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(20.0f,0.0f,-100.0f),vector3df(90.0f,0.0f,0.0f));
            }
            //physxManager->createBoxObject(vector3df(-200.0f,0.0f,500.0f),vector3df(0.0f,0.0f,0.0f),vector3df(200.0f,400.0f,200.0f), 30000000000000.0f);
            for (u32 i = 0; i < nodeFridge->getMaterialCount(); i++) 
            {
                nodeFridge->getMaterial(i).Lighting = true;
            }
            nodeFridge->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
            nodeFridge->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            //nodeFridge->setAutomaticCulling(EAC_BOX);
            
            IMeshSceneNode* nodeMicrowave = smgr->addMeshSceneNode(meshMicrowave, 0, -1, vector3df(-120.0f,100.0f,-100.0f), vector3df(90.0f,0.0f,0.0f), vector3df(100.0f,100.0f,100.0f));
            for (u32 i = 0 ; i < nodeMicrowave->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodeMicrowave->getMesh()->getMeshBuffer(i), vector3df(100.0,100.0f,100.0f));
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(-120.0f,100.0f,-100.0f),vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodeMicrowave->getMaterialCount(); i++) 
            {
                nodeMicrowave->getMaterial(i).Lighting = true;
            }
            nodeMicrowave->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            //nodeMicrowave->setAutomaticCulling(EAC_BOX);
            
            IMeshSceneNode* nodeMirror = smgr->addMeshSceneNode(meshMirror, 0, -1, vector3df(-395.0f,100.0f,-100.0f), vector3df(90.0f,0.0f,0.0f), vector3df(100.0f,100.0f,100.0f));
            for (u32 i = 0 ; i < nodeMirror->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodeMirror->getMesh()->getMeshBuffer(i), vector3df(100.0,100.0f,100.0f));
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(-395.0f,100.0f,-100.0f),vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodeMirror->getMaterialCount(); i++) 
            {
                nodeMirror->getMaterial(i).Lighting = true;
            }
            nodeMirror->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            //nodeMirror->setAutomaticCulling(EAC_BOX);

            NodeInit(nodeRoom, roomScale);
            //NodeInit(nodeFridge, roomScale);
            NodeInit(nodeBenchUpper, roomScale);
            NodeInit(nodeBenchLower, roomScale);
            //NodeInit(nodeMicrowave, roomScale);
            //NodeInit(nodeMirror, roomScale);
            //NodeInit(nodePlant, roomScale);
            NodeInit(nodeSink, roomScale);
            //NodeInit(nodeDoor, roomScale);
            //NodeInit(nodePantry, roomScale);
            
            game->LoadLevel();
            this->currentState = EGS_GAME;
        }
        break;
    }
}

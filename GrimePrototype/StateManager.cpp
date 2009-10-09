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
void StateManager::NodeInit(IMeshSceneNode* node, vector3df scale)
{
    for (u32 i = 0; i < node->getMaterialCount(); i++) 
    {
        node->getMaterial(i).Lighting = true;
    }

    //normalize mesh's normals as it has been scaled
    node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    //node->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
    node->setAutomaticCulling(EAC_BOX);
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

            //room = smgr->getMesh("media/level/kitchen retex final.b3d")->getMesh(1);
            
            vector3df roomScale(1200.0f,1200.0f,1200.0f);
            vector3df roomTranslate(0.0f,0.0f,0.0f);
            vector3df roomRotate(90.0f,0.0f,0.0f);
            IMesh* meshRoom = smgr->getMesh("media/roomplace.b3d")->getMesh(0);
            IMesh* meshFridge = smgr->getMesh("media/fridge.b3d")->getMesh(0);
            IMesh* meshBenchUpper = smgr->getMesh("media/kitchenbenchupper.b3d")->getMesh(0);
            IMesh* meshBenchLower = smgr->getMesh("media/kitchenbenchlower.b3d")->getMesh(0);
            IMesh* meshMicrowave = smgr->getMesh("media/kitchenmicrowave.b3d")->getMesh(0);
            IMesh* meshMirror = smgr->getMesh("media/kitchenmirror.b3d")->getMesh(0);
            IMesh* meshPlant = smgr->getMesh("media/kitchenplant.b3d")->getMesh(0);
            IMesh* meshSink = smgr->getMesh("media/kitchensink.b3d")->getMesh(0);
            IMesh* meshPantry = smgr->getMesh("media/pantry.b3d")->getMesh(0);

            IMeshSceneNode* nodeRoom = smgr->addMeshSceneNode(meshRoom, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodeFridge = smgr->addMeshSceneNode(meshFridge, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodeBenchUpper = smgr->addMeshSceneNode(meshBenchUpper, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodeBenchLower = smgr->addMeshSceneNode(meshBenchLower, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodeMicrowave = smgr->addMeshSceneNode(meshMicrowave, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodeMirror = smgr->addMeshSceneNode(meshMirror, 0, -1, roomTranslate, roomRotate, roomScale);
            //IMeshSceneNode* nodePlant = smgr->addMeshSceneNode(meshPlant, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodeSink = smgr->addMeshSceneNode(meshSink, 0, -1, roomTranslate, roomRotate, roomScale);
            IMeshSceneNode* nodePantry = smgr->addMeshSceneNode(meshPantry, 0, -1, roomTranslate, roomRotate, roomScale);
            
            IMeshSceneNode* nodePlantTwo = smgr->addMeshSceneNode(meshPlant, 0, -1, vector3df(-120.0f,0.0f,-10.0f), vector3df(90.0f,0.0f,0.0f), vector3df(100.0f,100.0f,100.0f));
            for (u32 i = 0 ; i < nodePlantTwo->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodePlantTwo->getMesh()->getMeshBuffer(i), vector3df(100.0,100.0f,100.0f));
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(-120.0f,0.0f,-10.0f),vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodePlantTwo->getMaterialCount(); i++) 
            {
                nodePlantTwo->getMaterial(i).Lighting = true;
            }
            nodePlantTwo->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            nodePlantTwo->setAutomaticCulling(EAC_BOX);
            
            IMeshSceneNode* nodeFridge = smgr->addMeshSceneNode(meshFridge, 0, -1, vector3df(20.0f,0.0f,-100.0f), vector3df(90.0f,0.0f,0.0f), vector3df(130.0f,130.0f,130.0f));
            for (u32 i = 0 ; i < nodeFridge->getMesh()->getMeshBufferCount(); ++i) 
            {
                //first calculate the mesh triangles and make physx object
                IPhysxMesh* triMesh = physxManager->createTriangleMesh(nodeFridge->getMesh()->getMeshBuffer(i), vector3df(130.0,130.0f,130.0f));
                //secondly add the object to the world
                physxManager->createTriangleMeshObject(triMesh,vector3df(20.0f,0.0f,-100.0f),vector3df(90.0f,0.0f,0.0f));
            }
            for (u32 i = 0; i < nodeFridge->getMaterialCount(); i++) 
            {
                nodeFridge->getMaterial(i).Lighting = true;
            }
            nodeFridge->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
            nodeFridge->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
            nodeFridge->setAutomaticCulling(EAC_BOX);
            
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
            nodeMicrowave->setAutomaticCulling(EAC_BOX);
            
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
            nodeMirror->setAutomaticCulling(EAC_BOX);
            
            NodeInit(nodeRoom, roomScale);
            //NodeInit(nodeFridge, roomScale);
            NodeInit(nodeBenchUpper, roomScale);
            NodeInit(nodeBenchLower, roomScale);
           // NodeInit(nodeMicrowave, roomScale);
            //NodeInit(nodeMirror, roomScale);
            //NodeInit(nodePlant, roomScale);
            NodeInit(nodeSink, roomScale);
            NodeInit(nodePantry, roomScale);
            
            game->LoadLevel();
            this->currentState = EGS_GAME;
        }
        break;
    }
}

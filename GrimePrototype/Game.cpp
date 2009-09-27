#include "Game.h"

Game::Game(ISceneManager* smgr, ISoundEngine* soundEngine, IPhysxManager* physxManager, EffectHandler* effect)
{
    this->smgr = smgr;
    this->soundEngine = soundEngine;
    this->physxManager = physxManager;
    this->effect = effect;
    
    startPosition = vector3df(0.0f,20.0f,0.0f);
    this->CreateCamera();
    player = new Player(smgr, soundEngine, physxManager, cameraPair, effect);
    spawnManager = new SpawnManager(smgr, soundEngine, physxManager, &enemyObjects, player);
    // Preload texture animators
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
}

Game::~Game(void)
{
}

void Game::CreateCamera() {
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
    cameraPair->camera->setPosition(startPosition);
    cameraPair->camera->setTarget(core::vector3df(0.0f,32.0f,5.0f));
    cameraPair->SceneNode = cameraPair->camera;
    vector3df physxStartPos = startPosition;
    physxStartPos.Y += 100.0f;
    cameraPair->PhysxObject = physxManager->createSphereObject(startPosition, core::vector3df(0,0,0), 15.0f, 30000.0f);
    cameraPair->PhysxObject->setAngularDamping(1000.0f); // Stops the sphere from rolling
    cameraPair->PreviousPosition = startPosition;
    cameraPair->CameraOffset = core::vector3df(0,10,0);
    cameraPair->camera->setFarValue(2200.0f);
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

void Game::CreateImpactEffect( vector3df position, vector3df normal )
{
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
    bill->setMaterialTexture(0, smgr->getVideoDriver()->getTexture("media/explosion/01.jpg"));
    bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
    // Add the animator
    bill->addAnimator(anim);
    anim->drop();
    // create deletion animator to automatically remove the billboard
    anim = smgr->createDeleteAnimator(10*10);

    bill->addAnimator(anim);
    anim->drop();
}

void Game::Update( s32 time )
{
    cameraPair->updateTransformation();
    player->Update(time);
    //spawnManager->Update(time);
    for (u32 i = 0; i < enemyObjects.size(); i++) {
        enemyObjects[i]->Update(time);
        //effect->addEffectToNode(enemyObjects[i]->pair->SceneNode,(E_EFFECT_TYPE)5);
    }
    for (u32 i = 0; i < projectileObjects.size(); i++)
    {
        projectileObjects[i]->Update(time);
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

//simple random number function
s32 Game::GetRandom(s32 upper) {
    return (rand() % upper) + 1;
}

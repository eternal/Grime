#include "Projectile.h"
#include <iostream>
//#define WEAPONDEBUG 1

Projectile::Projectile( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IPhysxManager* manager, SPhysxAndNodePair* pair, core::array<Projectile*>* objectArray, core::array<video::ITexture*> explosionTextures, core::array<Enemy*>* enemyArray, core::array<SPhysxAndNodePair*>* clutterArray, core::array<Block*>* blockArray)
{
    this->smgr = sceneManager;
    this->soundEngine = soundEngine;
    this->physxManager = manager;
    this->pair = pair;
    this->projectileObjects = objectArray;
    this->explosionTextures = explosionTextures;
    this->enemyArray = enemyArray;
    this->clutterArray = clutterArray;
    this->blockArray = blockArray;
    active = true;
    timeElapsed = 0;
    radius = 250.0f;
    power = 5;
    projectileFlight = soundEngine->play2D("media/sounds/weapons/rpg/Flight4.wav", true, true, true);
}

Projectile::~Projectile(void)
{

}
void Projectile::DamageTargets(vector3df pos, f32 radius, s32 power) 
{
    for (u32 i = 0; i < enemyArray->size(); i++)
    {
        try 
        {
            Enemy* enemy = (*enemyArray)[i];
            vector3df direction = enemy->pair->SceneNode->getAbsolutePosition() - this->pair->SceneNode->getAbsolutePosition(); 
            f32 distance = direction.getLength();
            if (distance <= radius) //explosion radius
            {
                f32 hitStrength = ((1 - (distance/radius)) * power) + 2.0f;
                enemy->health -= hitStrength;
                if (enemy->health > 0)
                {
                    enemy->pair->PhysxObject->setLinearVelocity(direction.normalize() * (hitStrength * 50.0f));
                    enemy->pair->updateTransformation();
                }
                std::cout << "Target hit: " << hitStrength << " damage." << " Distance: " << distance << std::endl;
            }
        }
        catch (...)
        {
            std::cerr << "Exception caught in Projectile::DamageTargets. Possible corruption of enemyArray." << std::endl;
        }
        
    }
    for (u32 i = 0; i < blockArray->size(); i++)
    {
        try 
        {
            Block* block = (*blockArray)[i];
            vector3df direction = block->pair->SceneNode->getAbsolutePosition() - this->pair->SceneNode->getAbsolutePosition(); 
            f32 distance = direction.getLength();
            if (distance <= radius) //explosion radius
            {
                f32 hitStrength = ((1 - (distance/radius)) * power) + 2.0f;
                block->pair->PhysxObject->setLinearVelocity(direction.normalize() * (hitStrength * 8.0f));
                block->pair->updateTransformation();
            }
        }
        catch (...)
        {
            std::cerr << "Exception caught in Projectile::DamageTargets. Possible corruption of blockArray." << std::endl;
        }

    }
    for (u32 i = 0; i < clutterArray->size(); i++)
    {
        try 
        {
            SPhysxAndNodePair* pair = (*clutterArray)[i];
            vector3df direction = pair->SceneNode->getAbsolutePosition() - this->pair->SceneNode->getAbsolutePosition(); 
            f32 distance = direction.getLength();
            if (distance <= radius) //explosion radius
            {   
                f32 hitStrength = ((1 - (distance/radius)) * power) + 2.0f;
                pair->PhysxObject->setLinearVelocity(direction.normalize() * (hitStrength * 100.0f));
            }
        }
        catch (...)
        {
            std::cerr << "Exception caught in Projectile::DamageTargets. Possible corruption of clutterArray." << std::endl;
        }

    }
}


void Projectile::Update( s32 time )
{
    if (active)
    {
        projectileFlight->setIsPaused(false);
        try 
        {
            pair->updateTransformation();
            //draw rocket trail
            scene::ISceneNodeAnimator* anim = NULL;
            // create animation for explosion
            anim = smgr->createTextureAnimator(explosionTextures, 100, false);
            // create explosion billboard
            scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(40,40), this->pair->SceneNode->getAbsolutePosition());
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
            this->pair->PhysxObject->setAngularDamping(100000.0f);
            line3df line;
            line.start = this->pair->SceneNode->getAbsolutePosition();
            vector3df vel;
            this->pair->PhysxObject->getLinearVelocity(vel);
            line.end = line.start + (vel.normalize() * 5000.0f);
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
                        if (dis < dis2) 
                        {
                            closestObject = ray;
                        }
                    }
                }
                if (closestObject.Object)
                {
                    if (closestObject.Object->getType() != EOT_SPHERE)
                    {
                        f32 distance = (closestObject.HitPosition - line.start).getLength();      
#ifdef WEAPONDEBUG
                        std::cout << distance << std::endl;
#endif	                
                        if (closestObject.Object->getType() != EOT_SPHERE)
                        {

                            if (distance <= 105.0f)
                            {
                                Detonate();
                            }
                        }      
                    }
                }
            }
            timeElapsed += time;
            if (timeElapsed >= 5000)
            {
                Detonate();
            }
        }
        catch (char *str)
        {
            std::cout << str << std::endl;
        }
   }     
}

void Projectile::Detonate()
{
    physxManager->removePhysxObject(pair->PhysxObject);
    projectileFlight->setIsPaused(true);
#ifdef WEAPONDEBUG
    std::cout << "IMPACT" << std::endl;
#endif    
    u32 soundSelect = (rand() % 5);
    if (soundSelect == 0)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom1.wav");
    }
    else if (soundSelect == 1)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom2.wav");
    }
    else if (soundSelect == 2)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom3.wav");
    }
    else if (soundSelect == 3)
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom4.wav");
    }
    else
    {
        soundEngine->play2D("media/sounds/weapons/rpg/Boom5.wav");
    }
    scene::ISceneNodeAnimator* anim = NULL;
    // create animation for explosion
    anim = smgr->createTextureAnimator(explosionTextures, 100, false);
    // create explosion billboard
    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(300,300), this->pair->SceneNode->getAbsolutePosition());
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

    this->DamageTargets(pair->SceneNode->getAbsolutePosition(), radius, power);
    //physxManager->createExplosion(pair->SceneNode->getPosition(), 0.0f, power * 10000.0f, power * 100000.0f, 0.4f);
    this->active = false;
    this->pair->SceneNode->setVisible(false);    
    this->pair->SceneNode->remove();
    for (u32 i = 0; i < projectileObjects->size(); i++)
    {
        if ((*projectileObjects)[i]->pair == this->pair)
        {
            projectileObjects->erase(i);
        }
    }
    
}
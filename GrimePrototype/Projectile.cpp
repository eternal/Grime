#include "Projectile.h"
#include <iostream>

Projectile::Projectile( scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IPhysxManager* manager, SPhysxAndNodePair* pair, core::array<Projectile*>* objectArray, core::array<video::ITexture*> explosionTextures, core::array<Enemy*>* enemyArray )
{
    this->smgr = sceneManager;
    this->soundEngine = soundEngine;
    this->physxManager = manager;
    this->pair = pair;
    this->projectileObjects = objectArray;
    this->explosionTextures = explosionTextures;
    this->enemyArray = enemyArray;
    active = true;
    timeElapsed = 0;
    radius = 200.0f;
    power = 5;
}

Projectile::~Projectile(void)
{

}
void Projectile::DamageTargets(vector3df pos, f32 radius, s32 power) 
{
    for (u32 i = 0; i < enemyArray->size(); i++)
    {
        Enemy* enemy = (*enemyArray)[i];
        f32 distance = (enemy->pair->SceneNode->getAbsolutePosition() - this->pair->SceneNode->getAbsolutePosition()).getLength();
        if (distance <= radius) //explosion radius
        {
            f32 hitStrength = ((distance/radius) * power) + 2.0f;
            enemy->health -= hitStrength;
            std::cout << "Target hit: " << hitStrength << " damage." << "Distance: " << distance << std::endl;
        }
    }
}


void Projectile::Update( s32 time )
{
    if (active)
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
        bill->setMaterialTexture(0, smgr->getVideoDriver()->getTexture("media/explosion/01.jpg"));
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
	        for (u32 i = 0; i < rayArray.size(); ++i) {
	            SRaycastHitData ray = rayArray[i];
	            if (ray.Object->getType() != EOT_SPHERE)
	            {
	                f32 dis = (ray.HitPosition - line.start).getLength();
	                f32 dis2 = (closestObject.HitPosition - line.start).getLength();
	                if (dis < dis2) {
	                    closestObject = ray;
	                }
	            }
	        }
	        std::cout << (E_OBJECT_TYPE)closestObject.Object->getType() << std::endl;
	        if (closestObject.Object)
	        {
	            if (closestObject.Object->getType() != EOT_SPHERE)
	            {
	                f32 distance = (closestObject.HitPosition - line.start).getLength();      
	                std::cout << distance << std::endl;
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
}

void Projectile::Detonate()
{
    std::cout << "IMPACT" << std::endl;
    scene::ISceneNodeAnimator* anim = NULL;
    // create animation for explosion
    anim = smgr->createTextureAnimator(explosionTextures, 100, false);
    // create explosion billboard
    scene::IBillboardSceneNode* bill = smgr->addBillboardSceneNode(smgr->getRootSceneNode(), core::dimension2d<f32>(300,300), this->pair->SceneNode->getAbsolutePosition());
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

    this->DamageTargets(pair->SceneNode->getAbsolutePosition(), radius, power);
    physxManager->createExplosion(pair->SceneNode->getAbsolutePosition(), radius * 3.0f, power * 1000000.0f, power * 1000000.0f, 0.2f);
    this->active = false;
    this->pair->SceneNode->setVisible(false);
    physxManager->removePhysxObject(pair->PhysxObject);
}
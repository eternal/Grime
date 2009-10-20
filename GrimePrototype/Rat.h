#pragma once
#include "enemy.h"

class Rat :
    public Enemy
{
public:
    Rat(scene::ISceneManager* sceneManager, irrklang::ISoundEngine* soundEngine, IAnimatedMesh* mesh, IPhysxManager* manager, core::array<Enemy*>* objectArray, Player* player, core::array<video::ITexture*>* explosionTextures, vector3df position = vector3df(-501.0f,100.0f,-230.0f));
    ~Rat(void);
    
    IAnimatedMesh* mesh;
    s32 endingExplosionTimer;
    u32 explosionPhase;
    core::array<video::ITexture*>* explosionTextures;
    
    vector3df previousPosition;
    f32 distanceSinceFootstep;
    
    s32 roarTimer;
    
    void Update(s32 time);
    void CheckPhase();
    void CreateExplosion( vector3df position, bool massive );
    s32 GetRandom(s32 upper);
};

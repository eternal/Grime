#pragma once
#include <irrlicht.h>


class followingCamera
    {
    irr::scene::ISceneNode* m_pTargetNode;
    irr::scene::ICameraSceneNode* m_cam;

    irr::core::vector3df lastTargetPos;

    irr::f32 m_height; //distance above object camera tries to float at
    irr::f32 m_leash;  //max distance object can be from camera before it moves
    irr::f32 m_speed;  //rate at which camera moves per Update()

    public:
        // class constructor
        followingCamera(irr::scene::ISceneNode* targetNode, irr::scene::ISceneManager* smgr,
            irr::f32 height = 80.0f, irr::f32 leash = 100.0f, irr::f32 speed = 1.0f, irr::s32 id = -1);
        // class destructor
        ~followingCamera();

        irr::scene::ICameraSceneNode* getCam() {  return m_cam; }

        void Update();
    };

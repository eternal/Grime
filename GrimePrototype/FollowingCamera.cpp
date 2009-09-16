#include "FollowingCamera.h"

// class constructor
followingCamera::followingCamera(irr::scene::ISceneNode* targetNode, irr::scene::ISceneManager* smgr,
                                 irr::f32 height /*30.0f*/, irr::f32 leash /*100.0f*/, irr::f32 speed /*5.0f*/, irr::s32 id /*-1*/)
    {
    m_speed = speed;
    m_height = height;
    m_leash = leash;

    //create a camera
    lastTargetPos = targetNode->getPosition();
    m_cam = smgr->addCameraSceneNode(0, lastTargetPos + irr::core::vector3df(0, 20, 20), lastTargetPos, id );

    m_pTargetNode = targetNode;
    m_pTargetNode->grab(); //make sure the node wont dissappear on us
    }

// class destructor
followingCamera::~followingCamera()
    {
    m_pTargetNode->drop(); //make sure we let go of the node resource
    }

void followingCamera::Update() {
    if(!m_cam || !m_pTargetNode) return;

    irr::core::vector3df currTargetPos = m_pTargetNode->getPosition();

    //if too far away, move camera closer
    irr::core::vector3df camToTarg = currTargetPos - m_cam->getPosition() ;

    //leash is only in the X-Z plane, so only count distance using X and Z
    irr::core::vector2df xzDist( camToTarg.X, camToTarg.Z );

    if(xzDist.getLength() > m_leash) { //need to move closer
        camToTarg = irr::core::vector3df( xzDist.X, 0, xzDist.Y);
        camToTarg = camToTarg.normalize() * m_speed;

        //set X-Z position
        m_cam->setPosition( m_cam->getPosition() + camToTarg ); //move closer

        //set Y position
        irr::f32 h_tolerance = m_height / 10;  //the ammount of leway given to how close we need to be to the right height
        irr::f32 h_delta = m_cam->getPosition().Y - currTargetPos.Y + m_height; //distance from prefered height position
        if( m_cam->getPosition().Y < ( currTargetPos.Y + m_height) - h_tolerance) {

            m_cam->setPosition( irr::core::vector3df( m_cam->getPosition().X, m_cam->getPosition().Y + h_delta/2, m_cam->getPosition().Z) );
            }else if( m_cam->getPosition().Y > ( currTargetPos.Y + m_height) + h_tolerance ) {
                m_cam->setPosition( irr::core::vector3df( m_cam->getPosition().X, m_cam->getPosition().Y - h_delta/2, m_cam->getPosition().Z) );
            }
        }

    m_cam->setTarget( currTargetPos ); //look at Target position
    lastTargetPos = currTargetPos;
    }

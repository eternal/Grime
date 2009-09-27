#include "Block.h"

Block::Block(scene::ISceneManager* smgr, IPhysxManager* physxManager, core::array<Enemy*>* enemyObjects)
{
    core::line3df line;
    line.start = smgr->getActiveCamera()->getPosition();
    //put the end of the line off in the distance 
    line.end = line.start + (smgr->getActiveCamera()->getTarget() - line.start).normalize() * 5000.0f;
    //access the physics engine to find the intersection point
    core::array<SRaycastHitData> rayArray = physxManager->raycastAllRigidObjects(line);
    //core::array<SRaycastHitData> filteredRayArray;
    SRaycastHitData closestObject;
    closestObject = rayArray[0];
    for (u32 i = 0; i < rayArray.size(); ++i) {
        SRaycastHitData ray = rayArray[i];
        if (ray.Object->getType() == EOT_TRIANGLE_MESH)
        {
            f32 dis = (ray.HitPosition - line.start).getLength();
            f32 dis2 = (closestObject.HitPosition - line.start).getLength();
            std::cout << "Test Distance: " << dis << std::endl;
            std::cout << "Current Closest: " << dis2 << std::endl;
            if (dis < dis2) {
                closestObject = ray;
            }
        }
    }
    for (u32 i = 0; i < enemyObjects->size(); i++)
    {
        Enemy* enemy = (*enemyObjects)[i];
        f32 distance = (enemy->pair->SceneNode->getAbsolutePosition() - closestObject.HitPosition).getLength();
        if (distance <= 32.0f)
        {
            enemy->health = 0;
        }
    }
    std::cout << "=========" << std::endl;
    if (closestObject.Object->getType() == EOT_TRIANGLE_MESH) {
        vector3df scale(1,1,1);
        vector3df rot(0,0,0);
        closestObject.HitPosition.Y += scale.Y / 2;

        vector3df blockPosition = closestObject.HitPosition;
        //Quantise block positions
        f32 blockPositionX = round_(blockPosition.X/30) * 30;
        f32 blockPositionZ = round_(blockPosition.Z/40) * 40;
        ///TODO FIX Y QUANTISE
        //f32 blockPositionY = round_(blockPosition.Y/30) * 30;
        blockPosition.X = blockPositionX;
        blockPosition.Z = blockPositionZ;
        vector3df blockPhysicsPosition = blockPosition;
        blockPhysicsPosition.Y -= scale.Y / 2;
        blockPhysicsPosition.X += scale.X / 6.25f;
        pair = new SPhysxAndNodePair;

        //IMesh* cubeMesh = smgr->getMesh("media/cube.obj");
        IMesh* cubeMesh = smgr->getMesh("media/block.obj");
        //pair->SceneNode = smgr->addMeshSceneNode(cubeMesh, 0, -1, temp, vector3df(0,0,0), scale);
        //pair->PhysxObject = physxManager->createBoxObject(intersection, core::vector3df(0,0,0), scale/2.0f, 30000000.0f, &(vector3df(0,0,0)));                        
        pair->PhysxObject = physxManager->createTriangleMeshObject(physxManager->createTriangleMesh(cubeMesh->getMeshBuffer(0), scale), blockPhysicsPosition);
        //pair->SceneNode = smgr->addCubeSceneNode(1, 0, -1, intersection, rot, scale);
        pair->SceneNode = smgr->addMeshSceneNode(cubeMesh, 0, -1, blockPosition, rot, scale);
        pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);            
    }
}

Block::~Block(void)
{
}

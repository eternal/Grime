#include "ShapeCreation.h"
#include "CGridSceneNode.h"

c8 textures[NUM_TEXTURES][256] = {	"../media/blue.JPG",
									"../media/green.JPG",
									"../media/pink.JPG",
									"../media/red.JPG",
									"../media/yellow.JPG"  };

// Creates a box at the specified position, of the specified size and with the specified intial velocity (useful for throwing it)
SPhysxAndNodePair* createBox(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, const core::vector3df& pos, const core::vector3df& scale, f32 density, core::vector3df* initialVelocity) {

	SPhysxAndNodePair* pair = new SPhysxAndNodePair;
	pair->PhysxObject = physxManager->createBoxObject(pos, core::vector3df(0,0,0), scale/2.0f, density, initialVelocity);
	pair->SceneNode = smgr->addCubeSceneNode(1, 0, -1, pos, core::vector3df(0,0,0), scale);
	pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);

	return pair;

}

// Create a stack of boxes, with the bottom row having the specified number of boxes
void createBoxStack(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, core::array<SPhysxAndNodePair*>& objects, f32 stackSize, f32 density, const core::vector3df& position) {

	const core::vector3df scale(4,4,4);
	const f32 spacing = -2.0f*physxManager->getSkinWidth();
	core::vector3df pos = position + core::vector3df(0.0f, scale.Y/2.0f, 0.0f);
	f32 offset = -stackSize * (scale.X + spacing) * 0.5f;

	while (stackSize > 0) {
		for (s32 i = 0 ; i < stackSize ; ++i) {
			pos.X = offset + (f32)i * (scale.X + spacing);
			objects.push_back(createBox(physxManager, smgr, driver, pos, scale, density));
		}

		offset += scale.X / 2.0f;
		pos.Y += (scale.Y + spacing);
		stackSize--;
	}

}

// Create a tower of the specified number of boxes
void createBoxTower(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, core::array<SPhysxAndNodePair*>& objects, f32 towerSize, f32 density, const core::vector3df& position) {

	const core::vector3df scale(4,4,4);
	const f32 spacing = -2.0f*physxManager->getSkinWidth();
	core::vector3df pos = position + core::vector3df(0.0f, scale.Y/2.0f, 0.0f);

	while (towerSize > 0) {
		objects.push_back(createBox(physxManager, smgr, driver, pos, scale, density));
		pos.Y += (scale.Y + spacing);
		towerSize--;
	}

}

// Creates a multishape actor from the bounding boxes of the meshbuffers in the specified mesh stuck together at the specified position, with the specified intial velocity (useful for throwing it)
SPhysxAndNodePair* createMeshBoundingBox(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, scene::IMesh* mesh, const core::vector3df& pos, const core::vector3df& scale, f32 density, core::vector3df* initialVelocity) {

	SPhysxAndNodePair* pair = new SPhysxAndNodePair;
	
	core::array<core::aabbox3df> boxes;
	core::aabbox3df box;
	for (u32 mb = 0 ; mb < mesh->getMeshBufferCount() ; ++mb) {
	  scene::IMeshBuffer* buffer = mesh->getMeshBuffer(mb);
	  core::aabbox3df box = buffer->getBoundingBox();
	  box.MinEdge /= 2.0f;
	  box.MinEdge *= scale;
	  box.MaxEdge /= 2.0f;
	  box.MaxEdge *= scale;
	  boxes.push_back(box);
	}

	pair->PhysxObject = physxManager->createMultiShapeObject(boxes, core::array<SSphereDesc>(), core::array<SCapsuleDesc>(), pos, core::vector3df(0,0,0), density, initialVelocity);
	pair->SceneNode = smgr->addMeshSceneNode(mesh, 0, -1, pos, core::vector3df(0,0,0), scale);
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 
	
	return pair;

}

// Create a stack of meshes, with the bottom row having the specified number of cubes
void createMeshStack(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, core::array<SPhysxAndNodePair*>& objects, scene::IMesh* mesh, f32 stackSize, f32 density) {

	const core::vector3df meshSize = mesh->getBoundingBox().getExtent();
	const f32 spacing = -2.0f*physxManager->getSkinWidth();
	core::vector3df pos(0.0f, meshSize.Y/2.0f, 0.0f);
	f32 offset = -stackSize * (meshSize.X + spacing) * 0.5f;

	while (stackSize > 0) {
		for (s32 i = 0 ; i < stackSize ; ++i) {
			pos.X = offset + (f32)i * (meshSize.X + spacing);
			objects.push_back(createMeshBoundingBox(physxManager, smgr, driver, mesh, pos, core::vector3df(1,1,1), density));
		}

		offset += meshSize.X / 2.0f;
		pos.Y += (meshSize.Y + spacing);
		stackSize--;
	}

}

// Creates a sphere at the specified position, of the specified size and with the specified intial velocity (useful for throwing it)
SPhysxAndNodePair* createSphere(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, const core::vector3df& pos, f32 radius, f32 density, core::vector3df* initialVelocity) {

	SPhysxAndNodePair* pair = new SPhysxAndNodePair;
	pair->PhysxObject = physxManager->createSphereObject(pos, core::vector3df(0,0,0), radius, density, initialVelocity);
	pair->SceneNode = smgr->addSphereSceneNode(radius, 12, 0, -1, pos, core::vector3df(0, 0, 0), core::vector3df(1.0f, 1.0f, 1.0f));
	pair->SceneNode->setMaterialTexture(0, driver->getTexture(textures[rand()%NUM_TEXTURES]));
	pair->SceneNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true); 
	
	return pair;

}

// Create a sphere from the camera with an intial velocity to throw it into the scene
SPhysxAndNodePair* createSphereFromCamera(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, scene::ICameraSceneNode* camera, f32 radius, f32 density) {

	core::vector3df t = camera->getPosition();
	core::vector3df vel = camera->getTarget() - camera->getPosition();
	vel.normalize();
	vel*=200.0f;

	return createSphere(physxManager, smgr, driver, t, radius, density, &vel);

}

// Add a plane at the specified distance from the origin and the specified normal
void addPlane(IPhysxManager* physxManager, scene::ISceneManager* smgr, f32 d, const core::vector3df& normal) {

	// Add a plane with the specified parameters
	physxManager->createPlaneObject(d, normal);
	// Add a grid scene node to visually represent the floor
	scene::CGridSceneNode* grid = new scene::CGridSceneNode(smgr->getRootSceneNode(), smgr, -1, 25, 1000, video::SColor(255,0,200,0), 0);
	grid->setPosition(normal * d);
	grid->drop();

}

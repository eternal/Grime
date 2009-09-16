#ifndef INC_SHAPECREATION_H
#define INC_SHAPECREATION_H

#include <irrlicht.h>
#include <IrrPhysx.h>

using namespace irr;
using namespace IrrPhysx;

#define NUM_TEXTURES 5
extern c8 textures[NUM_TEXTURES][256];

// A struct to hold both the physical and visual representations of an object
struct SPhysxAndNodePair {
	
	SPhysxAndNodePair() {
		PhysxObject = NULL;
		SceneNode = NULL;
	}

	void updateTransformation() {
		if (PhysxObject->getType() == EOT_CLOTH) { // If it's a cloth we just need to update it (for tearing purposes)
			((IClothPhysxObject*)PhysxObject)->update();
		} else { // Otherwise we update its transformation
			core::vector3df vec;
			// Update the node's position to that of the physx object
			PhysxObject->getPosition(vec);
			SceneNode->setPosition(vec);
			// Update the node's rotation to that of the physx object
			PhysxObject->getRotation(vec);
			SceneNode->setRotation(vec);
		}
	}

	IPhysxObject* PhysxObject;
	scene::ISceneNode* SceneNode;

};

// Box functions
SPhysxAndNodePair* createBox(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, const core::vector3df& pos, const core::vector3df& scale = core::vector3df(2,2,2), f32 density = 10.0f, core::vector3df* initialVelocity = NULL);
void createBoxStack(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, core::array<SPhysxAndNodePair*>& objects, f32 stackSize, f32 density = 10.0f, const core::vector3df& position = core::vector3df(0,0,0));
void createBoxTower(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, core::array<SPhysxAndNodePair*>& objects, f32 towerSize, f32 density = 10.0f, const core::vector3df& position = core::vector3df(0,0,0));

// Multishape functions
SPhysxAndNodePair* createMeshBoundingBox(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, scene::IMesh* mesh, const core::vector3df& pos,  const core::vector3df& scale = core::vector3df(1,1,1), f32 density = 10.0f, core::vector3df* initialVelocity = NULL);
void createMeshStack(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, core::array<SPhysxAndNodePair*>& objects, scene::IMesh* mesh, f32 stackSize, f32 density = 10.0f);

// Sphere functions
SPhysxAndNodePair* createSphere(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, const core::vector3df& pos, f32 radius = 2.0f, f32 density = 10.0f, core::vector3df* initialVelocity = NULL);
SPhysxAndNodePair* createSphereFromCamera(IPhysxManager* physxManager, scene::ISceneManager* smgr, video::IVideoDriver* driver, scene::ICameraSceneNode* camera, f32 radius = 2.0f, f32 density = 10.0f);

// Other shape functions
SPhysxAndNodePair* createCloth(IPhysxManager* physxManager, scene::ISceneManager* smgr, scene::IMeshBuffer* meshBuffer, const core::vector3df& pos, const core::vector3df& rotation = core::vector3df(0,0,0), const core::vector3df& scale = core::vector3df(1,1,1), f32 density = 10.0f, f32 pressure = -1.0f, bool tearable = false);
void addPlane(IPhysxManager* physxManager, scene::ISceneManager* smgr, f32 d, const core::vector3df& normal);

#endif /* INC_SHAPECREATION_H */

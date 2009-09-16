#ifndef INC_CCAPSULESCENENODE_H
#define INC_CCAPSULESCENENODE_H

#include <irrlicht.h>

using namespace irr;

class CCapsuleSceneNode : public scene::ISceneNode {

	public:

		//! constructor
		// height = distance between sphere centres
		// extent of the capsule is (2 * radius) + height
		CCapsuleSceneNode(f32 size, f32 height, u32 polyCountX, u32 polyCountY, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,
			const core::vector3df& position = core::vector3df(0,0,0),
			const core::vector3df& rotation = core::vector3df(0,0,0),
			const core::vector3df& scale = core::vector3df(1.0f, 1.0f, 1.0f));

		//! destructor
		virtual ~CCapsuleSceneNode();

		virtual void OnRegisterSceneNode();

		//! renders the node.
		virtual void render();

		//! returns the axis aligned bounding box of this node
		virtual const core::aabbox3d<f32>& getBoundingBox() const;

		//! returns the material based on the zero based index i. To get the amount
		//! of materials used by this scene node, use getMaterialCount().
		//! This function is needed for inserting the node into the scene hirachy on a
		//! optimal position for minimizing renderstate changes, but can also be used
		//! to directly modify the material of a scene node.
		virtual video::SMaterial& getMaterial(u32 i);

		//! returns amount of materials used by this scene node.
		virtual u32 getMaterialCount() const;

		//! Returns type of the scene node
		virtual scene::ESCENE_NODE_TYPE getType() const { return scene::ESNT_UNKNOWN; }

		//! Writes attributes of the scene node.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the scene node.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

		//! Creates a clone of this scene node and its children.
		virtual scene::ISceneNode* clone(scene::ISceneNode* newParent=0, scene::ISceneManager* newManager=0);

	private:
		scene::SMeshBuffer* Buffer;
		core::aabbox3d<f32> Box;
		f32 Radius;
		f32 Height;
		u32 PolyCountX;
		u32 PolyCountY;

		void createCapsuleMesh(f32 radius, f32 height, u32 polyCountX, u32 polyCountY);

};

#endif /* INC_CCAPSULESCENENODE_H */

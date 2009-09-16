#include "CCapsuleSceneNode.h"

//! constructor
CCapsuleSceneNode::CCapsuleSceneNode(f32 radius, f32 height, u32 polyCountX, u32 polyCountY, scene::ISceneNode* parent, scene::ISceneManager* mgr, s32 id,
			const core::vector3df& position, const core::vector3df& rotation, const core::vector3df& scale)
: scene::ISceneNode(parent, mgr, id, position, rotation, scale), Buffer(0),
	Radius(radius), Height(height), PolyCountX(polyCountX), PolyCountY(polyCountY)
{
	#ifdef _DEBUG
	setDebugName("CCapsuleSceneNode");
	#endif

	createCapsuleMesh(radius, height, polyCountX, polyCountY);
}



//! destructor
CCapsuleSceneNode::~CCapsuleSceneNode()
{
	if (Buffer)
		Buffer->drop();
}


void CCapsuleSceneNode::createCapsuleMesh(f32 radius, f32 height, u32 polyCountX, u32 polyCountY) {

	Buffer = new scene::SMeshBuffer();

	// we are creating the sphere mesh here.
	if (polyCountX < 2)
		polyCountX = 2;
	if (polyCountY < 2)
		polyCountY = 2;
	if (polyCountX * polyCountY > 32767) // prevent u16 overflow
	{
		if (polyCountX > polyCountY) // prevent u16 overflow
			polyCountX = 32767/polyCountY-1;
		else
			polyCountY = 32767/(polyCountX+1);
	}

	u32 polyCountXPitch = polyCountX+1; // get to same vertex on next level
	Buffer->Vertices.set_used((polyCountXPitch * polyCountY) + 2);
	Buffer->Indices.set_used((polyCountX * polyCountY) * 6);

	video::SColor clr(100, 255,255,255);

	u32 i=0;
	u32 level = 0;

	for (u32 p1 = 0; p1 < polyCountY-1; ++p1)
	{
		//main quads, top to bottom
		for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
		{
			const u32 curr = level + p2;
			Buffer->Indices[i] = curr + polyCountXPitch;
			Buffer->Indices[++i] = curr;
			Buffer->Indices[++i] = curr + 1;
			Buffer->Indices[++i] = curr + polyCountXPitch;
			Buffer->Indices[++i] = curr+1;
			Buffer->Indices[++i] = curr + 1 + polyCountXPitch;
			++i;
		}

		// the connectors from front to end
		Buffer->Indices[i] = level + polyCountX - 1 + polyCountXPitch;
		Buffer->Indices[++i] = level + polyCountX - 1;
		Buffer->Indices[++i] = level + polyCountX;
		++i;

		Buffer->Indices[i] = level + polyCountX - 1 + polyCountXPitch;
		Buffer->Indices[++i] = level + polyCountX;
		Buffer->Indices[++i] = level + polyCountX + polyCountXPitch;
		++i;
		level += polyCountXPitch;
	}

	const u32 polyCountSq = polyCountXPitch * polyCountY; // top point
	const u32 polyCountSq1 = polyCountSq + 1; // bottom point
	const u32 polyCountSqM1 = (polyCountY - 1) * polyCountXPitch; // last row's first vertex

	for (u32 p2 = 0; p2 < polyCountX - 1; ++p2)
	{
		// create triangles which are at the top of the sphere

		Buffer->Indices[i] = polyCountSq;
		Buffer->Indices[++i] = p2 + 1;
		Buffer->Indices[++i] = p2;
		++i;

		// create triangles which are at the bottom of the sphere

		Buffer->Indices[i] = polyCountSqM1 + p2;
		Buffer->Indices[++i] = polyCountSqM1 + p2 + 1;
		Buffer->Indices[++i] = polyCountSq1;
		++i;
	}

	// create final triangle which is at the top of the sphere

	Buffer->Indices[i] = polyCountSq;
	Buffer->Indices[++i] = polyCountX;
	Buffer->Indices[++i] = polyCountX-1;
	++i;

	// create final triangle which is at the bottom of the sphere

	Buffer->Indices[i] = polyCountSqM1 + polyCountX - 1;
	Buffer->Indices[++i] = polyCountSqM1;
	Buffer->Indices[++i] = polyCountSq1;

	// calculate the angle which separates all points in a circle
	const f64 AngleX = 2 * core::PI / polyCountX;
	const f64 AngleY = core::PI / polyCountY;

	i = 0;
	f64 axz;

	// we don't start at 0.

	f64 ay = 0;//AngleY / 2;
	
	f32 halfHeight = Height/2.0f;
	u32 halfPolyCountY = polyCountY/2 - 1;

	for (u32 y = 0; y < polyCountY; ++y)
	{
		ay += AngleY;
		const f64 sinay = sin(ay);
		axz = 0;

		// calculate the necessary vertices without the doubled one
		for (u32 xz = 0;xz < polyCountX; ++xz)
		{
			// calculate points position
			core::vector3df pos((f32)(radius * cos(axz) * sinay),
						(f32)(radius * cos(ay)),
						(f32)(radius * sin(axz) * sinay));
			if (y != halfPolyCountY) {
			  if (y > halfPolyCountY)
                pos.Y -= halfHeight;
			  else
                pos.Y += halfHeight;
            }
			
			// for spheres the normal is the position
			core::vector3df normal(pos);
			normal.normalize();

			// calculate texture coordinates via sphere mapping
			// tu is the same on each level, so only calculate once
			f32 tu = 0.5f;
			if (y==0)
			{
				if (normal.Y != -1.0f && normal.Y != 1.0f)
					tu = (f32)(acos(core::clamp(normal.X/sinay, -1.0, 1.0)) * 0.5 *core::RECIPROCAL_PI64);
				if (normal.Z < 0.0f)
					tu=1-tu;
			}
			else
				tu = Buffer->Vertices[i-polyCountXPitch].TCoords.X;
			Buffer->Vertices[i] = video::S3DVertex(pos.X, pos.Y, pos.Z,
						normal.X, normal.Y, normal.Z,
						clr, tu,
						(f32)(ay*core::RECIPROCAL_PI64));
			++i;
			axz += AngleX;
		}
		// This is the doubled vertex on the initial position
		Buffer->Vertices[i] = video::S3DVertex(Buffer->Vertices[i-polyCountX]);
		Buffer->Vertices[i].TCoords.X=1.0f;
		++i;
	}

	// the vertex at the top of the sphere
	Buffer->Vertices[i] = video::S3DVertex(0.0f,radius + halfHeight,0.0f, 0.0f,1.0f,0.0f, clr, 0.5f, 0.0f);

	// the vertex at the bottom of the sphere
	++i;
	Buffer->Vertices[i] = video::S3DVertex(0.0f,-radius - halfHeight,0.0f, 0.0f,-1.0f,0.0f, clr, 0.5f, 1.0f);

	// recalculate bounding box
	Buffer->BoundingBox.reset(Buffer->Vertices[i].Pos);
	Buffer->BoundingBox.addInternalPoint(Buffer->Vertices[i-1].Pos);
	Buffer->BoundingBox.addInternalPoint(radius,0.0f,0.0f);
	Buffer->BoundingBox.addInternalPoint(-radius,0.0f,0.0f);
	Buffer->BoundingBox.addInternalPoint(0.0f,0.0f,radius);
	Buffer->BoundingBox.addInternalPoint(0.0f,0.0f,-radius);

	// recalc normals (currently they're based on the mesh being a sphere.. which it's not...)
	//SceneManager->getMeshManipulator()->recalculateNormals(Buffer, true);

}

//! renders the node.
void CCapsuleSceneNode::render()
{
	video::IVideoDriver* driver = SceneManager->getVideoDriver();

	if (Buffer && driver)
	{
		driver->setMaterial(Buffer->getMaterial());
		driver->setTransform(video::ETS_WORLD, AbsoluteTransformation);
		driver->drawMeshBuffer(Buffer);
		if ( DebugDataVisible & scene::EDS_BBOX )
		{
			video::SMaterial m;
			m.Lighting = false;
			driver->setMaterial(m);
			driver->draw3DBox(Buffer->getBoundingBox(), video::SColor(255,255,255,255));
		}
	}
}



//! returns the axis aligned bounding box of this node
const core::aabbox3d<f32>& CCapsuleSceneNode::getBoundingBox() const
{
	return Buffer ? Buffer->getBoundingBox() : Box;
}


void CCapsuleSceneNode::OnRegisterSceneNode()
{
	if (IsVisible)
		SceneManager->registerNodeForRendering(this);

	scene::ISceneNode::OnRegisterSceneNode();
}


//! returns the material based on the zero based index i. To get the amount
//! of materials used by this scene node, use getMaterialCount().
//! This function is needed for inserting the node into the scene hirachy on a
//! optimal position for minimizing renderstate changes, but can also be used
//! to directly modify the material of a scene node.
video::SMaterial& CCapsuleSceneNode::getMaterial(u32 i)
{
	if (i>0 || !Buffer)
		return scene::ISceneNode::getMaterial(i);
	else
		return Buffer->getMaterial();
}


//! returns amount of materials used by this scene node.
u32 CCapsuleSceneNode::getMaterialCount() const
{
	return 1;
}


//! Writes attributes of the scene node.
void CCapsuleSceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	scene::ISceneNode::serializeAttributes(out, options);

	out->addFloat("Radius", Radius);
	out->addInt("PolyCountX", PolyCountX);
	out->addInt("PolyCountY", PolyCountY);
}


//! Reads attributes of the scene node.
void CCapsuleSceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	f32 oldRadius = Radius;
	u32 oldPolyCountX = PolyCountX;
	u32 oldPolyCountY = PolyCountY;

	Radius = in->getAttributeAsFloat("Radius");
	PolyCountX = in->getAttributeAsInt("PolyCountX");
	PolyCountY = in->getAttributeAsInt("PolyCountY");
	// legacy values read for compatibility with older versions
	u32 polyCount = in->getAttributeAsInt("PolyCount");
	if (PolyCountX ==0 && PolyCountY == 0)
		PolyCountX = PolyCountY = polyCount;

	Radius = core::max_(Radius, 0.0001f);

	if ( !core::equals(Radius, oldRadius) || PolyCountX != oldPolyCountX || PolyCountY != oldPolyCountY)
	{
		if (Buffer)
			Buffer->drop();
		createCapsuleMesh(Radius, Height, PolyCountX, PolyCountY);
	}

	scene::ISceneNode::deserializeAttributes(in, options);
}

//! Creates a clone of this scene node and its children.
scene::ISceneNode* CCapsuleSceneNode::clone(scene::ISceneNode* newParent, scene::ISceneManager* newManager)
{
	if (!newParent)
		newParent = Parent;
	if (!newManager)
		newManager = SceneManager;

	CCapsuleSceneNode* nb = new CCapsuleSceneNode(Radius, Height, PolyCountX, PolyCountY, newParent, 
		newManager, ID, RelativeTranslation);

	nb->cloneMembers(this, newManager);
	nb->getMaterial(0) = Buffer->getMaterial();

	nb->drop();
	return nb;
}

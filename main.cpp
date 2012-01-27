
#include <irrlicht.h>
#include "Target.h"

using namespace irr;

enum
{
	// I use this ISceneNode ID to indicate a scene node that is
	// not pickable by getSceneNodeAndCollisionPointFromRay()
	ID_IsNotPickable = 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be picked by ray selection.
	IDFlag_IsPickable = 1 << 0,

	// I use this flag in ISceneNode IDs to indicate that the
	// scene node can be highlighted.  In this example, the
	// homonids can be highlighted, but the level mesh can't.
	IDFlag_IsHighlightable = 1 << 1
};

int main()
{

	IrrlichtDevice *device =
		createDevice(video::EDT_OPENGL, core::dimension2d<u32>(800, 600), 32, false);

	if (device == 0)
		return 1; // could not create selected driver.

	video::IVideoDriver* driver = device->getVideoDriver();
	scene::ISceneManager* smgr = device->getSceneManager();

	device->getFileSystem()->addZipFileArchive("media/map-20kdm2.pk3");

	scene::IAnimatedMesh* q3levelmesh = smgr->getMesh("20kdm2.bsp");
	scene::IMeshSceneNode* q3node = 0;

	// The Quake mesh is pickable, but doesn't get highlighted.
	if (q3levelmesh)
		q3node = smgr->addOctreeSceneNode(q3levelmesh->getMesh(0), 0, IDFlag_IsPickable);

	/*
	So far so good, we've loaded the quake 3 level like in tutorial 2. Now,
	here comes something different: We create a triangle selector. A
	triangle selector is a class which can fetch the triangles from scene
	nodes for doing different things with them, for example collision
	detection. There are different triangle selectors, and all can be
	created with the ISceneManager. In this example, we create an
	OctreeTriangleSelector, which optimizes the triangle output a little
	bit by reducing it like an octree. This is very useful for huge meshes
	like quake 3 levels. After we created the triangle selector, we attach
	it to the q3node. This is not necessary, but in this way, we do not
	need to care for the selector, for example dropping it after we do not
	need it anymore.
	*/
	gui::IGUIEnvironment* guienv = device->getGUIEnvironment();
	gui::IGUIFont* font = guienv->getFont("media/bigfont.png");
	gui::IGUIStaticText* textinfo = guienv->addStaticText(L"", core::rect<s32>(10,10,200,50), false);
	textinfo->setOverrideColor(video::SColor(120,255,255,255));
	textinfo->setOverrideFont(font);
	textinfo->setWordWrap(true);

	scene::ITriangleSelector* selector = 0;

	if (q3node)
	{
		q3node->setPosition(core::vector3df(-1350,-130,-1400));

		selector = smgr->createOctreeTriangleSelector(
				q3node->getMesh(), q3node, 128);
		q3node->setTriangleSelector(selector);
		// We're not done with this selector yet, so don't drop it.
	}

	 SKeyMap keyMap[8];
	 keyMap[0].Action = EKA_JUMP_UP;
	 keyMap[0].KeyCode = KEY_SPACE;

	 keyMap[1].Action = EKA_MOVE_FORWARD;
	 keyMap[1].KeyCode = KEY_KEY_W;

	 keyMap[2].Action = EKA_MOVE_BACKWARD;
	 keyMap[2].KeyCode = KEY_KEY_S;

	 keyMap[3].Action = EKA_STRAFE_LEFT;
	 keyMap[3].KeyCode = KEY_KEY_A;

	 keyMap[4].Action = EKA_STRAFE_RIGHT;
	 keyMap[4].KeyCode = KEY_KEY_D;

	scene::ICameraSceneNode* camera =
		smgr->addCameraSceneNodeFPS(0, 100.0f, .3f, ID_IsNotPickable, keyMap, 5, true, 3.f);
	camera->setPosition(core::vector3df(50,50,-60));
	camera->setTarget(core::vector3df(-70,30,-60));

	if (selector)
	{
		scene::ISceneNodeAnimator* anim = smgr->createCollisionResponseAnimator(
			selector, camera, core::vector3df(30,50,30),
			core::vector3df(0,-10,0), core::vector3df(0,30,0));
		selector->drop(); // As soon as we're done with the selector, drop it.
		camera->addAnimator(anim);

		anim->drop();  // And likewise, drop the animator when we're done referring to it.
	}

	device->getCursorControl()->setVisible(false);

	// Add the billboard.
	scene::IBillboardSceneNode * bill = smgr->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
	bill->setMaterialTexture(0, driver->getTexture("media/particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	bill->setID(ID_IsNotPickable); // This ensures that we don't accidentally ray-pick it

	scene::IAnimatedMeshSceneNode* node = 0;

	video::SMaterial material;

	// This X files uses skeletal animation, but without skinning.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("media/dwarf/dwarf1.x"),
						0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setPosition(core::vector3df(160,-66,-10)); // Put its feet on the floor.
	node->setRotation(core::vector3df(0,-90,0)); // And turn it towards the camera.
	node->setLoopMode(true);
	node->setAnimationSpeed(10.f);
	node->setScale(core::vector3df(12,12,12));
    node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
//	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
    node->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(node->isDebugDataVisible()^scene::EDS_SKELETON));
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop();

	// This X files uses skeletal animation, but without skinning.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("media/dwarf/dwarf2.x"),
						0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setPosition(core::vector3df(-20,-66,-10)); // Put its feet on the floor.
	node->setRotation(core::vector3df(0,90,0)); // And turn it towards the camera.
	node->setLoopMode(true);
	node->setAnimationSpeed(10.f);
	node->setScale(core::vector3df(12,12,12));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
//	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR );
    node->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(node->isDebugDataVisible()^scene::EDS_SKELETON));
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop();


	// And this B3D file uses skinned skeletal animation.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("media/ninja/ninja.b3d"),
						0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setScale(core::vector3df(10, 10, 10));
	node->setPosition(core::vector3df(-70,-66,-120));
	node->setRotation(core::vector3df(0,90,0));
	node->setAnimationSpeed(20.f);
	node->getMaterial(0).NormalizeNormals = true;
	// Just do the same as we did above.
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop();
    node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    node->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(node->isDebugDataVisible()^scene::EDS_SKELETON));
	node->setFrameLoop(32,83);

	// And this B3D file uses skinned skeletal animation.
	node = smgr->addAnimatedMeshSceneNode(smgr->getMesh("media/zombie/zombie.b3d"),
						0, IDFlag_IsPickable | IDFlag_IsHighlightable);
	node->setScale(core::vector3df(5, 5, 5));
	node->setPosition(core::vector3df(50,-66,-120));
	node->setRotation(core::vector3df(0,90,0));
	node->setAnimationSpeed(10.f);
	node->getMaterial(0).NormalizeNormals = true;
	// Just do the same as we did above.
	selector = smgr->createTriangleSelector(node);
	node->setTriangleSelector(selector);
	selector->drop();
    node->setDebugDataVisible((scene::E_DEBUG_SCENE_TYPE)(node->isDebugDataVisible()^scene::EDS_SKELETON));
	node->setLoopMode(true);
	node->setFrameLoop(2,18);
	node->setFrameLoop(137,169);
//    node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
    node->setAnimationSpeed(5);
    node->setJointMode(scene::EJUOR_CONTROL);
	material.setTexture(0, 0);
	material.Lighting = false;


	// Add a light, so that the unselected nodes aren't completely dark.
	scene::ILightSceneNode * light = smgr->addLightSceneNode(0, core::vector3df(-60,100,50),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), 600.0f);
	light->setID(ID_IsNotPickable); // Make it an invalid target for selection.
	light = smgr->addLightSceneNode(0, core::vector3df(60,100,-150),
		video::SColorf(1.0f,1.0f,1.0f,1.0f), 600.0f);
	light->setID(ID_IsNotPickable); // Make it an invalid target for selection.

	// Remember which scene node is highlighted
	scene::ISceneNode* highlightedSceneNode = 0;
	scene::ISceneCollisionManager* collMan = smgr->getSceneCollisionManager();
	int lastFPS = -1;

	// draw the selection triangle only as wireframe
	material.Wireframe=true;
    scene::IBoneSceneNode *sbone;
    static int angles[] = {2, 3, 4, 4, 3, 2, 1, 0, 0, 1, 2};
//    node->animateJoints();   // not needed if you control all joints


    Target target(device);

	while(device->run())
	if (device->isWindowActive())
	{
		driver->beginScene(true, true, 0);
        int i = (int) node->getFrameNr();

        sbone = node->getJointNode((u32)9);
        sbone->positionHint
        sbone->setRotation(core::vector3df(20.0f*angles[i%11],0,0) );

        target.show();
		smgr->drawAll();
		guienv->drawAll();

		// Unlight any currently highlighted scene node
		if (highlightedSceneNode)
		{
			highlightedSceneNode->setMaterialFlag(video::EMF_LIGHTING, true);
			highlightedSceneNode = 0;
		}

		// All intersections in this example are done with a ray cast out from the camera to
		// a distance of 1000.  You can easily modify this to check (e.g.) a bullet
		// trajectory or a sword's position, or create a ray from a mouse click position using
		// ISceneCollisionManager::getRayFromScreenCoordinates()
		core::line3d<f32> ray;
		ray.start = camera->getPosition();
		ray.end = ray.start + (camera->getTarget() - ray.start).normalize() * 1000.0f;

		// Tracks the current intersection point with the level or a mesh
		core::vector3df intersection;
		// Used to show with triangle has been hit
		core::triangle3df hitTriangle;

		// This call is all you need to perform ray/triangle collision on every scene node
		// that has a triangle selector, including the Quake level mesh.  It finds the nearest
		// collision point/triangle, and returns the scene node containing that point.
		// Irrlicht provides other types of selection, including ray/triangle selector,
		// ray/box and ellipse/triangle selector, plus associated helpers.
		// See the methods of ISceneCollisionManager
		scene::ISceneNode * selectedSceneNode =
			collMan->getSceneNodeAndCollisionPointFromRay(
					ray,
					intersection, // This will be the position of the collision
					hitTriangle, // This will be the triangle hit in the collision
					IDFlag_IsPickable, // This ensures that only nodes that we have
							// set up to be pickable are considered
					0); // Check the entire scene (this is actually the implicit default)

		// If the ray hit anything, move the billboard to the collision position
		// and draw the triangle that was hit.
		if(selectedSceneNode)
		{
			bill->setPosition(intersection);

			// We need to reset the transform before doing our own rendering.
			driver->setTransform(video::ETS_WORLD, core::matrix4());
			driver->setMaterial(material);
			driver->draw3DTriangle(hitTriangle, video::SColor(220,255,200,0));

			// We can check the flags for the scene node that was hit to see if it should be
			// highlighted. The animated nodes can be highlighted, but not the Quake level mesh
			if((selectedSceneNode->getID() & IDFlag_IsHighlightable) == IDFlag_IsHighlightable)
			{
				highlightedSceneNode = selectedSceneNode;

				// Highlighting in this case means turning lighting OFF for this node,
				// which means that it will be drawn with full brightness.
				highlightedSceneNode->setMaterialFlag(video::EMF_LIGHTING, false);
			}
		}
		// We're all done drawing, so end the scene.
		driver->endScene();

		int fps = driver->getFPS();

		if (lastFPS != fps)
		{
			core::stringw str = L"Irrlicht Engine, FPS:";
			core::stringw istr = L"FPS: ";
			str += fps;
			istr += fps;
			textinfo->setText(istr.c_str());
			device->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
	}

	device->drop();

	return 0;
}

/*
**/

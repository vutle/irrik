/*
 * irrik.cpp
 *
 *  Created on: 13-12-2011
 *      Author: Marcin Załuski
 */

#include <irrlicht.h>
#include "Target.h"
#include "Input.h"
#include <iostream>
int main() {
	Input input;
	//create irrlicht opengl device
	IrrlichtDevice *dev = createDevice(video::EDT_OPENGL,
			core::dimension2d<u32>(800, 600), 32, false, false, false, &input);

	if (!dev)
		return EXIT_FAILURE;

	scene::ISceneManager *scene = dev->getSceneManager();
	video::IVideoDriver *drv = dev->getVideoDriver();

	//get maya like camera
	scene::ICameraSceneNode* camera = scene->addCameraSceneNodeMaya(NULL, 150,
			150, 150, 1, 1);

	//load an q3 map
	dev->getFileSystem()->addZipFileArchive("media/map-20kdm2.pk3");
	scene::IAnimatedMesh* q3map = scene->getMesh("20kdm2.bsp");
	scene::IMeshSceneNode* q3mapNode = scene->addOctreeSceneNode(
			q3map->getMesh(0));
	q3mapNode->setPosition(core::vector3df(-1500, -150, -1200));

	scene::IAnimatedMeshSceneNode* node = 0;

	video::SMaterial material;

	// Load a dwarf model
	node = scene->addAnimatedMeshSceneNode(
			scene->getMesh("media/dwarf/dwarf1.x"));
	node->setPosition(core::vector3df(0, -86, 50)); // Put its feet on the floor.
	node->setRotation(core::vector3df(0, -90, 0)); // And turn it towards the camera.
	node->setAnimationSpeed(10.f);
	node->setScale(core::vector3df(12, 12, 12));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));

	// Load a dwarf model
	node = scene->addAnimatedMeshSceneNode(
			scene->getMesh("media/dwarf/dwarf2.x"));
	node->setPosition(core::vector3df(-200, -86, 50)); // Put its feet on the floor.
	node->setRotation(core::vector3df(0, 90, 0)); // And turn it towards the camera.
	node->setAnimationSpeed(10.f);
	node->setScale(core::vector3df(12, 12, 12));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));

//	// Load a ninja model
	node = scene->addAnimatedMeshSceneNode(
			scene->getMesh("media/ninja/ninja.b3d"));

	node->setScale(core::vector3df(10, 10, 10));
	node->setPosition(core::vector3df(-200, -86, 150));
	node->setRotation(core::vector3df(0, 90, 0));
	node->setAnimationSpeed(20.f);
	node->getMaterial(0).NormalizeNormals = true;
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));
	node->setFrameLoop(32, 83);

	// Load a zobie model
	node = scene->addAnimatedMeshSceneNode(
			scene->getMesh("media/zombie/zombie.b3d"));
	node->setScale(core::vector3df(5, 5, 5));
	node->setPosition(core::vector3df(0, -86, 150));
	node->setRotation(core::vector3df(0, 90, 0));
	node->setAnimationSpeed(10.f);
	node->getMaterial(0).NormalizeNormals = true;
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));
	node->setLoopMode(true);
	node->setFrameLoop(2, 18);
	node->setFrameLoop(137, 169);
	node->setAnimationSpeed(5);
	node->setJointMode(scene::EJUOR_CONTROL);
//	material.setTexture(0, 0);
//	material.Lighting = false;

	//let there be light
	scene::ILightSceneNode * light = scene->addLightSceneNode(0,
			core::vector3df(0, 80, 100), video::SColorf(1.0f, 1.0f, 1.0f, 1.0f),
			800.0f);
	light = scene->addLightSceneNode(0, core::vector3df(-100, 80, 0),
			video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 800.0f);

	scene::IBillboardSceneNode * bill = scene->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, drv->getTexture("media/particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	bill->setPosition(core::vector3df(-100, 80, 0));

	bill = scene->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, drv->getTexture("media/particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	bill->setPosition(core::vector3df(0, 80, 100));

	gui::IGUIEnvironment* guienv = dev->getGUIEnvironment();
	gui::IGUIFont* font = guienv->getFont("media/bigfont.png");
	gui::IGUIStaticText* textinfo = guienv->addStaticText(L"", core::rect<s32>(10,10,200,50), false);
	textinfo->setOverrideColor(video::SColor(120,255,255,255));
	textinfo->setOverrideFont(font);
	textinfo->setWordWrap(true);
	int lastFPS = -1;
	camera->bindTargetAndRotation(false);
	//main loop
	while (dev->run()) {
		drv->beginScene(true, true, video::SColor(1));

		scene->drawAll();
		guienv->drawAll();

		drv->endScene();
		int fps = drv->getFPS();

		if (lastFPS != fps)
		{
			core::stringw str = L"Irrlicht, FPS: ";
			core::stringw istr = L"FPS: ";
			str += fps;
			istr += fps;
			textinfo->setText(istr.c_str());
			dev->setWindowCaption(str.c_str());
			lastFPS = fps;
		}

		if( input.isPressed(irr::KEY_ESCAPE) ){
			dev->drop();
			return EXIT_SUCCESS;
		}
		if( input.isPressed(irr::KEY_KEY_W) ){
			core::vector3df vec,normal;
			vec = camera->getTarget();

			std::cout<<"W"<<std::endl;
			std::cout<<"vec( "<<vec.X<<", "<<vec.Y<<", "<<vec.Z<<" )"<<std::endl;

			normal=camera->getUpVector();
			std::cout<<"rot( "<<normal.X<<", "<<normal.Y<<", "<<normal.Z<<" )"<<std::endl;
			normal.normalize();
			vec+=normal;
			camera->setTarget(vec);

			std::cout<<"vec( "<<vec.X<<", "<<vec.Y<<", "<<vec.Z<<" )"<<std::endl;

		}





	}


	dev->drop();


	return EXIT_SUCCESS;
}

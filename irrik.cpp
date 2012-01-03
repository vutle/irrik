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

void anim_ccd(scene::IBoneSceneNode &effector,scene::IBoneSceneNode &bone, core::vector3df target, int steps=3){
	core::vector3df vec1, vec2,ort;
	float anglex=0,angley=0,anglez=0;
	scene::IBoneSceneNode *parent;

	vec1 = bone.getAbsolutePosition(); //get abs pos to calculate vector to target
	vec2 = target - vec1;
	vec1 = effector.getPosition();
//	ort = vec1.crossProduct(vec2); //TODO ok or invert?

	core::quaternion quat;
	quat.rotationFromTo(vec1,vec2);
	std::cout << "1: " << vec1.X << ", " << vec1.Y << ", " << vec1.Z<<std::endl;
	std::cout << "2: " << vec2.X << ", " << vec2.Y << ", " << vec2.Z<<std::endl;
	core::vector3df eulers;
	quat.toEuler(eulers);
	eulers *= core::RADTODEG;
	bone.setRotation(eulers);
	bone.updateAbsolutePosition();

	parent=(scene::IBoneSceneNode*)bone.getParent();
	if(steps>0)
		anim_ccd(bone,*parent,target, steps-1);
}
void boneLabels(scene::ISceneManager *scene, gui::IGUIFont* font, scene::IBoneSceneNode *root){

	scene::IBoneSceneNode *child;

	core::list<scene::ISceneNode*>::ConstIterator begin = root->getChildren().begin();
	core::list<scene::ISceneNode*>::ConstIterator end = root->getChildren().end();
		for(;begin != end; ++begin){
			child = (scene::IBoneSceneNode *)(*begin);
			std::cout<<"joint name: "<<child->getName()<<std::endl;
			boneLabels(scene,font,child);
		}
		core::stringw str(root->getName());
		scene->addBillboardTextSceneNode(font, str.c_str(),root, core::dimension2d<f32>(10.0f, 4.0f));
}

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
			500, 150, 1, 1);

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
	node->setPosition(core::vector3df(0, -86, 50));
	node->setRotation(core::vector3df(0, -90, 0));
	node->setAnimationSpeed(10.f);
	node->setScale(core::vector3df(12, 12, 12));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));

	// Load a dwarf model
	node = scene->addAnimatedMeshSceneNode(
			scene->getMesh("media/dwarf/dwarf2.x"));
	node->setPosition(core::vector3df(-200, -86, 50));
	node->setRotation(core::vector3df(0, 90, 0));
	node->setAnimationSpeed(10.f);
	node->setScale(core::vector3df(12, 12, 12));
	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));

	// Load a ninja model
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
	node->setPosition(core::vector3df(50, -86, 150));
	node->setRotation(core::vector3df(0, 90, 0));
	node->setAnimationSpeed(10.f);
	node->getMaterial(0).NormalizeNormals = true;
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_SKELETON));
	node->setAnimationSpeed(5);
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
	node->setAnimationSpeed(5);
	node->setJointMode(scene::EJUOR_CONTROL);
	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);

	// material.Lighting = false;

	//let there be light
	scene::ILightSceneNode * light = scene->addLightSceneNode(0,
			core::vector3df(0, 80, 100), video::SColorf(1.0f, 1.0f, 1.0f, 1.0f),
			800.0f);
	light = scene->addLightSceneNode(0, core::vector3df(-100, 80, 0),
			video::SColorf(1.0f, 1.0f, 1.0f, 1.0f), 800.0f);
	light->render();

	//light position indicator
	scene::IBillboardSceneNode * bill = scene->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, drv->getTexture("media/particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	bill->setPosition(core::vector3df(-100, 80, 0));
	//light position indicator
	bill = scene->addBillboardSceneNode();
	bill->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	bill->setMaterialTexture(0, drv->getTexture("media/particle.bmp"));
	bill->setMaterialFlag(video::EMF_LIGHTING, false);
	bill->setMaterialFlag(video::EMF_ZBUFFER, false);
	bill->setSize(core::dimension2d<f32>(20.0f, 20.0f));
	bill->setPosition(core::vector3df(0, 80, 100));

	//text
	gui::IGUIEnvironment* guienv = dev->getGUIEnvironment();
	gui::IGUIFont* font = guienv->getFont("media/bigfont.png");
	gui::IGUIStaticText* textinfo = guienv->addStaticText(L"", core::rect<s32>(10,10,200,50), false);
	textinfo->setOverrideColor(video::SColor(120,255,255,255));
	textinfo->setOverrideFont(font);
	textinfo->setWordWrap(true);

	int lastFPS = -1;
	float wheel=0;

	scene::IBoneSceneNode *bone, *rootbone = node->getJointNode((u32)0);

	boneLabels(scene,font,rootbone);

	static int angles[] = {2, 3, 4, 4, 3, 2, 1, 0, 0, 1, 2};

	Target target(dev,-0, -0, 0);
    target.show();

    core::vector3df vec,normal;

	//main loop
	while (dev->run()) {

		drv->beginScene(true, true, video::SColor(1));

		bone=node->getJointNode("Joint13");
//        int i = (int) node->getFrameNr();
        rootbone->updateAbsolutePositionOfAllChildren();
//        bone->setRotation(core::vector3df(20.0f*angles[i%11],0,0) );
        anim_ccd(*bone,(scene::IBoneSceneNode &)*(bone->getParent()),target.getAbsolutePosition());
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
			vec=target.getPosition();
			vec.X-=1;
			target.setPosition(vec);
		}
		if( input.isPressed(irr::KEY_KEY_S) ){
			vec=target.getPosition();
			vec.X+=1;
			target.setPosition(vec);
		}
		if( input.isPressed(irr::KEY_KEY_A) ){
			vec=target.getPosition();
			vec.Z-=1;
			target.setPosition(vec);
		}
		if( input.isPressed(irr::KEY_KEY_D) ){
			vec=target.getPosition();
			vec.Z+=1;
			target.setPosition(vec);
		}
		if( input.isPressed(irr::KEY_KEY_Q) ){
			vec=target.getPosition();
			vec.Y-=1;
			target.setPosition(vec);
		}
		if( input.isPressed(irr::KEY_KEY_E) ){
			vec=target.getPosition();
			vec.Y+=1;
			target.setPosition(vec);
		}
		wheel = input.getWheel();
		if( wheel != 0 ){
			vec = camera->getTarget();
			normal=camera->getUpVector();
			normal.normalize();
			vec+=normal*wheel*10;
			camera->setTarget(vec);
			std::cout<<"vec( "<<vec.X<<", "<<vec.Y<<", "<<vec.Z<<" )"<<std::endl;
		}

	}


	dev->drop();


	return EXIT_SUCCESS;
}

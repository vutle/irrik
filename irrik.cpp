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

#define RED video::SColor(205, 255, 50, 50)
#define GREEN video::SColor(205, 50, 255, 50)
#define BLUE video::SColor(205, 50, 50, 255)
#define VEC0 core::vector3df(0, 0, 0)

void limitDOF(core::vector3df &eulers, scene::IBoneSceneNode &bone) {
	if ((bone.getBoneName()[5]) == '2' && (bone.getBoneName()[6] == '2')) {
		eulers.Z = 0;
		eulers.Y = 0;

		if (eulers.X > 15)
			eulers.X = 15;
		if (eulers.X < -160)
			eulers.X = -160;
	}
}
void anim_ccd(scene::IBoneSceneNode &effector, // k-chain end-effector node
		scene::IBoneSceneNode &bone, // bone to adjust
		core::vector3df target, // target to reach
		int steps, // how many parent bones adjust
		video::IVideoDriver *drv, // drv for debug lines
		bool inner = false) {

	core::vector3df vec1, vec2, vec3, test, eulers;
	scene::IBoneSceneNode *parent;

	vec3 = bone.getAbsolutePosition(); //get abs pos to calculate vector to target
	vec2 = target - vec3;
	vec1 = effector.getAbsolutePosition() - vec3;

	vec3.normalize();
	vec2.normalize();
	vec1.normalize();

	for (int i = 0; i < 15; ++i) {
		if ((bone.getBoneName()[5]) == '2' && (bone.getBoneName()[6] == '2')) {
			// 1DOF for a knee
			vec1.X = 0;
			vec2.X = 0;

			eulers.X = core::RADTODEG
					* acos(
							vec1.dotProduct(vec2)
									/ (vec1.getLength() * vec2.getLength()));

			if (vec1.crossProduct(vec2).X > 0 && eulers.X > 15)
				eulers.X = 15;
			else
				eulers.X *= -1;

			drv->draw3DLine(VEC0, vec1.crossProduct(vec2) * 50,
					video::SColor(205, 250, 250, 255));
			eulers.Y = 0;
			eulers.Z = 0;
		} else {

			core::quaternion quat;
			quat.rotationFromTo(vec1, vec2);
			quat.toEuler(eulers);

			eulers *= core::RADTODEG;

			vec1.X = 0;
			vec2.X = 0;

			eulers.X = core::RADTODEG
					* acos(
							vec1.dotProduct(vec2)
									/ (vec1.getLength() * vec2.getLength()));

			if (vec1.crossProduct(vec2).X < 0)
				eulers.X *= -1;
			eulers.Y = 0;
			eulers.Z = 0;
		}

		std::cout << i << " step " << steps << " " << bone.getBoneName()
				<< " euler X: " << eulers.X << std::endl;
		test = vec1;

		bone.setRotation(eulers);
		parent = (scene::IBoneSceneNode*) bone.getParent();
		parent->updateAbsolutePositionOfAllChildren();
		video::SMaterial material;
		material.setFlag(video::EMF_LIGHTING, false);

		drv->setMaterial(material);
		drv->setTransform(video::ETS_WORLD, core::IdentityMatrix);

		drv->draw3DLine(vec3, vec1 * 50, RED);
		drv->draw3DLine(vec3, effector.getAbsolutePosition(), GREEN);
		drv->draw3DLine(vec3, target, BLUE);
		drv->draw3DLine(vec3, bone.getAbsolutePosition(),
				video::SColor(205, 150, 150, 255));

		drv->draw3DLine(VEC0, vec1 * 50, RED);
		drv->draw3DLine(VEC0, vec2 * 50, GREEN);
		drv->draw3DLine(VEC0, target, BLUE);
		drv->draw3DLine(VEC0, vec3 * 50, video::SColor(205, 150, 150, 255));
		drv->draw3DLine(VEC0, eulers.rotationToDirection(test),
				video::SColor(255, 250, 250, 255));

		if (steps > 0 && !inner) {
			anim_ccd(effector, *parent, target, steps - 1, drv, true);
		} else
			return;
	}

}
void boneLabels(scene::ISceneManager *scene, gui::IGUIFont* font,
		scene::IBoneSceneNode *root) {

	scene::IBoneSceneNode *child;

	core::list<scene::ISceneNode*>::ConstIterator begin =
			root->getChildren().begin();
	core::list<scene::ISceneNode*>::ConstIterator end =
			root->getChildren().end();
	for (; begin != end; ++begin) {
		child = (scene::IBoneSceneNode *) (*begin);
		std::cout << "joint name: " << child->getName() << std::endl;
		boneLabels(scene, font, child);
	}
	core::stringw str(root->getName());
	scene->addBillboardTextSceneNode(font, str.c_str(), root,
			core::dimension2d<f32>(10.0f, 4.0f));
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
	camera->setTarget(core::vector3df(50, 50, -30));
	//load an q3 map
	dev->getFileSystem()->addZipFileArchive("media/map-20kdm2.pk3");
	scene::IAnimatedMesh* q3map = scene->getMesh("20kdm2.bsp");
	scene::IMeshSceneNode* q3mapNode = scene->addOctreeSceneNode(
			q3map->getMesh(0));
	q3mapNode->setPosition(core::vector3df(-1400, -70, -1400));

	scene::IAnimatedMeshSceneNode* node = 0;

	scene::ITriangleSelector* selector = 0;
	selector = scene->createOctreeTriangleSelector(q3mapNode->getMesh(),
			q3mapNode, 128);
	q3mapNode->setTriangleSelector(selector);
	if (!selector)
		return EXIT_FAILURE;
	video::SMaterial material;

	scene::ISceneNodeAnimator* anim = NULL;

//	// Load a dwarf model
//	node = scene->addAnimatedMeshSceneNode(
//			scene->getMesh("media/dwarf/dwarf1.x"));
//	node->setPosition(core::vector3df(0, -86, 50));
//	node->setRotation(core::vector3df(0, -90, 0));
//	node->setAnimationSpeed(10.f);
//	node->setScale(core::vector3df(12, 12, 12));
//	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
//	node->setDebugDataVisible(
//			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
//					^ scene::EDS_SKELETON));
//
//	// Load a dwarf model
//	node = scene->addAnimatedMeshSceneNode(
//			scene->getMesh("media/dwarf/dwarf2.x"));
//	node->setPosition(core::vector3df(-200, -86, 50));
//	node->setRotation(core::vector3df(0, 90, 0));
//	node->setAnimationSpeed(10.f);
//	node->setScale(core::vector3df(12, 12, 12));
//	node->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
//	node->setDebugDataVisible(
//			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
//					^ scene::EDS_SKELETON));
//
//	// Load a ninja model
//	node = scene->addAnimatedMeshSceneNode(
//			scene->getMesh("media/ninja/ninja.b3d"));
//
//	node->setScale(core::vector3df(10, 10, 10));
//	node->setPosition(core::vector3df(-200, -86, 150));
//	node->setRotation(core::vector3df(0, 90, 0));
//	node->setAnimationSpeed(20.f);
//	node->getMaterial(0).NormalizeNormals = true;
//	node->setDebugDataVisible(
//			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
//					^ scene::EDS_SKELETON));
//	node->setFrameLoop(32, 83);

	// Load a zobie model
//	scene::IAnimatedMeshSceneNode* node1 = 0;
//
//	node1 = scene->addAnimatedMeshSceneNode(
//			scene->getMesh("media/zombie/zombie.b3d"));
//	node1->setScale(core::vector3df(5, 5, 5));
//	node1->setPosition(core::vector3df(-200, 0, 150));
//	node1->setRotation(core::vector3df(0, -0, 0));
//	node1->setAnimationSpeed(10.f);
//	node1->getMaterial(0).NormalizeNormals = true;
//	node1->setDebugDataVisible(
//			(scene::E_DEBUG_SCENE_TYPE) (node1->isDebugDataVisible()
//					^ scene::EDS_SKELETON));
//	node1->setAnimationSpeed(5);
//    anim = scene->createFlyStraightAnimator(core::vector3df(100,0,60),
//    core::vector3df(-100,0,60), 3500, true);
//if (anim)
//{
//    node1->addAnimator(anim);
//    anim->drop();
//}
//	const core::aabbox3d<f32>& box = node1->getBoundingBox();
//	core::vector3df radius = box.MaxEdge - box.getCenter();
//	anim = scene->createCollisionResponseAnimator(selector, node1, radius,
//			core::vector3df(0, -10, 0), core::vector3df(0, -9, 0));
//	node1->addAnimator(anim);
//
//	anim->drop();

	// Load a zobie model
	node = scene->addAnimatedMeshSceneNode(
			scene->getMesh("media/zombie/zombie.b3d"));
	node->setScale(core::vector3df(5, 5, 5));
//	node->setPosition(core::vector3df(0, -50, 150));
//	node->setRotation(core::vector3df(0, 90, 0));
	node->setAnimationSpeed(10.f);
	node->getMaterial(0).NormalizeNormals = true;
	node->setDebugDataVisible(
			(scene::E_DEBUG_SCENE_TYPE) (node->isDebugDataVisible()
					^ scene::EDS_HALF_TRANSPARENCY));

	node->setAnimationSpeed(5);
	node->setJointMode(scene::EJUOR_CONTROL);
	node->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
	node->setMaterialFlag(video::EMF_LIGHTING, false);

	anim = scene->createCollisionResponseAnimator(selector, node,
			core::vector3df(30, 50, 30), core::vector3df(0, -10, 0),
			core::vector3df(0, 30, 0));
	selector->drop();
	node->addAnimator(anim);
	anim->drop();

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
	gui::IGUIStaticText* textinfo = guienv->addStaticText(L"",
			core::rect<s32>(10, 10, 200, 50), false);
	textinfo->setOverrideColor(video::SColor(120, 255, 255, 255));
	textinfo->setOverrideFont(font);
	textinfo->setWordWrap(true);

	int lastFPS = -1;
	float wheel = 0;

	scene::IBoneSceneNode *bone, *rootbone = node->getJointNode((u32) 0);
	bone = node->getJointNode("Joint23");

	//boneLabels(scene, font, rootbone);

	Target target(dev, 0, 0, 0);
	target.show();

	core::vector3df vec, normal;

	//main loop
	while (dev->run()) {

		drv->beginScene(true, true, video::SColor(1));
		material.setFlag(video::EMF_LIGHTING, false);
		drv->setMaterial(material);
		node->updateAbsolutePosition();
		drv->setTransform(video::ETS_WORLD, core::IdentityMatrix);
		drv->draw3DLine(core::vector3df(0, 0, 0), core::vector3df(10, 0, 0),
				video::SColor(55, 255, 0, 0));
		drv->draw3DLine(core::vector3df(0, 0, 0), core::vector3df(0, 10, 0),
				video::SColor(55, 0, 255, 0));
		drv->draw3DLine(core::vector3df(0, 0, 0), core::vector3df(0, 0, 10),
				video::SColor(55, 0, 0, 255));

		int fps = drv->getFPS();

		if (lastFPS != fps) {
			core::stringw str = L"Irrlicht, FPS: ";
			core::stringw istr = L"FPS: ";
			str += fps;
			istr += fps;
			textinfo->setText(istr.c_str());
			dev->setWindowCaption(str.c_str());
			lastFPS = fps;
		}
		float scalefactor = 1;

		vec = target.getPosition();

		if (input.isPressed(irr::KEY_ESCAPE)) {
			dev->drop();
			return EXIT_SUCCESS;
		}
		if (input.isPressed(irr::KEY_KEY_W)) {
			vec.X -= 1 * scalefactor;

		}
		if (input.isPressed(irr::KEY_KEY_S)) {
			vec.X += 1 * scalefactor;
		}
		if (input.isPressed(irr::KEY_KEY_A)) {
			vec.Z -= 1 * scalefactor;
		}
		if (input.isPressed(irr::KEY_KEY_D)) {
			vec.Z += 1 * scalefactor;
		}
		if (input.isPressed(irr::KEY_KEY_Q)) {
			vec.Y -= 1 * scalefactor;
		}
		if (input.isPressed(irr::KEY_KEY_E)) {
			vec.Y += 1 * scalefactor;
		}
		if (vec != target.getPosition() || input.isPressed(irr::KEY_SPACE)) {
			target.setPosition(vec);
			rootbone->updateAbsolutePositionOfAllChildren();
			anim_ccd(*bone, (scene::IBoneSceneNode &) *(bone->getParent()),
					target.getAbsolutePosition(), 2, drv);

		}
		wheel = input.getWheel();
		if (wheel != 0) {
			vec = camera->getTarget();
			normal = camera->getUpVector();
			normal.normalize();
			vec += normal * wheel * 10;
			camera->setTarget(vec);
			//std::cout<<"vec( "<<vec.X<<", "<<vec.Y<<", "<<vec.Z<<" )"<<std::endl;
		}
		scene->drawAll();
		guienv->drawAll();

		drv->endScene();

	}

	dev->drop();

	return EXIT_SUCCESS;
}

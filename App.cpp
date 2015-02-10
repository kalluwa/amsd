//created by kalluwa
//2015/1/29

//#include "stdafx.h"
#include "App.h"
#include "Src/Core/core.h"
#include "Src/EventTypes.h"
#include "Src/Scene/COpenglVolumeTexture.h"
using namespace kk;
//test ray
core::line3df line;

	IApp::IApp(kk::video::EDriverType type)
	{

		switch(type)
		{
		case kk::video::EDT_OPENGL:
			driver = kk::video::createOpenGLDriver(640,480);
			break;
		}
		
		init();

		//add camera node
		kk::scene::CSimpleCameraNode* simplecamera = new kk::scene::CSimpleCameraNode(driver,
			kk::core::vector3df(0,0,-10),kk::core::vector3df(0,0,0),kk::core::vector3df(0,1,0));
		scene->addCamera(simplecamera);
		//nodes.push_back(simplecamera);

		//volume node
		kk::scene::IVolumeTexture* volumeTexture = new kk::scene::COpenglVolumeTexture(driver,
			"F:\\CT\\N42.45.rcn",768,656,126);
		scene->addNodeToRender(volumeTexture);
		//set start pos
		SEvent e;
		e.type= ET_USER;
		
		simplecamera->OnEvent(e);

		//
	}

	//deconstructor
	IApp::~IApp()
	{
		if(driver)
		{
			delete driver;
			driver = 0;
		}

		if(scene)
		{
			delete scene;
			scene = 0;
		}
	}

	void IApp::init()
	{
		scene = new kk::scene::CSceneManager();
		scene->initEnvironment();
	}

	

	//update key events
	void IApp::update()
	{
	}

	//render
	void IApp::draw()
	{
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
		scene->getActiveCamera()->setUpMatrices();
		
		scene->render();

		scene->getActiveCamera()->setUpMatrices();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45,driver->getViewport().X/(float)driver->getViewport().Y,0.1f,1000.0f);
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		glColor3ub(255,255,0);
		driver->draw3dBox(core::aabbox3df(0,0,0,1,1,1));
		
		::glColor3ub(255,0,0);
		driver->draw3dBox(core::aabbox3df(-1,-1,-1,1,1,1));
		driver->draw3dLine(line.start,line.end);

		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

	}


	//dispatch event
	void IApp::OnEvent(SEvent event)
	{
		switch(event.type)
		{
		case ET_KEY:
			break;
		case ET_MOUSE:
			if(event.Mouse.mouseaction == EMT_MIDDLE_DOWN)
			{
				//test get ray
				line = driver->getRayFromScreenPoint(core::vector2di(event.Mouse.x,event.Mouse.y));
			}
			break;
		case ET_USER:
			switch(event.UserData.type)
			{
			case EUT_RESIZE:
				driver->setViewport(event.UserData.Size.width,event.UserData.Size.height);
				break;
			}
			break;
		}
		scene->onEvent(event);
	}
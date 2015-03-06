//created by kalluwa
//2015/1/29

//#include "stdafx.h"
#include "App.h"
#include "Src/Core/core.h"
#include "Src/EventTypes.h"
#include "Src/Scene/COpenglVolumeTexture.h"
#include "Src/Scene/ImageBatches.h"

//for calculation
#include "LengthAccuracy.h"
#include "TrianglePathLength.h"

using namespace kk;
using namespace Calculation;

//test ray
core::line3df line;
core::aabbox3df mainBox(-0.5f,-0.5f,-0.5f,0.5f,0.5f,0.5f);
core::aabbox3df pickBox(-1,-1,-1,0,0,0);
core::vector3df intersectionPoint;

kk::scene::IVolumeTexture* volumeTexture;	
f32* img=0;
bool keyCtrl=false;

	IApp::IApp(kk::video::EDriverType type)
		:driver(0),scene(0)
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
			kk::core::vector3df(0,0,3),kk::core::vector3df(0,0,0),kk::core::vector3df(0,1,0));
		scene->addCamera(simplecamera);
		//nodes.push_back(simplecamera);

		//volume node
		//<<- image is too large for my videocard-> result in showing nothing ->>
		// 756*656*252*4bytes = 484.3125MB and my video card memory is 492MB
		volumeTexture = new kk::scene::COpenglVolumeTexture(driver,
			//"F:\\CT\\data\\N42.45_A.raw",640,544,240,1.2f,1.2f,3.333f);//288 error
			"F:\\CT\\N42.45.rcn",768,656,126,1.35f,1.35f,6.67f);//,125);//125
		scene->addNodeToRender(volumeTexture);

		scene::ImageBatches* batches = new scene::ImageBatches(driver);
		scene->addNodeToRender(batches);
		img = new f32[100*100];
		s32 k=0;
		for(s32 i=0;i<100;i++)
		{
			for(s32 j=0;j<100;j++)
			{
				//img[k++] = rand()/(float)RAND_MAX;
				if(j<50&&i<50)
					img[i*100+j]=1.0f;
				else
					img[i*100+j]=0.5f;
				//img[k++]=1.0f;
			}
		}

		//kk::scene::CImageOpenGL* imgToAdd = new kk::scene::CImageOpenGL(img,100,100);
        //batches->addImage(imgToAdd,core::rect<s32>(0,0,100,100));
		//kk::scene::CImageOpenGL* imgToAdd2 = new kk::scene::CImageOpenGL(img,100,100);
		//batches->addImage(imgToAdd2,core::rect<s32>(100,0,100,100));
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
		if(img)
			delete []img;
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
		
		glColor3ub(255,0,0);
		driver->draw3dBox(pickBox);
		glColor3ub(255,255,0);
		driver->draw3dBox(mainBox);
		::glColor3ub(255,0,255);
		driver->draw3dBox(core::aabbox3df(-1,-1,-1,1,1,1));
		//
		glColor3ub(255,255,0);
		//draw intersection point
		driver->draw3dBox(core::aabbox3df(intersectionPoint.X-0.1f,intersectionPoint.Y-0.1f,intersectionPoint.Z-0.1f,
			intersectionPoint.X+0.1f,intersectionPoint.Y+0.1f,intersectionPoint.Z+0.1f));
		driver->draw3dLine(line.start,line.end);
		
		//driver->drawQuad(core::vector2di(0,0),core::vector2di(100,100));
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		//driver->drawImage(img,100,100,core::rect<s32>(0,0,100,100));

	}


	//dispatch event
	void IApp::OnEvent(SEvent event)
	{
		switch(event.type)
		{
		case ET_KEY:
			keyCtrl = event.Key.control;
			break;
		case ET_MOUSE:
			if(event.Mouse.mouseaction == EMT_MIDDLE_DOWN)
			{
				//test get ray
				line = driver->getRayFromScreenPoint(core::vector2di(event.Mouse.x,event.Mouse.y));
				//test line's intersection with box
				if(mainBox.getIntersectionPoint(line,intersectionPoint))
				{
#ifdef WIN32
					char str[100];
					sprintf_s(str,100,"%.3f %.3f %.3f",intersectionPoint.X,intersectionPoint.Y,intersectionPoint.Z);
					::OutputDebugStringA(str);
#endif
					line.start = intersectionPoint;
					//add box
					if(keyCtrl)
					{
						pickBox.addInternalBox(volumeTexture->getPointedData(line,true));
						//update volume selected box
					}
					else
						pickBox = volumeTexture->getPointedData(line);
				}
				
			}
			break;
		case ET_USER:
			switch(event.UserData.type)
			{
			case EUT_RESIZE:
				driver->setViewport(event.UserData.Size.width,event.UserData.Size.height);
				break;
			case EUT_CALCULATION:
				runCalculation(event.UserData.calType);
				break;
			}
			break;
		}
		scene->onEvent(event);
	}

	void IApp::runCalculation(Calculation::ECalType type)
	{
		kk::scene::ImageBatches* batches = dynamic_cast<kk::scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
		batches->clearAllTextures();

		switch(type)
		{
		case Calculation::ECT_OBJ_LENGTH:
			{
				Calculation::BoxData* boxData =new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),
					volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),
					volumeTexture->getSelectedBoundingBox());
				
				Amsd_LengthAccuracy(boxData,scene);

				if(boxData)
				delete boxData;
			}
			break;
		case Calculation::ECT_TRIANGLE_PATH_LENGTH:
			{
				Calculation::BoxData* boxData = new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),volumeTexture->getSelectedBoundingBox());
				TrianglePathLength(boxData,scene);
				
				if(boxData)
				delete boxData;
			}
			break;
		}


	}

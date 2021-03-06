//created by kalluwa
//2015/1/29

//#include "stdafx.h"
#include "App.h"
#include "Src/Core/Core.h"
#include "Src/EventTypes.h"
#include "Src/Scene/COpenglVolumeTexture.h"
#include "Src/Scene/ImageBatches.h"

//for calculation
#include "Calculation/LengthAccuracy.h"
#include "Calculation/TrianglePathLength.h"
#include "Calculation/NeqCalculation.h"
#include "Calculation/MetalUniformity.h"
#include "Calculation/Amsd_SSP.h"
//write to file
#include "Src/IO/CWriteFileW.h"

using namespace kk;
using namespace Calculation;

//test ray
core::line3df line;
core::aabbox3df mainBox(-0.5f,-0.5f,-0.5f,0.5f,0.5f,0.5f);
core::aabbox3df pickBox(0,0,0,0.01f,0.01f,0.01f);
core::vector3df intersectionPoint;

kk::scene::IVolumeTexture* volumeTexture;	
f32* img=0;
bool keyCtrl=false;

io::IWriteFile* Output;
//reference: http://stackoverflow.com/questions/308276/c-call-constructor-from-constructor
//no way to call construtor from constructor for c++03
	IApp::IApp(kk::video::EDriverType type)  //test volume
		:driver(0),scene(0)
	{
		//error:
		//IApp(core::stringc("F:\\CT\\N42.45.rcn"),768,656,126,1.35f,1.35f,6.67f,0,type);
		
		switch(type)
		{
		case kk::video::EDT_OPENGL:
			driver = kk::video::createOpenGLDriver(640,480);
			break;
		}
		
		init();

		//add camera node
		kk::scene::CSimpleCameraNode* simplecamera = new kk::scene::CSimpleCameraNode(driver,
			kk::core::vector3df(-2,0,1),kk::core::vector3df(0,0,0),kk::core::vector3df(0,1,0));
		scene->addCamera(simplecamera);
		//nodes.push_back(simplecamera);

		//test file append
		//io::IWriteFile* file = new io::CWriteFileW("hello.txt",false,true);
		//int A[]={1,2,3,4,5};
		//file->writeString(core::stringc("�ı�����"));
		//file->writeEmptyLine();
		//file->writeInt(100);
		//file->writeEmptyLine();
		//file->writeSingle(4.3f);
		//file->writeEmptyLine();
		//file->writeDouble(4.34);
		//file->writeEmptyLine();
		//file->writeArrayInt(A,5,' ');
		//file->closeFile();
		//delete file;

		//volume node
		//core::stringc fileName("F:\\CT\\N42.45.rcn");
		//<<- image is too large for my videocard-> result in showing nothing ->>
		// 756*656*252*4bytes = 484.3125MB and my video card memory is 492MB
		//volumeTexture = new kk::scene::COpenglVolumeTexture(driver,
			//"F:\\CT\\data\\N42.45_A.raw",640,544,240,1.2f,1.2f,3.333f);//288 error
		//	fileName.c_str(),768,656,126,1.35f,1.35f,6.67f,126);//,125);//125
		//scene->addNodeToRender(volumeTexture);

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
	}

	IApp::IApp(const kk::core::stringc& fileName,s32 sizeX,s32 sizeY,s32 sizeZ,
		f32 mmX,f32 mmY,f32 mmZ,s32 offset,bool inverseLoad,kk::video::EDriverType type)
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
			kk::core::vector3df(-2,0,1),kk::core::vector3df(0,0,0),kk::core::vector3df(0,1,0));
		scene->addCamera(simplecamera);
		//nodes.push_back(simplecamera);

		//volume node
		//<<- image is too large for my videocard-> result in showing nothing ->>
		// 756*656*252*4bytes = 484.3125MB and my video card memory is 492MB
		volumeTexture = new kk::scene::COpenglVolumeTexture(driver,
			//"F:\\CT\\data\\N42.45_A.raw",640,544,240,1.2f,1.2f,3.333f);//288 error
			fileName.c_str(),sizeX,sizeY,sizeZ,mmX,mmY,mmZ,offset,inverseLoad);//,125);//125
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
		//remove output file handle
		if(Output)
			delete Output;
	}
	void IApp::init()
	{
		//
		PickThreshold = 0.2f;

		scene = new kk::scene::CSceneManager();
		scene->initEnvironment();

		//output
		Output = new io::CWriteFileW("OutputResult.txt");
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
			
			//debug slice
			viewSliceZ(event.Key.keycode);

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
						pickBox.addInternalBox(volumeTexture->getPointedData(line,true,PickThreshold));
						//update volume selected box
					}
					else
						pickBox = volumeTexture->getPointedData(line,false,PickThreshold);
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
				
				Amsd_LengthAccuracy(boxData,scene,Output);

				if(boxData)
				delete boxData;
			}
			break;
		case Calculation::ECT_TRIANGLE_PATH_LENGTH:
			{
				Calculation::BoxData* boxData = new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),volumeTexture->getSelectedBoundingBox());
				TrianglePathLength(boxData,scene,Output);
				
				if(boxData)
				delete boxData;
			}
			break;
		case Calculation::ECT_NEQ_CALCULATION:
			{
				Calculation::BoxData* boxData = new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),volumeTexture->getSelectedBoundingBox());
				core::aabbox3di tmpBox = Amsd_NEQCalculation(boxData,scene,Output);
				//debug
				core::vector3df offset(-0.5f,-0.5f,-0.5f);
				pickBox.MinEdge = offset+core::vector3df(tmpBox.MinEdge.X/(f32)volumeTexture->getVolumeSizeX(),tmpBox.MinEdge.Y/(f32)volumeTexture->getVolumeSizeY(),tmpBox.MinEdge.Z/(f32)volumeTexture->getVolumeSizeZ());
				pickBox.MaxEdge = offset+core::vector3df(tmpBox.MaxEdge.X/(f32)volumeTexture->getVolumeSizeX(),tmpBox.MaxEdge.Y/(f32)volumeTexture->getVolumeSizeY(),tmpBox.MaxEdge.Z/(f32)volumeTexture->getVolumeSizeZ());
				//debug end
				if(boxData)
				delete boxData;
			}
			break;
		case Calculation::ECT_METAL_UNIFORMITY:
			{
				Calculation::BoxData* boxData = new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),volumeTexture->getSelectedBoundingBox());
				core::aabbox3di tmpBox = Amsd_MetalUniformity(boxData,scene,Output);
				//debug
				core::vector3df offset(-0.5f,-0.5f,-0.5f);
				pickBox.MinEdge = offset+core::vector3df(tmpBox.MinEdge.X/(f32)volumeTexture->getVolumeSizeX(),tmpBox.MinEdge.Y/(f32)volumeTexture->getVolumeSizeY(),tmpBox.MinEdge.Z/(f32)volumeTexture->getVolumeSizeZ());
				pickBox.MaxEdge = offset+core::vector3df(tmpBox.MaxEdge.X/(f32)volumeTexture->getVolumeSizeX(),tmpBox.MaxEdge.Y/(f32)volumeTexture->getVolumeSizeY(),tmpBox.MaxEdge.Z/(f32)volumeTexture->getVolumeSizeZ());
				//debug end
				if(boxData)
				delete boxData;
			}
			break;
		case Calculation::ECT_SSP:
			{
				Calculation::BoxData* boxData = new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),volumeTexture->getSelectedBoundingBox());
				Amsd_SSP_Calculation(boxData,scene,Output);
				
				//debug end
				if(boxData)
				delete boxData;
			}
			break;
		}
	}

	void IApp::viewSliceZ(kk::s32 keyCode)
	{
		bool needUpdate = false;
		//for debug slice change
			switch(keyCode)
			{
			case VK_LEFT:
				Calculation::debugSlicePos--;
				needUpdate = true;
				break;
			case VK_RIGHT:
				Calculation::debugSlicePos++;
				needUpdate = true;
				break;
			case 'O':
				keyCtrl = false;
				break;
			}
		char debugSliceIndexStr[20];
		sprintf_s(debugSliceIndexStr,20,"debugPos=%i#",Calculation::debugSlicePos);
		::OutputDebugStringA(debugSliceIndexStr);
		if(!needUpdate)
			return ;
		Calculation::BoxData* boxData =new BoxData(volumeTexture->getVolumeData(),volumeTexture->getVolumeSizeX(),
					volumeTexture->getVolumeSizeY(),volumeTexture->getVolumeSizeZ(),
					volumeTexture->getSelectedBoundingBox());
		if(Calculation::debugSlicePos>=volumeTexture->getVolumeSizeZ())
			Calculation::debugSlicePos = volumeTexture->getVolumeSizeZ()-1;
		if(Calculation::debugSlicePos<=0)
			Calculation::debugSlicePos =0;

		/*if(Calculation::debugSlicePos <= (pickBox.MaxEdge.Z+0.5f)*volumeTexture->getVolumeSizeZ())
					Calculation::debugSlicePos = (pickBox.MaxEdge.Z+0.5f)*volumeTexture->getVolumeSizeZ()-1;
		if(Calculation::debugSlicePos >= (pickBox.MinEdge.Z+0.5f)*volumeTexture->getVolumeSizeZ())
					Calculation::debugSlicePos = (0.5f+pickBox.MinEdge.Z)*volumeTexture->getVolumeSizeZ()-1;
*/
				
		f32* sliceImage=0;
		s32 width=0;
		s32 height=0;
		if(debugSlicePos == -1)
		debugSlicePos = (boxData->Box.MinEdge.Z+boxData->Box.MaxEdge.Z)/2;
		boxData->getSliceZ(sliceImage,debugSlicePos,width,height,0.2f);
		f32 tmpMin,tmpMax;
		s32 count = boxData->getSliceZPassVoxelCount(debugSlicePos,tmpMax,tmpMin);
#ifdef _DEBUG
		char tm[50];
		sprintf_s(tm,50,"count=%i MaxValue=%.3f",count,boxData->getSliceZMaxValue(debugSlicePos));
		::OutputDebugStringA(tm);
#endif
		//image batches
		scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
		scene::CImageOpenGL* img = new scene::CImageOpenGL(sliceImage,width,height,true);
		batches->addImage(img,core::rect<s32>(0,0,width,height));

		if(sliceImage)
			delete []sliceImage;
		if(boxData)
			delete boxData;
	}

//created by kalluwa
//2015/1/29

//#include "StdAfx.h"

#ifndef _APP_H_
#define _APP_H_

#include "Src/Driver/COpenGLDriver.h"

#include "Src/Core/core.h"
#include "Src/EventTypes.h"
#include "Src/Driver/EDriverType.h"
#include "Src/Scene/CSimpleCameraNode.h"
#include "Src/Scene/CSceneManager.h"
#include "Calculation/calcHelper.h"

using namespace kk;
using namespace core;
class IApp
{
public:
	//constructor
	IApp(kk::video::EDriverType type=video::EDT_OPENGL);

	IApp(const kk::core::stringc& fileName,s32 sizeX,s32 sizeY,s32 sizeZ,
		f32 mmX,f32 mmY,f32 mmZ,s32 offset,kk::video::EDriverType type= video::EDT_OPENGL);
	//deconstructor
	virtual ~IApp();

	//update key events
	virtual void update();

	//init 
	virtual void init();
	//render
	virtual void draw();

	virtual void runCalculation(Calculation::ECalType type);
	virtual void viewSliceZ(kk::s32 keyCode);
	//dispatch event
	virtual void OnEvent(SEvent event);

	kk::video::IVideoDriver* getDriver()
	{
		return driver;
	}
	

protected:
	kk::video::IVideoDriver* driver;
	kk::scene::ISceneManager* scene;

};//end class

#endif // app
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
#include "calcHelper.h"

class IApp
{
public:
	//constructor
	IApp(kk::video::EDriverType type=kk::video::EDT_OPENGL);
	//deconstructor
	virtual ~IApp();

	//update key events
	virtual void update();

	//init 
	virtual void init();
	//render
	virtual void draw();

	virtual void runCalculation(Calculation::ECalType type);

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
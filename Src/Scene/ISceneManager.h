//created by kalluwa
//2015/2/7

#ifndef _I_SCENE_MANAGER_H_
#define _I_SCENE_MANAGER_H_

//get basic types
#include "../Core/core.h"

#include"ISceneNode.h"
#include "CSimpleCameraNode.h"

#ifdef WIN32
//for OutputDebugStrA
#include <windows.h>
#endif

namespace kk
{
namespace scene
{
class ISceneManager
{
public:

//deconstructor
	virtual ~ISceneManager(){};
//i will init opengl glew environment here or dx's environment
	virtual void initEnvironment()=0;
//add a camera
	virtual void addCamera(kk::scene::CSimpleCameraNode* camera)=0;
//scene register node
	virtual void addNodeToRender(kk::scene::ISceneNode* node)=0;
//render
	virtual void render()=0;
//remove scene node
	virtual void removeNode(ISceneNode* node)=0;
//dispatch event to nodes
	virtual void onEvent(SEvent event)=0;
//get camera
	virtual kk::scene::CSimpleCameraNode* getActiveCamera()=0;
//get specific node
	virtual kk::scene::ISceneNode* getSpecificNodeById(core::stringc name)=0;
};

}
}

#endif

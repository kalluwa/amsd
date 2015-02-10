//created by kalluwa
//2014/2/7

#ifndef _C_SCENE_MANAGER_H__
#define _C_SCENE_MANAGER_H__

#include "ISceneManager.h"

namespace kk
{
namespace scene
{
class CSceneManager : public virtual ISceneManager
{
public:
	CSceneManager();
//deconstructor
	~CSceneManager();
//init glew or dx environment
	virtual void initEnvironment();
//add camera
	virtual void addCamera(CSimpleCameraNode* camera);
//register node
	virtual void addNodeToRender(ISceneNode* node);
//render all nodes
	virtual void render();
//remove scenenode
	virtual void removeNode(ISceneNode* node);
//dispatch Event
	virtual void onEvent(SEvent event);
//get camera
	virtual CSimpleCameraNode* getActiveCamera();
private:
	//variables
	core::array<ISceneNode*> Nodes;

	CSimpleCameraNode* Camera;
};
}
}

#endif

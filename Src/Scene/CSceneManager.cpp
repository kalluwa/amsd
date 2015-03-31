//created by kalluwa
//2014/2/7

#include "CSceneManager.h"
#include <gl/glew.h>

namespace kk
{
namespace scene
{

CSceneManager::CSceneManager()
{
	
};
//deconstructor
CSceneManager::~CSceneManager()
{
	s32 lastIndex = (s32)Nodes.size()-1;
	for(s32 i =lastIndex;i>=0;i--)
	{
		if(Nodes[i])
		delete Nodes[i];
	}
};
//init glew or dx environment
void CSceneManager::initEnvironment()
{
#ifndef UseDX 
//opengl mode
	int result = glewInit();

	// initialize all the OpenGL extensions
	glewGetExtension("glMultiTexCoord2fvARB");  
	glewGetExtension("GL_EXT_framebuffer_object");
	glewGetExtension("GL_EXT_renderbuffer_object");
	glewGetExtension("GL_ARB_vertex_buffer_object");

	glewGetExtension("GL_ARB_fragment_shader");
	glewGetExtension("GL_ARB_vertex_shader");       
	glewGetExtension("GL_ARB_shader_objects");      
	glewGetExtension("GL_ARB_shading_language_100");

	
#else
	//Dx initialization
#endif

	//init CG
	
	//this will be done in IVideoDriver's constructor



};
//register node
void CSceneManager::addNodeToRender(ISceneNode* node)
{
	if(node!=NULL)
		Nodes.push_back(node);
};
//render all nodes
void CSceneManager::render()
{
	for(s32 i=0;i<(s32)Nodes.size();i++)
	{
		Nodes[i]->render();
	}
};
//remove scenenode
void CSceneManager::removeNode(ISceneNode* node)
{
	s32 index = Nodes.linear_reverse_search(node);
	if(index!=-1)
	{
		Nodes.erase(index);
	}
};
void CSceneManager::onEvent(SEvent event)
{
	for(int i=0;i<(s32)Nodes.size();i++)
	{
		Nodes[i]->OnEvent(event);
	}
};

CSimpleCameraNode* CSceneManager::getActiveCamera()
{
	return Camera;
}

void CSceneManager::addCamera(CSimpleCameraNode* camera)
{
	Camera = camera;
	addNodeToRender(camera);
}

kk::scene::ISceneNode* CSceneManager::getSpecificNodeById(core::stringc name)
{
	for(s32 i=0;i<(s32)Nodes.size();i++)
	{
		if(name == Nodes[i]->getDebugName())
			return Nodes[i];
	}

	return 0;
}
}
}
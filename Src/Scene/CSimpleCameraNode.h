//created by kalluwa
//2015/1/29


#ifndef _C_SIMPLE_CAMERA_NODE_
#define _C_SIMPLE_CAMERA_NODE_

#include "../Driver/IVideoDriver.h"
#include "ISceneNode.h"

namespace kk
{
namespace scene
{
	class CSimpleCameraNode : public virtual ISceneNode
	{
	public:
		CSimpleCameraNode(kk::video::IVideoDriver* _driver,const core::vector3df& Pos,const core::vector3df& Target,const vector3df& upDir);

		virtual ~CSimpleCameraNode();

		virtual void OnEvent(SEvent event);
		
		void HandleMouseEvent(SEvent e);
		
		void HandleKeyEvent(SEvent e);
		
		virtual void render();

		//set up view and projection and world
		virtual void setUpMatrices();
	private:
		kk::core::vector3df pos;
		kk::core::vector3df target;
		kk::core::vector3df up;
		//we need driver to update our view matrix
		//kk::video::IVideoDriver* driver;
		//store some data for tmp use
		int lastX,lastY;
		bool isFirstUpdate;
		bool mouseLeftDown;
	};//end class
}
}
#endif

//created by kalluwa
//2015/1/29

#ifndef _I_SCENE_NODE_
#define _I_SCENE_NODE_

//include basic element for use
#include "../Core/core.h"
#include "../Driver/IVideoDriver.h"
#include "../EventTypes.h"

namespace kk
{
namespace scene
{
	class ISceneNode
	{
	public:
		//this is an abstract class
		ISceneNode()
			//:Driver(0)
		{
			//throw "Driver Can't NULL";
		}

		ISceneNode(kk::video::IVideoDriver* driver)
			:Driver(driver)
		{
		}

		void  setDebugName(const c8* name)
		{
			debugName = core::stringc(name);
		}
		virtual ~ISceneNode(){};

		virtual void OnEvent(SEvent event)=0;
		
		virtual void render()=0;
	protected:
		kk::video::IVideoDriver* Driver;
		core::stringc debugName;
	};//end class
}
}
#endif

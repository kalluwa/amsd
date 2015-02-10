//created by kk
//2014/1/27

#ifndef _I_VIDEO_DRIVER_H_
#define _I_VIDEO_DRIVER_H_

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef _DEBUG
#include "assert.h"
#endif

#include "../Core/core.h"
//shader
#include <Cg/cg.h>




namespace kk
{
using namespace kk::core;

namespace video
{
	//cg error callback
	inline void cgErrorCallback()
	{
		CGerror lastError = cgGetError(); 
		if(lastError)
		{
#ifdef WIN32
			char errorText[128];
			sprintf_s(errorText,"%s",cgGetErrorString(lastError));
			::OutputDebugStringA(errorText);
#endif
		}
	};


	class IVideoDriver
	{
	public:
		IVideoDriver(int width,int height)
			:Width(width),Height(height)
		{
			//init cg
			cgSetErrorCallback(cgErrorCallback);
			ShaderContext = cgCreateContext();
		};
		//deconstructor
		virtual ~IVideoDriver(){};

		//create rendering driver device
		virtual void createDevice() = 0;

		//draw triangles
		virtual void drawUserIndexedPrimitives(void* vertices,s32 vertexSize,u16* indices,s32 indexCount)=0;

		virtual void drawUserIndexedPrimitives(void* vertices,s32 vertexSize,u32* indices,s32 indexCount)=0;

		//set new render state
		virtual void setRenderState(s32 nameOfState,s32 newStateValue)=0;

		//create simple drawing
		virtual void draw2dLine(s32 sx1,s32 sy1,s32 sx2,s32 sy2)=0;

		virtual void draw2dLine(const vector2df&  v1,const vector2df& v2)=0;

		virtual void draw3dLine(const vector3df& v1,const vector3df& v2)=0;

		virtual void drawFullScreenQuad()=0;

		virtual void drawQuad(const core::vector2di& leftTop,const core::vector2di& rightBottom)=0;

		virtual void draw3dBox(const aabbox3df& bbox)=0;

		virtual void draw3dSphere(const core::vector3df& pos,f32 radius)=0;

		virtual core::line3df getRayFromScreenPoint(const core::vector2di& posInScreen)=0;
		//force present
		virtual void present()=0;

		//get
		//virtual matrix4* getProjection()=0;
		
		//virtual matrix4* getView()=0;

		virtual void setViewport(s32 width,s32 height)=0;

		virtual core::vector2di getViewport()=0;
		
		//set view
		virtual void setView(const core::vector3df& pos,const core::vector3df& target,const core::vector3df& up)=0;


		//shader programe
		//vertex shader
		virtual void loadVertexShader(char* filename,char* entryPoint,CGprogram& vs_main)=0;

		virtual void loadPixelShader(char* filename,char* entryPoint,CGprogram& ps_main)=0;

		//test for opengl
		virtual void setShaderTexParam(char* par, unsigned int tex,const CGprogram &program,CGparameter param)=0;

		//shader profile
		virtual const CGprofile& getVSProfile()=0;

		virtual const CGprofile& getPSProfile()=0;

	protected:
		CGcontext ShaderContext; 
		s32 Width,Height;
	};
}
}

#endif
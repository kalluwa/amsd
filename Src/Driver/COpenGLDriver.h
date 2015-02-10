//created by kk
//2014/1/27
#ifndef _C_OPENGL_VIDEO_DRIVER_H_
#define _C_OPENGL_VIDEO_DRIVER_H_


//load library
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"glu32.lib") 
#pragma comment(lib,"glaux.lib")

//library for cg
#pragma comment(lib,"cg.lib")
#pragma comment(lib,"cgGL.lib")
//#pragma comment(lib,"glew.lib")

#include "../Core/core.h"
#include "IVideoDriver.h"

//headers for opengl
#include <iostream>
#include "math.h"
#include <GL/glew.h>
#include <gl/gl.h> //need <windows.h>
#include <gl/glu.h>

#include "glaux.h"
#include <Cg/cgGL.h>

namespace kk
{
using namespace kk::core;

namespace video
{
	class COpenGLDriver : public virtual IVideoDriver
	{
	public:

		COpenGLDriver(int width,int height);

		virtual ~COpenGLDriver(){ }
		//create rendering driver device
		virtual void createDevice();

		//draw triangles
		virtual void drawUserIndexedPrimitives(void* vertices,s32 vertexSize,u16* indices,s32 indexCount);

		virtual void drawUserIndexedPrimitives(void* vertices,s32 vertexSize,u32* indices,s32 indexCount);

		//set new render state
		virtual void setRenderState(s32 nameOfState,s32 newStateValue);

		//create simple drawing
		virtual void draw2dLine(s32 sx1,s32 sy1,s32 sx2,s32 sy2);

		virtual void draw2dLine(const vector2df&  v1,const vector2df& v2);

		virtual void draw3dLine(const vector3df& v1,const vector3df& v2);

		virtual void draw3dBox(const aabbox3df& bbox);

		virtual void draw3dSphere(const core::vector3df& pos,f32 radius);

		virtual void drawFullScreenQuad();

		virtual void drawQuad(const core::vector2di& leftTop,const core::vector2di& rightBottom);

		virtual core::line3df getRayFromScreenPoint(const core::vector2di& posInScreen);

		virtual void present();

		virtual void setView(const core::vector3df& pos,const core::vector3df& target,const core::vector3df& up);

		virtual void setViewport(s32 width,s32 height);

		virtual core::vector2di getViewport();

		//vertex shader
		virtual void loadVertexShader(char* filename,char* entryPoint,CGprogram& vs_main);

		virtual void loadPixelShader(char* filename,char* entryPoint,CGprogram& ps_main);

		//test for opengl
		virtual void setShaderTexParam(char* par, unsigned int tex,const CGprogram &program,CGparameter param);
	
		//shader profile
		virtual const CGprofile& getVSProfile(){ return VertexProfile; };

		virtual const CGprofile& getPSProfile(){ return PixelProfile; };
	private:

		CGprofile VertexProfile, PixelProfile;
	};

	extern IVideoDriver* createOpenGLDriver(int width,int height);
}
}

#endif
//created by kalluwa
//2014/1/27


//#include "StdAfx.h"
#include "COpenGLDriver.h"


namespace kk
{
namespace video
{
	
	COpenGLDriver::COpenGLDriver(int width,int height)
		:IVideoDriver(width,height)
	{
#if TRUE
		//get shader profile
		{
			if (cgGLIsProfileSupported(CG_PROFILE_VP40))
			{
				VertexProfile = CG_PROFILE_VP40;
			}
			else 
			{
				if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1))
					VertexProfile = CG_PROFILE_ARBVP1;
				else
				{
#ifdef WIN32
			char tmp[256];
			sprintf_s(tmp,"error in VertexProfile");
			OutputDebugStringA(tmp);
#endif
				}
			}

			if (cgGLIsProfileSupported(CG_PROFILE_FP40))
			{
				PixelProfile = CG_PROFILE_FP40;
			}
			else 
			{
				if (cgGLIsProfileSupported(CG_PROFILE_ARBFP1))
					PixelProfile = CG_PROFILE_ARBFP1;
				else
				{
#ifdef WIN32
			char tmp[256];
			sprintf_s(tmp,"error in PixelProfile");
			OutputDebugStringA(tmp);
#endif
				}
			}
		}
#endif
	};


	void COpenGLDriver::createDevice()
	{
	};

	void COpenGLDriver::drawUserIndexedPrimitives(void* vertices,s32 vertexSize,u16* indices,s32 indexCount)
	{
	};

	void COpenGLDriver::drawUserIndexedPrimitives(void* vertices,s32 vertexSize,u32* indices,s32 indexCount)
	{
	};

	void COpenGLDriver::setRenderState(s32 nameOfState,s32 newStateValue)
	{
	};

	void COpenGLDriver::draw2dLine(s32 sx1,s32 sy1,s32 sx2,s32 sy2)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_LINES);
		glVertex2f((f32)sx1,(f32)sy1);
		glVertex2f((f32)sx2,(f32)sy2);
		glEnd();

		glPopMatrix();
	};

	void COpenGLDriver::draw2dLine(const vector2df&  v1,const vector2df& v2)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		glBegin(GL_LINES);
		glVertex2f(v1.X,v1.Y);
		glVertex2f(v2.X,v2.Y);
		glEnd();
		
		glPopMatrix();
	};

	void COpenGLDriver::draw3dLine(const vector3df& v1,const vector3df& v2)
	{
		glBegin(GL_LINES);
		glVertex3f(v1.X,v1.Y,v1.Z);
		glVertex3f(v2.X,v2.Y,v2.Z);
		glEnd();
	};

	void COpenGLDriver::draw3dLine(f32 x1,f32 y1,f32 z1,f32 x2,f32 y2,f32 z2)
	{
		glBegin(GL_LINES);
		glVertex3f(x1,y1,z1);
		glVertex3f(x2,y2,z2);
		glEnd();
	}

	void COpenGLDriver::draw3dBox(const aabbox3df& bbox)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		//aux leads to memory leaks
		//auxSolidBox(size.X,size.Y,size.Z);
		glBegin(GL_LINES);

		//lines from min corner
		glVertex3f(bbox.MinEdge.X,bbox.MinEdge.Y,bbox.MinEdge.Z);
		glVertex3f(bbox.MaxEdge.X,bbox.MinEdge.Y,bbox.MinEdge.Z);

		glVertex3f(bbox.MinEdge.X,bbox.MinEdge.Y,bbox.MinEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MaxEdge.Y,bbox.MinEdge.Z);

		glVertex3f(bbox.MinEdge.X,bbox.MinEdge.Y,bbox.MinEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MinEdge.Y,bbox.MaxEdge.Z);
		
		//lines from max corner
		glVertex3f(bbox.MaxEdge.X,bbox.MaxEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MaxEdge.Y,bbox.MaxEdge.Z);

		glVertex3f(bbox.MaxEdge.X,bbox.MaxEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MaxEdge.X,bbox.MinEdge.Y,bbox.MaxEdge.Z);

		glVertex3f(bbox.MaxEdge.X,bbox.MaxEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MaxEdge.X,bbox.MaxEdge.Y,bbox.MinEdge.Z);

		//others 1
		glVertex3f(bbox.MaxEdge.X,bbox.MaxEdge.Y,bbox.MinEdge.Z);
		glVertex3f(bbox.MaxEdge.X,bbox.MinEdge.Y,bbox.MinEdge.Z);

		glVertex3f(bbox.MaxEdge.X,bbox.MinEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MaxEdge.X,bbox.MinEdge.Y,bbox.MinEdge.Z);

		glVertex3f(bbox.MaxEdge.X,bbox.MinEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MinEdge.Y,bbox.MaxEdge.Z);

		//others 2
		glVertex3f(bbox.MinEdge.X,bbox.MaxEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MinEdge.Y,bbox.MaxEdge.Z);

		glVertex3f(bbox.MinEdge.X,bbox.MaxEdge.Y,bbox.MaxEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MaxEdge.Y,bbox.MinEdge.Z);

		glVertex3f(bbox.MaxEdge.X,bbox.MaxEdge.Y,bbox.MinEdge.Z);
		glVertex3f(bbox.MinEdge.X,bbox.MaxEdge.Y,bbox.MinEdge.Z);

		glEnd();
		glPopMatrix();
	};

	void COpenGLDriver::draw3dSphere(const core::vector3df& pos,f32 radius)
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(pos.X,pos.Y,pos.Z);
		glColor3ub(255,0,0);
		//aux leads to memory leaks
		//::auxSolidSphere(radius);
		f32 alpha=0.0f,delta = 0.0f;
		f32 pi2=2*3.1415f;
		f32 pi=3.1415f;
		f32 piover2 = 1.5708f;
		f32 step = pi/8;
		glBegin(GL_LINES);
		for(alpha=step;alpha<pi2;alpha+=step)
		{
			
			glVertex3f(0,radius,0);
			for(delta =step;delta<pi+2*step;delta+=step)
			{
				f32 y = radius* cosf(delta);
				f32 x = radius*sinf(delta)*cosf(alpha);
				f32 z = radius*sinf(delta)*sinf(alpha);

				f32 y2 = radius* cosf(delta);
				f32 x2 = radius*sinf(delta)*cosf(alpha+step);
				f32 z2 = radius*sinf(delta)*sinf(alpha+step);

				glVertex3f(x,y,z);

				glVertex3f(x,y,z);
				glVertex3f(x2,y2,z2);

				glVertex3f(x,y,z);
			}
			glVertex3f(0,-radius,0);
		}

		
		glEnd();
		glPopMatrix();
	}
	u32 tmpTexture=-1;
	void COpenGLDriver::drawImage(f32* imageData,s32 width,s32 height,const core::rect<s32>& rectangle)
	{
		glColor3ub(255,255,255);
		//generate texture
		if(tmpTexture==-1)
		{
			glGenTextures(1, &tmpTexture);					// 创建纹理
			// 使用来自位图数据生成 的典型纹理
			glBindTexture(GL_TEXTURE_2D, tmpTexture);
			// 生成纹理
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_LUMINANCE, GL_FLOAT, imageData);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// 线形滤波
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// 线形滤波
		}
		//绑定纹理
		glEnable(GL_TEXTURE_2D); 
		glBindTexture(GL_TEXTURE_2D, tmpTexture);
		core::vector2df left_top(rectangle.UpperLeftCorner.X/(f32)Width,rectangle.UpperLeftCorner.Y/(f32)Height);
		core::vector2df bottom_right(rectangle.LowerRightCorner.X/(f32)Width,rectangle.LowerRightCorner.Y/(f32)Height); 
		left_top.X = 2*left_top.X-1;
		bottom_right.X = 2*bottom_right.X-1;

		left_top.Y = -2*left_top.Y+1;
		bottom_right.Y = -2*bottom_right.Y+1;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
   
		//0,0
		glTexCoord2f(0.0,0.0); 
		glVertex2f(left_top.X,left_top.Y);
		
		//1,0
		glTexCoord2f(1.0f,0.0f); 
		glVertex2f(bottom_right.X,left_top.Y);
		//1,1
		glTexCoord2f(1.0f,1.0f); 
		glVertex2f(bottom_right.X, bottom_right.Y);
		//0,1
		glTexCoord2f(0.0f,1.0f); 
		glVertex2f(left_top.X, bottom_right.Y);

		glEnd();
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D); 
	};

	void COpenGLDriver::present()
	{
		glFlush();
	}

	
	void COpenGLDriver::setView(const core::vector3df& pos,const core::vector3df& target,const core::vector3df& up)
	{
		//gluLookAt(0,0,100,0,0,99,0,1,0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(pos.X,pos.Y,pos.Z,target.X,target.Y,target.Z,up.X,up.Y,up.Z);
	};

	void COpenGLDriver::setViewport(s32 width,s32 height)
	{
		Width = width;
		Height = height;
	}
	core::vector2di COpenGLDriver::getViewport()
	{
		//return core::vector2di(640,480);
		return core::vector2di(Width,Height);
	}

	void COpenGLDriver::loadVertexShader(char* filename,char* entryPoint,CGprogram& vs_main)
	{
		//assert
		assert(cgIsContext(ShaderContext));

		vs_main = cgCreateProgramFromFile(ShaderContext, CG_SOURCE,filename,
			VertexProfile,entryPoint, NULL);	
		if (!cgIsProgramCompiled(vs_main))
			cgCompileProgram(vs_main);

		cgGLEnableProfile(VertexProfile);
		cgGLLoadProgram(vs_main);
		cgGLDisableProfile(VertexProfile);

		//exist some error?
		cgErrorCallback();
	};

	void COpenGLDriver::loadPixelShader(char* filename,char* entryPoint,CGprogram& ps_main)
	{
		//assert
		assert(cgIsContext(ShaderContext));

		ps_main = cgCreateProgramFromFile(ShaderContext, CG_SOURCE,filename,
			PixelProfile,entryPoint, NULL);	
		if (!cgIsProgramCompiled(ps_main))
			cgCompileProgram(ps_main);

		cgGLEnableProfile(PixelProfile);
		cgGLLoadProgram(ps_main);
		cgGLDisableProfile(PixelProfile);

		//exist some error?
		cgErrorCallback();
	};
		//test for opengl
	void COpenGLDriver::setShaderTexParam(char* par, unsigned int tex,const CGprogram &program,CGparameter param)
	{
		param = cgGetNamedParameter(program, par); 
		cgGLSetTextureParameter(param, tex); 
		cgGLEnableTextureParameter(param);
	};

	void COpenGLDriver::drawFullScreenQuad()
	{
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
   
		glTexCoord2f(0,0); 
		glVertex2f(0,0);

		glTexCoord2f(1,0); 
		glVertex2f(1,0);

		glTexCoord2f(1, 1); 

		glVertex2f(1, 1);
		glTexCoord2f(0, 1); 
		glVertex2f(0, 1);

		glEnd();
		glEnable(GL_DEPTH_TEST);
	};

	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];
	core::line3df COpenGLDriver::getRayFromScreenPoint(const core::vector2di& posInScreen)
	{
		core::line3df line;
		
		glGetIntegerv(GL_VIEWPORT, viewport);   
		glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);
		double dX, dY, dZ;  
		gluUnProject (posInScreen.X, Height-posInScreen.Y, 0.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
		line.start = core::vector3df( (float) dX, (float) dY, (float) dZ );
		gluUnProject (posInScreen.X, Height-posInScreen.Y, 1.0, mvmatrix, projmatrix, viewport, &dX, &dY, &dZ);
		line.end  = core::vector3df( (float) dX, (float) dY, (float) dZ );

		line.end = line.end - line.start;
		line.end.normalize();
		return line;
	};
	void COpenGLDriver::drawQuad(const core::vector2di& leftTop,const core::vector2di& bottomRight)
	{
		core::vector2df left_top(leftTop.X/(f32)Width,leftTop.Y/(f32)Height); 
 		core::vector2df bottom_right((leftTop.X+bottomRight.X)/(f32)Width,bottomRight.Y/(f32)Height);  
 		
		left_top.X = 2*left_top.X-1;
		bottom_right.X = 2*bottom_right.X-1;

		left_top.Y = -2*left_top.Y+1;
		bottom_right.Y = -2*bottom_right.Y+1;
		glEnable(GL_TEXTURE_2D);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glDisable(GL_DEPTH_TEST); 
 		glBegin(GL_QUADS); 
     
 		//0,0 
 		glTexCoord2f(0,0);  
 		glVertex2f(left_top.X,left_top.Y); 
 		 
 		//1,0 
 		glTexCoord2f(1,0);  
 		glVertex2f(bottom_right.X,left_top.Y); 
 		//1,1 
 		glTexCoord2f(1,1);  
 		glVertex2f(bottom_right.X, bottom_right.Y); 
 		//0,1 
 		glTexCoord2f(0,1);  
 		glVertex2f(left_top.X, bottom_right.Y); 
 

 		glEnd(); 
 		glEnable(GL_DEPTH_TEST); 
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glDisable(GL_TEXTURE_2D);
		/*
		core::vector2df left_top(leftTop.X/(f32)Width,leftTop.Y/(f32)Height);
		core::vector2df bottom_right(bottomRight.X/(f32)Width,bottomRight.Y/(f32)Height); 
		left_top.X = 2*left_top.X-1;
		bottom_right.X = 2*bottom_right.X-1;

		left_top.Y = -2*left_top.Y+1;
		bottom_right.Y = -2*bottom_right.Y+1;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();
		//glTranslatef(-1.0f,1.0f,0.0f);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		//glColor3ub(255,255,255);
		glDisable(GL_DEPTH_TEST);
		glBegin(GL_QUADS);
   
		//0,0
		glTexCoord2f(left_top.X,left_top.Y); 
		glVertex2f(left_top.X,left_top.Y);
		
		//1,0
		glTexCoord2f(bottom_right.X,left_top.Y); 
		glVertex2f(bottom_right.X,left_top.Y);
		//1,1
		glTexCoord2f(bottom_right.X, bottom_right.Y); 
		glVertex2f(bottom_right.X, bottom_right.Y);
		//0,1
		glTexCoord2f(left_top.X, bottom_right.Y); 
		glVertex2f(left_top.X, bottom_right.Y);

		glEnd();
		glEnable(GL_DEPTH_TEST);

		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();*/
	};
	//############################################################
	IVideoDriver* createOpenGLDriver(int width,int height)
	{
		return new COpenGLDriver(width,height);
	};
	
}
}

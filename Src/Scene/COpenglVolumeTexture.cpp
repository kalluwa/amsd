//created by kalluwa
//2014/2/7

#include "COpenglVolumeTexture.h"

namespace kk
{
namespace scene
{
	
COpenglVolumeTexture::COpenglVolumeTexture(video::IVideoDriver* driver)
	:IVolumeTexture(driver),SizeX(0),SizeY(0),SizeZ(0),Data(0)
{

}

COpenglVolumeTexture::COpenglVolumeTexture(video::IVideoDriver* driver,int _sizeX,int _sizeY,int _sizeZ)
	:IVolumeTexture(driver),SizeX(_sizeX),SizeY(_sizeY),SizeZ(_sizeZ),Data(0)
{

}

COpenglVolumeTexture::COpenglVolumeTexture(video::IVideoDriver* driver,const char* fileName,int sizeX,int sizeY,int sizeZ)
	:IVolumeTexture(driver),SizeX(sizeX),SizeY(sizeY),SizeZ(sizeZ),Data(0)
{
	if(strlen(fileName)>0)
		this->loadVolumeFromFile(fileName,sizeX,sizeY,sizeZ);
}
//deconstructor
COpenglVolumeTexture::~COpenglVolumeTexture()
{
	if(Data)
		delete []Data;
}

void COpenglVolumeTexture::loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ)
{
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0);

	int sizeVoxel = sizeX*sizeY*sizeZ;
	int size = sizeX*sizeY*sizeZ*4;
	Data = new u8[size];

	FILE* fp = 0;//fopen(fileName,"r");
	fopen_s(&fp,fileName,"r");
	if(!fp)
	{
#ifdef WIN32
		char tmp[256];
		sprintf_s(tmp,"file[%s] not found",fileName);
		OutputDebugStringA(tmp);
#endif
		return;
	}

	//read volume data
	fread(Data,4,sizeVoxel,fp);

	//close file
	fclose(fp);

	//find max value
	float maxValue=0;
	float minValue=0;
	float* data_f = (float*)Data;
	for(int i=0;i<sizeVoxel;i++)
	{
		if(data_f[i]>maxValue)
			maxValue = data_f[i];
		else if(data_f[i]<minValue)
			minValue = data_f[i];
	}

	float inverserRangeMulti255 = 255*1/(maxValue - minValue);
	//calculate rgba
	for(int i=0;i<sizeVoxel;i++)
	{
		u8 byte = 255;//(data_f[i]-minValue)*inverserRangeMulti255;
			if(data_f[i]<0.1f)//fThreshold)
				byte=0;
	
		//memset(data+
		Data[4*i+0] = 255;
		Data[4*i+1] = 255;
		Data[4*i+2] = 255;
		Data[4*i+3] = byte;
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volumeTexture);
	glBindTexture(GL_TEXTURE_3D, volumeTexture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, sizeX, sizeY, sizeZ, 0, GL_RGBA, GL_UNSIGNED_BYTE,Data);


	//set render state
	Driver->loadVertexShader("raycasting_shader.cg","vertex_main",vs_main);
	Driver->loadPixelShader("raycasting_shader.cg","fragment_main",ps_main);

	// Create the to FBO's one for the backside of the volumecube and one for the finalimage rendering
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);

	int WINDOW_SIZE_X = Driver->getViewport().X;
	int WINDOW_SIZE_Y = Driver->getViewport().Y;
	glGenTextures(1, &backface_buffer);
	glBindTexture(GL_TEXTURE_2D, backface_buffer);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0);

	glGenTextures(1, &final_image);
	glBindTexture(GL_TEXTURE_2D, final_image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenRenderbuffersEXT(1, &renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
};

void COpenglVolumeTexture::setRenderState()
{
	glDisable(GL_LIGHTING);
}
void resize(int w, int h)
{
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)w/(GLfloat)h, 0.1f, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	
}

void COpenglVolumeTexture::render()
{
	resize(Driver->getViewport().X,Driver->getViewport().Y);
	glTranslatef(-0.5f,-0.5f,-0.5f);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, framebuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);

	render_backface();
	raycasting_pass();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	render_buffer_to_screen();
}

void COpenglVolumeTexture::reshape_ortho(int w, int h)
{
	if (h == 0) h = 1;
	//glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	
}

void COpenglVolumeTexture::vertex(float x, float y, float z)
{
	glColor3f(x,y,z);
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, x, y, z);
	glVertex3f(x,y,z);
}
void COpenglVolumeTexture::drawQuads(float x, float y, float z)
{
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1.0);
	vertex(0.0, 0.0, 0.0);
	vertex(0.0, y, 0.0);
	vertex(x, y, 0.0);
	vertex(x, 0.0, 0.0);

	glNormal3f(0.0, 0.0, 1.0);
	vertex(0.0, 0.0, z);
	vertex(x, 0.0, z);
	vertex(x, y, z);
	vertex(0.0, y, z);

	glNormal3f(0.0, 1.0, 0.0);
	vertex(0.0, y, 0.0);
	vertex(0.0, y, z);
    vertex(x, y, z);
	vertex(x, y, 0.0);

	glNormal3f(0.0, -1.0, 0.0);
	vertex(0.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0);
	vertex(x, 0.0, z);
	vertex(0.0, 0.0, z);

	glNormal3f(-1.0, 0.0, 0.0);
	vertex(0.0, 0.0, 0.0);
	vertex(0.0, 0.0, z);
	vertex(0.0, y, z);
	vertex(0.0, y, 0.0);

	glNormal3f(1.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0);
	vertex(x, y, 0.0);
	vertex(x, y, z);
	vertex(x, 0.0, z);
	glEnd();
	
}

void COpenglVolumeTexture::render_buffer_to_screen()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,final_image);
	//glBindTexture(GL_TEXTURE_2D,backface_buffer);
	reshape_ortho(Driver->getViewport().X,Driver->getViewport().Y);
	Driver->drawFullScreenQuad();
	glDisable(GL_TEXTURE_2D);

}
void COpenglVolumeTexture::render_backface()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawQuads(1.0,1.0, 1.0);
	glDisable(GL_CULL_FACE);
}

void COpenglVolumeTexture::raycasting_pass()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, final_image, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	cgGLEnableProfile(Driver->getVSProfile());
	cgGLEnableProfile(Driver->getPSProfile());
	cgGLBindProgram(vs_main);
	cgGLBindProgram(ps_main);
	cgGLSetParameter1f( cgGetNamedParameter( ps_main, "stepsize") , 0.02f);
	Driver->setShaderTexParam("tex",backface_buffer,ps_main,param1);
	Driver->setShaderTexParam("volume_tex",volumeTexture,ps_main,param2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(1.0,1.0, 1.0);
	glDisable(GL_CULL_FACE);
	cgGLDisableProfile(Driver->getVSProfile());
	cgGLDisableProfile(Driver->getPSProfile());
}

//resize the frame buffers
void COpenglVolumeTexture::OnEvent(SEvent event)
{
	if(event.type = ET_USER)
	{
		if(event.UserData.type == EUT_RESIZE)
		{
			int WINDOW_SIZE_X = Driver->getViewport().X;
			int WINDOW_SIZE_Y = Driver->getViewport().Y;
			glBindTexture(GL_TEXTURE_2D, backface_buffer);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0);

			glBindTexture(GL_TEXTURE_2D, final_image);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_SIZE_X, WINDOW_SIZE_Y);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}
	
}

}
}

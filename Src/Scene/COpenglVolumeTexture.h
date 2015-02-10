//created by kalluwa
//2014/2/7

#ifndef _C_OPENGL_VOLUME_TEXTURE_H_
#define _C_OPENGL_VOLUME_TEXTURE_H_

#include "IVolumeTexture.h"
#include "../Driver/COpenGLDriver.h"

#include <GL/GL.h>
#include<GL/glew.h>

namespace kk
{
namespace scene
{
class COpenglVolumeTexture : public virtual IVolumeTexture
{
public:
//created a sized data,fill it later
	COpenglVolumeTexture(video::IVideoDriver* driver);

	COpenglVolumeTexture(video::IVideoDriver* driver,int sizeX,int sizeY,int sizeZ);

//create a volumeTexture from file
	COpenglVolumeTexture(video::IVideoDriver* driver,const char* fileName,int sizeX,int sizeY,int sizeZ);
//deconstructor
	virtual ~COpenglVolumeTexture();
//load volume 
	virtual void loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ);
//set render state
	virtual void setRenderState();
//render
	virtual void render();

	void reshape_ortho(int w, int h);

	void vertex(float x, float y, float z);

	void drawQuads(float x, float y, float z);

	void render_buffer_to_screen();

	void render_backface();

	void raycasting_pass();

	virtual void OnEvent(SEvent event);
private:
	//volume data
	u8* Data;
	//volume size
	s32 SizeX,SizeY,SizeZ;
	//volume texture handle

	CGprogram vs_main,ps_main;
	unsigned int volumeTexture;
	unsigned int framebuffer,backface_buffer;
	unsigned int final_image,renderbuffer;
	CGparameter param1,param2;
};
}
}

#endif //end opengl volume texture

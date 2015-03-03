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
	COpenglVolumeTexture(video::IVideoDriver* driver,const char* fileName,int sizeX,int sizeY,int sizeZ,int startSlice = 0);
//deconstructor
	virtual ~COpenglVolumeTexture();
//load volume 
	virtual void loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ,int startSlice=0);
//set render state
	virtual void setRenderState();
//render
	virtual void render();
//get pointed data
	virtual core::aabbox3df getPointedData(const core::line3df& ray);

	virtual void getNeighborData(s32 ix,s32 iy,s32 iz,s32& minX,s32& minY,s32& minZ,s32& maxX,s32& maxY,s32& maxZ);

	virtual void getNeighborData2(s32 ix,s32 iy,s32 iz,s32& minX,s32& minY,s32& minZ,s32& maxX,s32& maxY,s32& maxZ);

	void reshape_ortho(int w, int h);

	void vertex(float x, float y, float z);

	void drawQuads(float x, float y, float z);

	void render_buffer_to_screen();

	void render_backface();

	void raycasting_pass();

	virtual void OnEvent(SEvent event);
private:
	//volume data
	f32* Data;
	bool*DataVisited;
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

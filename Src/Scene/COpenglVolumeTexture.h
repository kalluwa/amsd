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
	COpenglVolumeTexture(video::IVideoDriver* driver,const char* fileName,int sizeX,int sizeY,int sizeZ,f32 mmInX,f32 mmInY,f32 mmInZ,int startSlice = 0,bool ZInverseLoad=false);
//deconstructor
	virtual ~COpenglVolumeTexture();
//load volume 
	virtual void loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ,int startSlice=0,bool ZInverseLoad=false);
//set render state
	virtual void setRenderState();
//render
	virtual void render();
//get pointed data
	virtual core::aabbox3df getPointedData(const core::line3df& ray,bool appendBox,f32 threshold);
//get volume data
	virtual f32* getVolumeData();
//get volume size
	virtual s32 getVolumeSizeX(){return SizeX;};
	virtual s32 getVolumeSizeY(){return SizeY;};
	virtual s32 getVolumeSizeZ(){return SizeZ;};
//get scale
	virtual f32 getMilimeterInX(){ return mmInX;};
	virtual f32 getMilimeterInY(){ return mmInY;};
	virtual f32 getMilimeterInZ(){ return mmInZ;};
//get selected bounding box
	virtual core::aabbox3di getSelectedBoundingBox(){ return SelectedBox; };

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
	//selected bounding box
	core::aabbox3di SelectedBox;
	//volume texture handle
	f32 mmInX,mmInY,mmInZ;

	CGprogram vs_main,ps_main;
	unsigned int volumeTexture;
	unsigned int framebuffer,backface_buffer;
	unsigned int final_image,renderbuffer;
	CGparameter param1,param2;
};

}
}

#endif //end opengl volume texture

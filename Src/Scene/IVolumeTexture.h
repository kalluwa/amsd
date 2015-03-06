//created by kalluwa
//2015/2/7

#ifndef _I_VOLUME_TEXTURE_H_
#define _I_VOLUME_TEXTURE_H_

#ifdef WIN32
#include <Windows.h>
#endif

#include "../Core/core.h"
#include "ISceneNode.h"

namespace kk
{
namespace scene
{
class IVolumeTexture : public virtual ISceneNode
{
public:
	IVolumeTexture(video::IVideoDriver* driver);

	virtual void OnEvent(SEvent event){};

	virtual ~IVolumeTexture(){}
//load volume data
	virtual void loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ,int startSlice)=0;
//set Driver States before rendering[we should close opengl light before rendering]
	virtual void setRenderState() = 0;
//render volume data to screen
	//virtual void render()=0;
//get pointed data
	virtual core::aabbox3df getPointedData(const core::line3df& ray,bool appendBox=false)=0;

	virtual f32* getVolumeData()=0;

//get volume real size
	virtual s32 getVolumeSizeX()=0;
	virtual s32 getVolumeSizeY()=0;
	virtual s32 getVolumeSizeZ()=0;
//get scale
	virtual f32 getMilimeterInX()=0;
	virtual f32 getMilimeterInY()=0;
	virtual f32 getMilimeterInZ()=0;
//get last pointed box
	virtual core::aabbox3di getSelectedBoundingBox()=0;

};

}
}
#endif // volume texture

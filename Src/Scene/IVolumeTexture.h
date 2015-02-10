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
	virtual void loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ)=0;
//set Driver States before rendering[we should close opengl light before rendering]
	virtual void setRenderState() = 0;
//render volume data to screen
	//virtual void render()=0;

};

}
}
#endif // volume texture
//2015/3/4
#ifndef _C_IMAGE_H_
#define _C_IMAGE_H_
#include <Windows.h>
#include "IImage.h"

namespace kk
{
namespace scene
{

class CImageOpenGL
{
public:
	CImageOpenGL();
	CImageOpenGL(f32* data,s32 width,s32 height,bool addEdge=false,bool logData = false);

	virtual ~CImageOpenGL();
	virtual f32* getData(){return Data;};

	virtual void fillData(f32* data,s32 width,s32 height,bool addEdge,bool logData=false);
	virtual void render();

	virtual core::vector2di getImageSize();
	virtual void applyTexture();

	virtual s32 getImageWidth();
	virtual s32 getImageHeight();

private:
	s32 Width,Height;
	f32* Data;
	u32 TextureID;
};//end class
}
}
#endif 
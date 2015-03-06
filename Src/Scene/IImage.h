//2015/3/4
#ifndef _I_IMAGE_H_
#define _I_IMAGE_H_

#include "../Core/core.h"

namespace kk
{
namespace scene
{
	class IImage
	{
	public:
		//IImage():Width(0),Height(0) {}
		//IImage(s32 width,s32 height):Width(width),Height(height){};
		//deconstruct
		virtual ~IImage(){};
		//get data [ for CT use ]
		virtual f32* getData()=0;
		//fill data and generate a Device Dependent Texture
		virtual void fillData(f32* data,s32 width,s32 height)=0;
		virtual void render()=0;
		//get image size
		virtual core::vector2di getImageSize() = 0;
		virtual s32 getImageWidth()=0;
		virtual s32 getImageHeight()=0;
	};//end class

}//end scene
}//end namespace kk

#endif

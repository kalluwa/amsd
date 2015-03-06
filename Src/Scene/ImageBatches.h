//2015/3/4

#ifndef _IMAGE_BATCHES_H_
#define _IMAGE_BATCHES_H_H

#include "ISceneNode.h"
#include "CImageOpenGL.h"

namespace kk
{
namespace scene
{
struct ImageItem
{	
//img to be drawn
	CImageOpenGL* img;
//drawing destination
	core::rect<s32> rectangle;
	
	bool operator < (const ImageItem& item) const
	{
		return ((u32)img) < ((u32)item.img);
	};
	
	bool operator == (const ImageItem& item) const
	{
		return ((u32)img == (u32)item.img)&&
			(rectangle == item.rectangle);
	}
};

class ImageBatches : public virtual ISceneNode
{
public:
	ImageBatches(kk::video::IVideoDriver* driver=0);
	//destroy images' data
	virtual ~ImageBatches();
	//add a new image
	virtual void addImage(CImageOpenGL* img,const core::rect<s32>& rectangle);
	//remove image
	//void removeImage(CImage* img);
	//render these images
	virtual void render();

	virtual void OnEvent(SEvent e);

	virtual void clearAllTextures();
protected:
	//images
	core::array<ImageItem> Images;

};//end class
}
}
#endif

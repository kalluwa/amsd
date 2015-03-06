//2015/3/4

#include "ImageBatches.h"

namespace kk
{
namespace scene
{
ImageBatches::ImageBatches(kk::video::IVideoDriver* driver)
	:ISceneNode(driver)
{
	setDebugName("ImageBatches");
}

ImageBatches::~ImageBatches()
{
	for(s32 i=Images.size()-1;i>=0;i--)
	{
		//if(Images[i].img)
		delete Images[i].img;
		Images[i].img = 0;
	}

	Images.clear();
}

void ImageBatches::addImage(CImageOpenGL* img,const core::rect<s32>& destPos)
{
	ImageItem item;
	item.img = img;
	item.rectangle  = destPos;
	s32 result=Images.binary_search(item);
	if(result!=-1 && destPos == Images[result].rectangle)
		return;

	Images.push_back(item);//we just need one copy
}


void ImageBatches::render()
{
//render all images
	for(s32 i=0;i<(s32)Images.size();i++)
	{
		Images[i].img->applyTexture();
		Driver->drawQuad(Images[i].rectangle.UpperLeftCorner,
			Images[i].rectangle.LowerRightCorner);
		//Driver->drawImage(Images[i].img->getData(),Images[i].img->getImageWidth(),
		//	Images[i].img->getImageHeight(),Images[i].rectangle);
	}
	
}

void ImageBatches::clearAllTextures()
{
	Images.clear();
}
void ImageBatches::OnEvent(SEvent e)
{
	//nothing will happen here
};
}
}

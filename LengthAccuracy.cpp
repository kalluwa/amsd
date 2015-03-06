//2015/3/3

#include "LengthAccuracy.h"
//debug
#include "Src/Scene/CImageOpenGL.h"
#include "Src/Scene/ImageBatches.h"
#include "Src/Scene/IVolumeTexture.h"
#include <Windows.h>
#include <queue>


namespace Calculation
{
	
	//just static function header
void Amsd_LengthAccuracy(BoxData* data,kk::scene::ISceneManager* scene)
{
	f32* cornerImage=0;
	s32 width=0;
	s32 height=0;
	data->getCoronalImage(cornerImage,width,height);
	//fill image edge and we will get two hole

	//get max and min value
	f32 maxValue=0.0f,minValue = 1.0f;
	for(s32 i=0;i<height*width;i++)
	{
		if(cornerImage[i]>maxValue)
			maxValue = cornerImage[i];
		else if(cornerImage[i]<minValue)
			minValue = cornerImage[i];
	}
	f32 halfMax = 0.5f*maxValue;
	//get F1
	f32* F1;
	F1= new f32[width*height];
	memcpy(F1,cornerImage,width*height*sizeof(f32));
	for(s32 i=0;i<width*height;i++)
		if(F1[i]>halfMax)
			F1[i]=1.0f;
		else
			F1[i]=0.0f;
	//get F2
	f32* F2;
	F2= new f32[width*height];
	memcpy(F2,F1,width*height*sizeof(f32));
	for(s32 i=0;i<width*height;i++)
		F2[i]=1.0f-F1[i];
	//remove outline pixels
	std::queue<core::vector2di> checkPos;
	for(s32 i=0;i<width;i++)
	{ 
		checkPos.push(core::vector2di(i,0));
		checkPos.push(core::vector2di(i,height-1));
	}
	for(s32 i=0;i<height;i++)
	{
		checkPos.push(core::vector2di(0,i));
		checkPos.push(core::vector2di(width-1,i));
	}
#define removePixel(x,y) F2[(x)+(y)*width]=0.0f
#define pixelFailTest(x,y) (F2[(x)+(y)*width]==1.0f)
	while(checkPos.size())
	{
		core::vector2di curPos = checkPos.front();
		removePixel(curPos.X,curPos.Y);
		checkPos.pop();
		//add its neighbor
		s32 nX = (curPos.X-1)>=0?(curPos.X-1):0;
		s32 nY = curPos.Y;
		if(pixelFailTest(nX,nY))
			checkPos.push(core::vector2di(nX,nY));
		
		nX = (curPos.X+1)<width?(curPos.X+1):width-1;
		nY = curPos.Y;
		if(pixelFailTest(nX,nY))
			checkPos.push(core::vector2di(nX,nY));
		
		nX = curPos.X;
		nY = (curPos.Y-1)>=0?(curPos.Y-1):0;
		if(pixelFailTest(nX,nY))
			checkPos.push(core::vector2di(nX,nY));
		
		nX = curPos.X;
		nY = (curPos.Y+1)<height?(curPos.Y+1):height-1;
		if(pixelFailTest(nX,nY))
			checkPos.push(core::vector2di(nX,nY));
	}

	//calculate the center
	//hole1
	f32 xSum=0.0f,nSum=0.0f,zSum=0.0f;
	s32 step = width/6;
	for(s32 z=0;z<height;z++)
	{
		for(s32 x=0;x<step;x++)
		{
			s32 index = z*width+x;
			f32 tmpValue = F2[index]*(halfMax-cornerImage[index]);
			xSum +=tmpValue*z;
			zSum +=tmpValue*x;

			nSum +=tmpValue;
		}
	}
	f32 _xC1 = xSum / nSum;
	f32 _zC1 = zSum / nSum;
	//hole 2
	xSum=0.0f;nSum=0.0f;zSum=0.0f;
	for(s32 z=0;z<height;z++)
	{
		for(s32 x=width-step;x<width;x++)
		{
			s32 index = z*width+x;
			f32 tmpValue = F2[index]*(halfMax-cornerImage[index]);
			xSum +=tmpValue*z;
			zSum +=tmpValue*x;

			nSum +=tmpValue;
		}
	}
	f32 _xC2 = xSum / nSum;
	f32 _zC2 = zSum / nSum;
	//convert length to mm
	kk::scene::IVolumeTexture* volume = dynamic_cast<kk::scene::IVolumeTexture*>(scene->getSpecificNodeById("IVolumeTexture"));
	f32 xC1=_xC1*volume->getMilimeterInX();f32 xC2=_xC2*volume->getMilimeterInX();
	f32 zC1=_zC1*volume->getMilimeterInZ();f32 zC2=_zC2*volume->getMilimeterInZ();
	
	f32 Length = sqrtf((zC1-zC2)*(zC1-zC2)+(xC1-xC2)*(xC1-xC2));
	f32 alpha = asinf((xC2-xC1)/Length);

	f32 xCenter = (xC1+xC2)*0.5f;
	f32 zCenter = (zC1+zC2)*0.5f;
	//TODO:bag offset data need more information

	//get 2 subvolume
	
	f32 radiusX = 10.0f/volume->getMilimeterInX();//10/1.3333
	f32 radiusZ = 10.0f/volume->getMilimeterInZ();//10/6.67
	s32 strideInVoxel = (s32)(radiusX<radiusZ?radiusX:radiusZ);//need adjust?
	core::aabbox3di box1 = data->Box;
	core::aabbox3di box2 = data->Box;
	s32 baseX =box1.MinEdge.X;
	s32 baseZ =box1.MinEdge.Z;
	box1.MinEdge.X =baseX+ (s32)_xC1-strideInVoxel;
	box1.MaxEdge.X =baseX+ (s32)_xC1+strideInVoxel;
	box1.MinEdge.Z =baseZ+ (s32)_zC1-strideInVoxel;
	box1.MaxEdge.Z =baseZ+ (s32)_zC1+strideInVoxel;
	baseX =box2.MinEdge.X;
	baseZ =box2.MinEdge.Z;
	box2.MinEdge.X =baseX+ (s32)_xC2-strideInVoxel;
	box2.MaxEdge.X =baseX+ (s32)_xC2+strideInVoxel;
	box2.MinEdge.Z =baseZ+ (s32)_zC2-strideInVoxel;
	box2.MaxEdge.Z =baseZ+ (s32)_zC2+strideInVoxel;

	BoxData subVolume1(data->OriginalData,data->SizeX,data->SizeY,data->SizeZ,box1);
	BoxData subVolume2(data->OriginalData,data->SizeX,data->SizeY,data->SizeZ,box2);
	

	f32 sumValue = subVolume1.getSumValue();
	f32 sumValueY = subVolume1.getSumValumeY();

	f32 yC1 = sumValueY / sumValue+subVolume1.Box.MinEdge.Y;
	f32 sumValue2 = subVolume2.getSumValue();
	f32 sumValueY2 = subVolume2.getSumValumeY();

	f32 yC2 = sumValueY2 / sumValue2+subVolume2.Box.MinEdge.Y;

	f32 yCenter = (yC1+yC2)*0.5f;
	f32 value = Length / 609.6f;//actual length = 609.6mm
#if 1 //Debug
	kk::scene::ImageBatches* batches = dynamic_cast<kk::scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	
	kk::scene::CImageOpenGL* img = new kk::scene::CImageOpenGL(F2,width,height,true);
	batches->addImage(img,core::rect<s32>(0,0,width,height));

	char tmp[50]={0};
	sprintf_s(tmp,50,"length=%.3f value=%.3f alpha=%.2f",Length,value,alpha);
	::OutputDebugStringA(tmp);
#endif

	if(F2)
		delete []F2;
	if(F1)
		delete []F1;
	if(cornerImage)
	delete[] cornerImage;
};
}
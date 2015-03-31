//2015/3/6

#include "TrianglePathLength.h"
#include "../Src/scene/IVolumeTexture.h"
//debug
#include <windows.h>
#include "../src/scene/CImageOpenGL.h"
#include "../src/scene/ImageBatches.h"


namespace Calculation
{
//for simplified
using namespace kk;

void TrianglePathLength(BoxData* data,kk::scene::ISceneManager* scene,kk::io::IWriteFile* Output)
{
	f32* coronalImage = 0;
	s32 width =0;
	s32 height=0;
	//remove air
	f32 thresholdForAir = 0.1f;
	data->getCoronalImageEliminateAir(coronalImage,width,height,thresholdForAir);

	//find start and end pos
	core::array<s32> startAndEnd;
	for(s32 i=0;i<width;i++)
	{
		s32 start = -1;
		s32 end =-1;
		f32 averageValue=0.0f;
		for(s32 j=0;j<height;j++)
		{
			averageValue += coronalImage[i+j*width];
		}
		averageValue /=height;
		//find start pos
		s32 j=0;
		for(j=0;j<height;j++)
		{
			if(coronalImage[i+j*width] > averageValue)
			{
				start = j;break;
			}
		}
		//find end
		for(;j<height;j++)
		{
			if(coronalImage[i+j*width] < averageValue)
			{
				end = j;break;
			}
		}
		if(start==-1||end == -1)
		{ end = height -1; start =0;}

		startAndEnd.push_back(start);
		startAndEnd.push_back(end);
		
	}
	//get the median range values
	kk::scene::IVolumeTexture* volume = dynamic_cast<kk::scene::IVolumeTexture*>(scene->getSpecificNodeById("IVolumeTexture"));
	s32 halfNumVoxelToInclude=(s32)(10.0f/volume->getMilimeterInX());//20mm / 2 =10mm
	core::array<f32> medianValues;
	core::array<f32> arrayToSort2Cm;
	core::array<f32> pathLengths;
	for(s32 i=0;i<width;i++)
	{
		//calculate the path length
		pathLengths.push_back((startAndEnd[i*2+1]-startAndEnd[i*2])*volume->getMilimeterInX());
		//calculate the median value
		s32 medianPos = (startAndEnd[i*2]+startAndEnd[i*2+1])/2;
		s32 startPos = medianPos - halfNumVoxelToInclude;
		s32 endPos = medianPos + halfNumVoxelToInclude;
		if(startPos < 0) startPos = 0;
		if(endPos >= height) endPos = height-1;

		for(s32 j=startPos;j<=endPos;j++)
			arrayToSort2Cm.push_back(coronalImage[i+j*width]);

		arrayToSort2Cm.sort();
		medianValues.push_back(arrayToSort2Cm[arrayToSort2Cm.size()/2]);
		char tmp[50];
		sprintf_s(tmp,50,"length=%.3f",medianValues[medianValues.size()-1]);
		::OutputDebugStringA(tmp);
		arrayToSort2Cm.clear(); 
	}
	//we don't need start and end position anymore
	startAndEnd.clear();

	//find the slice which has the value closet to 10cm(100mm)
	s32 index=-1;
	f32 difference=100.0f;
	for(s32 i=0;i<width;i++)
		if(abs(pathLengths[i] - 100.0f)<difference)
		{
			difference = abs(pathLengths[i] - 100.0f);
			index = i;
		}

	//normalize all median values
	if(index==-1)
	{
		char tmp[50];
		sprintf_s(tmp,50,"error in calculate the Triangle");
		::OutputDebugStringA(tmp);
		return ;
	}
	for(s32 i=0;i<width;i++)
		medianValues[i] /=medianValues[index];

	//output 
	Output->writeString(core::stringc("\n\n指标二:\n路径长度:\n"));
	Output->writeArraySingle(medianValues,' ');
#if 1 //draw selected coronal image

	//coronal image
	scene::ImageBatches* batches =dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* img1 = new scene::CImageOpenGL(coronalImage,width,height,false);
	batches->addImage(img1,core::rect<s32>(0,0,width,height));

	//media result
	f32* medians = new f32[medianValues.size()];
	for(s32 i=0;i<(s32)medianValues.size();i++)
	{
		if(abs(medianValues[i]-1.0f)<1.0f)
			medians[i] = medianValues[i];
		else
			medians[i] = 1.0f;
	}
	//this image isn't correct
	//the true x-axis label is [path-length]
	scene::CImageOpenGL* img = new scene::CImageOpenGL(medians,medianValues.size(),1,false);
	batches->addImage(img,core::rect<s32>(width,0,width,height));

	delete []medians;
#endif
	//release data
	if(coronalImage)
		delete [] coronalImage;
	medianValues.clear();
}
}

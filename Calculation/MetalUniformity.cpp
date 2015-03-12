//2015/3/9

#include "NeqCalculation.h"

//for debug purpose
#include "../Src/scene/IVolumeTexture.h"
#include <Windows.h>
#include "../Src/scene/CImageOpenGL.h"
#include "../Src/scene/ImageBatches.h"

namespace Calculation
{
//debug
	s32 debugSlicePos = -1;
/*
in this function 
we need to figure out 3 parameters:Sout & MTF & NPS
they are 3 main part in the following code
*/
core::aabbox3di Amsd_MetalUniformity(BoxData* data,scene::ISceneManager* scene)
{
//pre process
//remove extra part shold be done in volumeTexture class
	//get max voxel count
	s32 maxValue=0;
	core::array<s32> passedVoxelCount;
	for(s32 slicePos = data->Box.MinEdge.Z;slicePos <= data->Box.MaxEdge.Z;slicePos++)
	{
		s32 count = data->getSliceZPassVoxelCount(slicePos,0.2f);
		if(count > maxValue)
			maxValue = count;

		passedVoxelCount.push_back(count);
	}
	//ignore those slice which valide voxel count < testValue
	s32 testValue = maxValue / 2;
	
	//find the longest valid range
	s32 realStart=-1,realEnd = -1;
	s32 maxValidCount=0;
	s32 tmpCount=0;
	for(s32 i=0;i<(s32)passedVoxelCount.size();i++)
	{
		f32 sliceMaxValue = data->getSliceZMaxValue(data->Box.MinEdge.Z+i);
		if(passedVoxelCount[i] > testValue || sliceMaxValue>6.0f)
		{
			tmpCount++;
			if(tmpCount>maxValidCount)
			{
				realStart = i-tmpCount;
				realEnd = i;
				maxValidCount = tmpCount;
			}
			//test
			if(sliceMaxValue)
			{
				//realStart = i-tmpCount;
				realEnd = i;
				maxValidCount = realEnd-realStart+1;
			}
		}
		else
		{
			tmpCount =0;
		}
	}
	passedVoxelCount.clear();
	//adjust boundingbox
	core::aabbox3di adjustedBox = data->Box;
	adjustedBox.MinEdge.Z = data->Box.MinEdge.Z+realStart;
	adjustedBox.MaxEdge.Z = data->Box.MinEdge.Z+realEnd;

	
#if 1 // for debug
	
	//char tm[50];
	//sprintf_s(tm,50,"maxValidCount=%i[%i %i]",maxValidCount,realStart,realEnd);
	//::OutputDebugStringA(tm);

	f32* sliceImage=0;
	s32 width=0;
	s32 height=0;
	if(debugSlicePos == -1)
	debugSlicePos = (data->Box.MinEdge.Z+data->Box.MaxEdge.Z)/2;
	data->getSliceZ(sliceImage,debugSlicePos,width,height);

	//image batches
	scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* img = new scene::CImageOpenGL(sliceImage,width,height,true);
	batches->addImage(img,core::rect<s32>(0,0,width,height));

	if(sliceImage)
		delete []sliceImage;
#endif
	
//1 for Sout Calculation
//Sout is average CT Value of the object

//2 NPS

//3 MTF

return adjustedBox;
}
}

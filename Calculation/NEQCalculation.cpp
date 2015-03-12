//2015/3/9

#include "NeqCalculation.h"
#include "../Src/Scene/IVolumeTexture.h"
//for debug purpose
#include <Windows.h>
#include "../Src/scene/CImageOpenGL.h"
#include "../Src/scene/ImageBatches.h"
//fft helper
#include "FFTHelper.h"

namespace Calculation
{

/*
in this function 
we need to figure out 3 parameters:Sout & MTF & NPS
they are 3 main part in the following code
*/
core::aabbox3di Amsd_NEQCalculation(BoxData* data,scene::ISceneManager* scene)
{
	data->Box.MinEdge.X-=5;
	data->Box.MinEdge.Y-=5;
	data->Box.MaxEdge.X+=5;
	data->Box.MaxEdge.Y+=5;
	data->updateSize();
//pre process
//remove extra part shold be done in volumeTexture class
#if 1 //pre process
	s32 Width = data->Box.getExtent().X;
	s32 Height = data->Box.getExtent().Y;
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
#endif //end pre process
	

//sout
#if 1
//1 for Sout Calculation
//Sout is average CT Value of the object
#endif

	SliceData* MiddleSlice;
//2 NPS
#if 1
	//get 32 paired images and get differences
	core::array<SliceData*> slices;
	s32 middleSliceIndex = (adjustedBox.MinEdge.Z + adjustedBox.MaxEdge.Z)/2;
	for(s32 i=adjustedBox.MinEdge.Z+3;i<adjustedBox.MaxEdge.Z-3;i+=2)
	{
		SliceData* slice1=new SliceData(data->OriginalData,data,i);
		SliceData* slice2=new SliceData(data->OriginalData,data,i+1);

		slice1->subSlice(slice2);
		slice1->resizeImage();
		delete slice2;
		slices.push_back(slice1);

		//mtf slice
		if(i==middleSliceIndex)
		{
			MiddleSlice = new SliceData(data->OriginalData,data,i);
			MiddleSlice->resizeImage();
		}
		else if(i+1 == middleSliceIndex)
		{
			middleSliceIndex = i+1;
			MiddleSlice = new SliceData(data->OriginalData,data,i+1);
			MiddleSlice->resizeImage();
		}
	}


	Width = slices[0]->Width;
	Height = slices[0]->Height;

	//apply radial window
	//scene::IVolumeTexture* volume = dynamic_cast<scene::IVolumeTexture>(scene->getSpecificNodeById("IVolumeTexture"));
	kk::scene::IVolumeTexture* volume = dynamic_cast<kk::scene::IVolumeTexture*>(scene->getSpecificNodeById("IVolumeTexture"));
	//TODO: requirement: must mmX == mmY
	f32 mmInXY = volume->getMilimeterInX();
	for(s32 i=0;i<(s32)slices.size();i++)
	{
		slices[i]->applyRadialWindow(mmInXY);
	}
	
	//apply 2 dimensional fft to each image=>get nps image[2d]
	core::array<SliceData*> squareAmplitudes;
	
	for(s32 i=0;i<(s32)slices.size();i++)
	{
		SliceData* sliceForAmplitude = new SliceData;
		sliceForAmplitude->Data = 0;
		Calculation::getImageDataSquareAmplitude(slices[i]->Data,sliceForAmplitude->Data,
			slices[i]->Width,slices[i]->Height);

		sliceForAmplitude->Width = slices[i]->Width;
		sliceForAmplitude->Height = slices[i]->Height;

		//debug
		if(sliceForAmplitude->getMinValue()<0.0f)
		{
			throw "切片大小不一致";
		}
		squareAmplitudes.push_back(sliceForAmplitude);
	}
	slices.clear();
	
	//average all nps image
	SliceData* npsAverage = new SliceData(Width,Height);
	for(s32 i=0;i<(s32)squareAmplitudes.size();i++)//sum
	{
		for(s32 x=0;x<(s32)Width;x++)
		{
		for(s32 y=0;y<(s32)Height;y++)
		{
			npsAverage->Data[x+y*Width] += squareAmplitudes[i]->Data[x+y*Width];
		}
		}
	}
	f32 inverseDivideNumber = 1.0f/(s32)squareAmplitudes.size();
	for(s32 x=0;x<(s32)Width;x++)//average
	{
	for(s32 y=0;y<(s32)Height;y++)
	{
		npsAverage->Data[x+y*Width] *= inverseDivideNumber;
	}
	}
	
	squareAmplitudes.clear();
	
	//scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	//scene::CImageOpenGL* img = new scene::CImageOpenGL(npsAverage->Data,npsAverage->Width,npsAverage->Height,true,true);
	//batches->addImage(img,core::rect<s32>(0,0,npsAverage->Width,npsAverage->Height));
	//

	//get 1d nps from 2d nps image
	//eg:xy:1.35 mm/voxel---FFT-->1 voxel-1   =  1 pixel-1
	//	=  (1.35mm)-1 ==7.4 cm-1
	// sample step = 0.5 cm-1

	//we use actual stride = 0.1px 
	core::vector2di centerNPS = core::vector2di(npsAverage->Width/2,npsAverage->Height/2);//npsAverage->getLocalCenter();
	f32 stride = 0.4f;
	f32 inverseStride = 1.0f/stride;
	//##########################unit
	f32 stepUnit = 1.0f/(0.1f*mmInXY);//7.4 cm-1
	stepUnit = stepUnit / ((Width - centerNPS.X)/stride);//7.4 /howManySteps cm-1
	//##########################unit
	s32 stepCount = (s32)(sqrtf((f32)((Width-centerNPS.X)*(Width-centerNPS.X)+
			(Height-centerNPS.Y)*(Height-centerNPS.Y)))/stride)+100;
	f32* nps1dValues = new f32[stepCount];//interval is stepUnit
	s32* nps1dValuesCount = new s32[stepCount];
	memset(nps1dValues,0,sizeof(f32)*stepCount);
	memset(nps1dValuesCount,0,sizeof(s32)*stepCount);
	for(s32 x=0;x<(s32)Width;x++)//average
	{
	for(s32 y=0;y<(s32)Height;y++)
	{
		//f32 distance = sqrtf((f32)((x-centerNPS.X)*(x-centerNPS.X)+
		//	(y-centerNPS.Y)*(y-centerNPS.Y)));
		f32 dis1 = sqrtf((f32)(x*x+y*y));
		f32 dis2 = sqrtf((f32)((Width-1-x)*(Width-1-x)+y*y));
		f32 dis3 = sqrtf((f32)(x*x+(Height-1-y)*(Height-1-y)));
		f32 dis4 = sqrtf((f32)((Width-1-x)*(Width-1-x)+(Height-1-y)*(Height-1-y)));

		f32 distance = min(min(dis1,dis2),min(dis3,dis4));
		distance *=inverseStride;
		if(distance >= stepCount)
			continue;//ignore 4 corner
		//set nps1d value
		nps1dValues[(s32)distance] += npsAverage->Data[x+y*Width];
		nps1dValuesCount[(s32)distance]++;
	}
	}
	//average nps1dvalue
	for(s32 i=0;i<stepCount;i++)
	{
		if(!nps1dValuesCount[i])
			continue;

		nps1dValues[i]/=nps1dValuesCount[i];
	}
	//get sample result
	s32 actualStride = (s32)(0.5f / stepUnit);
	core::array<f32> sampleValues;
	for(s32 i = actualStride; i<stepCount;i+= actualStride)
	{
		sampleValues.push_back(nps1dValues[i]);
	}

#if 1 // for debug
	
	//char tm[50];
	//sprintf_s(tm,50,"maxValidCount=%i[%i %i]",maxValidCount,realStart,realEnd);
	//::OutputDebugStringA(tm);

	//f32* sliceImage=0;
	s32 width=0;
	s32 height=0;
	//if(debugSlicePos == -1)
	//debugSlicePos = (data->Box.MinEdge.Z+data->Box.MaxEdge.Z)/2;
	//data->getSliceZ(sliceImage,debugSlicePos,width,height);

	f32* nps1dgraph = new f32[sampleValues.size()];
	for(s32 i=0;i<(s32)sampleValues.size();i++)
	{
		nps1dgraph[i]=sampleValues[i];
	}
	//image batches
	scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* img = new scene::CImageOpenGL(nps1dgraph,sampleValues.size(),1,true);
	batches->addImage(img,core::rect<s32>(0,0,Width,Height));

	//if(sliceImage)
		//delete []sliceImage;
#endif
	//clear for nps calculation
	sampleValues.clear();
	if(nps1dValues)
		delete []nps1dValues;
	if(nps1dValuesCount)
		delete []nps1dValuesCount;
	if(npsAverage)
		delete npsAverage;
#endif //nps
//3 MTF
	if(!MiddleSlice)
		throw "Not Found Center Slice";
	//process center slice
	//3_1: ERF
	/*scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* img = new scene::CImageOpenGL(MiddleSlice->Data,MiddleSlice->Width,MiddleSlice->Height,true);
	batches->addImage(img,core::rect<s32>(0,0,Width,Height));
*/
	//3_2: PSF
	//3_3: MTF
	//3_4: normalized MTF
	//release data
	if(MiddleSlice)
		delete MiddleSlice;
	

return adjustedBox;
}
}

//2015/3/9

#include "NeqCalculation.h"
#include "../Src/Scene/IVolumeTexture.h"
//for debug purpose
#include <Windows.h>
#include "../Src/scene/CImageOpenGL.h"
#include "../Src/scene/ImageBatches.h"

//fft helper
#include "FFTHelper.h"
//fit help.h
#include "PolyfitHelper.h"

//ctvalueconsistency
#include "CTValueConsistency.h"

namespace Calculation
{

/*
in this function 
we need to figure out 3 parameters:Sout & MTF & NPS
they are 3 main part in the following code
*/
core::aabbox3di Amsd_NEQCalculation(BoxData* data,scene::ISceneManager* scene)
{
	//make the rectangle larger to contain every pixel inside the ball
	s32 enlargeNumber = abs(data->Box.getExtent().X - data->Box.getExtent().Y);
	data->Box.MinEdge.X-=enlargeNumber;
	data->Box.MinEdge.Y-=enlargeNumber;
	data->Box.MaxEdge.X+=enlargeNumber;
	data->Box.MaxEdge.Y+=enlargeNumber;
	data->updateSize();
//pre process
//remove extra part shold be done in volumeTexture class
#if 1 //pre process
	s32 Width = data->Box.getExtent().X;
	s32 Height = data->Box.getExtent().Y;
	//get max voxel count
	s32 maxValue=0;
	core::array<s32> passedVoxelCount;
	f32 tmpMin,tmpMax;
	for(s32 slicePos = data->Box.MinEdge.Z;slicePos <= data->Box.MaxEdge.Z;slicePos++)
	{
		s32 count = data->getSliceZPassVoxelCount(slicePos,tmpMax,tmpMin,0.2f);
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

	//#####################################################
	//perform CT Value Consistency
	CTValueConsistencyResult consistency_result= Amsd_CTValueConsistency(data,scene,slices);
	//#####################################################
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
	s32 NPS_stepCount = (s32)(sqrtf((f32)((Width-centerNPS.X)*(Width-centerNPS.X)+
			(Height-centerNPS.Y)*(Height-centerNPS.Y)))/stride)+100;
	f32* nps1dValues = new f32[NPS_stepCount];//interval is stepUnit
	s32* nps1dValuesCount = new s32[NPS_stepCount];
	memset(nps1dValues,0,sizeof(f32)*NPS_stepCount);
	memset(nps1dValuesCount,0,sizeof(s32)*NPS_stepCount);
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
		if(distance >= NPS_stepCount)
			continue;//ignore 4 corner
		//set nps1d value
		nps1dValues[(s32)distance] += npsAverage->Data[x+y*Width];
		nps1dValuesCount[(s32)distance]++;
	}
	}
	//average nps1dvalue
	for(s32 i=0;i<NPS_stepCount;i++)
	{
		if(!nps1dValuesCount[i])
			continue;

		nps1dValues[i]/=nps1dValuesCount[i];
	}
	//get sample result
	s32 actualStride = (s32)(0.5f / stepUnit);
	core::array<f32> sampleValues;
	for(s32 i = actualStride; i<NPS_stepCount;i+= actualStride)
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
	//scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	//scene::CImageOpenGL* img = new scene::CImageOpenGL(nps1dgraph,sampleValues.size(),1,true);
	//batches->addImage(img,core::rect<s32>(0,0,Width,Height));

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

	f32 ERF_step = 0.1f;//.1 pixel
	core::vector2di ERF_center = core::vector2di(Width/2,Height/2);
	//get longest distance to center
	f32 distance_1 = sqrtf((f32)(ERF_center.X*ERF_center.X+ERF_center.Y+ERF_center.Y));
	f32 distance_2 = sqrtf((f32)((Width-1-ERF_center.X)*(Width-1-ERF_center.X)+ERF_center.Y+ERF_center.Y));
	f32 distance_3 = sqrtf((f32)(ERF_center.X*ERF_center.X+(Height-1-ERF_center.Y)+(Height-1-ERF_center.Y)));
	f32 distance_4 = sqrtf((f32)((Width-1-ERF_center.X)*(Width-1-ERF_center.X)+(Height-1-ERF_center.Y)+(Height-1-ERF_center.Y)));

	f32 maxDistance = max(max(distance_1,distance_2),max(distance_3,distance_4));
	s32 ERF_stepCount = (s32)(maxDistance/ERF_step)+1;
	f32* ERF_array = new f32[ERF_stepCount];
	s32* ERF_array_count = new s32[ERF_stepCount];
	memset(ERF_array,0,sizeof(f32)*ERF_stepCount);
	memset(ERF_array_count,0,sizeof(s32)*ERF_stepCount);
	f32 ERF_maxValue=-10000.0f,ERF_minValue = 10000.0f;
	for(s32 y=0;y<Height;y++)
	{
		for(s32 x=0;x<Width;x++)
		{
			f32 distance = sqrtf((f32)(x-ERF_center.X)*(x-ERF_center.X)+(y-ERF_center.Y)*(y-ERF_center.Y));
			ERF_array[(s32)distance] += MiddleSlice->Data[y*Width+x];
			ERF_array_count[(s32)distance]++;

			if(MiddleSlice->Data[y*Width+x]>ERF_maxValue)
				ERF_maxValue = MiddleSlice->Data[y*Width+x];
			if(MiddleSlice->Data[y*Width+x]<ERF_minValue)
				ERF_minValue =MiddleSlice->Data[y*Width+x];
		}
	}
	for(s32 i=0;i<ERF_stepCount;i++)
	{
		if(ERF_array_count[i])
			ERF_array[i] /=ERF_array_count[i];
	}
	//get valid range
	f32 thresholdForValidRange = (ERF_minValue + ERF_maxValue) / 2;
	s32 ERF_validStart =0,ERF_validEnd = ERF_stepCount;
	//if we have a continuous array which all pixel value inside > threshold
	//we mark it as the start
	s32 validCount=0,validCountTest = (s32)(10/ERF_step);//0.1pixel->100 
	for(s32 i=0;i<ERF_stepCount;i++)
	{
		if(ERF_array[i]>thresholdForValidRange)
		{
			validCount++;
			if(validCount > validCountTest)
			{
				ERF_validStart = i-validCountTest/2+1;
				break;
			}
		}
		else
		{
			validCount = 0;
		}
	}
	validCount =0;
	for(s32 i=ERF_stepCount-1;i>0;i--)
	{
		if(ERF_array[i]<thresholdForValidRange)
		{
			validCount++;
			if(validCount > validCountTest)
			{
				ERF_validEnd = i+validCountTest/2-1;
				break;
			}
		}
		else
		{
			validCount = 0;
		}
	}

	if(ERF_validStart >= ERF_validEnd)
		throw "Not Found Valid Range In ERF";
	//a piece-wise, least-squares cubic fit 
	f32* fitResult = new f32[ERF_validEnd-ERF_validStart+1];
	for(s32 i=ERF_validStart;i<=ERF_validEnd;i++)
	{
		fitResult[i-ERF_validStart]= ERF_array[i];
	}
	//fit 11 points and replace the 6th 
	s32 ERF_fitNumber =11;
	s32 ERF_fitMiddleIndex = ERF_fitNumber/2;//=5
	for(s32 i=ERF_validStart+ERF_fitMiddleIndex;i<=ERF_validEnd-ERF_fitMiddleIndex;i++)
	{
		fitResult[i-ERF_validStart]= getPolyfitCubicMiddleResult(ERF_array+i-ERF_fitMiddleIndex,ERF_fitNumber);
	}
	
	//3_2: PSF
	s32 PSF_count = ERF_validEnd-ERF_fitNumber-(ERF_validStart+ERF_fitNumber)+1;
	f32* PSF =new f32[PSF_count];
	f32 PSF_minValue =10000.0f;
	f32 PSF_maxValue =-10000.0f;
	f32 PSF_absMaxValue=-10000.0f;
	s32 PSF_absMaxIndex=0;

	for(s32 i=ERF_fitNumber;i<=ERF_validEnd-ERF_validStart-2*ERF_fitNumber;i++)//2 should be removed
	{
		PSF[i-ERF_fitNumber]=
			getPolyfitCubicDerivativeResult(fitResult+i-ERF_fitMiddleIndex,ERF_fitNumber);

		//get min and max value for normalizing psf
		if(PSF[i-ERF_fitNumber]>PSF_maxValue)
			PSF_maxValue = PSF[i-ERF_fitNumber];
		if(PSF[i-ERF_fitNumber]<PSF_minValue)
			PSF_minValue =PSF[i-ERF_fitNumber];
		if(PSF_absMaxValue < abs(PSF[i-ERF_fitNumber]))
		{
			PSF_absMaxValue = abs(PSF[i-ERF_fitNumber]);
			PSF_absMaxIndex = i;
		}
	}
	//normalize the PSF
	f32 PSF_inverseScale = 1.0f;
	if(PSF_maxValue>PSF_minValue)
		PSF_inverseScale = 1.0f/(PSF_maxValue-PSF_minValue);
	for(s32 i=0;i<PSF_count;i++)
	{
		PSF[i]*=PSF_inverseScale;
	}
#ifdef _DEBUG

	scene::ImageBatches* batchesPSF = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* imgPSF = new scene::CImageOpenGL(PSF,100,1,true);
	batchesPSF->addImage(imgPSF,core::rect<s32>(0,Height+2,Width,2*Height+2));
#endif

	//3_3: MTF
	//valid range
	s32 MTF_start =max(PSF_absMaxIndex - 50,0);
	s32 MTF_end =min(PSF_absMaxIndex + 50,PSF_count-1);
	f32* MTF;
	
	s32 MTF_count = MTF_end-MTF_start+1;
	getTransfromed1DDataMagnitude(PSF+MTF_start,MTF,MTF_count);
	//3_4: normalized MTF
	f32 MTF_maxValue=-10000000.0f;
	f32 MTF_minValue =10000000.0f;
	for(s32 i=0;i<MTF_count;i++)
	{
		if(MTF[i] > MTF_maxValue)
			MTF_maxValue =MTF[i];
		if(MTF[i] < MTF_minValue)
			MTF_minValue = MTF[i];
	}
	f32 MTF_inverseScale = 1.0f;
	if(MTF_maxValue>MTF_minValue)
		MTF_inverseScale = 1.0f / (MTF_maxValue-MTF_minValue);
	for(s32 i=0;i<MTF_count;i++)
	{
		MTF[i] *= MTF_inverseScale;
	}
	
#ifdef _DEBUG
	scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* imgMTF = new scene::CImageOpenGL(MTF,MTF_count,1,true,true);
	batchesPSF->addImage(imgMTF,core::rect<s32>(0,0,Width,Height));
#endif
	//release data
	if(PSF)
		delete []PSF;
	if(MTF)
		delete []MTF;
	if(fitResult)
		delete []fitResult;
	if(ERF_array_count)
		delete[] ERF_array_count;
	if(ERF_array)
		delete[] ERF_array;
	if(MiddleSlice)
		delete MiddleSlice;
	

return adjustedBox;
}
}

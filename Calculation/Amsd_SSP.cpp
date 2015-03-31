//2015/3/25

#include "Amsd_SSP.h"
#include "../Src/Scene/ImageBatches.h"
#include "PolyfitHelper.h"
#include "FFTHelper.h"
#include <iostream>
#include <sstream>

namespace Calculation
{
using namespace kk;


void Amsd_SSP_Calculation(BoxData* data,scene::ISceneManager* scene,kk::io::IWriteFile* Output)
{
	// remove head and tail
	// /\y
	// |
	// |   head				tail
	// |	**==============**
	//------------------------>x
	s32 removedLength = 5;
	data->Box.MinEdge.X+=removedLength;
	data->Box.MaxEdge.X-=removedLength;
	data->LocalSize.X-=removedLength*2;

	f32* coronalImage=0;
	s32 width=0;//10
	s32 height=0;//>100
	data->getCoronalImage(coronalImage,width,height);
	
	//calculate the mass center along y
	core::array<f32> middlePoints;
	for(s32 i=0;i<height;i++)
	{
		//get weight center along y
		f32 sumY=0.0f;
		f32 sumN=0;
		for(s32 j=0;j<width;j++)
		{
			sumY += coronalImage[i*width+j]*j;
			sumN += coronalImage[i*width+j];
		}
		//calculate the center
		sumY /= sumN;
		middlePoints.push_back(sumY);
	}
	//find maximum CT Value in RIO
	f32 maxCTValue = data->getMaxValue();
	if(maxCTValue ==0.0f)
		maxCTValue = 1.0f;
	//scale pixel to [ correct for beam hardening and scatter effects]
	for(s32 i=0;i<height;i++)
	{
		for(s32 j=0;j<width;j++)
		{
			coronalImage[i*width+j] /=maxCTValue;
		}
	}

	//the paper say nothing about merge the rows
	//i merge all rows in the beginning
	f32* rodPixels = 0;
	s32* rodCounts = 0;
	f32 rodStep = 0.1f;
	s32 rodPixelsCount = (s32)(width/rodStep);
	rodPixels = new f32[rodPixelsCount];
	memset(rodPixels,0,sizeof(f32)*rodPixelsCount);
	rodCounts = new s32[rodPixelsCount];
	memset(rodCounts,0,sizeof(s32)*rodPixelsCount);
	//fit the middlePoints to a line
	core::array<f32> middleFittedValues;
	getPolyfitLineResult(middlePoints,middleFittedValues);
	//calculate the distance and sort it
	for(s32 i=0;i<height;i++)
	{
		//for each row
		for(s32 j=0;j<width;j++)
		{
			//distance
			f32 distance = abs(middleFittedValues[i] - j);
			s32 curPixelIndex = (s32)(distance / rodStep);
			if(curPixelIndex >= rodPixelsCount)
				continue;
			//sum this value
			rodPixels[curPixelIndex] += coronalImage[i*width+j];
			rodCounts[curPixelIndex] ++;
		}
	}
	for(s32 i=0;i<rodPixelsCount;i++)
		if(rodCounts[i])
			rodPixels[i] /= rodCounts[i];

	//now we got a sorted pixels
	//1 edge response function
	//(1) cubic fit
	s32 ERF_validStart=0;
	s32 ERF_validEnd = rodPixelsCount;
	//fit 11 points and replace the 6th 
	s32 ERF_fitNumber =11;
	s32 ERF_fitMiddleIndex = ERF_fitNumber/2;//=5
	
	s32 fitCount = rodPixelsCount;//-(ERF_fitNumber/2)*2;
	f32* fitResult = new f32[fitCount];
	memset(fitResult,0,sizeof(f32)*fitCount);
	s32 fitIndex=0;
	for(s32 i=ERF_validStart+ERF_fitMiddleIndex;i<=ERF_validEnd-ERF_fitMiddleIndex;i++)
	{
		f32 debugValue = rodPixels[i-ERF_fitMiddleIndex+5];
		f32 fitMiddle= getPolyfitCubicMiddleResult(rodPixels+i-ERF_fitMiddleIndex,ERF_fitNumber);
		fitResult[i-ERF_validStart] = fitMiddle;
	}
	//(2) remove head and tail
	//ignore 
	//2 point spread function
	//(1) cubic fit derivative
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
		PSF[i]= (PSF[i] -PSF_minValue)* PSF_inverseScale;
	}
	//3 the modulation transfer function
	s32 MTF_start =0;
	s32 MTF_end =PSF_count-1;
	f32* MTF;
	
	s32 MTF_count = (PSF_count-5)*3/4;//MTF_end-MTF_start+1;
	//test
	//PSF[0]=1.0f;PSF[1]=.8f;PSF[2]=.6f;PSF[3]=.0f;PSF[4]=0.2f;PSF[5]=.7f;PSF[6]=.9f;

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
		//MTF[i] *= MTF_inverseScale;
		//MTF[i] = (MTF[i] - MTF_minValue)*MTF_inverseScale;
		MTF[i] /=MTF[0];
	}

	Output->writeString(core::stringc("\n\nÖ¸±êÁù£º\nSSP:\n"));
	Output->writeArraySingle(MTF,MTF_count,' ');
#if 1 //Debug
	kk::scene::ImageBatches* batches = dynamic_cast<kk::scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	
	//kk::scene::CImageOpenGL* img = new kk::scene::CImageOpenGL(coronalImage,width,height,true);
	//batches->addImage(img,core::rect<s32>(0,0,width,height));
	kk::scene::CImageOpenGL* img = new kk::scene::CImageOpenGL(fitResult,100,1,true);
	batches->addImage(img,core::rect<s32>(0,0,100,100));

	kk::scene::CImageOpenGL* img2 = new kk::scene::CImageOpenGL(rodPixels,rodPixelsCount,1,true);
	batches->addImage(img2,core::rect<s32>(0,100,100,200));
	kk::scene::CImageOpenGL* img3 = new kk::scene::CImageOpenGL(PSF,80,1,true);
	batches->addImage(img3,core::rect<s32>(100,0,100,100));
	
	kk::scene::CImageOpenGL* img4 = new kk::scene::CImageOpenGL(MTF,MTF_count,1,true,true);
	batches->addImage(img4,core::rect<s32>(100,100,100,100));
/*
	char tmp[50]={0};
	sprintf_s(tmp,50,"length=%.3f value=%.3f alpha=%.2f",Length,value,alpha);
	::OutputDebugStringA(tmp);*/
#endif
	if(MTF)
		delete []MTF;
	if(PSF)
		delete[] PSF;
	if(fitResult)
		delete []fitResult;
	if(rodCounts)
		delete[]rodCounts;
	if(rodPixels)
	delete []rodPixels;
	middleFittedValues.clear();
	middlePoints.clear();
	if(coronalImage)
		delete coronalImage;
}
}

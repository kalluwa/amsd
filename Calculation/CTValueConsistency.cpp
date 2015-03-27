//2015/3/13
#include "CTvalueConsistency.h"
#include "../Src/core/core.h"
#include "../Src/scene/IVolumeTexture.h"

//we use slicedata to store slices
namespace Calculation
{
using namespace kk;

CTValueConsistencyResult Amsd_CTValueConsistency(BoxData* data,scene::ISceneManager* scene,const core::array<SliceData*>& slices)
{
	//get volume texture
	scene::IVolumeTexture* volume = dynamic_cast<scene::IVolumeTexture*>(scene->getSpecificNodeById("IVolumeTexture"));
	//circle with radius 10mm less than real object radius[148mm]
	//10mm->?voxel    radius=148/2-10
	s32 circleRadius =(s32)( (148/2-10) / volume->getMilimeterInX());
	core::array<f32> insideTheCircle;
	//slice std and mean value
	core::array<f32> sliceStdValues;
	core::array<f32> sliceMeanValues;
	for(s32 i=0;i<(s32)slices.size();i++)
	{
		insideTheCircle.clear();
		core::vector2di sliceCenter=slices[i]->getLocalCenter();
		for(s32 y=0;y<slices[i]->Height;y++)
		for(s32 x=0;x<slices[i]->Width;x++)
		{
			f32 distanceToCenter = sqrtf((f32)((x-sliceCenter.X)*(x-sliceCenter.X)+(y-sliceCenter.Y)*(y-sliceCenter.Y)));
			//use < instead of <=
			if(distanceToCenter < circleRadius)
			{
			//store this value
				insideTheCircle.push_back(data->OriginalData[y*slices[i]->Width+x]);
			}
		}
		
		//get the mean value
		f32  meanValue = 0.0f;
		s32 pointSize = insideTheCircle.size();
		f32 inverseSize = 1.0f/insideTheCircle.size();
		for(s32 i=0;i<pointSize;i++)
		{
			meanValue += insideTheCircle[i]*inverseSize;
		}
		sliceMeanValues.push_back(meanValue);
		//get the std value
		f32 stdSum=0.0f;
#define Square(x) (x)*(x)
		for(s32 i=0;i<pointSize;i++)
		{
			stdSum += Square(insideTheCircle[i] - sliceMeanValues[sliceMeanValues.size()-1])*inverseSize;
		}
		sliceStdValues.push_back(stdSum);
	}

	insideTheCircle.clear();
	//then sort two array and get median values
	sliceMeanValues.sort();
	sliceStdValues.sort();
	//organize data
	CTValueConsistencyResult result;
	result.Mean_MedianValue = sliceMeanValues[sliceMeanValues.size()/2];
	result.Std_MedianValue = sliceStdValues[sliceStdValues.size()/2];

	//for stdValues
	f32 meanOfMeanArray=0.0f;
	f32 meanOfStdArray=0.0f;
	s32 sizeSlices = sliceStdValues.size();
	f32 inverseSize = 1.0f/sizeSlices;
		//mean
	for(s32 i=0;i<sizeSlices;i++)
	{
		meanOfMeanArray += sliceMeanValues[i]*inverseSize;
		meanOfStdArray += sliceStdValues[i]*inverseSize;
	}
		//std
	f32 stdOfMeanArray=.0f;
	f32 stdOfStdArray=0.0f;
	for(s32 i=0;i<sizeSlices;i++)
	{
		stdOfMeanArray += Square(sliceMeanValues[i]-meanOfMeanArray)*inverseSize;
		stdOfStdArray += Square(sliceStdValues[i]-meanOfStdArray)*inverseSize;
	}

	result.Std_MedianValue = stdOfMeanArray;
	result.Std_StdValue = stdOfStdArray;

	return result;
}
}

//2015/3/13

//this file use the data generated by NPS 
//eg:64 images

//find the center
//and define a circle with radius 10mm less than the real object
//get the voxels,and  then calculate its std value and mean value
//after that sort a array of std value and mean values ,sort these two array and get median one

#ifndef _CT_VALUE_CONSISTENCY_H_
#define _CT_VALUE_CONSISTENCY_H_

#include "CalcHelper.h"
#include "../Src/scene/ISceneManager.h"
#include "../Src/io/IWriteFile.h"

namespace Calculation
{
	struct CTValueConsistencyResult
	{
		f32 Mean_MedianValue;
		f32 Mean_StdValue;

		f32 Std_MedianValue;
		f32 Std_StdValue;
	};
	extern CTValueConsistencyResult Amsd_CTValueConsistency(BoxData* data,kk::scene::ISceneManager*scene,kk::io::IWriteFile* Output,const core::array<SliceData*>& slices);
}
#endif

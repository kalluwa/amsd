//2015/3/3

#ifndef _CALC_H_
#define _CALC_H_
#include "Src\Core\core.h"

namespace Calculation
{
	using namespace kk;

	enum ECalType
	{
		ECT_OBJ_LENGTH,
		ECT_TRIANGLE_PATH_LENGTH
	};

	class BoxData
	{
	public:
		//this is a helper class to pretend that we are using
		//a full data
		BoxData(kk::f32* data,kk::s32 sizeX,kk::s32 sizeY,
			kk::s32 sizeZ,const kk::core::aabbox3di& box);

		//(x,y,z)is local coordinate
		f32 getVoxelLocal(s32 x,s32 y,s32 z);
		//(x,y,z) is absolute coordinate
		f32 getVoxelAbsolute(s32 x,s32 y,s32 z);

		//function
		f32 getMaxValue() const;
		core::vector3di getMaxValueLocalCoordinate() const;
		core::vector3di getMaxValueAbsoluteCoordinate() const;

		f32 getMinValue();
		core::vector3di getMinValueLocalCoordinate() const;
		core::vector3di getMinValueAbsoluteCoordinate() const;
		
		//sum all
		f32 getSumValue() const;
		//sum and multi X,Y,Z
		f32 getSumValumeX() const;
		f32 getSumValumeY() const;
		f32 getSumValumeZ() const;

		//standard error
		f32 getStdValue() const;

		//mean value
		f32 getMeanValue() const;

		//extract slice
		void getSliceX(f32* dataToFill,s32 sliceNo,s32& width,s32& height) const;
		void getSliceY(f32* dataToFill,s32 sliceNo,s32& width,s32& height) const;
		void getSliceZ(f32* dataToFill,s32 sliceNo,s32& width,s32& height) const;

		void getCoronalImage(f32*& dataToFill,s32& width,s32&height) const;
		void getCoronalImageEliminateAir(f32*& dataToFill,s32& width,s32&height,f32 threshold) const;

		BoxData& operator=(const BoxData& other)
		{
			Box = other.Box;
			OriginalData = other.OriginalData;
			SizeX = other.SizeX;
			SizeY = other.SizeY;
			SizeZ = other.SizeZ;

			LocalSize = other.LocalSize;
		};

	public:
		kk::core::aabbox3di Box;
		kk::f32* OriginalData;

	public:
		s32 SizeX,SizeY,SizeZ;
		core::vector3di LocalSize;
	};//end of class
}

#endif//end header

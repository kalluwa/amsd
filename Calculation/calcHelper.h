//2015/3/3

#ifndef _CALC_H_
#define _CALC_H_
#include "..\Src\Core\core.h"

namespace Calculation
{
	using namespace kk;

	enum ECalType
	{
		ECT_OBJ_LENGTH,
		ECT_TRIANGLE_PATH_LENGTH,
		ECT_NEQ_CALCULATION,
		ECT_METAL_UNIFORMITY
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
		//sliceNo is absolute position 
		void getSliceX(f32*& dataToFill,s32 sliceNo,s32& width,s32& height,f32 threshold = -1000.0f) const;
		void getSliceY(f32*& dataToFill,s32 sliceNo,s32& width,s32& height,f32 threshold = -1000.0f) const;
		void getSliceZ(f32*& dataToFill,s32 sliceNo,s32& width,s32& height,f32 threshold = -1000.0f) const;
		f32 getSliceZMaxValue(s32 sliceAbsolutePos);

		s32 getSliceZPassVoxelCount(s32 sliceAbsolutePos,f32 threshold=0.2f) const;
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

		void updateSize()
		{
			LocalSize = Box.MaxEdge - Box.MinEdge + core::vector3di(1,1,1);
		}
	public:
		kk::core::aabbox3di Box;
		kk::f32* OriginalData;

	public:
		s32 SizeX,SizeY,SizeZ;
		core::vector3di LocalSize;
	};//end of class

	//Z-direction struct
	class SliceData
	{
	public:
		SliceData(f32* CTData,BoxData* boxData,s32 absoluteSlicePos);

		SliceData(s32 width,s32 height);

		SliceData():Data(0){}

		~SliceData(){ if(Data) delete []Data; };

		//subtract
		bool subSlice(SliceData* other);
		//weighted center
		f32 getMinValue();
		core::vector2di getLocalCenter();
		core::vector2df getLocalCenterF();
		core::vector3di getAbsoluteCenter();
		//apply radial hanning window
		void applyRadialWindow(f32 mmInXY);
		void resizeImage();

		f32* Data;
		s32 Width, Height;
		core::vector3di BasePos;

	};//end class
}

#endif//end header

//2015/3/3

#include "calcHelper.h"
using namespace kk;

namespace Calculation
{
//helper
#define getValue(x,y,z) OriginalData[SizeX*SizeY*(z)+SizeX*(y)+(x)]


BoxData::BoxData(kk::f32* data,kk::s32 sizeX,kk::s32 sizeY,
	kk::s32 sizeZ,const kk::core::aabbox3di& box)
	:Box(box),OriginalData(data),SizeX(sizeX),SizeY(sizeY),SizeZ(sizeZ)
{
	LocalSize = box.getExtent()+core::vector3di(1,1,1);
};

//(x,y,z)is local coordinate
f32 BoxData::getVoxelLocal(s32 x,s32 y,s32 z)
{
	return getVoxelAbsolute(x+Box.MinEdge.X,
		y+Box.MinEdge.Y,z+Box.MinEdge.Z);
};
//(x,y,z) is absolute coordinate
f32 BoxData::getVoxelAbsolute(s32 x,s32 y,s32 z)
{
	return OriginalData[SizeX*SizeY*z+SizeX*y+x];
};

//function
f32 BoxData::getMaxValue() const
{

	f32 maxValue = OriginalData[SizeX*SizeY*Box.MinEdge.Z+
		SizeX*Box.MinEdge.Y+Box.MinEdge.X];
	f32 tmpValue=0;

	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				tmpValue = getValue(x,y,z);
				if(tmpValue > maxValue)
					maxValue = tmpValue;
			}

	return maxValue;
};
core::vector3di BoxData::getMaxValueLocalCoordinate() const
{
	return getMaxValueAbsoluteCoordinate()-Box.MinEdge;
};
core::vector3di BoxData::getMaxValueAbsoluteCoordinate() const
{
	f32 maxValue = OriginalData[SizeX*SizeY*Box.MinEdge.Z+
		SizeX*Box.MinEdge.Y+Box.MinEdge.X];
	f32 tmpValue=0;
	core::vector3di pos=Box.MinEdge;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				tmpValue = getValue(x,y,z);
				if(tmpValue > maxValue)
				{
					maxValue = tmpValue;
					pos.X = x;
					pos.Y = y;
					pos.Z = z;
				}
			}
	return pos;
};

f32 BoxData::getMinValue()
{
	f32 minValue = OriginalData[SizeX*SizeY*Box.MinEdge.Z+
		SizeX*Box.MinEdge.Y+Box.MinEdge.X];
	f32 tmpValue=0;

	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				tmpValue = getValue(x,y,z);
				if(tmpValue < minValue)
					minValue = tmpValue;
			}
	return minValue;
};
core::vector3di BoxData::getMinValueLocalCoordinate() const
{
	return getMinValueAbsoluteCoordinate() - Box.MinEdge;
};
core::vector3di BoxData::getMinValueAbsoluteCoordinate() const
{
	f32 minValue = OriginalData[SizeX*SizeY*Box.MinEdge.Z+
		SizeX*Box.MinEdge.Y+Box.MinEdge.X];
	f32 tmpValue=0;
	core::vector3di pos=Box.MinEdge;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				tmpValue = getValue(x,y,z);
				if(tmpValue < minValue)
				{
					minValue = tmpValue;
					pos.X = x;
					pos.Y = y;
					pos.Z = z;
				}
			}
	return pos;
};

//sum all
f32 BoxData::getSumValue() const
{
	float sumValue=0.0f;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				sumValue+= getValue(x,y,z);
			}
	return sumValue;
};
//sum and multi X,Y,Z
f32 BoxData::getSumValumeX() const
{
	float sumValue=0.0f;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				sumValue+= (x-Box.MinEdge.X)*getValue(x,y,z);
			}
	return sumValue;
};
f32 BoxData::getSumValumeY() const
{
	float sumValue=0.0f;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				sumValue+= (y-Box.MinEdge.Y)*getValue(x,y,z);
			}
	return sumValue;
};
f32 BoxData::getSumValumeZ() const
{
	float sumValue=0.0f;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				sumValue+= (z-Box.MinEdge.Z)*getValue(x,y,z);
			}
	return sumValue;
};


//standard error
f32 BoxData::getStdValue() const
{
	//TODO:
	return 0;
};

//mean value
f32 BoxData::getMeanValue() const
{
	f32 sum=0.0f;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
	for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
	for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
	{
		sum += getValue(x,y,z);
	}

	sum /=(SizeX*SizeY*SizeZ);

	return sum;
};

//extract slice
void BoxData::getSliceX(f32* dataToFill,s32 slicePos,s32& width,s32& height) const
{
	width = SizeZ;height = SizeY;

	dataToFill = new f32[width*height];
	//fill
	for(s32 y=0;y<height;y++)//y
	{
		for(s32 x=0;x<width;x++)//z
		{
			dataToFill[y*LocalSize.Z+x] = getValue(slicePos,
				y+Box.MinEdge.Y,x+Box.MinEdge.Z);
		}
	}

};
void BoxData::getSliceY(f32* dataToFill,s32 slicePos,s32& width,s32& height) const
{
	width = SizeZ;height = SizeX;

	dataToFill = new f32[width*height];
	//fill
	for(s32 y=0;y<height;y++)//z
	{
		for(s32 x=0;x<width;x++)//x
		{
			dataToFill[y*LocalSize.Z+x] = getValue(
				y+Box.MinEdge.X,slicePos,x+Box.MinEdge.Z);
		}
	}
};
void BoxData::getSliceZ(f32* dataToFill,s32 slicePos,s32& width,s32& height) const
{
	width = SizeX;height = SizeY;

	dataToFill = new f32[width*height];
	//fill
	for(s32 y=0;y<height;y++)//z
	{
		for(s32 x=0;x<width;x++)//x
		{
			dataToFill[y*LocalSize.Z+x] = getValue(
				x+Box.MinEdge.X,y+Box.MinEdge.Y,slicePos);
		}
	}
};

void BoxData::getCoronalImage(f32*& dataToFill,s32& width,s32&height) const
{
	width = LocalSize.Z;height = LocalSize.X;

	dataToFill = new f32[width*height];

	f32 sum=0.0f;
	
	for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)//y
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)//x
	{
		sum = 0.0f;
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
		{
			sum += getValue(x,y,z);
		}
		dataToFill[width*(x-Box.MinEdge.X)+z-Box.MinEdge.Z] =sum; 
	}
};

void BoxData::getCoronalImageEliminateAir(f32*& dataToFill,s32& width,s32&height,f32 threshold) const
{
	width = LocalSize.Z;height = LocalSize.X;

	dataToFill = new f32[width*height];

	f32 sum=0.0f;
	f32 tmpValue;
	for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)//y
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)//x
	{
		sum = 0.0f;
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
		{
			tmpValue = getValue(x,y,z);
			if(tmpValue > threshold)
				sum += tmpValue;
		}
		dataToFill[width*(x-Box.MinEdge.X)+z-Box.MinEdge.Z] =sum; 
	}
};

}//end namespace 
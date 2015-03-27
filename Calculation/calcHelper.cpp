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
	f32 meanValue = getMeanValue();
	f32 stdSum=0.0f;
	s32 boxSize = (1+Box.MaxEdge.Z-Box.MinEdge.Z)+
		(1+Box.MaxEdge.Y-Box.MinEdge.Y)+
		(1+Box.MaxEdge.X-Box.MinEdge.X);
	f32 inverseBoxSize = 1.0f / boxSize;
	for(int z=Box.MinEdge.Z;z<=Box.MaxEdge.Z;z++)
	for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
	for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
	{
		stdSum += (getValue(x,y,z) - meanValue)*(getValue(x,y,z) - meanValue)*inverseBoxSize ;
	}
	//TODO:
	return stdSum;
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
void BoxData::getSliceX(f32*& dataToFill,s32 slicePos,s32& width,s32& height,f32 threshold) const
{
	width = LocalSize.Z;height = LocalSize.Y;

	dataToFill = new f32[width*height];
	f32 tmp=0.0f;
	//fill
	for(s32 y=0;y<height;y++)//y
	{
		for(s32 x=0;x<width;x++)//z
		{
			tmp = getValue(slicePos,
				y+Box.MinEdge.Y,x+Box.MinEdge.Z);
			if(tmp > threshold)
				dataToFill[y*width+x] = tmp;
			else
				dataToFill[y*width+x] = 0.0f;
		}
	}

};
void BoxData::getSliceY(f32*& dataToFill,s32 slicePos,s32& width,s32& height,f32 threshold) const
{
	width = LocalSize.Z;height = LocalSize.X;

	dataToFill = new f32[width*height];
	f32 tmp=0.0f;
	//fill
	for(s32 y=0;y<height;y++)//z
	{
		for(s32 x=0;x<width;x++)//x
		{
			tmp = getValue(
				y+Box.MinEdge.X,slicePos,x+Box.MinEdge.Z);
			if(tmp > threshold)
				dataToFill[y*width+x] = tmp;
			else
				dataToFill[y*width+x] = 0.0f;
		}
	}
};
void BoxData::getSliceZ(f32*& dataToFill,s32 slicePos,s32& width,s32& height,f32 threshold) const
{
	width = LocalSize.X;height =LocalSize.Y;
	
	dataToFill = new f32[width*height];
	f32 tmp=0.0f;
	//fill
	for(s32 y=0;y<height;y++)//z
	{
		for(s32 x=0;x<width;x++)//x
		{
			tmp = getValue(
				x+Box.MinEdge.X,y+Box.MinEdge.Y,slicePos);
			if(tmp > threshold)
				dataToFill[y*width+x] = tmp;
			else 
				dataToFill[y*width+x] = 0.0f;
		}
	}
};
f32 BoxData::getSliceZMaxValue(s32 sliceAbsolutePos)
{
	if(sliceAbsolutePos < Box.MinEdge.Z || sliceAbsolutePos >Box.MaxEdge.Z)
		return -100.0f;

	f32 maxValue=0.0f;
	f32 tmpValue=0.0f;
	for(int z=sliceAbsolutePos;z<=sliceAbsolutePos;z++)
		for(int y=Box.MinEdge.Y;y<=Box.MaxEdge.Y;y++)
			for(int x=Box.MinEdge.X;x<=Box.MaxEdge.X;x++)
			{
				tmpValue = getValue(x,y,z);
				if(tmpValue > maxValue)
				{
					maxValue = tmpValue;
				}
			}

	return maxValue;
};
s32 BoxData::getSliceZPassVoxelCount(s32 sliceAbsolutePos,f32& maxValue,f32& minValue,f32 threshold) const
{
	s32 width = LocalSize.X;s32 height =LocalSize.Y;
	s32 count=0;
	maxValue=-10000.0f;
	minValue = 10000.0f;
	f32 tmpValue =0.0f;
	//fill
	for(s32 y=0;y<height;y++)//z
	{
		for(s32 x=0;x<width;x++)//x
		{
			tmpValue = getValue(x+Box.MinEdge.X,y+Box.MinEdge.Y,sliceAbsolutePos);
			if(tmpValue>threshold)
				count++;
			if(minValue > tmpValue)
				minValue = tmpValue;
			if(maxValue < tmpValue)
				maxValue = tmpValue;
		}
	}

	return count;
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
/*#########################################################################*/
/*  class SliceData */
SliceData::SliceData(s32 width,s32 height)
	:Width(width),Height(height)
{
	Data = new f32[width*height];
	memset(Data,0,sizeof(f32)*width*height);
}

SliceData::~SliceData()
{
	if(Data)
		delete []Data;
}
SliceData::SliceData(f32* CTData,BoxData* boxData,s32 slicePos)
{
	core::aabbox3di box = boxData->Box;

	Width = box.MaxEdge.X-box.MinEdge.X+1;
	Height = box.MaxEdge.Y- box.MinEdge.Y+1;
	Data = 0;//new f32[Width*Height];
	s32 z= slicePos;
	boxData->getSliceZ(Data,slicePos,Width,Height);
	//for(s32 x=0;x<Width;x++)
	//	for(s32 y=0;y<Height;y++)
	//	{
	//		//OriginalData[SizeX*SizeY*(z)+SizeX*(y)+(x)]
	//		Data[y*Height+x]=CTData[z*boxData->SizeX*boxData->SizeY+
	//			(y+box.MinEdge.Y)*boxData->SizeX+
	//			(x+box.MinEdge.X)]; 
	//	}

	BasePos = boxData->Box.MinEdge;
}

bool SliceData::subSlice(SliceData* other)
{
	if(Width != other->Width || Height != other->Height)
		return false;

	for(s32 x=0;x<Width;x++)
	{
		for(s32 y= 0;y<Height;y++)
		{
			Data[x+y*Width] -=other->Data[x+y*Width];
		}
	}
	return true;
}

bool SliceData::plusSlice(SliceData* other)
{
	if(Width != other->Width || Height != other->Height)
		return false;

	for(s32 x=0;x<Width;x++)
	{
		for(s32 y= 0;y<Height;y++)
		{
			Data[x+y*Width] +=other->Data[x+y*Width];
		}
	}
	return true;
}
f32 SliceData::getMinValue()
{
	f32 minValue = Data[0];
	for(s32 x=0;x<Width;x++)
		for(s32 y=0;y<Height;y++)
		{
			if(minValue > Data[x+y*Width])
				minValue = Data[x+y*Width];
		}

	return minValue;
}
core::vector2di SliceData::getLocalCenter()
{
	f32 minValue = getMinValue();
	f32 xSum=0.0f,ySum=0.0f,nSum=0.0f;
	for(s32 x=0;x<Width;x++)
		for(s32 y=0;y<Height;y++)
		{
			xSum += (Data[x+y*Width]-minValue)*x;
			ySum += (Data[x+y*Width]-minValue)*y;
			nSum += (Data[x+y*Width]-minValue);
		}

	xSum /= nSum;
	ySum /= nSum;

	return core::vector2di((s32)xSum,(s32)ySum);
};

core::vector2df SliceData::getLocalCenterF()
{
	
	f32 minValue = getMinValue();
	f32 xSum=0.0f,ySum=0.0f,nSum=0.0f;
	for(s32 x=0;x<Width;x++)
		for(s32 y=0;y<Height;y++)
		{
			xSum += (Data[x+y*Width]-minValue)*x;
			ySum += (Data[x+y*Width]-minValue)*y;
			nSum += (Data[x+y*Width]-minValue);
		}

	xSum /= nSum;
	ySum /= nSum;

	return core::vector2df((f32)xSum,(f32)ySum);
}
void SliceData::resizeImage()
{
	core::vector2di center = getLocalCenter();
#define min(x,y) ((x)>(y)?(y):(x))
	f32 minRadius_1 =(f32) min(center.X,Width-1-center.X);
	f32 minRadius_2 =(f32) min(center.Y,Height-1-center.Y);
	f32 minRadius =(f32) min(minRadius_1,minRadius_2);

	core::vector2di offset = core::vector2di((s32)(center.X-minRadius),
		(s32)(center.Y-minRadius));
	s32 newWidth = (s32)(2*minRadius+1);
	s32 newHeight = (s32)(2*minRadius+1);

	f32* newData = new f32[newWidth*newHeight];
	for(s32 y=0;y<newHeight;y++)
	{
		for(s32 x=0;x<newWidth;x++)
		{
			newData[y*newWidth +x] = Data[(y+offset.Y)*Width+(x+offset.X)];
		}
	}
	Width = newWidth;
	Height = newHeight;
	BasePos.X += offset.X;
	BasePos.Y += offset.Y;
	if(Data)
		delete []Data;

	Data = newData;
	newData = 0;
}
core::vector3di SliceData::getAbsoluteCenter()
{
	core::vector2di centerPos2D = getLocalCenter();
	core::vector3di centerPos = core::vector3di(centerPos2D.X+BasePos.X,centerPos2D.Y+BasePos.Z,BasePos.Z);
	return centerPos;
}

f32 SliceData::getPixelValue(s32 x,s32 y)
{
	return Data[x+y*Width];
};

void SliceData::setPixelValue(s32 x,s32 y,f32 value)
{
	Data[x+y*Width]=value;
}
void SliceData::applyRadialWindow(f32 mmInXY)
{
	const f32 R = 148/2/mmInXY; //R in Voxel Count
	const f32 PI = 3.1415926f;
	//get center
	core::vector2di center = getLocalCenter();
	for(s32 x=0;x<Width;x++)
	{
		for(s32 y=0;y<Height;y++)
		{
			//distance
			f32 distance = sqrtf((f32)((x-center.X)*(x-center.X)+
				(y-center.Y)*(y-center.Y)));

			if(distance < R)
				Data[x+y*Width]*=cosf(distance * PI / (2*R));
			else
				Data[x+y*Width] = 0.0f;
		}
	}
};

RawData::RawData(f32*raw,s32 length)
	:Raw(raw),Length(length)
{

};

f32 RawData::getMinValue() const
{
	f32 minValue = Raw[0];
	for(s32 i=0;i<Length;i++)
	{
		if(minValue > Raw[i])
			minValue = Raw[i];
	}

	return minValue;
};
f32 RawData::getMaxValue() const
{
	f32 maxValue = Raw[0];
	for(s32 i=0;i<Length;i++)
	{
		if(maxValue < Raw[i])
			maxValue = Raw[i];
	}

	return maxValue;
};
f32 RawData::getStdValue() const
{
	f32 meanValue = getMeanValue();
	f32 sum=0.0f;
	for(s32 i=0;i<Length;i++)
	{
		sum +=(Raw[i]-meanValue)*(Raw[i]-meanValue);
	}

	return (sum / Length);
};
f32 RawData::getMeanValue()const
{
	f32 meanValue = 0.0f;
	for(s32 i=0;i<Length;i++)
	{
		meanValue+= Raw[i];
	}
	meanValue /= Length;
	return meanValue;
};

//raw data array###################################
RawDataArray::RawDataArray(core::array<f32>* rawArray)
	:Raw(rawArray)
{

};

f32 RawDataArray::getMinValue() const
{
	f32 minValue = (*Raw)[0];
	for(s32 i=0;i<(s32)Raw->size();i++)
	{
		if(minValue > (*Raw)[i])
			minValue = (*Raw)[i];
	}

	return minValue;
};
f32 RawDataArray::getMaxValue() const
{
	f32 maxValue = (*Raw)[0];
	for(s32 i=0;i<(s32)Raw->size();i++)
	{
		if(maxValue < (*Raw)[i])
			maxValue = (*Raw)[i];
	}

	return maxValue;
};
f32 RawDataArray::getStdValue() const
{
	f32 meanValue = getMeanValue();
	f32 sum=0.0f;
	for(s32 i=0;i<(s32)Raw->size();i++)
	{
		sum +=((*Raw)[i]-meanValue)*((*Raw)[i]-meanValue);
	}

	return (sum / (s32)Raw->size());
};
f32 RawDataArray::getMeanValue()const
{
	f32 meanValue = 0.0f;
	for(s32 i=0;i<(s32)Raw->size();i++)
	{
		meanValue+= (*Raw)[i];
	}
	meanValue /= (s32)Raw->size();
	return meanValue;
};

}//end namespace 
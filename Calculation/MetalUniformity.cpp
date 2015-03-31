//2015/3/9

#include "NeqCalculation.h"

//for debug purpose
#include "../Src/scene/IVolumeTexture.h"
#include <Windows.h>
#include "../Src/scene/CImageOpenGL.h"
#include "../Src/scene/ImageBatches.h"
//queue
#include <queue>

//memory leaks
namespace Calculation
{
//debug
	s32 debugSlicePos = -1;
/*
in this function 
we need to figure out 3 parameters:Sout & MTF & NPS
they are 3 main part in the following code
*/
core::aabbox3di Amsd_MetalUniformity(BoxData* data,scene::ISceneManager* scene,kk::io::IWriteFile* Output)
{
//pre process
//remove extra part shold be done in volumeTexture class
	//get max voxel count
	s32 maxValue=0;
	f32 CTValue_max=0.0f;
	f32 CTValue_min=0.0f;
	core::array<f32> CTSliceMaxCTValues;
	core::array<s32> passedVoxelCount;
	for(s32 slicePos = data->Box.MinEdge.Z;slicePos <= data->Box.MaxEdge.Z;slicePos++)
	{
		s32 count = data->getSliceZPassVoxelCount(slicePos,CTValue_max,CTValue_min,0.2f);
		if(count > maxValue)
			maxValue = count;
		CTSliceMaxCTValues.push_back(CTValue_max);
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
		if(passedVoxelCount[i] > testValue)// || sliceMaxValue>6.0f)//test:contain
		{
			tmpCount++;
			if(tmpCount>maxValidCount)
			{
				realStart = i-tmpCount;
				realEnd = i;
				maxValidCount = tmpCount;
			}
			//test
			//if(sliceMaxValue)
			//{
			//	//realStart = i-tmpCount;
			//	realEnd = i;
			//	maxValidCount = realEnd-realStart+1;
			//}
		}
		else
		{
			tmpCount =0;
		}
	}
	
	//adjust boundingbox
	core::aabbox3di adjustedBox = data->Box;
	adjustedBox.MinEdge.Z = data->Box.MinEdge.Z+realStart;
	adjustedBox.MaxEdge.Z = data->Box.MinEdge.Z+realEnd;

	//we use threshold=0.35 to remove volxel which isnot belong to metal
	f32 thresholdToEliminateActal = 0.35;
	//then we need to find 4 peaks
	core::array<s32> Peaks;
	core::array<f32> BaseLinePoss;
	//debug
	f32 debugValuePeaks;
	for(s32 i = realStart;i<realEnd;i++)
	{
		//-1  0   1
		if(CTSliceMaxCTValues[i]>thresholdToEliminateActal&&
			CTSliceMaxCTValues[i+1]<thresholdToEliminateActal)
		{
			BaseLinePoss.push_back(i);
		}
		else//1  0  -1
		if(CTSliceMaxCTValues[i]<thresholdToEliminateActal&&
			CTSliceMaxCTValues[i+1]>thresholdToEliminateActal)
		{
			BaseLinePoss.push_back(i);
		}
	}
	for(s32 i=0;i<(s32)BaseLinePoss.size();i+=2)
	{
		//find max value in this range
		f32 maxValue = CTSliceMaxCTValues[BaseLinePoss[i]];
		s32 rightIndex = BaseLinePoss[i];
		s32 start = BaseLinePoss[i];
		s32 end = BaseLinePoss[i+1];
		for(s32 x=start;x<end;x++)
			if(CTSliceMaxCTValues[x] > maxValue)
			{
				rightIndex = x;
				maxValue = CTSliceMaxCTValues[x];
			}
		Peaks.push_back(rightIndex);
	}
	
	//now we got peaks

	if((s32)Peaks.size()!=4)
	{
		::MessageBoxA(NULL,"Not Found 4 Metal!","error",0);
		throw "Not Found 4 Metal!";
	}//	

	s32 AluminumSlicePos = Peaks[0] + data->Box.MinEdge.Z;
	//add control region
	f32 minValue = 100.0f;
	s32 minActalIndex = -1;
	for(s32 i=BaseLinePoss[BaseLinePoss.size()-1];i<realEnd;i++)
	{
		if(minValue > CTSliceMaxCTValues[i])
		{
			minValue = CTSliceMaxCTValues[i];
			minActalIndex = i;
		}
	}
	Peaks.push_back(minActalIndex);//this isnot the real peak

	kk::scene::IVolumeTexture* volume = dynamic_cast<kk::scene::IVolumeTexture*>(scene->getSpecificNodeById("IVolumeTexture"));
	core::array<f32> metalMeansAndStds;
	//we will get peak slice
	for(s32 i=0;i<(s32)Peaks.size();i++)
	//for(s32 i=0;i<1;i++)//debug use
	{
		s32 slicePosZ = data->Box.MinEdge.Z+ Peaks[i];
		//get sliceData
		//f32* sliceData = 0;
		//s32 Width=0,Height=0;
		//data->getSliceZ(sliceData,slicePosZ,Width,Height);
		//area[4cmX4cm]
		SliceData* metalSlice = new SliceData(data->OriginalData,data,slicePosZ);
		core::vector2di centerMetalSlice = metalSlice->getLocalCenter();
		s32 voxelNumFor4cm = (s32)(40.0f/volume->getMilimeterInX());
		voxelNumFor4cm = voxelNumFor4cm/2*2+1;//make it odd
		//get 4x4 data
		f32* data4x4=new f32[voxelNumFor4cm*voxelNumFor4cm];
		s32 index=0;
		for(s32 x = centerMetalSlice.X-voxelNumFor4cm/2;x<=centerMetalSlice.X+voxelNumFor4cm/2;x++)
		{
			for(s32 y=centerMetalSlice.Y-voxelNumFor4cm/2;y<=centerMetalSlice.Y+voxelNumFor4cm/2;y++)
			{
				data4x4[index++] = metalSlice->getPixelValue(x,y);
			}
		}
		//get meanvalue and stdvalue
		RawData* raw4x4 = new RawData(data4x4,voxelNumFor4cm*voxelNumFor4cm);
		f32 metalMean = raw4x4->getMeanValue();
		f32 metalStd = raw4x4->getStdValue();
		metalMeansAndStds.push_back(metalMean);
		metalMeansAndStds.push_back(metalStd);

		if(data4x4)
			delete []data4x4;
		if(metalSlice)
			delete metalSlice;
	}
	Output->writeString(core::stringc("\n\n指标五:\n金属:\n：从左至右依次是 \
									  金属Al Cu Sn Pb和非金属\n"));

	//normalize all metal mean and std
	for(s32 i=0;i<(s32)metalMeansAndStds.size();i+=2)
	{
		metalMeansAndStds[i] /= metalMeansAndStds[metalMeansAndStds.size()-2];
		metalMeansAndStds[i+1] /= metalMeansAndStds[metalMeansAndStds.size()-1];
		
		Output->writeString(core::stringc("均值比="));
		Output->writeSingle(metalMeansAndStds[i]);
		Output->writeEmptyLine();
		Output->writeString(core::stringc("方差比="));
		Output->writeSingle(metalMeansAndStds[i+1]);
		Output->writeEmptyLine();

		f32 debugMean=metalMeansAndStds[i];
		f32 debugStd = metalMeansAndStds[i+1];
		s32 ki=0;
	}
	//test Alumminum
	SliceData* aluminumSlice = new SliceData(data->OriginalData,data,AluminumSlicePos);
	f32 thresholdForEliminateActealToGetAl = 0.35f;
	core::array<f32> aluminumVoxels;
	for(s32 i=0;i<aluminumSlice->Width;i++)
	{
		for(s32 j=0;j<aluminumSlice->Height;j++)
		{
			if(aluminumSlice->getPixelValue(i,j)>thresholdForEliminateActealToGetAl)
				aluminumVoxels.push_back(aluminumSlice->getPixelValue(i,j));
		}
	}
	RawDataArray* rawArray_Aluminum = new RawDataArray(&aluminumVoxels);
	f32 meanValue_Aluminum = rawArray_Aluminum->getMeanValue();
	f32 stdValue_Aluminum = rawArray_Aluminum->getStdValue();
	
	Output->writeEmptyLine();	
	Output->writeString(core::stringc("Al的均值="));
	Output->writeSingle(meanValue_Aluminum);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("Al的标准差"));
	Output->writeSingle(stdValue_Aluminum);
	Output->writeEmptyLine();

	delete rawArray_Aluminum;
	aluminumVoxels.clear();
	if(aluminumSlice)
		delete aluminumSlice;
//########################################
	//control region
	s32 controlSlicePosZ = data->Box.MinEdge.Z+ minActalIndex;
	//get control slice data
	f32* controlSlice = 0 ;
	SliceData* controlSliceData = new SliceData(data->OriginalData,data,controlSlicePosZ);

	//find its center and radius
	s32 control_LineCount=0,control_LineEnd=0;
	s32 control_MaxLineCount=0,control_MaxLineEnd=0;
	f32 thresholdForEliminateAir=0.2f;
	s32 centerXPos=0,centerYPos=0;
	for(s32 i=0;i<controlSliceData->Width;i++)
	{
		control_LineCount=0;
		for(s32 j=0;j<controlSliceData->Height;j++)
		{
			if(controlSliceData->getPixelValue(i,j)>thresholdForEliminateAir)
			{	
				control_LineEnd=j;
				control_LineCount++;
				if(control_LineCount > control_MaxLineCount)
				{
					control_MaxLineCount = control_LineCount;
					control_MaxLineEnd = control_LineEnd;
					centerYPos = j;
				}
			}
			else
			{
				control_LineCount=0;
			}
		}
	}
	s32 control_MaxLineCount2=0;
	for(s32 j=0;j<controlSliceData->Height;j++)
	{
		control_LineCount=0;
		for(s32 i=0;i<controlSliceData->Width;i++)
		{
			if(controlSliceData->getPixelValue(i,j)>thresholdForEliminateAir)
			{	
				control_LineEnd=j;
				control_LineCount++;
				if(control_LineCount > control_MaxLineCount2)
				{
					control_MaxLineCount2 = control_LineCount;
					control_MaxLineEnd = control_LineEnd;
					centerXPos = i;
				}
			}
			else
			{
				control_LineCount=0;
			}
		}
	}
	s32 actealTestRadius = min(control_MaxLineCount2,control_MaxLineCount);
	actealTestRadius = actealTestRadius/2-10;
	actealTestRadius= actealTestRadius>0?actealTestRadius:10;//should remove this
	core::vector2di centerControlArea = controlSliceData->getLocalCenter();//core::vector2di(centerXPos,centerYPos);
	centerXPos = centerControlArea.X;
	centerYPos = centerControlArea.Y;
	//check line1 and line2
	core::array<f32> line1_control;
	core::array<f32> line2_control;
	line1_control.push_back(controlSliceData->getPixelValue(centerXPos,centerYPos));
	line2_control.push_back(controlSliceData->getPixelValue(centerXPos,centerYPos));
	for(s32 i=1;i<actealTestRadius;i++)
	{
		//     line1   line2
		//     /		\
		//    /			 \
		//   /			  \     end
		line1_control.push_back(controlSliceData->getPixelValue(centerXPos+i,centerYPos+i));
		line1_control.push_back(controlSliceData->getPixelValue(centerXPos-i,centerYPos-i));

		line2_control.push_back(controlSliceData->getPixelValue(centerXPos+i,centerYPos-i));
		line2_control.push_back(controlSliceData->getPixelValue(centerXPos-i,centerYPos+i));
	}
	RawDataArray* rawArray_Line1_control = new RawDataArray(&line1_control);
	f32 meanValue_Line1_control = rawArray_Line1_control->getMeanValue();
	f32 stdValue_Line1_control = rawArray_Line1_control->getStdValue();
	f32 peakVariation_Line1_control = rawArray_Line1_control->getMaxValue()-rawArray_Line1_control->getMinValue();
	delete rawArray_Line1_control;
	RawDataArray* rawArray_Line2_control = new RawDataArray(&line2_control);
	f32 meanValue_Line2_control = rawArray_Line2_control->getMeanValue();
	f32 stdValue_Line2_control = rawArray_Line2_control->getStdValue();
	f32 peakVariation_Line2_control = rawArray_Line2_control->getMaxValue()-rawArray_Line2_control->getMinValue();
	delete rawArray_Line2_control;

	Output->writeString(core::stringc("\n\n指标六：\n不带金属的控制区域:\n"));
	Output->writeString(core::stringc("直线1上的体素的均值:\n"));
	Output->writeSingle(meanValue_Line1_control);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线1上的体素的标准差:\n"));
	Output->writeSingle(stdValue_Line1_control);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线1上的体素的峰值变化:\n"));
	Output->writeSingle(peakVariation_Line1_control);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线2上的体素的均值:\n"));
	Output->writeSingle(meanValue_Line2_control);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线2上的体素的标准差:\n"));
	Output->writeSingle(stdValue_Line2_control);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线2上的体素的峰值变化:\n"));
	Output->writeSingle(peakVariation_Line2_control);
	Output->writeEmptyLine();

	line1_control.clear();
	line2_control.clear();
	//roi area
	core::array<f32> roiAreaControl;
	actealTestRadius = (s32)(40 / (f32)volume->getMilimeterInZ());
	for(s32 i=centerXPos-actealTestRadius;i<=centerXPos+actealTestRadius;i++)
	{
		for(s32 j=centerYPos-actealTestRadius;j<=centerYPos+actealTestRadius;j++)
		{
			roiAreaControl.push_back(controlSliceData->getPixelValue(i,j));
		}
	}
	RawDataArray* rawArray_Roi_control = new RawDataArray(&roiAreaControl);
	f32 meanValue_ROI_contorl = rawArray_Roi_control->getMeanValue();
	f32 stdValue_ROI_control = rawArray_Roi_control->getStdValue();
	f32 peakVariation_ROI_control = rawArray_Roi_control->getMaxValue()-rawArray_Roi_control->getMinValue();
	delete rawArray_Line2_control;

	Output->writeString(core::stringc("ROI区域上的体素的均值:\n"));
	Output->writeSingle(meanValue_ROI_contorl);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("ROI区域上的体素的标准差:\n"));
	Output->writeSingle(stdValue_ROI_control);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("ROI区域上的体素的峰值变化:\n"));
	Output->writeSingle(peakVariation_ROI_control);
	Output->writeEmptyLine();

	roiAreaControl.clear();
	if(controlSliceData)
		delete controlSliceData;
//########################################
	//w pin
	s32 wuPinStart=-1,wuPinEnd=-1;
	for(s32 i=realEnd;i<(s32)passedVoxelCount.size();i++)
	{
		f32 sliceMaxValue = data->getSliceZMaxValue(data->Box.MinEdge.Z+i);
		//f32 maxValue,minvalue;
		//s32 count = data->getSliceZPassVoxelCount(data->Box.MinEdge.Z+i,maxValue,minvalue,6.0f);
		if(sliceMaxValue>6.0f)
		{
			if(wuPinStart == -1)
			{
				wuPinStart = i;
			}

			wuPinEnd = i;
		}
		else if(wuPinStart!=-1)
		{
			break;
		}
	}
	s32 testWuPinSlice = data->Box.MinEdge.Z+(wuPinStart + wuPinEnd)/2;
	SliceData* wuSliceData = new SliceData(data->OriginalData,data,testWuPinSlice);
	core::vector2di centerWuPinSlice = wuSliceData->getLocalCenter();
	s32 WuPinWidth = wuSliceData->Width;
	s32 WuPinHeight = wuSliceData->Height;
	//threshold for wu pin
	f32 wuThreshold = 6.0f;
	//mask and array
	bool* wuPinMask = new bool[wuSliceData->Width*wuSliceData->Height];
#define VisitMask(_u,_v) wuPinMask[wuSliceData->Width*(_v)+(_u)] = 1
#define isVisited(_u,_v) wuPinMask[wuSliceData->Width*(_v)+(_u)]

	memset(wuPinMask,0,sizeof(bool)*wuSliceData->Width*wuSliceData->Height);
	std::queue<core::vector2di> wuToBeProcessedPixel;

	core::array<vector2di> WuPositions;
	//find 4 wu pin pos
	for(s32 x=0;x<wuSliceData->Width;x++)
	{
		for(s32 y=0;y<wuSliceData->Height;y++)
		{
			f32 testValue = wuSliceData->getPixelValue(x,y);
			if(testValue > wuThreshold && !isVisited(x,y))
			{
				//set mask
				s32 sumX=0,sumY=0;
				s32 sumN=0;
				wuToBeProcessedPixel.push(core::vector2di(x,y));
				while(!wuToBeProcessedPixel.empty())
				{
					core::vector2di curPos = wuToBeProcessedPixel.front();
					s32 X = curPos.X;
					s32 Y = curPos.Y;
					sumX +=curPos.X;
					sumY +=curPos.Y;
					sumN +=1;
					VisitMask(X,Y);//mark visited
					//wuPinMask[wuSliceData->Width*(Y)+(X)] = true;
					wuToBeProcessedPixel.pop();
					s32 xMinus1 = X-1<0?0:X-1;
					s32 xPlus1 = X+1>=WuPinWidth?WuPinWidth-1:X+1;
					s32 yMinus1 = Y-1<0?0:Y-1;
					s32 yPlus1 = Y+1>=WuPinHeight?WuPinHeight-1:Y+1;
					if(!isVisited(xMinus1,Y) && wuSliceData->getPixelValue(xMinus1,Y)> wuThreshold)
						wuToBeProcessedPixel.push(core::vector2di(xMinus1,Y));
					if(!isVisited(xPlus1,Y) && wuSliceData->getPixelValue(xPlus1,Y)> wuThreshold)
						wuToBeProcessedPixel.push(core::vector2di(xPlus1,Y));
					if(!isVisited(X,yMinus1) && wuSliceData->getPixelValue(X,yMinus1)> wuThreshold)
						wuToBeProcessedPixel.push(core::vector2di(X,yMinus1));
					if(!isVisited(X,yPlus1) && wuSliceData->getPixelValue(X,yPlus1)> wuThreshold)
						wuToBeProcessedPixel.push(core::vector2di(X,yPlus1));
				}

				sumX /= sumN;
				sumY /= sumN;

				WuPositions.push_back(core::vector2di(sumX,sumY));
			}
		}
	}
	
	if(wuPinMask)
		delete[]wuPinMask;

	//construct mid-4 points
	core::array<core::vector2di> WuMiddlePoints;
	s32 wuPinSize = WuPositions.size();//should be 4
	//------------>x
	// |     2
	// |   1   3
	// |     4
	// Y
	core::vector2di pos1=WuPositions[0],pos2=WuPositions[0],pos3=WuPositions[0],pos4=WuPositions[0];
	for(s32 i=0;i<wuPinSize;i++)
	{
		if(pos1.X>WuPositions[i].X)
			pos1 =WuPositions[i];
		if(pos2.Y>WuPositions[i].Y)
			pos2 =WuPositions[i];
		if(pos3.X<WuPositions[i].X)
			pos3 =WuPositions[i];
		if(pos4.Y<WuPositions[i].Y)
			pos4 =WuPositions[i];
	}
	WuPositions.clear();
	//------------>x
	// |    1  2
	//      4  3
	// Y
	//get data along the line1 and line2
	WuMiddlePoints.push_back((pos1+pos2)/2);
	WuMiddlePoints.push_back((pos2+pos3)/2);
	WuMiddlePoints.push_back((pos3+pos4)/2);
	WuMiddlePoints.push_back((pos4+pos1)/2);
	//line1:1->3 line2 2->4
	core::array<f32> line1;
	s32 minIndex_middle = 0;
	s32 maxIndex_middle = 2;
	if((WuMiddlePoints[maxIndex_middle].X-WuMiddlePoints[minIndex_middle].X)<0)
	{
		minIndex_middle = 2;
		maxIndex_middle = 0;
	}

	f32 ratio = (WuMiddlePoints[maxIndex_middle].Y-WuMiddlePoints[minIndex_middle].Y)/(f32)(WuMiddlePoints[maxIndex_middle].X-WuMiddlePoints[minIndex_middle].X);
	for(f32 x=WuMiddlePoints[minIndex_middle].X;x<WuMiddlePoints[maxIndex_middle].X;x+=1)
	{
		s32 xPos = (s32)x;
		s32 yPos = (s32)(WuMiddlePoints[minIndex_middle].Y+(x-WuMiddlePoints[minIndex_middle].X)*ratio);
		line1.push_back(wuSliceData->getPixelValue(xPos,yPos));
	}
	//#######################################
	core::array<f32> line2;
	minIndex_middle = 3;
	maxIndex_middle = 1;
	if((WuMiddlePoints[maxIndex_middle].X-WuMiddlePoints[minIndex_middle].X)<0)
	{
		minIndex_middle = 3;
		maxIndex_middle = 1;
	}

	ratio = (WuMiddlePoints[maxIndex_middle].Y-WuMiddlePoints[minIndex_middle].Y)/(f32)(WuMiddlePoints[maxIndex_middle].X-WuMiddlePoints[minIndex_middle].X);
	for(f32 x=WuMiddlePoints[minIndex_middle].X;x<WuMiddlePoints[maxIndex_middle].X;x+=1)
	{
		s32 xPos = (s32)x;
		s32 yPos = (s32)(WuMiddlePoints[minIndex_middle].Y+(x-WuMiddlePoints[minIndex_middle].X)*ratio);
		line2.push_back(wuSliceData->getPixelValue(xPos,yPos));
	}
	RawDataArray* rawArray_Line1 = new RawDataArray(&line1);
	f32 meanValue_Line1 = rawArray_Line1->getMeanValue();
	f32 stdValue_Line1 = rawArray_Line1->getStdValue();
	f32 peakVariation_Line1_wu = rawArray_Line1->getMaxValue()-rawArray_Line1->getMinValue();
	delete rawArray_Line1;
	RawDataArray* rawArray_Line2 = new RawDataArray(&line2);
	f32 meanValue_Line2 = rawArray_Line2->getMeanValue();
	f32 stdValue_Line2 = rawArray_Line2->getStdValue();
	f32 peakVariation_Line2_Wu = rawArray_Line2->getMaxValue()-rawArray_Line2->getMinValue();
	delete rawArray_Line2;

	Output->writeString(core::stringc("\n\n金属钨区域:\n"));
	Output->writeString(core::stringc("直线1上的体素的均值:\n"));
	Output->writeSingle(meanValue_Line1);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线1上的体素的标准差:\n"));
	Output->writeSingle(stdValue_Line1);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线1上的体素的峰值变化:\n"));
	Output->writeSingle(peakVariation_Line1_wu);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线2上的体素的均值:\n"));
	Output->writeSingle(meanValue_Line2);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线2上的体素的标准差:\n"));
	Output->writeSingle(stdValue_Line2);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("直线2上的体素的峰值变化:\n"));
	Output->writeSingle(peakVariation_Line2_Wu);
	Output->writeEmptyLine();
	//ROI[rectangle]
	s32 wuPinMinX = max(WuMiddlePoints[0].X,WuMiddlePoints[3].X);
	s32 wuPinMinY = max(WuMiddlePoints[0].Y,WuMiddlePoints[1].Y);
	s32 wuPinMaxX = min(WuMiddlePoints[1].X,WuMiddlePoints[2].X);
	s32 wuPinMaxY = min(WuMiddlePoints[2].Y,WuMiddlePoints[3].Y);
	core::array<f32> wuRoiData;
	//use 4x4 cm?
	core::vector2di center_wu = wuSliceData->getLocalCenter();
	actealTestRadius = (s32)(40 / (f32)volume->getMilimeterInZ());
	for(s32 x = center_wu.X - actealTestRadius;x<=center_wu.X + actealTestRadius;x++)
	{
		for(s32 y=center_wu.Y - actealTestRadius;y<=center_wu.Y + actealTestRadius;y++)
		{
			wuRoiData.push_back(wuSliceData->getPixelValue(x,y));
		}
	}	
	//for(s32 x = wuPinMinX;x<=wuPinMaxX;x++)
	//{
	//	for(s32 y=wuPinMinY;y<=wuPinMaxY;y++)
	//	{
	//		wuRoiData.push_back(wuSliceData->getPixelValue(x,y));
	//	}
	//}
	RawDataArray* rawArray_ROI_Wu =new RawDataArray(&wuRoiData);
	f32 meanValue_ROI_wu = rawArray_ROI_Wu->getMeanValue();
	f32 stdValue_ROI_wu = rawArray_ROI_Wu->getStdValue();
	f32 peakVariation_ROI_Wu = rawArray_ROI_Wu->getMaxValue()-rawArray_ROI_Wu->getMinValue();
	delete rawArray_ROI_Wu;

	wuRoiData.clear();
	WuMiddlePoints.clear();

	Output->writeString(core::stringc("ROI区域上的体素的均值:\n"));
	Output->writeSingle(meanValue_ROI_wu);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("ROI区域上的体素的标准差:\n"));
	Output->writeSingle(stdValue_ROI_wu);
	Output->writeEmptyLine();
	Output->writeString(core::stringc("ROI区域上的体素的峰值变化:\n"));
	Output->writeSingle(peakVariation_ROI_Wu);
	Output->writeEmptyLine();
#if 1 // for debug
	
	//char tm[50];
	//sprintf_s(tm,50,"maxValidCount=%i[%i %i]",maxValidCount,realStart,realEnd);
	//::OutputDebugStringA(tm);

	f32* sliceImage=0;
	s32 width=0;
	s32 height=0;
	if(debugSlicePos == -1)
	debugSlicePos = (data->Box.MinEdge.Z+data->Box.MaxEdge.Z)/2;

	//debugSlicePos = data->Box.MinEdge.Z+ Peaks[0];
	//debugSlicePos = data->Box.MinEdge.Z+(wuPinStart + wuPinEnd)/2;
	//debugSlicePos = controlSlicePosZ;
	debugSlicePos = AluminumSlicePos;
	data->getSliceZ(sliceImage,debugSlicePos,width,height);

	char debugstr[20];
	sprintf_s(debugstr,20,"%i\t%i\n",debugSlicePos,controlSlicePosZ);
	::OutputDebugStringA(debugstr);
	s32 sizeToDraw = realEnd - realStart;
	f32* drawCTValues = new f32[sizeToDraw];
	for(s32 i=realStart;i<realEnd;i++)
	{
		drawCTValues[i-realStart] = CTSliceMaxCTValues[i];
		f32 DebugValue = drawCTValues[i-realStart];
	}
	//image batches
	scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* img = new scene::CImageOpenGL(sliceImage,width,height,false);
	batches->addImage(img,core::rect<s32>(0,0,width,height));
	/*scene::ImageBatches* batches = dynamic_cast<scene::ImageBatches*>(scene->getSpecificNodeById("ImageBatches"));
	scene::CImageOpenGL* img = new scene::CImageOpenGL(drawCTValues,realEnd-realStart,1,false);
	batches->addImage(img,core::rect<s32>(0,0,width,height));*/

	if(sliceImage)
		delete []sliceImage;
	if(drawCTValues)
		delete [] drawCTValues;
#endif
	passedVoxelCount.clear();
	BaseLinePoss.clear();
	CTSliceMaxCTValues.clear();
	Peaks.clear();
	if(wuSliceData)
		delete wuSliceData;

return adjustedBox;
}
}

//2015/3/4

#include "CImageOpenGL.h"
//for opengl 

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

namespace kk
{
namespace scene
{

CImageOpenGL::CImageOpenGL()
	:Width(0),Height(0),Data(0)
{
}
CImageOpenGL::CImageOpenGL(f32*data,s32 width,s32 height,bool addEdge,bool logData)
	:Width(width),Height(height),Data(0)
{
	fillData(data,width,height,addEdge,logData);
}

CImageOpenGL::~CImageOpenGL()
{
	if(Data)
	{
		delete []Data;
		Data = 0;
	}
}

void CImageOpenGL::fillData(f32*data,s32 width,s32 height,bool addEdge,bool logData)
{
	

	if(Data)
	{
		delete []Data;
	}

	//for 1D image
	if(height==1)
	{
		height=width*2;
		Data = new f32[width*height];
		memset(Data,0,width*height*4);
		//find max value
		f32 maxValue = data[0];
		f32 minValue = data[0];
		for(s32 i=0;i<width;i++)
		{
			if(maxValue < data[i])
				maxValue = data[i];
			if(minValue> data[i])
				minValue =data[i];
		}
		if(minValue == maxValue)
			maxValue+=1.0f;
		//fill image

		for(s32 i=0;i<width;i++)
		{
			s32 startDrawPos = (s32)(height-height*((data[i]-minValue)/(maxValue-minValue)));
			for(s32 j=startDrawPos;j<height;j++)
			{
				Data[i+j*width] = 1.0f;
			}
		}
	}
	else //2D image
	{
		Data = new f32[width*height];
		//update Size
		Width=width;
		Height = height;
		//scale image data to get a better visual
		if(logData)
		{
			for(s32 i=0;i<Height;i++)
			{
				for(s32 j=0;j<Width;j++)
				{
					Data[i*Width+j] = logf(data[i*Width+j]);
				}
			}
		}
		else
			memcpy(Data,data,width*height*sizeof(f32));
	}
	f32 maxValue = 0.0f,minValue =1.0f;
	for(s32 i=0;i<width*height;i++)
	{
		if(minValue>Data[i])
			minValue = Data[i];
		else if(maxValue < Data[i])
			maxValue = Data[i];
	}
	if(maxValue != minValue)
	{
		f32 ratio = 1.0f/(maxValue - minValue);
		for(s32 i=0;i<width*height;i++)
		{
			Data[i] = (Data[i]-minValue)*ratio;
		}
	}
	else
	{
		//return a white image
		for(s32 i=0;i<width*height;i++)
		{
			Data[i] =0.1f;
		}
	}
	//add outline
	if(addEdge)
	{
		for(s32 i=0;i<width;i++)
		{ Data[i]=1.0f; Data[(height-1)*Width+i] = 1.0f;}
		for(s32 i=0;i<height;i++)
		{ Data[i*Width]=1.0f; Data[i*Width+Width-1] = 1.0f;}
	}
	//generate a image[opengl]
	glGenTextures(1, &TextureID);					// 创建纹理
	// 使用来自位图数据生成 的典型纹理
	glBindTexture(GL_TEXTURE_2D, TextureID);
	// 生成纹理
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width, height, 0, GL_LUMINANCE, GL_FLOAT, Data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// 线形滤波
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// 线形滤波
}

void CImageOpenGL::applyTexture()
{
	if(TextureID!=-1)
		glBindTexture(GL_TEXTURE_2D, TextureID);
}
void CImageOpenGL::render()
{
	applyTexture();
}
core::vector2di CImageOpenGL::getImageSize()
{
	return core::vector2di(Width,Height);
}

s32 CImageOpenGL::getImageWidth()
{
	return Width;
}

s32 CImageOpenGL::getImageHeight()
{
	return Height;
}

}
}

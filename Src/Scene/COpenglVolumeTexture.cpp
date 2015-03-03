//created by kalluwa
//2014/2/7

#include "COpenglVolumeTexture.h"
#include <queue>

namespace kk
{
namespace scene
{

f32 threshold=0.2f;

COpenglVolumeTexture::COpenglVolumeTexture(video::IVideoDriver* driver)
	:IVolumeTexture(driver),SizeX(0),SizeY(0),SizeZ(0),Data(0),DataVisited(0)
{

}

COpenglVolumeTexture::COpenglVolumeTexture(video::IVideoDriver* driver,int _sizeX,int _sizeY,int _sizeZ)
	:IVolumeTexture(driver),SizeX(_sizeX),SizeY(_sizeY),SizeZ(_sizeZ),Data(0),DataVisited(0)
{

}

COpenglVolumeTexture::COpenglVolumeTexture(video::IVideoDriver* driver,const char* fileName,int sizeX,int sizeY,int sizeZ,int startSlice)
	:IVolumeTexture(driver),SizeX(sizeX),SizeY(sizeY),SizeZ(sizeZ),Data(0),DataVisited(0)
{
	if(strlen(fileName)>0)
		this->loadVolumeFromFile(fileName,sizeX,sizeY,sizeZ,startSlice);
}
//deconstructor
COpenglVolumeTexture::~COpenglVolumeTexture()
{
	if(Data)
		delete []Data;

	if(DataVisited)
		delete []DataVisited;
}

void COpenglVolumeTexture::loadVolumeFromFile(const char* fileName,int sizeX,int sizeY,int sizeZ,int startSlice)
{
	glEnable(GL_CULL_FACE);
	glClearColor(0.0, 0.0, 0.0, 0);

	int sizeVoxel = sizeX*sizeY*sizeZ;
	int size = sizeX*sizeY*sizeZ*4;
	Data = new f32[sizeVoxel];

	//u8* DataShow = new u8[ sizeX*sizeY*sizeZ];
	u8* DataShow = new u8[ size];
	FILE* fp = 0;//fopen(fileName,"r");
	fopen_s(&fp,fileName,"r");
	if(!fp)
	{
#ifdef WIN32
		char tmp[256];
		sprintf_s(tmp,"file[%s] not found",fileName);
		OutputDebugStringA(tmp);
#endif
		return;
	}
	//offset the data
	long offset = SizeX*SizeY*startSlice*4;//float ->4 bytes
	fseek(fp,offset,SEEK_SET);
	//read volume data
	fread(Data,4,sizeVoxel,fp);
	memcpy(DataShow,Data,sizeVoxel*4);
	//close file
	fclose(fp);

	//find max value
	float maxValue=0;
	float minValue=0;
	float* data_f = (float*)DataShow;
	//for(int i=0;i<sizeVoxel;i++)
	//{
	//	if(data_f[i]>maxValue)
	//		maxValue = data_f[i];
	//	else if(data_f[i]<minValue)
	//		minValue = data_f[i];
	//}

	//float inverserRangeMulti255 = 255*1/(maxValue - minValue);
	//calculate rgba
	for(int i=0;i<sizeVoxel;i++)
	{
		//if(data_f[i]<0.1f)
		//	DataShow[i] =0;
		//else 
		//	DataShow[i]=255;
		//DataShow[i]=data_f[i]*255;
		u8 byte = 255;//(data_f[i]-minValue)*inverserRangeMulti255;
		if(data_f[i]<0.1f)//fThreshold)
			byte=0;
	
		//memset(data+
		DataShow[4*i+0] = 255;
		DataShow[4*i+1] = 255;
		DataShow[4*i+2] = 255;
		DataShow[4*i+3] = byte;
		
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glGenTextures(1, &volumeTexture);
	glBindTexture(GL_TEXTURE_3D, volumeTexture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, sizeX, sizeY, sizeZ, 0, GL_RGBA, GL_UNSIGNED_BYTE,DataShow);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glTexImage3D(GL_TEXTURE_3D, 0,GL_RGBA, sizeX, sizeY, sizeZ, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,DataShow);

	//set render state
	Driver->loadVertexShader("raycasting_shader.cg","vertex_main",vs_main);
	Driver->loadPixelShader("raycasting_shader.cg","fragment_main",ps_main);

	// Create the to FBO's one for the backside of the volumecube and one for the finalimage rendering
	glGenFramebuffersEXT(1, &framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT,framebuffer);

	int WINDOW_SIZE_X = Driver->getViewport().X;
	int WINDOW_SIZE_Y = Driver->getViewport().Y;
	glGenTextures(1, &backface_buffer);
	glBindTexture(GL_TEXTURE_2D, backface_buffer);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0);

	glGenTextures(1, &final_image);
	glBindTexture(GL_TEXTURE_2D, final_image);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);

	glGenRenderbuffersEXT(1, &renderbuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_SIZE_X, WINDOW_SIZE_Y);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	delete []DataShow;
};

void COpenglVolumeTexture::setRenderState()
{
	glDisable(GL_LIGHTING);
}
void resize(int w, int h)
{
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)w/(GLfloat)h, 0.1f, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	
}

void COpenglVolumeTexture::render()
{
	resize(Driver->getViewport().X,Driver->getViewport().Y);
	glTranslatef(-0.5f,-0.5f,-0.5f);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, framebuffer);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);

	render_backface();
	raycasting_pass();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	render_buffer_to_screen();
}

void COpenglVolumeTexture::reshape_ortho(int w, int h)
{
	if (h == 0) h = 1;
	//glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	
}

void COpenglVolumeTexture::vertex(float x, float y, float z)
{
	glColor3f(x,y,z);
	glMultiTexCoord3fARB(GL_TEXTURE1_ARB, x, y, z);
	glVertex3f(x,y,z);
}
void COpenglVolumeTexture::drawQuads(float x, float y, float z)
{
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1.0);
	vertex(0.0, 0.0, 0.0);
	vertex(0.0, y, 0.0);
	vertex(x, y, 0.0);
	vertex(x, 0.0, 0.0);

	glNormal3f(0.0, 0.0, 1.0);
	vertex(0.0, 0.0, z);
	vertex(x, 0.0, z);
	vertex(x, y, z);
	vertex(0.0, y, z);

	glNormal3f(0.0, 1.0, 0.0);
	vertex(0.0, y, 0.0);
	vertex(0.0, y, z);
    vertex(x, y, z);
	vertex(x, y, 0.0);

	glNormal3f(0.0, -1.0, 0.0);
	vertex(0.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0);
	vertex(x, 0.0, z);
	vertex(0.0, 0.0, z);

	glNormal3f(-1.0, 0.0, 0.0);
	vertex(0.0, 0.0, 0.0);
	vertex(0.0, 0.0, z);
	vertex(0.0, y, z);
	vertex(0.0, y, 0.0);

	glNormal3f(1.0, 0.0, 0.0);
	vertex(x, 0.0, 0.0);
	vertex(x, y, 0.0);
	vertex(x, y, z);
	vertex(x, 0.0, z);
	glEnd();
	
}

void COpenglVolumeTexture::render_buffer_to_screen()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D,final_image);
	//glBindTexture(GL_TEXTURE_2D,backface_buffer);
	reshape_ortho(Driver->getViewport().X,Driver->getViewport().Y);
	Driver->drawFullScreenQuad();
	glDisable(GL_TEXTURE_2D);

}
void COpenglVolumeTexture::render_backface()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	drawQuads(1.0,1.0, 1.0);
	glDisable(GL_CULL_FACE);
}

void COpenglVolumeTexture::raycasting_pass()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, final_image, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	cgGLEnableProfile(Driver->getVSProfile());
	cgGLEnableProfile(Driver->getPSProfile());
	cgGLBindProgram(vs_main);
	cgGLBindProgram(ps_main);
	cgGLSetParameter1f( cgGetNamedParameter( ps_main, "stepsize") , 0.02f);
	Driver->setShaderTexParam("tex",backface_buffer,ps_main,param1);
	Driver->setShaderTexParam("volume_tex",volumeTexture,ps_main,param2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	drawQuads(1.0,1.0, 1.0);
	glDisable(GL_CULL_FACE);
	cgGLDisableProfile(Driver->getVSProfile());
	cgGLDisableProfile(Driver->getPSProfile());
}

//resize the frame buffers
void COpenglVolumeTexture::OnEvent(SEvent event)
{
	if(event.type = ET_USER)
	{
		if(event.UserData.type == EUT_RESIZE)
		{
			int WINDOW_SIZE_X = Driver->getViewport().X;
			int WINDOW_SIZE_Y = Driver->getViewport().Y;
			glBindTexture(GL_TEXTURE_2D, backface_buffer);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, backface_buffer, 0);

			glBindTexture(GL_TEXTURE_2D, final_image);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA16F_ARB, WINDOW_SIZE_X, WINDOW_SIZE_Y, 0, GL_RGBA, GL_FLOAT, NULL);

			glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, renderbuffer);
			glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, WINDOW_SIZE_X, WINDOW_SIZE_Y);
			glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer);
			glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		}
	}

	if(event.type == ET_MOUSE)
	{
		if(event.Mouse.mouseaction == EMT_MIDDLE_DOWN)
		{

		}
	}
	
}


int count =0;
core::aabbox3df COpenglVolumeTexture::getPointedData(const core::line3df& line)
{
	core::aabbox3df box;

	core::line3df ray = line;
	//transform ray from world pos to local pos
	//SizeX 768 SizeY 656 SizeZ 252
	if(!DataVisited)
	{
		DataVisited = new bool[SizeX*SizeY*SizeZ];
	}
	
	memset(DataVisited,0,sizeof(bool)*SizeX*SizeY*SizeZ);
	//1 offset  [-0.5,0.5][-0.5,0.5][-0.5,0.5]
	ray.start =ray.start + core::vector3df(0.5f,0.5f,0.5f);
	//2 transform cooridnate [0,1][0,1][0,1]
	ray.start.X *=SizeX;ray.start.Y *=SizeY;ray.start.Z *=SizeZ;
	ray.end.X *=SizeX;ray.end.Y *=SizeY;ray.end.Z *=SizeZ;
	ray.end.normalize();
	//[0,768][0,656][0,252]   dir:[-1,1]Ëõ·Å

	f32 step=1.0f;
	f32 maxLength= sqrtf((f32)(SizeX*SizeX+SizeY*SizeY+SizeZ*SizeZ));
	f32 curLength=0;
	//offset the start point from wall
	ray.start += ray.end+ray.end;
	//find the first point along the dir
	while(curLength < maxLength)
	{
		if(ray.start.X>SizeX-1 ||ray.start.X <0
			|| ray.start.Y>SizeY-1 ||ray.start.Y<0
			|| ray.start.Z>SizeZ-1 || ray.start.Z <0)
		{
			//found nothing
			return box;
		}
		//test threshold:0.2f
		if(Data[(s32)ray.start.X+(s32)ray.start.Y*SizeX+(s32)ray.start.Z*SizeX*SizeY]>threshold)
		{
			//found point
			break;
		}
		ray.start += ray.end;
		curLength += step;
	}

	//we found first point and then search its neighbor
	s32 maxX=0,maxY=0,maxZ = 0;
	s32 minX=SizeX-1,minY = SizeY-1,minZ = SizeZ-1;

#if 0//false
	//use Recursion
	getNeighborData((s32)ray.start.X,(s32)ray.start.Y,(s32)ray.start.Z,minX,minY,minZ,maxX,maxY,maxZ);
#else 
	//use queue
	getNeighborData2((s32)ray.start.X,(s32)ray.start.Y,(s32)ray.start.Z,minX,minY,minZ,maxX,maxY,maxZ);
#endif

	box.MinEdge = core::vector3df(minX/(f32)SizeX,minY/(f32)SizeY,minZ/(f32)SizeZ) + core::vector3df(-0.5f,-0.5f,-0.5f);
	box.MaxEdge = core::vector3df(maxX/(f32)SizeX,maxY/(f32)SizeY,maxZ/(f32)SizeZ) + core::vector3df(-0.5f,-0.5f,-0.5f);

	//clear visited flag
	for(s32 z=minZ;z<=maxZ;z++)
	{
		for(s32 y=minY;y<=maxY;y++)
		{
			for(s32 x=minX;x<=maxX;x++)
			{
				DataVisited[z*SizeX*SizeY+y*SizeX+x]= false;
			}
		}
	}
	count=0;
	return box;
}




#define getData(x,y,z) Data[(z)*SizeX*SizeY+(y)*SizeX+(x)]
#define visited(x,y,z) DataVisited[(z)*SizeX*SizeY+(y)*SizeX+(x)]
#define makeVisited(x,y,z) {DataVisited[(z)*SizeX*SizeY+(y)*SizeX+(x)]=true;}
#define VoxelPass(x,y,z) (getData(x,y,z)>threshold && !visited(x,y,z))
void COpenglVolumeTexture::getNeighborData(s32 ix,s32 iy,s32 iz,s32& minX,s32& minY,s32& minZ,s32& maxX,s32& maxY,s32& maxZ)
{
	//test threshold 0.2f
	if(   ix<1 || ix>=SizeX-1 
		||iy<1 || iy>=SizeY-1 
		||iz<1 || iz>=SizeZ-1
		|| visited(ix,iy,iz)// this voxel is marked visited
		|| getData(ix,iy,iz)<threshold //invalid voxel
		|| count++>2000 //test
		)
	{
		return;
	}
	makeVisited(ix,iy,iz);
	//x-axis
	if(!visited(ix-1,iy,iz))
		if(getData(ix-1,iy,iz)>threshold)
		{
			//makeVisited(ix-1,iy,iz)
			if(ix-1<minX)
				minX = ix-1;
			getNeighborData(ix-1,iy,iz,minX,minY,minZ,maxX,maxY,maxZ);
		}
	if(!visited(ix+1,iy,iz))
		if(getData(ix+1,iy,iz)>threshold)
		{
			//makeVisited(ix+1,iy,iz)
			if(ix+1>maxX)
				maxX = ix+1;

			getNeighborData(ix+1,iy,iz,minX,minY,minZ,maxX,maxY,maxZ);
		}
	//y-axis
	if(!visited(ix,iy-1,iz))
		if(getData(ix,iy-1,iz)>threshold)
		{
			//makeVisited(ix,iy-1,iz)
			if(iy-1<minY)
				minY = iy-1;
			
			getNeighborData(ix,iy-1,iz,minX,minY,minZ,maxX,maxY,maxZ);
		}
	if(!visited(ix,iy+1,iz))
		if(getData(ix,iy+1,iz)>threshold)
		{
			//makeVisited(ix,iy+1,iz)
			if(iy+1>maxY)
				maxY = iy+1;
			
			getNeighborData(ix,iy+1,iz,minX,minY,minZ,maxX,maxY,maxZ);
		}
	//z-axis
	if(!visited(ix,iy,iz-1))
		if(getData(ix,iy,iz-1)>threshold)
		{
			//makeVisited(ix,iy,iz-1)
			if(iz-1<minZ)
				minZ = iz-1;
			
			getNeighborData(ix,iy,iz-1,minX,minY,minZ,maxX,maxY,maxZ);
		}
	if(!visited(ix,iy,iz+1))
		if(getData(ix,iy,iz+1)>threshold)
		{
			//makeVisited(ix,iy,iz+1)
			if(iz+1>maxZ)
				maxZ = iz+1;
			
			getNeighborData(ix,iy,iz+1,minX,minY,minZ,maxX,maxY,maxZ);
		}
};

void COpenglVolumeTexture::getNeighborData2(s32 ix,s32 iy,s32 iz,s32& minX,s32& minY,s32& minZ,s32& maxX,s32& maxY,s32& maxZ)
{
	std::queue<core::vector3di> voxelsToBeProcessed;

	voxelsToBeProcessed.push(core::vector3di(ix,iy,iz));
	while(!voxelsToBeProcessed.empty())
	{
		core::vector3di& curVoxel = voxelsToBeProcessed.front();
		voxelsToBeProcessed.pop();

		ix = curVoxel.X;
		iy = curVoxel.Y;
		iz = curVoxel.Z;

		if(   ix<1 || ix>=SizeX-1 
			||iy<1 || iy>=SizeY-1 
			||iz<1 || iz>=SizeZ-1
			|| visited(ix,iy,iz)// this voxel is marked visited
			|| getData(ix,iy,iz)<threshold //invalid voxel
			//|| count++>2000 //test
			)
		{
			continue;//skip this voxel
		}
		
		makeVisited(ix,iy,iz);
		//test this voxel's neighbors
		//x-axis
		if(!visited(ix-1,iy,iz))
			if(getData(ix-1,iy,iz)>threshold)
			{
				//makeVisited(ix-1,iy,iz)
				if(ix-1<minX)
					minX = ix-1;
				voxelsToBeProcessed.push(core::vector3di(ix-1,iy,iz));
			}
		if(!visited(ix+1,iy,iz))
			if(getData(ix+1,iy,iz)>threshold)
			{
				//makeVisited(ix+1,iy,iz)
				if(ix+1>maxX)
					maxX = ix+1;

				voxelsToBeProcessed.push(core::vector3di(ix+1,iy,iz));
			}
		//y-axis
		if(!visited(ix,iy-1,iz))
			if(getData(ix,iy-1,iz)>threshold)
			{
				//makeVisited(ix,iy-1,iz)
				if(iy-1<minY)
					minY = iy-1;
			
				voxelsToBeProcessed.push(core::vector3di(ix,iy-1,iz));
			}
		if(!visited(ix,iy+1,iz))
			if(getData(ix,iy+1,iz)>threshold)
			{
				//makeVisited(ix,iy+1,iz)
				if(iy+1>maxY)
					maxY = iy+1;
			
				voxelsToBeProcessed.push(core::vector3di(ix,iy+1,iz));
			}
		//z-axis
		if(!visited(ix,iy,iz-1))
			if(getData(ix,iy,iz-1)>threshold)
			{
				//makeVisited(ix,iy,iz-1)
				if(iz-1<minZ)
					minZ = iz-1;
			
				voxelsToBeProcessed.push(core::vector3di(ix,iy,iz-1));
			}
		if(!visited(ix,iy,iz+1))
			if(getData(ix,iy,iz+1)>threshold)
			{
				//makeVisited(ix,iy,iz+1)
				if(iz+1>maxZ)
					maxZ = iz+1;
			
				voxelsToBeProcessed.push(core::vector3di(ix,iy,iz+1));
			}
	}

	while(!voxelsToBeProcessed.empty())
		voxelsToBeProcessed.pop();
};
}//end namespace scene
}//end namespace kk

//created by kalluwa
//2015/1/29

//#include "StdAfx.h"
#ifndef UseDx
#include <gl/glew.h>
#endif
#include "CSimpleCameraNode.h"

namespace kk
{
namespace scene
{
	

		CSimpleCameraNode::CSimpleCameraNode(kk::video::IVideoDriver* _driver,const core::vector3df& Pos,const core::vector3df& Target,const vector3df& upDir)
			:ISceneNode(_driver),pos(Pos),target(Target),up(upDir),isFirstUpdate(true),mouseLeftDown(false)
		{
			target = (Target-Pos);
			target.normalize();
			target = Pos + target;

			up.normalize();
		}

		CSimpleCameraNode::~CSimpleCameraNode()
		{
		}
		void CSimpleCameraNode::OnEvent(SEvent event)
		{
			switch(event.type)
			{
			case ET_MOUSE:
				if(event.Mouse.mouseaction == EMT_LEFT_DOWN)
				{
					mouseLeftDown = true;
					isFirstUpdate = true;
				}
				else if(event.Mouse.mouseaction == EMT_LEFT_UP)
				{
					mouseLeftDown = false;
					isFirstUpdate=false;
				}
				if(mouseLeftDown)
					HandleMouseEvent(event);
			break;
			case ET_KEY:
				HandleKeyEvent(event);
			break;
			case ET_USER:
			break;
			}

			//at last we update our view matrix
			Driver->setView(pos,target,up);
		}

		void CSimpleCameraNode::HandleMouseEvent(SEvent e)
		{
		//mouse mouse => rotate camera

			
			//handle startup exception
			if(isFirstUpdate)
			{
				lastX = e.Mouse.x;
				lastY = e.Mouse.y;

				isFirstUpdate = false;
			}
			
			float xdelta = (float)(e.Mouse.x - lastX);
			float ydelta = (float)(e.Mouse.y - lastY);

			core::vector3df dir = target - pos;
			core::vector3df right = dir.crossProduct(up);
			dir.normalize();
			right.normalize();
			dir = dir + 0.01f*xdelta*right-0.01f*ydelta*up;
			dir.normalize();
			target = pos + dir;
			//up = right.crossProduct(dir);
			//up.normalize();
			
			//record last position
			lastX = e.Mouse.x;
			lastY = e.Mouse.y;
		}

		void CSimpleCameraNode::HandleKeyEvent(SEvent e)
		{
		//move camera
			if(e.Key.iskeydown)
			{
				switch(e.Key.keycode)
				{
				case 'W':
				case 'w':
				{
				//move forward
				core::vector3df dir = target - pos;
				pos = pos + 0.1f*dir;
				target = target+0.1f*dir;
				}
				break;

				case 'S':
				case 's':
				{
				//move backward
				core::vector3df dir = pos - target;
				pos = pos + 0.1f*dir;
				target = target + 0.1f*dir;
				}
				break;

				case 'D':
				case 'd':
				{
				//move right
				core::vector3df right = (target-pos).crossProduct(up);
				right.normalize();
				pos = pos + 0.1f*right;
				target=target + 0.1f*right;
				}
				break;

				case 'A':
				case 'a':
				{
				//move right
				core::vector3df right = (target-pos).crossProduct(up);
				right.normalize();
				pos = pos - 0.1f*right;
				target=target - 0.1f*right;
				}
				break;
				}
			}
		}
	
	void CSimpleCameraNode::render()
	{
		//nothing will do in this node
	}

	void CSimpleCameraNode::setUpMatrices()
	{
#ifndef UseDX
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45,Driver->getViewport().X/(double)Driver->getViewport().Y,0.1f,1000.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(pos.X,pos.Y,pos.Z,target.X,target.Y,target.Z,
			up.X,up.Y,up.Z);

#endif
	}
	
}
}


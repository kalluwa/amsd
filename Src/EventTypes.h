//created by kalluwa
//2015/1/29

#ifndef _EVENT_TYPES_H_
#define _EVENT_TYPES_H_

//memset
#include "Core/kkString.h"
#include "../calcHelper.h"

//main event type
enum EventType
{
	ET_KEY,
	ET_MOUSE,
	ET_USER,
};

//key event type
//enum EKeyType
//{
//	EMT_PRESS_DOWN,
//	EMT_PRESS_UP
//};
//mouse event type
enum EMouseType
{
	EMT_LEFT_DOWN,
	EMT_LEFT_UP,
	EMT_RIGHT_DOWN,
	EMT_RIGHT_UP,
	EMT_MIDDLE_DOWN,
	EMT_MIDDLE_UP,
	EMT_WHEEL,
	EMT_MOUSE_MOVE
};

//user event
enum EUserType
{
	EUT_RESIZE,
	//calculation
	EUT_CALCULATION
};
struct SEvent
{
	EventType type;

	union
	{
		//key
		struct{
			int keycode;
		//EKeyType keyaction;
			bool iskeydown;
			bool shift;
			bool control;
		}Key;

		//mouse
		struct
		{
			int x,y;
			float wheel;
			EMouseType mouseaction;
		}Mouse;

		//use defined data
		//such like WM_SIZE
		struct
		{
			EUserType type;
			union
			{
				//for resize event
				struct
				{
					int width,height;
				}Size;

				//for calculation event
				Calculation::ECalType calType;
			};
		}UserData;

	};

	SEvent()
	{
		int size = sizeof(SEvent);
		memset(this,0,size);
	}
};
#endif
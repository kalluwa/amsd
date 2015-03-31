//2015/3/26

#ifndef _NEQ_NPS_METHOD_2_H_
#define _NEQ_NPS_METHOD_2_H
#include "calcHelper.h"
#include "../Src/Scene/ISceneManager.h"

#include "../Src/IO/IWriteFile.h"

namespace Calculation
{
	extern void Amsd_NEQ_NPS_UsingMethod2(BoxData* data,kk::scene::ISceneManager* scene,kk::io::IWriteFile* Output,const core::aabbox3di& adjustedBox);
}
#endif

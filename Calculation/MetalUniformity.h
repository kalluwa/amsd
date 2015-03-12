//2015/3/9

#ifndef _METAL_UNIFORMITY_H_
#define _METAL_UNIFORMITY_H_

#include "../Src/Core/core.h"
#include "CalcHelper.h"
#include "../Src/Scene/ISceneManager.h"


namespace Calculation
{
using namespace kk;

extern s32 debugSlicePos;
extern core::aabbox3di Amsd_MetalUniformity(BoxData* data,scene::ISceneManager* scene);

}//end namespace 

#endif //end 
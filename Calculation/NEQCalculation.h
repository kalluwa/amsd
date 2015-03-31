//2015/3/9

#ifndef _NEQ_CALCULATION_H_
#define _NEQ_CALCULATION_H_

#include "../Src/Core/core.h"
#include "CalcHelper.h"
#include "../Src/Scene/ISceneManager.h"
#include "../Src/io/IWriteFile.h"

namespace Calculation
{
using namespace kk;

extern s32 debugSlicePos;
extern core::aabbox3di Amsd_NEQCalculation(BoxData* data,scene::ISceneManager* scene,kk::io::IWriteFile* Output);

}//end namespace 

#endif //end 
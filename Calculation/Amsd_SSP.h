//2015/3/35
#ifndef _AMSD_SSP_H_
#define _AMSD_SSP_H_

#include "calcHelper.h"
#include "../Src/Core/core.h"
#include "../Src/Scene/ISceneManager.h"
#include "../Src/Scene/IVolumeTexture.h"
//debug
#include <Windows.h>

namespace Calculation
{

extern void Amsd_SSP_Calculation(BoxData* data,ISceneManager* scene);
}
#endif

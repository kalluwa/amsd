//2015/3/35
#ifndef _AMSD_SSP_H_
#define _AMSD_SSP_H_

#include "calcHelper.h"
#include "../Src/Core/core.h"
#include "../Src/Scene/ISceneManager.h"
#include "../Src/Scene/IVolumeTexture.h"
#include "../Src/IO/IWriteFile.h"
//debug
#include <Windows.h>

namespace Calculation
{

extern void Amsd_SSP_Calculation(BoxData* data,kk::scene::ISceneManager* scene,kk::io::IWriteFile* Output);
}
#endif

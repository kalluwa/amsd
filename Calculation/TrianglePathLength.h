//2015/3/6

#ifndef _TRIANGLE_PATH_LENGTH_H_
#define _TRIANGLE_PATH_LENGTH_H_

#include "CalcHelper.h"
#include "../Src/Core/Core.h"
#include "../Src/scene/ISceneManager.h"
#include "../src/io/iWriteFile.h"

namespace Calculation
{
extern void TrianglePathLength(BoxData* data,kk::scene::ISceneManager* scene,kk::io::IWriteFile* Output);
}


#endif

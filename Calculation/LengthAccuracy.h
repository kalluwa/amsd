//2015/3/3

#ifndef _LENGTH_ACCURACY_H_
#define _LENGHT_ACCYRACY_H_

#include "../Src/Core/core.h"
#include "calcHelper.h"
#include "../Src/scene/IScenemanager.h"
#include "../Src/io/IWriteFile.h"
namespace Calculation
{
	using namespace kk;
	//just static function header
	extern void Amsd_LengthAccuracy(BoxData* data,kk::scene::ISceneManager* scene,kk::io::IWriteFile* Output);
}//end namespace
#endif // end define
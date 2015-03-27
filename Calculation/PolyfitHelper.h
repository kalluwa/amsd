//2015/3/12

#ifndef _POLYFIT_HELPER_H_
#define _POLYFIT_HELPER_H_

#include "../Src/Core/Core.h"
#include "../alglib/interpolation.h"

namespace Calculation
{
	using namespace kk;
	using namespace alglib;

	extern f32 getPolyfitCubicMiddleResult(f32* input,s32 sizeToFit);
	extern f32 getPolyfitCubicDerivativeResult(f32* input,s32 sizeToFit);
	extern bool getPolyfitLineResult(const core::array<f32>& inputMiddlePoints,core::array<f32>& outputMiddlePoints);
	
}
#endif

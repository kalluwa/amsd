//2015/3/10

#ifndef _FFT_HELPER_H_
#define _FFT_HELPER_H_

#pragma comment(lib,"libfftw3f-3.lib")
//use double precision
//#pragma comment(lib,"libfftw3-3.lib")

//fftw header
#include "fftw3.h"
#include "../src/core/core.h" // my type definitions
namespace Calculation
{
	using namespace kk;
//replace all needed fftw definition
#define complex_type fftwf_complex
#define complex_allocate fftwf_malloc
#define complex_plan fftwf_plan
#define complex_fft_2d fftwf_plan_dft_2d
#define complex_fft_1d fftwf_plan_dft_1d
#define complex_fft_1d_real fftwf_plan_dft_r2c_1d
#define complex_execute fftwf_execute
#define complex_deplan fftwf_destroy_plan
#define complex_free fftwf_free
//internal functions######################################
	//generate N point
	//1D
	extern void generate1DData_random(complex_type*& dataToFill,s32 size);
	extern void generate1DData_given(complex_type*& dataToFill,f32* data,s32 size);
	//2D
	extern void generate2DData_random(complex_type*& dataToFill,s32 width,s32 height);
	extern void generate2DData_given(complex_type*& dataToFill,f32* data,s32 width,s32 height);

//internal functions #####################################
	extern void getImageDataSquareAmplitude(f32* data,f32*& outResult,s32 width,s32 height);

	extern void getTransfromed1DDataMagnitude(f32*data,f32*&outResult,s32 size);
}
#endif 

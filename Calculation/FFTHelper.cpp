//2015/3/10
#include "FFTHelper.h"

namespace Calculation
{
	using namespace kk;
//internal functions##############################
//###{
	void generate1DData_random(complex_type*& dataToFill,s32 size)
	{
		dataToFill = (complex_type*)complex_allocate(sizeof(complex_type) * size);
		
		for(s32 i=0;i<size;i++)
		{
			*(dataToFill+i)[0] = rand()/(f32)RAND_MAX;
			*(dataToFill+i)[1] = 0;
		}
	};
	void generate1DData_given(complex_type*& dataToFill,f32* data,s32 size)
	{
		dataToFill =(complex_type*)complex_allocate(sizeof(complex_type) * size);
		
		for(s32 i=0;i<size;i++)
		{
			*(dataToFill+i)[0] = data[i];
			*(dataToFill+i)[1] = 0;
		}
	};
//2D
	void generate2DData_random(complex_type*& dataToFill,s32 width,s32 height)
	{
		dataToFill = (complex_type*)complex_allocate(sizeof(complex_type) * width*height);
		
		for(s32 i=0;i<height;i++)
		{
		for(s32 j=0;j<width;j++)
		{
			*(dataToFill+i*width+j)[0] = rand()/(f32)RAND_MAX;
			*(dataToFill+i*width+j)[1] = 0;
		}
		}
	};
	void generate2DData_given(complex_type*& dataToFill,f32* data,s32 width,s32 height)
	{
		dataToFill = (complex_type*)complex_allocate(sizeof(complex_type)* width * height);
		for(s32 i=0;i<height;i++)
		{
		for(s32 j=0;j<width;j++)
		{
			*(dataToFill+i*width+j)[0] = data[i*width+j];
			*(dataToFill+i*width+j)[1] = 0;
		}
		}
	};
//    }###########################################
//internal functions##############################

//apply fft to the image and return result's squre amplitude
void getImageDataSquareAmplitude(f32* data,f32*& outResult,s32 width,s32 height)
{
	complex_type* in,*out;
	complex_plan plan;

	s32 Width = width;
	s32 Height = height;

	generate2DData_given(in,data,Width,Height);
	out = (complex_type*) complex_allocate(sizeof(complex_type)*Width*Height);
	//execute fft2d  [3, 3, in,out, FFTW_FORWARD, FFTW_ESTIMATE); ]
	plan = complex_fft_2d(Width, Height, in,out, FFTW_FORWARD, FFTW_ESTIMATE);
	complex_execute(plan);
	
	//fill square amplitude to outResult
	outResult = new f32[Width*Height];
	f32 real =0.0f;
	f32 imagine =0.0f;
	
	for(s32 i=0;i<Height;i++)
	{
	for(s32 j=0;j<Width;j++)
	{
		real = (*(out+i*Width+j))[0];
		imagine = (*(out+i*Width+j))[1];
		outResult[i*Width+j] =(f32)(real*real+imagine*imagine);//TODO:remove log
	}
	}
	//destroy and free data
	complex_deplan(plan);
	complex_free(in);
	complex_free(out);
}

void getTransfromed1DDataMagnitude(f32*data,f32*&outResult,s32 size)
{
	//float a[]={0,-0.1,-0.2,-0.3,-0.6,-0.35,-0.22,-0.1,0};
	//size = 101;
	fftwf_complex *in, *out;
	fftwf_plan p;
	int N=size;//sizeof(a)/sizeof(int);
	in = (fftwf_complex*) fftwf_malloc(sizeof(fftw_complex) * N);
	for(int i=0;i<N;i++)
	{
		((float*)in)[2*i+0]=data[i];
		((float*)in)[2*i+1]= 0;
	}
	out = (fftwf_complex*) fftwf_malloc(sizeof(fftw_complex) * N);
	p = fftwf_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	fftwf_execute(p); 

	outResult = new f32[size];
	for(s32 i=0;i<size;i++)
	{
		f32 real = out[i][0];
		f32 imagine = out[i][1];
		outResult[i] = sqrtf((f32)(real*real+imagine*imagine));
	}
	fftwf_destroy_plan(p);
	fftwf_free(in); fftwf_free(out);

	//fft
	/*complex_type* out;
	out = (complex_type*) complex_allocate(sizeof(complex_type)*size);
    complex_plan plan=complex_fft_1d_real(size,data,out,FFTW_ESTIMATE);
	complex_execute(plan);*/
	
	//complex_type* in,*out;
	//complex_plan plan;

	//in = (complex_type*) complex_allocate(sizeof(complex_type)*size);
	//for(s32 i=0;i<size;i++)
	//{
	//	in[i][0] = data[i];
	//	in[i][1] = 0.0f;
	//}
	//out = (complex_type*) complex_allocate(sizeof(complex_type)*size);
	////execute fft2d  [3, 3, in,out, FFTW_FORWARD, FFTW_ESTIMATE); ]
	//plan = complex_fft_1d(size,in,out, FFTW_FORWARD, FFTW_ESTIMATE);
	//complex_execute(plan);
	//
	////fill square amplitude to outResult
	//outResult = new f32[size];
	//f32 real =0.0f;
	//f32 imagine =0.0f;
	//
	//for(s32 i=0;i<size;i++)
	//{
	//	real = (*(out+i))[0];
	//	imagine = (*(out+i))[1];
	//	outResult[i] =(f32)sqrtf(real*real+imagine*imagine);//TODO:remove log
	//}

	////extract data
	///*outResult = new f32[size];
	//for(s32 i=0;i<size;i++)
	//{
	//	f32 real = out[i][0];
	//	f32 imagine = out[i][1];
	//	outResult[i] = sqrtf((f32)(real*real+imagine*imagine));
	//}*/
	//complex_deplan(plan);
	//complex_free(in);
	//complex_free(out);
};
}

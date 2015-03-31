//2015/3/12

#include "PolyfitHelper.h"
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

namespace Calculation
{
	using namespace kk;
	using namespace alglib;
	//return the middle polyfit value
	//sizeToFit maybe 11,21,41 middle =5,10,20[0-based index]
	f32 getPolyfitCubicMiddleResult(f32* input,s32 sizeToFit)
	{
		if(sizeToFit==0)
			return -1;
		std::stringstream streamX;
		streamX<<"[";
		streamX<<0;
		for(s32 i=1;i<sizeToFit;i++)
		{
			streamX<<","<<i;
		}
		streamX<<"]";
		char* tmpCharsX = new char[(u32)streamX.tellp()+10];
		streamX>>tmpCharsX;
		string inputX(tmpCharsX);
		if(tmpCharsX)
			delete []tmpCharsX;

		std::stringstream streamR;
		streamR<<"[";
		streamR.precision(2);//3.1113f->3.11f
		streamR<<input[0];
		for(s32 i=1;i<sizeToFit;i++)
		{
			streamR<<","<<input[i];
		}
		streamR<<"]";
		char* tmpCharsR = new char[(u32)streamR.tellp()+1];
		streamR>>tmpCharsR;
		string inputY(tmpCharsR);
		if(tmpCharsR)
		delete []tmpCharsR;
		// ready to continue
		//inputX="[0,1,2,3,4,5,6]"
		//inputY="[0.12,0.23,....]"
		real_1d_array x = inputX.c_str();
		real_1d_array y = inputY.c_str();
		//m=4 means cubit fit
		ae_int_t m=4;
		double middleX = sizeToFit/2;//middleX
		//something i don't care
		ae_int_t info;
		barycentricinterpolant p;
		polynomialfitreport rep;
		//fit middle result
		double fitResultValue;
		//fit without weights
		polynomialfit(x,y,m,info,p,rep);
		//get fit value in middleX
		fitResultValue = barycentriccalc(p,middleX);
		//clear
		inputX.clear();
		inputY.clear();
		streamR.clear();
		streamX.clear();
		return (f32)fitResultValue;
	};

	f32 getPolyfitCubicDerivativeResult(f32* input,s32 sizeToFit)
	{
		if(sizeToFit==0)
			return -1;
		std::stringstream streamX;
		streamX<<"[";
		streamX<<0;
		for(s32 i=1;i<sizeToFit;i++)
		{
			streamX<<","<<i;
		}
		streamX<<"]";
		char* tmpCharsX = new char[(u32)streamX.tellp()+1];
		streamX>>tmpCharsX;
		string inputX(tmpCharsX);
		if(tmpCharsX)
			delete []tmpCharsX;

		std::stringstream streamR;
		streamR<<"[";
		streamR.precision(2);//3.1113f->3.11f
		streamR<<input[0];
		for(s32 i=1;i<sizeToFit;i++)
		{
			streamR<<","<<input[i];
		}
		streamR<<"]";
		char* tmpCharsR = new char[(u32)streamR.tellp()+1];
		streamR>>tmpCharsR;
		string inputY(tmpCharsR);
		if(tmpCharsR)
		delete []tmpCharsR;
		// ready to continue
		//inputX="[0,1,2,3,4,5,6]"
		//inputY="[0.12,0.23,....]"
		real_1d_array x = inputX.c_str();
		real_1d_array y = inputY.c_str();
		//m=4 means cubit fit
		ae_int_t m=4;
		double middleX = sizeToFit/2;//middleX
		//something i don't care
		ae_int_t info;
		barycentricinterpolant p;
		polynomialfitreport rep;
		//fit middle result
		f64 fitDerivativeValue;
		//fit without weights
		polynomialfit(x,y,m,info,p,rep);
		//get fit value in middleX
		real_1d_array kk;
		polynomialbar2pow(p,0,1,kk);
		string strs = kk.tostring(10);

		f64 a,b,c,d;//a+bx+cx2+dx3
		sscanf_s(strs.c_str(),"[%lf,%lf,%lf,%lf]",&a,&b,&c,&d);

		fitDerivativeValue = b+2*c*middleX+d*3*middleX*middleX;
		//clear data
		//streamR.clear();
		//streamX.clear();
		return (f32)fitDerivativeValue;
	};

	bool getPolyfitLineResult(const core::array<f32>& inputMiddlePoints,core::array<f32>& outputMiddlePoints)
	{
		s32 sizeToFit = (s32)inputMiddlePoints.size();
		if(sizeToFit==0)
			return false;
		std::stringstream streamX;
		streamX<<"[";
		streamX<<0;
		for(s32 i=1;i<sizeToFit;i++)
		{
			streamX<<","<<i;
		}
		streamX<<"]";
		char* tmpCharsX = new char[(u32)streamX.tellp()+1];
		streamX>>tmpCharsX;
		string inputX(tmpCharsX);
		if(tmpCharsX)
			delete []tmpCharsX;

		std::stringstream streamR;
		streamR<<"[";
		streamR.precision(2);//3.1113f->3.11f
		streamR<<inputMiddlePoints[0];
		for(s32 i=1;i<sizeToFit;i++)
		{
			streamR<<","<<inputMiddlePoints[i];
		}
		streamR<<"]";
		char* tmpCharsR = new char[(u32)streamR.tellp()+1];
		streamR>>tmpCharsR;
		string inputY(tmpCharsR);
		if(tmpCharsR)
		delete []tmpCharsR;
		// ready to continue
		//inputX="[0,1,2,3,4,5,6]"
		//inputY="[0.12,0.23,....]"
		real_1d_array x = inputX.c_str();
		real_1d_array y = inputY.c_str();
		//m=2 means line fit
		ae_int_t m=2;
		//something i don't care
		ae_int_t info;
		barycentricinterpolant p;
		polynomialfitreport rep;
		//fit middle result
		double fitResultValue;
		//fit without weights
		polynomialfit(x,y,m,info,p,rep);
		//get fit value in pos[x=i]
		for(s32 i=0;i<sizeToFit;i++)
		{
			fitResultValue = barycentriccalc(p,(f64)i);
			outputMiddlePoints.push_back((f32)fitResultValue);
		}
		return true;
	};
}

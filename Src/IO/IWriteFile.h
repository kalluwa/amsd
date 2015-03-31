//2015/3/30

#ifndef _I_FILE_H_
#define _I_FILE_H_

#include "../Core/core.h"

namespace kk
{
namespace io
{
	class IWriteFile 
	{
	public:
		virtual ~IWriteFile()
		{
			//nothing here
		}
		virtual void openFile(const core::stringc& filename,bool createNew)=0;

		virtual void closeFile()=0;

		virtual void writeString(const core::stringc& str)=0;

		virtual void writeInt(s32 value)=0;

		virtual void writeSingle(f32 value)=0;

		virtual void writeDouble(f64 value)=0;

		virtual void writeEmptyLine()=0;

		virtual bool isFileExist(const core::stringc& filename)=0;
		//write array
		virtual void writeArrayInt(s32* values,s32 size,c8 separator)=0;
		virtual void writeArrayInt(const core::array<s32>& values,c8 separator)=0;

		virtual void writeArraySingle(f32* values,s32 size,c8 separator)=0;
		virtual void writeArraySingle(const core::array<f32>& values,c8 separator)=0;
		
		virtual void writeArrayDouble(f64* values,s32 size,c8 separator)=0;
		virtual void writeArrayDouble(const core::array<f64>& values,c8 separator)=0;
		
		virtual void writeArrayString(const core::array<core::stringc>& strs,c8 separator)=0;
	};
}
}
#endif

//2015/3/31

#ifndef _C_WRITE_FILE_WINDOWS_H_
#define _I_WRITE_FILE_WINDOWS_H_

#include "IWriteFile.h"
#include <iostream>
#include <fstream>
namespace kk
{
namespace io
{
	//writefile for windows
	class CWriteFileW : public IWriteFile
	{
	public:
		//createnew is false by default
		//we append the file if it exists on the disk
		CWriteFileW(const char* fileName,bool binary=false,bool createNew = false);

		CWriteFileW(const core::stringc& fileName,bool binary=false,bool createNew = false);

		virtual ~CWriteFileW();

		virtual void openFile(const core::stringc& filename,bool createNew);

		virtual void closeFile();

		virtual void writeString(const core::stringc& str);

		virtual void writeEmptyLine();

		virtual void writeInt(s32 value);

		virtual void writeSingle(f32 value);

		virtual void writeDouble(f64 value);
		
		virtual bool isFileExist(const core::stringc& filename);
		//write array
		virtual void writeArrayInt(s32* values,s32 size,c8 separator='\n');
		virtual void writeArrayInt(const core::array<s32>& values,c8 separator='\n');

		virtual void writeArraySingle(f32* values,s32 size,c8 separator='\n');
		virtual void writeArraySingle(const core::array<f32>& values,c8 separator='\n');

		virtual void writeArrayDouble(f64* values,s32 size,c8 separator='\n');
		virtual void writeArrayDouble(const core::array<f64>& values,c8 separator='\n');

		virtual void writeArrayString(const core::array<core::stringc>& strs,c8 separator='\n');
		
	private:
		core::stringc FileName;
		//store the fileHandle
		bool BinaryOperation;
		FILE* FilePointer;
		std::ofstream FileStream;

	};
}
}
#endif

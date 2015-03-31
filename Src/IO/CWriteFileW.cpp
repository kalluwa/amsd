//2015/3/31

#include "CWriteFileW.h"
//reference:http://www.cplusplus.com/doc/tutorial/files/
namespace kk
{
namespace io
{
	CWriteFileW::CWriteFileW(const char* filename,bool binary,bool createNew)
	{
		BinaryOperation = binary;
		openFile(core::stringc(filename),createNew);	
	}
	CWriteFileW::CWriteFileW(const core::stringc&filename,bool binary,bool createNew)
	{
		BinaryOperation= binary;
		openFile(filename,createNew);
	}
	
	CWriteFileW::~CWriteFileW()
	{
		closeFile();
	}
	void CWriteFileW::openFile(const core::stringc& filename,bool createNew)
	{
		//store the file name information
		FileName = core::stringc(filename.c_str());
		//create file pointer
		core::stringc operation = "a+";
		if(createNew&&isFileExist(filename))
		{
			//delete old file
			remove(filename.c_str());
		}
		if(BinaryOperation)
			fopen_s(&FilePointer,filename.c_str(),operation.c_str());
		else
			FileStream.open(filename.c_str(), std::ios::out | std::ios::app); 
	};
	
	void CWriteFileW::closeFile()
	{
		if(BinaryOperation&&FilePointer)
		{
			fflush(FilePointer);
			fclose(FilePointer);
		}
		else if(!BinaryOperation&&FileStream.is_open())
			FileStream.close();
	};

	void CWriteFileW::writeString(const core::stringc& str)
	{
		if(BinaryOperation)
			fwrite(str.c_str(),1,(s32)str.size(),FilePointer);
		else
			FileStream<<str.c_str();
	};
	
	void CWriteFileW::writeEmptyLine()
	{
		char anotherLine='\n';
		if(BinaryOperation)
			fwrite(&anotherLine,1,1,FilePointer);
		else
			FileStream<<anotherLine;
	}
	void CWriteFileW::writeInt(s32 value)
	{
		if(BinaryOperation)
			fwrite(&value,sizeof(s32),1,FilePointer);
		else
			FileStream<<value;
	};

	void CWriteFileW::writeSingle(f32 value)
	{
		if(BinaryOperation)
			fwrite(&value,sizeof(f32),1,FilePointer);
		else	
			FileStream<<value;
	};

	void CWriteFileW::writeDouble(f64 value)
	{
		if(BinaryOperation)
			fwrite(&value,sizeof(f64),1,FilePointer);
		else
			FileStream<<value;
	};
		
	bool CWriteFileW::isFileExist(const core::stringc& filename)
	{
		FILE* fp =0;
		fopen_s(&fp,filename.c_str(),"r");
		if(!fp)
		{
			return false;
		}

		fclose(fp);
		fp = 0;
		return true;
	};
	//write array
	void CWriteFileW::writeArrayInt(s32* values,s32 size,c8 separator)
	{
		for(s32 i=0;i<size;i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(values+i,sizeof(s32),1,FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<values[i]<<separator;
		}
	};
	void CWriteFileW::writeArrayInt(const core::array<s32>& values,c8 separator)
	{
		for(s32 i=0;i<(s32)values.size();i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(&values[i],sizeof(s32),1,FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<values[i]<<separator;
		}
	};
	void CWriteFileW::writeArraySingle(f32* values,s32 size,c8 separator)
	{
		for(s32 i=0;i<size;i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(values+i,sizeof(f32),1,FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<values[i]<<separator;
		}
	};
	void CWriteFileW::writeArraySingle(const core::array<f32>& values,c8 separator)
	{
		for(s32 i=0;i<(s32)values.size();i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(&values[i],sizeof(f32),1,FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<values[i]<<separator;
		}
	};
	void CWriteFileW::writeArrayDouble(f64* values,s32 size,c8 separator)
	{
		for(s32 i=0;i<size;i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(values+i,sizeof(f64),1,FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<values[i]<<separator;
		}
	};
	void CWriteFileW::writeArrayDouble(const core::array<f64>& values,c8 separator)
	{
		for(s32 i=0;i<(s32)values.size();i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(&values[i],sizeof(f64),1,FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<values[i]<<separator;
		}
	};
	void CWriteFileW::writeArrayString(const core::array<core::stringc>& strs,c8 separator)
	{
		for(s32 i=0;i<(s32)strs.size();i++)
		{
			//write value
			if(BinaryOperation)
			{
			fwrite(strs[i].c_str(),1,(s32)strs[i].size(),FilePointer);
			fwrite(&separator,1,1,FilePointer);
			}
			else
			FileStream<<strs[i].c_str()<<separator;
		}
	};
}
}

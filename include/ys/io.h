#pragma once
#include <typedef.h>
#include <ys/error.h>
namespace ys
{
	//二进制文件buffer
	class FileBufferB
	{
	public:
		BYTE* buffer;
		size_t size;

		FileBufferB(BYTE* pointer, size_t bufferSize);

		FileBufferB(const char* fileName);

		~FileBufferB();

		bool OutPut(const char* outfile, size_t outSize);
	};

	//文本文件buffer
	class FileBufferA
	{
	public:
		char* buffer;
		size_t size;

		FileBufferA(const char* fileName);

		~FileBufferA();
	};
}
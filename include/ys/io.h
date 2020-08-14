#pragma once
#include <typedef.h>
#include <ys/error.h>
namespace ys
{
	//�������ļ�buffer
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

	//�ı��ļ�buffer
	class FileBufferA
	{
	public:
		char* buffer;
		size_t size;

		FileBufferA(const char* fileName);

		~FileBufferA();
	};
}
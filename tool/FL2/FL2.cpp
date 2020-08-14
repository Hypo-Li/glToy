/*
 * 利用fast-lzma2对数据进行压缩和解压，可用于临时修改模型内部数据
*/
#include <ys/io.h>
#include <fastlzma2/fast-lzma2.h>
#include <getopt.h>
#include <typedef.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	char optchar;
	bool compressTF = true;
	while ((optchar = getopt(argc, argv, "cd")) != EOF)
	{
		switch (optchar)
		{
		case 'c':
			compressTF = true;
			break;
		case 'd':
			compressTF = false;
			break;
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;

	for (unsigned int i = 0; i < argc; i++)
	{
		ys::FileBufferB file(argv[i]);
		if (file.buffer == nullptr)
			continue;
		if (compressTF) {
			//压缩
			size_t srcSize = file.size;
			BYTE* buffer = (BYTE*)malloc(srcSize + 4);
			BYTE* outBuffer = buffer + 4;
			size_t outSize = FL2_compress(outBuffer, srcSize, file.buffer, srcSize, 9) + 4;
			*(DWORD*)buffer = srcSize;
			ys::FileBufferB outFile(buffer, outSize);
			string fileName = argv[i];
			if (fileName.find_last_of(".data") != fileName.npos)
				fileName = fileName.substr(0, fileName.find_last_of(".data") - 4);
			outFile.OutPut(fileName.c_str(), outFile.size);
		}
		else {
			//解压缩
			size_t srcSize = file.size;
			size_t outSize = *(DWORD*)file.buffer;
			BYTE* buffer = (BYTE*)malloc(outSize);
			FL2_decompress(buffer, outSize, file.buffer + 4, srcSize - 4);
			ys::FileBufferB outFile(buffer, outSize);
			string fileName = argv[i];
			fileName += string(".data");
			outFile.OutPut(fileName.c_str(), outFile.size);
		}
	}
}
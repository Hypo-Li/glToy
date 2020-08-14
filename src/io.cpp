#define _CRT_SECURE_NO_WARNINGS //±‹√‚fopen±®¥Ì
#include <ys/io.h>

ys::FileBufferB::FileBufferB(BYTE* pointer, size_t bufferSize)
{
    buffer = pointer;
    size = bufferSize;
}

ys::FileBufferB::FileBufferB(const char* fileName) :buffer(nullptr), size(0)
{
	FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        ys::PutErrorLog(fileName, "Couldn't open this file;");
        return;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (BYTE*)malloc(size);
    fread(buffer, size, 1, fp);
    fclose(fp);
}

ys::FileBufferB::~FileBufferB()
{
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
}

bool ys::FileBufferB::OutPut(const char* outfile, size_t outSize)
{
    if (buffer == nullptr || outSize > size)
        return false;
    FILE* fp = fopen(outfile, "wb");
    if (fwrite(buffer, outSize, 1, fp)) {
        fclose(fp);
        return true;
    }
    else {
        fclose(fp);
        return false;
    }
}

ys::FileBufferA::FileBufferA(const char* fileName) :buffer(nullptr), size(0)
{
    FILE* fp = fopen(fileName, "rb");
    if (!fp) {
        ys::PutErrorLog(fileName, "Couldn't open this file;");
        return;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    buffer = (char*)malloc(size + 1); //0x00
    buffer[size] = 0x00;
    fread(buffer, size, 1, fp);
    fclose(fp);
}

ys::FileBufferA::~FileBufferA()
{
    if (buffer) {
        free(buffer);
        buffer = nullptr;
    }
}
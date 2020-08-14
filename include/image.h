#pragma once
#include <typedef.h>
BYTE* LoadImage(const char* file, int* width, int* height, int* comp, int reqComp);

void FreeImage(void* data);
#include <image.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb-master/stb_image.h>

BYTE* LoadImage(const char* file, int* width, int* height, int* comp, int reqComp)
{
	return stbi_load(file, width, height, comp, reqComp);
}

void FreeImage(void* data)
{
	stbi_image_free(data);
}
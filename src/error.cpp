#include <ys/error.h>

void ys::PutErrorLog(const char* errorObj, const char* logInfo)
{
	std::cerr << "ERROR - " << errorObj << ":" << std::endl << logInfo << std::endl;
}
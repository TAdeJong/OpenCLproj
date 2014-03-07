#ifndef PLATFORM_H
#define PLATFORM_H

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#ifndef TARGET_PLATFORM
#	define TARGET_PLATFORM GPU
#endif

#define CONCAT(A, B) A ## B
#define GET_TARGET_PLATFORM TOKENPASTE2(CL_DEVICE_TYPE_, TARGET_PLATFORM)

#endif // PLATFORM_H

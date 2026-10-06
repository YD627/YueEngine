#pragma once
#include "Log.h"
//#include <intrin.h>

#ifdef YUE_ENABLE_ASSERT

	#define YUE_ASSERT(x, ...) { if(!(x)) { YUE_ERROR(__VA_ARGS__);} }
	#define YUE_CORE_ASSERT(x, message){if(!(x)) {YUE_CORE_ERROR(message);__debugbreak();}}

#else

	#define YUE_ASSERT(...)
	#define YUE_CORE_ASSERT(...)

#endif // YUE_ENABLE_ASSERT



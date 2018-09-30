#ifndef _REDEYE_DEVICE_DEBUG
#define _REDEYE_DEVICE_DEBUG

#include <stdio.h>

#define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[%s:%d] - %s : "fmt, __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__);

#endif

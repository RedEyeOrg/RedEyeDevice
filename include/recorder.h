#ifndef _REDEYE_DEVICE_RECORDER
#define _REDEYE_DEVICE_RECORDER

#include <stdbool.h>

bool init_recorder();
bool start_record();
bool stop_record();
bool is_recording();

#endif

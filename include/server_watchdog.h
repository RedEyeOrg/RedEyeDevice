#ifndef _REDEYE_DEVICE_SERVER_WATCHDOG
#define _REDEYE_DEVICE_SERVER_WATCHDOG

#include <stdbool.h>

bool init_server_watchdog(bool (*disconnect_cb)(), bool(*reconnect_cb)());
bool start_server_watchdog();

#endif

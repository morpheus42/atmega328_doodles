

#ifndef _DE_H_
#define _DE_H_

#include "unistd.h"

extern volatile char __emu_sleep_access;

#define sleep_disable() __emu_sleep_access=0
#define sleep_enable() __emu_sleep_access=1
#define sleep_cpu() if (__emu_sleep_access) usleep(100000)

#endif


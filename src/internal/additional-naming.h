#pragma once

// just some values for the exec,
// to not use some random values.
#define ATYPE_num 0b00
#define ATYPE_var 0b01
#define ATYPE_array 0b10
#define ATYPE_string 0b11

// lower 32-bit mask for splitting a 64-bit value
#define LOW_32bit_FULL 0xFFFFFFFFUL

// sleep(ms) macro
#ifdef _WIN32
#include <windows.h>
#define ASLEEP(ms) Sleep(ms);

#else // linux
#include <unistd.h>
#define ASLEEP(ms) usleep((ms) * 1000) // mic sec -> ms

#endif

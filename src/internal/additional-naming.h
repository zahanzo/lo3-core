// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#pragma once

// just some values for the exec,
// to not use some random values.
#define ATYPE_num 0b00
#define ATYPE_var 0b01
#define ATYPE_array 0b10
#define ATYPE_string 0b11

// sleep(ms) macro
#ifdef _WIN32
#include <windows.h>
#define ASLEEP(ms) Sleep(ms);

#else // linux
#include <unistd.h>
#define ASLEEP(ms) usleep((ms) * 1000) // mic sec -> ms

#endif

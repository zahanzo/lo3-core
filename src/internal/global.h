// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#pragma once

#include "bare-define.h"

#define G_SIZE 100
typedef struct lo3_g lo3_g;

///// from g.c ///// getter and setter
lo3_val g_get(int index);
void g_set(int index, lo3_val value);

int g_isSet(int index);
int g_getType(int index);
int g_getNum(int index); // risky
int g_setType(int index, lo3_val type);

lo3_val g_getValue(int index);
void g_fasterInit(char *line);

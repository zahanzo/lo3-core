// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#pragma once

#include "bare-define.h"
#include <string.h>

typedef enum {

	// assign
	BSC_asn = '=',

	// alu
	ALU_add = '+',
	ALU_sub = '-',
	ALU_div = '/',
	ALU_mul = '*',

	// control
	CNT_jmp = 'd',   // define a jmp point
	CNT_call = 'c',  // call extern lib (uses fixed path)
	CNT_callS = 'C', // call extern lib (better func!, uses relative path)
	CNT_label = 'l', // define new label
	CNT_new = 'n',   // define new variable
	CNT_cmp = '?',
	CNT_small = '<',
	CNT_big = '>',

	// stream
	STM_out = 'o', // shifts sth into stdout
	STM_in = 'i',  // shifts sth into stdin

	CNT_free = 'f', // free a var

	RET_good = '0', // stop the program with exit code 0
	RET_bad = '1',  // return the program with exit code 1
	RET_smart = 'r', // some feature which needs call func, but this is not avaible yet...

	// platform
	SYS_call = 's' // platform-specific syscall

} lo3_cmds;

////////// parser //////////
lo3_val pars_resv(char type[64]);
int pars_dispatch(lo3_cmds cmd, lo3_val a1, lo3_val a2);

// execute func
void exec_new(lo3_val a1, lo3_val a2);
void exec_asn(lo3_val a1, lo3_val a2);
void exec_add(lo3_val a1, lo3_val a2);
void exec_sub(lo3_val a1, lo3_val a2);
void exec_mul(lo3_val a1, lo3_val a2);
void exec_div(lo3_val a1, lo3_val a2);
void exec_jmp(lo3_val a1, lo3_val a2);
void exec_call(lo3_val a1, lo3_val a2);
void exec_callS(lo3_val a1, lo3_val a2);
void exec_label(lo3_val a1, lo3_val a2);
void exec_out(lo3_val a1, lo3_val a2);
void exec_in(lo3_val a1, lo3_val a2);
void exec_free(lo3_val a1, lo3_val a2);
void exec_cmp(lo3_val a1, lo3_val a2);
void exec_small(lo3_val a1, lo3_val a2);
void exec_big(lo3_val a1, lo3_val a2);
void exec_syscall(lo3_val a1, lo3_val a2);

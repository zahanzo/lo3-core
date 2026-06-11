// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#include "./internal/bare-var.h"
#include "./internal/core.h"
#include "./internal/global.h"
#include "./internal/specific-language.h"
#include "internal/control-flow.h"
#include <errno.h>
#include <limits.h>
#include <string.h>

void exec_new(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {

		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		lo3_warn("Are you sure you want to set the name as a number?", buf);
		name = buf;
	} else {
		name = a1.value.string;
	}

	// get the type
	unsigned int type;
	if (!a2.chooseType) {
		type = (unsigned int)a2.value.num;
	} else {
		char *end;
		errno = 0;
		long val = strtol((char *)a2.value.string, &end, 10);
		if (errno != 0 || end == (char *)a2.value.string || val < 0 || val > INT_MAX) {
			lo3_error("Invalid type value", (char *)a2.value.string);
			return;
		}
		type = (unsigned int)val;
	}

	// duplication check
	if (var_find(name) != -1) {
		lo3_error("Variable already exists!", name);
		return;
	}

	if (type != 0 && type != 3) {

		snprintf(buf, sizeof(buf), "%d", type);
		lo3_warn("Variable type might not be num or string!", buf);
	}

	var_create(name, type);
}

void exec_free(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {
		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		lo3_warn("Are you sure you want to delete some var called:\nDoing it anyways... ",
		         buf);
		name = buf;
	} else {
		name = a1.value.string;
	}

	// ignore arg1

	var_free(name);
}

/*
 * algo:
 * if the type of a1 is the same type, else lo3_error.
 * assign a1 = a2; if everything is correct!
 *
 * what is ok?:
 * string as name, num as name
 */
void exec_asn(lo3_val a1, lo3_val a2) {

	unsigned char buf[64], *name;
	unsigned char numNameBuf[64];

	if (!a1.chooseType) {
		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;

	} else {
		name = a1.value.string;
	}

	// var_get() would also work here, but it is just a wrapper around var_find()
	if (var_find(name) == -1) {
		lo3_error("Could not find <var> please check if it really exists!", "");
		return;
	}

	// set
	if (!a2.chooseType) {
		var_setNum(name, a2.value.num);
	} else {
		var_setString(name, a2.value.string);
	}
}

///// alu /////

void exec_add(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {
		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;

	} else {
		name = a1.value.string;
	}

	if (var_find(name) == -1) {
		lo3_error("Could not find this var, please check if you got something wrong", name);
		return;
	}

	lo3_var *oldVar = var_get(name); // it must be right now, else var_find() was wrong before!

	if (a2.chooseType) {
		lo3_error("Arg1 requires TYPE String for +=", "");
		return;
	}

	var_setNum(name, var_getNum(oldVar) + a2.value.num);
}

void exec_sub(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {
		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;

	} else {
		name = a1.value.string;
	}

	if (var_find(name) == -1) {
		lo3_error("Could not find this var, please check if you got something wrong", name);
		return;
	}

	lo3_var *oldVar = var_get(name); // it must be right now, else var_find() was wrong before!

	if (a2.chooseType) {
		lo3_error("Arg1 requires TYPE String for -=", "");
		return;
	}

	var_setNum(name, var_getNum(oldVar) - a2.value.num);
}

void exec_mul(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {
		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;

	} else {
		name = a1.value.string;
	}

	if (var_find(name) == -1) {
		lo3_error("Could not find this var, please check if you got something wrong", name);
		return;
	}

	lo3_var *oldVar = var_get(name); // it must be right now, else var_find() was wrong before!

	if (a2.chooseType) {
		lo3_error("Arg1 requires TYPE String for *=", "");
		return;
	}

	var_setNum(name, var_getNum(oldVar) * a2.value.num);
}

void exec_div(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {
		snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;

	} else {
		name = a1.value.string;
	}

	if (var_find(name) == -1) {
		lo3_error("Could not find this var, please check if you got something wrong", name);
		return;
	}

	lo3_var *oldVar = var_get(name); // it must be right now, else var_find() was wrong before!

	if (a2.chooseType) {
		lo3_error("Arg1 requires TYPE String for /=", "");
		return;
	}

	if (a2.value.num == 0) {
		lo3_error("Division by zero", name);
		return;
	}

	var_setNum(name, var_getNum(oldVar) / a2.value.num);
}

// will cmp and jmp! #?
void exec_jmp(lo3_val a1, lo3_val a2) {

	char buf[64], buf2[64];
	char *name, *name2;

	// label
	if (!a1.chooseType) {
		(void)snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;
	} else {
		name = a1.value.string;
	}

	if (cf_findLabel(name) == -1) {
		return;
	}

	// g[0] == name2
	if (a2.chooseType != 0) {
		lo3_error("Illegal type, here arg1 must be an integer value to compare against!",
		          "");
		return;
	}

	if (g_getNum(0) == a2.value.num) {
		rush = TRUE;

		(void)strncpy(rush_target, name, sizeof(rush_target) - 1);
		cf_jumpToLabel(name);
	}
	return;
}

// #c
void exec_call(lo3_val a1, lo3_val a2) {
}

// #C - not in use - UB
void exec_callS(lo3_val a1, lo3_val a2) {
}

void exec_label(lo3_val a1, lo3_val a2) {

	char buf[64];
	char *name;

	if (!a1.chooseType) {
		(void)snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;
	} else {
		name = a1.value.string;
	}

	if (cf_findLabel(name) != -1) {

		if (rush && !strcmp(rush_target, name)) {
			rush = FALSE;
			return;
		}
		return;
	}

	cf_addLabel(name, lastLineOffset);
}

void exec_out(lo3_val a1, lo3_val a2) {

	unsigned char buf[64];
	unsigned char *name;

	if (!a1.chooseType) {
		(void)snprintf(buf, sizeof(buf), "%d", a1.value.num);
		name = buf;
	} else {
		name = a1.value.string;
	}

	printf("%s\n", name);
}

void exec_in(lo3_val a1, lo3_val a2) {

	unsigned char buf[64] = {0}; // privides buf not initing
	unsigned char *res;

	unsigned char numNameBuf[64];
	unsigned char *name;

	res = fgets(buf, sizeof(buf), stdin);

	if (res == NULL) {
		lo3_error("Something went wrong while reading the stdin stream.\n"
		          "Please check if you done anything wrong...",
		          buf);
		return;
	}

	// parse the '\n' away
	buf[strcspn(buf, "\n")] = '\0';

	lo3_val temp = pars_resv(buf);

	// correct the a1.~type
	if (!a1.chooseType) {
		snprintf(numNameBuf, sizeof(numNameBuf), "%d", a1.value.num);
		name = numNameBuf;
	} else {
		name = a1.value.string;
	}

	if (!a2.chooseType) {
		var_setNum(name, temp.value.num);
	} else {
		var_setString(name, temp.value.string);
	}
}

// compare num == num, if true -> g[0] = 1, else g[0] = 0;!!!
void exec_cmp(lo3_val a1, lo3_val a2) {

	if (a1.chooseType || a2.chooseType) {
		lo3_error("You can not compare char*'s,\n"
	    "Please use the coresponding std-func!, from {string.c}", "");
		return;
	}

	lo3_val true, false;
	true.value.num = TRUE;
	true.chooseType = 0;

	false.value.num = FALSE;
	false.chooseType = 0;

	if (a1.value.num == a2.value.num) {
		g_set(0, true);
		return;
	}
	g_set(0, false);
}

// compare num < num, if true -> g[0] = 1, else g[0] = 0;!!!
void exec_small(lo3_val a1, lo3_val a2) {

	if (a1.chooseType || a2.chooseType) {
		lo3_error("You can not compare char*'s,\n"
	    "Please use the coresponding std-func!, from {string.c}", "");
		return;
	}

	lo3_val true, false;
	true.value.num = TRUE;
	true.chooseType = 0;

	false.value.num = FALSE;
	false.chooseType = 0;

	if (a1.value.num < a2.value.num) {
		g_set(0, true);
		return;
	}
	g_set(0, false);
}

// compare num > num, if true -> g[0] = 1, else g[0] = 0;!!!
void exec_big(lo3_val a1, lo3_val a2) {

	if (a1.chooseType || a2.chooseType) {
		lo3_error("You can not compare char*'s,\n"
	    "Please use the coresponding std-func!, from {string.c}", "");
		return;
	}

	lo3_val true, false;
	true.value.num = TRUE;
	true.chooseType = 0;

	false.value.num = FALSE;
	false.chooseType = 0;

	if (a1.value.num > a2.value.num) {
		g_set(0, true);
		return;
	}
	g_set(0, false);
}

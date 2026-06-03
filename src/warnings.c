// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#include "./internal/core.h"

#define LO3_DEBUG(msg, context) \	
	if (_check_null(msg) == -1) { \
		(void)fprintf(stderr, CLR_BOLD CLR_RED "[ERROR]: Something went wrong. \nOr msg == empty?\n" CLR_RESET); \						\
	} \
	else { \
		_bare_send(msg, context, "DEBUG"); \
	} \

static int _check_null(const char *msg) {

	if (msg == NULL || msg[0] == '\0') {
		
		(void)fprintf(stderr, CLR_BOLD CLR_RED "[ERROR]: Something went wrong. \nOr msg == empty?\n" CLR_RESET);
		return -1;
	}
	return 0;
}

static void _bare_send(const char *msg, const char *context, const char *type) {
	
	if (context != NULL) {
		(void)fprintf(
		    stderr, CLR_BOLD CLR_RED "[%s]: %s " CLR_RESET "(?: '%s', line: %d)\n",
		    type, msg, context, currentLine);
	} else {
		(void)fprintf(stderr, CLR_BOLD CLR_RED "[%s]: %s \n" CLR_RESET, type, msg);
	}
}

void lo3_warn(const char *msg, const char *context) {

	if (_check_null(msg) == -1) {
		_bare_send("Could not send error message!", context, "ERROR");
		return;
	}
	_bare_send(msg, context, "WARN");
}

void lo3_error(const char *msg, const char *context) {

	if (_check_null(msg) == -1) {
		_bare_send("Could not send error message!", context, "ERROR");
		return;
	}
	_bare_send(msg, context, "ERROR");
}
#include "./internal/control-flow.h"
#include "./internal/core.h"
#include "./internal/cf-define.h"
#include "./internal/specific-language.h"
#include <string.h>

lo3_label cf;
char rush = FALSE;
char isWarped = FALSE;
char rush_target[256];

int cf_findLabel(const char *name) {

	for (int i = 0; i < cf.nextFreePos; i++) {
		if (cf.names[i] && !strcmp(name, cf.names[i])) {
			return i;
		}
	}
	return -1;
}

int cf_addLabel(const char *name, const int pos) {

	if (cf.nextFreePos == ARRAY_SIZE - 1) {
		return -1;
	}

	// is already known?
	if (cf_findLabel(name) != -1) {
		lo3_error("Label already known!", name);
		return -1;
	}

	cf.names[cf.nextFreePos] = strdup(name);
	cf.pos[cf.nextFreePos] = pos;
	cf.nextFreePos++;
	return 0;
}

int cf_getPos(const char *name) {

	int index = cf_findLabel(name);

	if (index == -1) {
		return -1;
	}

	return cf.pos[index];
}

// important: this func should only be used intern, because it's not error proof
void cf_setCursorPos(const int i) {

	(void)fseek(openFile, i, SEEK_SET);
}

int cf_getCursorPos(void) {
	return -1;
}

int cf_jumpToLabel(const char *name) {

	int index = cf_findLabel(name);

	if (index == -1) {
		lo3_error("Label not found!, Was this the right name?", name);
		return -1;
	}

	cf_setCursorPos(cf.pos[index]);
	return 0;
}

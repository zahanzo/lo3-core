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

inline char *cf_buildPiece(char *buf, char line[BUF_SIZE], int index) {
	
	int max = 0; // if err, it will not break;
	for (int i = index; i < BUF_SIZE; i++) {

		if (line[i] == '\0') {
			break;
		}
		buf[max++] = line[i];
	}
	buf[max] = '\0';

	return buf;
}

void cf_fasterLabel(char line[BUF_SIZE]) {

	if (line == NULL | line[0] == '\0') {
		lo3_error("Could not execute the fasterLabel func - are you sure crocodile?", line);
		return;
	}

	// NOTICE: index: 2 because it should always skip: "@:"
	// from name
	char buf[BUF_SIZE];
	cf_buildPiece(buf, line, 2); 

	lo3_val val;
	val.chooseType = 3;
	val.value.string = buf;

	// quick hack... do never do that if you are not sure what that func does...
	// a2 is normaly doing nothing so this is ok here...
	exec_label(val, val);
}

void cf_fasterJumptoLabel(char line[255]) {
	
	if (line == NULL | line[0] == '\0') {
		lo3_error("Could not execute the fasterLabel func - are you sure crocodile?", line);
		return;
	}

	// NOTICE: index: 2 because it should always skip: "@>"
	// from name
	char buf[BUF_SIZE];
	cf_buildPiece(buf, line, 2); 
	cf_jumpToLabel(buf);
}
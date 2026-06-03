// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#include "./internal/bare-var.h"
#include "./internal/core.h"

// dev
struct lo3_var {

	unsigned char name[64];
	int type; // 0 = num, 3 = string

	lo3_value value;
};

struct lo3_varList {

	signed int index;
	lo3_var *array[256];
};
//

lo3_varList *list = NULL;

void var_init(void) {

	list = malloc(sizeof(lo3_varList));
	if (list == NULL) {
		lo3_error("List for creating an var could not be created!", "");
		return;
	}
	list->index = 0;
}

int var_find(const char *name) {

	if (list == NULL) {
		return -1;
	}
	for (int i = 0; i < list->index; i++) {
		if (strcmp(list->array[i]->name, name) == 0) {
			return i;
		}
	}
	return -1;
}

void var_create(const char *name, int type) {

	if (name == NULL || strcmp(name, "") == 0) {
		lo3_error("Name is invalid, Please check your code!", name);
		return;
	}

	if (list->index >= 256) {
		lo3_error("Max. variables reached! Please free() any var before define any "
		          "again!",
		          name);
		return;
	}

	// create new var
	lo3_var *var = malloc(sizeof(lo3_var));
	(void)memset(var, 0, sizeof(lo3_var)); // so var_free works correctly

	(void)strncpy(var->name, name, 63);
	var->name[63] = '\0';
	var->type = type;

	// save it
	list->array[list->index] = var;
	list->index++;
}

lo3_var *var_get(const char *name) {

	int index = var_find(name);
	if (index == -1) {

		lo3_error("Invalid <var> name, is this the right name?", name);
		return NULL;
	}

	return list->array[index];
}

/*
 * algo
 * designed to:
 * 1. delete on index {returnVal} (/free)
 * 2. put val index-1 in index {returnVal}
 * index-- so index can be used again.
 *
 * Why?:
 * This is designed as it is, because so it is a fast and short algo.
 * O(1) complexity !!!
 */
void var_free(const char *name) {

	signed int returnVal = var_find(name);

	if (returnVal == -1) {
		lo3_error(
		    "Could not free <var>. Is there any change that the var has an other name?",
		    name);
		return;
	}

	if (list->array[returnVal]->type == 3 && list->array[returnVal]->value.string != NULL) {
		free(list->array[returnVal]->value.string);
	}
	free(list->array[returnVal]);

	list->array[returnVal] = list->array[list->index - 1];
	list->index--;
}

void var_freeAll(void) {

	for (int i = 0; i < list->index; i++) {
		if (list->array[i]->type == 3) {
			free(list->array[i]->value.string);
		}
		free(list->array[i]);
	}

	free(list);
	list = NULL;
}

void var_setNum(const char *name, int num) {

	signed int i = var_find(name);

	if (i == -1) {
		lo3_error("<var> not found, please check if this is the right var name", name);
		return;
	}

	if (list->array[i]->type == 3) {
		lo3_error("Can not set <var> to any num, because its type is set to string!", name);
		return;
	}
	else {
		list->array[i]->value.num = num;
	}
}
void var_setString(const char *name, char *string) {

	signed int i = var_find(name);

	if (i == -1) {
		lo3_error("<var> not found, please check if this is the right var name", name);
		return;
	}

	if (list->array[i]->type == 0) {
		lo3_error("Can not set <var> to any string, because its type is set to num", name);
		return;
	}

	list->array[i]->value.string = string;
}

int var_getNum(const lo3_var *var) {
	return var->value.num;
}

const char *var_getString(const lo3_var *var) {
	return var->value.string;
}

int var_getType(const lo3_var *var) {
	return var->type;
}

lo3_value var_getValue(const lo3_var *var) {
	return var->value;
}

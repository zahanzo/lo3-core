// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.

#pragma once

typedef enum {

	TYPE_num = '$',
	TYPE_string = '_',
	TYPE_array = '*',
	TYPE_var = '%',
	TYPE_double = '/'
} lo3_types;

typedef union {
	 signed int num;
	unsigned char *string;
	double numD;
} lo3_value;

typedef struct {

	lo3_types type;
	signed int chooseType;

	lo3_value value;

} lo3_val;

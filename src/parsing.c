// Copyright (c) 2026 seesee010
// Read the License file for more informations about the license.
//
// You can find todos by reading the "todo" blocks.
// But please read the whole block not only the single todo line

#include "./internal/bare-define.h"
#include "./internal/bare-var.h"
#include "./internal/control-flow.h"
#include "./internal/core.h"
#include "./internal/global.h"
#include "./internal/specific-language.h"
#include <errno.h>
#include <limits.h>

volatile char LO3_STARTING_LINE = '#';

int currentLine = 0;
int lastLineOffset = 0;

int pars_isFileValid(char *name, FILE **file) {

	*file = fopen(name, "r");

	if (*file == NULL) {
		return -1;
	}

	size_t len = strlen(name);

	if (len < 4 || strcmp(&name[len - 4], ".lo3") != 0) {
		lo3_error("File must end with .lo3\n", name);
		(void)(fclose((*file)));
		return -1;
	}
	return 0;
}

int pars_file(FILE *file) {
    char *line = NULL;
    size_t len = 0;
    char arg1[64] = {0}, arg2[64] = {0};

    int pos0 = ftell(file);

	// todo: make getline avaible on other os
	//
	// ///// MORE INFORMATIONS /////
	// getline is not a c standardized lib and therefore should be defined somewhere else.
	// Ways to solve that problem:
	// - Create a getline MACRO and use that here
	// - use the preprocessor funcs, like ifdef check for linux and win etc

parsing:
    while (lastLineOffset = ftell(file), GETLINE(&line, &len, file)) {

        currentLine++;
        line[strcspn(line, "\r\n")] = '\0'; 

        // 0xRobert: Ignore emoty lines or comments
        if (line[0] == '\0' || (line[0] == '/' && line[1] == '/')) {
            free(line);
            line = NULL;
            len = 0;
            continue;
        }

        // 0xRobert: Treats syntax sugar
        if (line[0] == '@') {
            if (line[1] == '.') {
                int idx = 2;
                while (line[idx] == ' ' || line[idx] == '\t') idx++;
                if (line[idx] != '\0') {
                    LO3_STARTING_LINE = line[idx];
                }

                free(line);
                line = NULL;
                len = 0;
                continue; 
            } else if (line[1] == '{') {
                g_fasterInit(line);
                free(line);
                line = NULL;
                len = 0;
                continue;
            }
        }

        // 0xRobert: Verify if the line starts with a character invalid of cmd
        if (line[0] != LO3_STARTING_LINE) {
            free(line);
            line = NULL;
            len = 0;
            continue;
        }

        if (strlen(line) < 3) {
            lo3_warn("You used some kind of magic line...", "");
            free(line);
            line = NULL;
            len = 0;
            continue;
        }

        lo3_cmds cmds = (lo3_cmds)line[1];

        memset(arg1, 0, sizeof(arg1));
        memset(arg2, 0, sizeof(arg2));

        char *ptr = &line[3];
        int arg_idx = 0;
        char *args[2] = {arg1, arg2};

        while (arg_idx < 2) {
            while (*ptr == ' ' || *ptr == '\t') ptr++; 
            if (*ptr == '\0') break; 

            int j = 0;
            if (*ptr == '"') {
                args[arg_idx][j++] = TYPE_string; 
                ptr++; 
                
                while (*ptr && j < 62) {
                    if (*ptr == '\\' && *(ptr + 1) != '\0') {
                        args[arg_idx][j++] = *ptr++;
                        args[arg_idx][j++] = *ptr++;
                        continue;
                    }
                    if (*ptr == '"') {
                        ptr++; 
                        break;
                    }
                    args[arg_idx][j++] = *ptr++;
                }
                args[arg_idx][j] = '\0';
            } else {
                while (*ptr && *ptr != ' ' && *ptr != '\t' && j < 62) {
                    args[arg_idx][j++] = *ptr++;
                }
                args[arg_idx][j] = '\0';
            }
            arg_idx++;
        }

        lo3_val a1 = pars_resv(arg1);
        lo3_val a2 = pars_resv(arg2);

        signed int returnVal = pars_dispatch(cmds, a1, a2);

        if (returnVal != -1) {
            free(line);
            return returnVal;
        }

        free(line);
        line = NULL;
        len = 0;
    }

    if (rush) {
        fseek(file, pos0, SEEK_SET);
        if (!isWarped) {
            isWarped = TRUE;
            goto parsing;
        } else {
            lo3_error("Could not find the label...", "");
            return -1;
        }
    }
    return 0;
}

lo3_val pars_resv(char type[64]) {
	lo3_val result;
	result.type = type[0];
	char *end;

	int value;
	lo3_var *var;

	switch (result.type) {
	
	// 0xRobert: Treats the empty args in a safe way
	case '\0':
		result.chooseType = 0;
		result.value.num = 0;
		break;

	// find the corresponding type
	case TYPE_num:
		errno = 0;

		long val = strtol(&type[1], &end, 10);
		if (errno != 0 || end == &type[1] || val > INT_MAX || val < INT_MIN) {
			lo3_error("<num> is typeof integer!\nBut this value is not!", type);
			result.value.num = 0;
			result.chooseType = 0;
			break;
		}
		result.value.num = (int)val;
		result.chooseType = 0;
		break;

	case TYPE_array:

		// logic:
		// *X, lookup X, resolve the value as long as it is a number,
		// 0 - 9 counts as values
		//
		// not allowed: "*A"

		// *100 -> _Hello
		errno = 0;

		long idx = strtol(&type[1], &end, 10);
		if (errno != 0 || end == &type[1] || idx < 0 || idx > INT_MAX) {
			lo3_error("Invalid g[] index", type);
			break;
		}
		lo3_val value = g_get((int)idx);

		result.type = value.chooseType ? TYPE_string : TYPE_num;
		result.value = value.value;
		result.chooseType = value.chooseType ? 3 : 0;
		break;

	case TYPE_string:
		result.value.string = &type[1];
		result.chooseType = 3;
		break;

	case TYPE_var:

		// resolve var
		var = var_get(&type[1]);

		if (var == NULL) {
			lo3_error("Could not resolve var, because it was invalid! Or empty.", "");
			break;
		}

		result.type = var_getType(var) ? TYPE_string : TYPE_num;
		result.value = var_getValue(var);

		result.chooseType = var_getType(var) ? 3 : 0;

		// type: 0=num, 3=string (from ATYPE_ ... bitmasks, 1 and 2 are getting
		// resolved, so it would be useless if you can write them)

		// todo:
		// If double exists: var need to resolve this!
		break;

	case TYPE_double:

		// todo:
		// Add doubles in lo3
		//
		// ///// More Information: /////
		// Every exec_ will need other/more/etc code.
		break;
	default:

		lo3_error("Could not fild the corresponding type! …\n Please enter "
		          "something valid,"
		          "You may want to visit "
		          "https://github.com/lo3-lang/learn-the-syntax !!!",
		          type);
		break;
	}
	return result;
}

int pars_dispatch(lo3_cmds cmd, lo3_val a1, lo3_val a2) {

	if (rush) {

		switch (cmd) {
		case CNT_label:
			exec_label(a1, a2);
			break;

		default:
			break;
		}

		return -1;
	}

	switch (cmd) {

	case BSC_asn:

		exec_asn(a1, a2);
		break;

	case ALU_add:

		exec_add(a1, a2);
		break;

	case ALU_sub:

		exec_sub(a1, a2);
		break;

	case ALU_mul:

		exec_mul(a1, a2);
		break;

	case ALU_div:

		exec_div(a1, a2);
		break;

	case CNT_jmp:

		exec_jmp(a1, a2);
		break;

	case CNT_call:

		exec_call(a1, a2);
		break;

	case CNT_callS:

		exec_callS(a1, a2);
		break;

	case CNT_label:

		exec_label(a1, a2);
		break;

	case CNT_new:
		exec_new(a1, a2);
		break;

	case CNT_free:
		exec_free(a1, a2);
		break;

	case STM_out:

		exec_out(a1, a2);
		break;

	case STM_in:

		exec_in(a1, a2);
		break;

	case RET_good:
		return 0;

	case RET_bad:
		return 1;

	case CNT_cmp:

		exec_cmp(a1, a2);
		break;

	case CNT_small:
		exec_small(a1, a2);
		break;

	case CNT_big:
		exec_big(a1, a2);
		break;

	default:
		lo3_error("Unknown command!", "");
		break;
	}

	return -1;
}

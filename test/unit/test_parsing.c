// Copyright (c) 2026 seesee010
// Unit tests for src/parsing.c

#define _POSIX_C_SOURCE 200809L

#include "unity.h"
#include "../../src/internal/bare-define.h"
#include "../../src/internal/bare-var.h"
#include "../../src/internal/global.h"
#include "../../src/internal/control-flow.h"
#include "../../src/internal/specific-language.h"
#include "../../src/internal/core.h"
#include <string.h>
#include <stdio.h>

// openFile defined in main.c — stub for tests
FILE *openFile = NULL;

void setUp(void) {
    var_init();
    memset(&cf, 0, sizeof(cf));
    rush = FALSE;
    isWarped = FALSE;
}

void tearDown(void) {
    var_freeAll();
}

// --- pars_resv: TYPE_num ($) ---

void test_pars_resv_num_positive(void) {
    char arg[64] = "$42";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(0, v.chooseType);
    TEST_ASSERT_EQUAL_INT(42, v.value.num);
}

void test_pars_resv_num_zero(void) {
    char arg[64] = "$0";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(0, v.chooseType);
    TEST_ASSERT_EQUAL_INT(0, v.value.num);
}

void test_pars_resv_num_negative(void) {
    char arg[64] = "$-5";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(0, v.chooseType);
    TEST_ASSERT_EQUAL_INT(-5, v.value.num);
}

void test_pars_resv_num_large(void) {
    char arg[64] = "$1000";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(0, v.chooseType);
    TEST_ASSERT_EQUAL_INT(1000, v.value.num);
}

// --- pars_resv: TYPE_string (_) ---

void test_pars_resv_string_basic(void) {
    char arg[64] = "_hello";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(3, v.chooseType);
    TEST_ASSERT_EQUAL_STRING("hello", (const char *)v.value.string);
}

void test_pars_resv_string_single_char(void) {
    char arg[64] = "_x";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(3, v.chooseType);
    TEST_ASSERT_EQUAL_STRING("x", (const char *)v.value.string);
}

void test_pars_resv_string_points_into_buffer(void) {
    // pars_resv returns &type[1] for strings — pointer into the input buffer
    char arg[64] = "_world";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_PTR(&arg[1], v.value.string);
}

// --- pars_resv: TYPE_var (%) ---

void test_pars_resv_var_resolves_num_var(void) {
    var_create("myv", 0);
    var_setNum("myv", 77);
    char arg[64] = "%myv";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(0, v.chooseType);
    TEST_ASSERT_EQUAL_INT(77, v.value.num);
}

void test_pars_resv_var_resolves_string_var(void) {
    var_create("sv", 3);
    // strdup so var_freeAll can safely free it (var_setString stores raw pointer)
    var_setString("sv", strdup("world"));
    char arg[64] = "%sv";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(3, v.chooseType);
    TEST_ASSERT_EQUAL_STRING("world", (const char *)v.value.string);
}

void test_pars_resv_var_nonexistent_no_crash(void) {
    char arg[64] = "%ghost";
    lo3_val v = pars_resv(arg); // error printed, no crash
    (void)v;
}

// --- pars_resv: TYPE_array (*) ---

void test_pars_resv_array_resolves_num(void) {
    lo3_val gv;
    gv.type = TYPE_num;
    gv.chooseType = 0;
    gv.value.num = 55;
    g_set(5, gv);

    char arg[64] = "*5";
    lo3_val v = pars_resv(arg);
    TEST_ASSERT_EQUAL_INT(0, v.chooseType);
    TEST_ASSERT_EQUAL_INT(55, v.value.num);
}

// --- pars_isFileValid ---

void test_pars_isFileValid_valid_lo3_file(void) {
    const char *path = "/tmp/lo3_unit_test_valid.lo3";
    FILE *tmp = fopen(path, "w");
    TEST_ASSERT_NOT_NULL(tmp);
    fclose(tmp);

    FILE *f = NULL;
    int r = pars_isFileValid((char *)path, &f);
    TEST_ASSERT_EQUAL_INT(0, r);
    if (f) fclose(f);
    remove(path);
}

void test_pars_isFileValid_wrong_extension(void) {
    const char *path = "/tmp/lo3_unit_test_wrong.txt";
    FILE *tmp = fopen(path, "w");
    TEST_ASSERT_NOT_NULL(tmp);
    fclose(tmp);

    FILE *f = NULL;
    int r = pars_isFileValid((char *)path, &f);
    TEST_ASSERT_EQUAL_INT(-1, r);
    if (f) fclose(f);
    remove(path);
}

void test_pars_isFileValid_nonexistent_file(void) {
    FILE *f = NULL;
    int r = pars_isFileValid("/tmp/lo3_does_not_exist_at_all.lo3", &f);
    TEST_ASSERT_EQUAL_INT(-1, r);
}

// --- pars_dispatch ---

void test_pars_dispatch_ret_good_returns_zero(void) {
    lo3_val dummy = {0};
    // RET_good = '0' character — should return 0
    int r = pars_dispatch(RET_good, dummy, dummy);
    TEST_ASSERT_EQUAL_INT(0, r);
}

void test_pars_dispatch_ret_bad_returns_one(void) {
    lo3_val dummy = {0};
    int r = pars_dispatch(RET_bad, dummy, dummy);
    TEST_ASSERT_EQUAL_INT(1, r);
}

void test_pars_dispatch_exec_new_returns_minus1(void) {
    lo3_val a1, a2;
    unsigned char name[] = "dispatch_var";
    a1.type = TYPE_string; a1.chooseType = 3; a1.value.string = name;
    a2.type = TYPE_num;    a2.chooseType = 0; a2.value.num    = 0;
    int r = pars_dispatch(CNT_new, a1, a2);
    TEST_ASSERT_EQUAL_INT(-1, r); // exec functions return -1 via dispatch
    TEST_ASSERT_TRUE(var_find("dispatch_var") >= 0);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_pars_resv_num_positive);
    RUN_TEST(test_pars_resv_num_zero);
    RUN_TEST(test_pars_resv_num_negative);
    RUN_TEST(test_pars_resv_num_large);

    RUN_TEST(test_pars_resv_string_basic);
    RUN_TEST(test_pars_resv_string_single_char);
    RUN_TEST(test_pars_resv_string_points_into_buffer);

    RUN_TEST(test_pars_resv_var_resolves_num_var);
    RUN_TEST(test_pars_resv_var_resolves_string_var);
    RUN_TEST(test_pars_resv_var_nonexistent_no_crash);

    RUN_TEST(test_pars_resv_array_resolves_num);

    RUN_TEST(test_pars_isFileValid_valid_lo3_file);
    RUN_TEST(test_pars_isFileValid_wrong_extension);
    RUN_TEST(test_pars_isFileValid_nonexistent_file);

    RUN_TEST(test_pars_dispatch_ret_good_returns_zero);
    RUN_TEST(test_pars_dispatch_ret_bad_returns_one);
    RUN_TEST(test_pars_dispatch_exec_new_returns_minus1);

    return UNITY_END();
}

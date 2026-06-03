// Copyright (c) 2026 seesee010
// Unit tests for src/execute.c — exec_cmp, exec_small, exec_big, exec_label

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

FILE *openFile = NULL;

static lo3_val nval(int n) {
    lo3_val v;
    v.type = TYPE_num;
    v.chooseType = 0;
    v.value.num = n;
    return v;
}

static lo3_val sval(const char *s) {
    lo3_val v;
    v.type = TYPE_string;
    v.chooseType = 3;
    v.value.string = (unsigned char *)s;
    return v;
}

void setUp(void) {
    var_init();
    memset(&cf, 0, sizeof(cf));
    rush = FALSE;
    isWarped = FALSE;
}

void tearDown(void) {
    var_freeAll();
}

// ── exec_cmp ──────────────────────────────────────────────────────────────────

void test_exec_cmp_equal_sets_true(void) {
    exec_cmp(nval(5), nval(5));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_cmp_not_equal_sets_false(void) {
    exec_cmp(nval(3), nval(7));
    TEST_ASSERT_EQUAL_INT(FALSE, g_getNum(0));
}

void test_exec_cmp_zero_equal(void) {
    exec_cmp(nval(0), nval(0));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_cmp_negative_equal(void) {
    exec_cmp(nval(-4), nval(-4));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_cmp_negative_not_equal(void) {
    exec_cmp(nval(-1), nval(1));
    TEST_ASSERT_EQUAL_INT(FALSE, g_getNum(0));
}

void test_exec_cmp_string_arg_no_crash(void) {
    // lo3_error is called; result is undefined but must not crash
    exec_cmp(sval("a"), nval(0));
}

// ── exec_small ────────────────────────────────────────────────────────────────

void test_exec_small_less_than_sets_true(void) {
    exec_small(nval(2), nval(5));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_small_equal_sets_false(void) {
    exec_small(nval(5), nval(5));
    TEST_ASSERT_EQUAL_INT(FALSE, g_getNum(0));
}

void test_exec_small_greater_sets_false(void) {
    exec_small(nval(8), nval(3));
    TEST_ASSERT_EQUAL_INT(FALSE, g_getNum(0));
}

void test_exec_small_negative_less_sets_true(void) {
    exec_small(nval(-5), nval(0));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_small_string_arg_no_crash(void) {
    exec_small(sval("x"), nval(0));
}

// ── exec_big ──────────────────────────────────────────────────────────────────

void test_exec_big_greater_than_sets_true(void) {
    exec_big(nval(9), nval(3));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_big_equal_sets_false(void) {
    exec_big(nval(4), nval(4));
    TEST_ASSERT_EQUAL_INT(FALSE, g_getNum(0));
}

void test_exec_big_less_sets_false(void) {
    exec_big(nval(1), nval(10));
    TEST_ASSERT_EQUAL_INT(FALSE, g_getNum(0));
}

void test_exec_big_negative_greater_sets_true(void) {
    exec_big(nval(0), nval(-1));
    TEST_ASSERT_EQUAL_INT(TRUE, g_getNum(0));
}

void test_exec_big_string_arg_no_crash(void) {
    exec_big(nval(0), sval("y"));
}

// ── exec_label ────────────────────────────────────────────────────────────────

void test_exec_label_registers_label(void) {
    exec_label(sval("myLabel"), nval(0));
    TEST_ASSERT_TRUE(cf_findLabel("myLabel") >= 0);
}

void test_exec_label_duplicate_no_crash(void) {
    // second call triggers lo3_error but must not crash
    exec_label(sval("dupLabel"), nval(0));
    exec_label(sval("dupLabel"), nval(0));
    TEST_ASSERT_TRUE(cf_findLabel("dupLabel") >= 0);
}

void test_exec_label_numeric_name(void) {
    exec_label(nval(42), nval(0));
    TEST_ASSERT_TRUE(cf_findLabel("42") >= 0);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_exec_cmp_equal_sets_true);
    RUN_TEST(test_exec_cmp_not_equal_sets_false);
    RUN_TEST(test_exec_cmp_zero_equal);
    RUN_TEST(test_exec_cmp_negative_equal);
    RUN_TEST(test_exec_cmp_negative_not_equal);
    RUN_TEST(test_exec_cmp_string_arg_no_crash);

    RUN_TEST(test_exec_small_less_than_sets_true);
    RUN_TEST(test_exec_small_equal_sets_false);
    RUN_TEST(test_exec_small_greater_sets_false);
    RUN_TEST(test_exec_small_negative_less_sets_true);
    RUN_TEST(test_exec_small_string_arg_no_crash);

    RUN_TEST(test_exec_big_greater_than_sets_true);
    RUN_TEST(test_exec_big_equal_sets_false);
    RUN_TEST(test_exec_big_less_sets_false);
    RUN_TEST(test_exec_big_negative_greater_sets_true);
    RUN_TEST(test_exec_big_string_arg_no_crash);

    RUN_TEST(test_exec_label_registers_label);
    RUN_TEST(test_exec_label_duplicate_no_crash);
    RUN_TEST(test_exec_label_numeric_name);

    return UNITY_END();
}

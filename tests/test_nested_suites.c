#include "munit.h"

typedef struct {
    int suite_setup_calls;
    int suite_teardown_calls;
    int test_setup_calls;
    int test_teardown_calls;
    int test_calls_a;
    int test_calls_b;
} test_call_t;

static void reset_test(test_call_t *tests) {
    tests->suite_setup_calls = 0;
    tests->suite_teardown_calls = 0;
    tests->test_setup_calls = 0;
    tests->test_teardown_calls = 0;
    tests->test_calls_a = 0;
    tests->test_calls_b = 0;
}

static void assert_calls(test_call_t *tests, int suite_setup_calls, int suite_teardown_calls, int test_setup_calls,
                         int test_teardown_calls, int test_calls_a, int test_calls_b) {
    munit_assert_int(suite_setup_calls, ==, tests->suite_setup_calls);
    munit_assert_int(suite_teardown_calls, ==, tests->suite_teardown_calls);
    munit_assert_int(test_setup_calls, ==, tests->test_setup_calls);
    munit_assert_int(test_teardown_calls, ==, tests->test_teardown_calls);
    munit_assert_int(test_calls_a, ==, tests->test_calls_a);
    munit_assert_int(test_calls_b, ==, tests->test_calls_b);
}

static void* suite_setup(void* data) {
    ((test_call_t*) data)->suite_setup_calls++;
    return data;
}

static void suite_teardown(void* data) {
    ((test_call_t*) data)->suite_teardown_calls++;
}


static void* test_setup(const MunitParameter params[], void* data) {
    ((test_call_t*) data)->test_setup_calls++;
    return data;
}

static void test_teardown(void* data) {
    ((test_call_t*) data)->test_teardown_calls++;
}

static MunitResult test_func_1(const MunitParameter params[], void* data) {
    ((test_call_t*) data)->test_calls_a++;
    return MUNIT_OK;
}


static MunitResult test_func_2(const MunitParameter params[], void* data) {
    ((test_call_t*) data)->test_calls_b++;
    return MUNIT_OK;
}

static MunitTest test_a[] = {
        {"test_a1", test_func_1, test_setup, test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
        {"test_a2", test_func_2, test_setup, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {NULL, NULL, NULL, NULL, 0, MUNIT_TEST_OPTION_NONE}
};


static MunitTest test_b[] = {
        {"test_b1", test_func_1, NULL, test_teardown, MUNIT_TEST_OPTION_NONE, NULL},
        {"test_b2", test_func_2, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
        {NULL, NULL, NULL, NULL, 0, MUNIT_TEST_OPTION_NONE}
};

static MunitSuite main_suite[] = {
        {"nested_a/", test_a, suite_setup, suite_teardown, NULL, 7, MUNIT_SUITE_OPTION_NONE},
        {"nested_b/", test_b, NULL, NULL, NULL, 5, MUNIT_SUITE_OPTION_NONE},
        {NULL, NULL, NULL, NULL, NULL, 0, MUNIT_SUITE_OPTION_NONE}
};

static const MunitSuite root_suite = {
        "",
        /* No tests running for root suite needed */
        NULL,
        NULL,
        NULL,
        main_suite,
        0,
        MUNIT_SUITE_OPTION_NONE
};


int main(int argc, char *argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    //test all suites run
    char* args_all_tests[] = {
            "Program",
            "--no-fork",
            NULL
    };
    char* args_all_suite_a[] = {
            "Program",
            "--no-fork",
            "nested_a",
            NULL
    };
    char* args_all_suite_b[] = {
            "Program",
            "--no-fork",
            "nested_b",
            NULL
    };
    char* args_test_a1[] = {
            "Program",
            "--no-fork",
            "nested_a/test_a1",
            NULL
    };
    char* args_test_b2[] = {
            "Program",
            "--no-fork",
            "nested_b/test_b2",
            NULL
    };
    char* args_no_test_case[] = {
            "Program",
            "--no-fork",
            "something",
            NULL
    };

    //Test calling cases by name and suite name
    test_call_t calls;
    reset_test(&calls);
    munit_assert(munit_suite_main(&root_suite, (void *) &calls, 2, args_all_tests) == EXIT_SUCCESS);
    assert_calls(&calls, 1, 1, 14, 12, 12, 12);

    reset_test(&calls);
    munit_assert(munit_suite_main(&root_suite, (void *) &calls, 3, args_all_suite_a) == EXIT_SUCCESS);
    assert_calls(&calls, 1, 1, 14, 7, 7, 7);

    reset_test(&calls);
    munit_assert(munit_suite_main(&root_suite, (void *) &calls, 3, args_all_suite_b) == EXIT_SUCCESS);
    assert_calls(&calls, 0, 0, 0, 5, 5, 5);

    reset_test(&calls);
    munit_assert(munit_suite_main(&root_suite, (void *) &calls, 3, args_test_a1) == EXIT_SUCCESS);
    assert_calls(&calls, 1, 1, 7, 7, 7, 0);

    reset_test(&calls);
    munit_assert(munit_suite_main(&root_suite, (void *) &calls, 3, args_test_b2) == EXIT_SUCCESS);
    assert_calls(&calls, 0, 0, 0, 0, 0, 5);

    reset_test(&calls);
    munit_assert(munit_suite_main(&root_suite, (void *) &calls, 3, args_no_test_case) == EXIT_FAILURE);
    assert_calls(&calls, 0, 0, 0, 0, 0, 0);

    return EXIT_SUCCESS;
}

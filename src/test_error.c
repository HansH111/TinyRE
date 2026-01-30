/* Test program for tre_last_error and safety limits */

#include <stdio.h>
#include <string.h>
#include "tre.h"

#define OK    1
#define NOK   0

typedef struct {
    int  expect_match;     // OK = should match, NOK = should NOT match
    char *pattern;
    char *text;
    int  expected_length;  // expected match length from start position
    int  igncase;          // 0 = case-sensitive, 1 = case-insensitive
    int  expected_error;   // expected tre_last_error value
} test_t;

test_t tests[] = {
    // Normal match (OK, no error)
    { OK,  "abc",          "abc",           3,  0,  TRE_OK },

    // Normal no match (NOK, no error, but tre_last_error = TRE_ERROR_NO_MATCH)
    { NOK, "abc",          "def",           0,  0,  TRE_ERROR_NO_MATCH },

    // Pattern too long (set limit low)
    { NOK, "a very long pattern that exceeds the limit set in test", "text", 0, 0, TRE_ERROR_PATTERN_TOO_LONG },

    // Recursion depth exceeded (deep pattern, set max_depth low)
    { NOK, "a+a+a+a+a+a+a+a+a+a+a+a+a+a",  // long chain of +
           "aaaaaaaaaaaaaaa",
           0, 0, TRE_ERROR_RECURSION_DEPTH },

    // Backtrack limit exceeded (pathological pattern, set max_backtrack_steps low)
    { NOK, "a+a+a+a+b",  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",  0, 0, TRE_ERROR_BACKTRACK_LIMIT },

    // Malformed pattern (invalid {n})
    { NOK, "[0-9]{abc}",  "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
    { NOK, "[0-9]{0}",    "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
    { NOK, "[0-9]{ }",    "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
    { NOK, "[0-9]{",      "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
};

alpnbasalpnperl:/appl/shared/git/TinyRE/src# /* Test program for tre_last_error and safety limits */

#include <stdio.h>
#include <string.h>
#include "tre.h"

#define OK    1
#define NOK   0

typedef struct {
    int  expect_match;     // OK = should match, NOK = should NOT match
    char *pattern;
    char *text;
    int  expected_length;  // expected match length from start position
    int  igncase;          // 0 = case-sensitive, 1 = case-insensitive
    int  expected_error;   // expected tre_last_error value
} test_t;

test_t tests[] = {
    // Normal match (OK, no error)
    { OK,  "abc",          "abc",           3,  0,  TRE_OK },

    // Normal no match (NOK, no error, but tre_last_error = TRE_ERROR_NO_MATCH)
    { NOK, "abc",          "def",           0,  0,  TRE_ERROR_NO_MATCH },

    // Pattern too long (set limit low)
    { NOK, "a very long pattern that exceeds the limit set in test", "text", 0, 0, TRE_ERROR_PATTERN_TOO_LONG },

    // Recursion depth exceeded (deep pattern, set max_depth low)
    { NOK, "a+a+a+a+a+a+a+a+a+a+a+a+a+a",  // long chain of +
           "aaaaaaaaaaaaaaa",
           0, 0, TRE_ERROR_RECURSION_DEPTH },

    // Backtrack limit exceeded (pathological pattern, set max_backtrack_steps low)
    { NOK, "a+a+a+a+b",  "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",  0, 0, TRE_ERROR_BACKTRACK_LIMIT },

    // Malformed pattern (invalid {n})
    { NOK, "[0-9]{abc}",  "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
    { NOK, "[0-9]{0}",    "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
    { NOK, "[0-9]{ }",    "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
    { NOK, "[0-9]{",      "123",            0, 0, TRE_ERROR_MALFORMED_PATTERN },
};

int main(void) {
    size_t total = sizeof(tests) / sizeof(tests[0]);
    size_t passed = 0;

    printf("Running %zu test cases for tre_last_error...\n\n", total);

    for (size_t i = 0; i < total; i++) {
        test_t *t = &tests[i];
        int length = -1;

        // Set tiny limits to trigger errors
        tre_max_depth = 20;              // tiny for deep recursion
        if (i == 3) tre_max_depth = 5;
        tre_max_backtrack_steps = 512;   // tiny for backtracking
        tre_max_pattern_length = 50;

        char *result = match(t->pattern, t->text, &length, t->igncase, 1);

        int matched = (result != NULL);
        int len_ok  = (matched && length == t->expected_length) || (!matched && t->expected_length == 0);
        int err_ok  = (tre_last_error == t->expected_error);

        if (matched == t->expect_match && len_ok && err_ok) {
            printf("[PASS] %3zu  %-28s  -  \"%s\"  len=%d  err=%d\n",
                   i+1, t->pattern, t->text, length, tre_last_error);
            passed++;
        } else {
            printf("[FAIL] %3zu  %-28s  -  \"%s\"\n",
                   i+1, t->pattern, t->text);
            printf("       Expected: %s match, len=%d, err=%d\n",
                   t->expect_match ? "YES" : "NO ", t->expected_length, t->expected_error);
            printf("       Got:      %s match, len=%d, err=%d\n",
                   matched ? "YES" : "NO ", length, tre_last_error);
            printf("\n");
        }
    }

    printf("\n%zu / %zu tests passed (%.1f%%)\n", passed, total, (double)passed / total * 100);

    return (passed == total) ? 0 : 1;
}

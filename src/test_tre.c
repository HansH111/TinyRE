/* Comprehensive test suite for the TinyRE engine */

#include <stdio.h>
#include <string.h>
#include "tre.h"   // regex API

#define OK    1
#define NOK   0

typedef struct {
    int  expect_match;     // OK = should match, NOK = should NOT match
    char *pattern;
    char *text;
    int  expected_length;  // expected match length from start position
    int  igncase;          // 0 = case-sensitive, 1 = case-insensitive
} test_t;

test_t tests[] = {
    // ───────────────────────────────────────────────
    // 1. Basic literal matching
    // ───────────────────────────────────────────────
    { OK,  "abc",          "abc",           3,  0 },
    { OK,  "abc",          "xabcy",         3,  0 },
    { NOK, "abc",          "abC",           0,  0 },
    { OK,  "abc",          "abC",           3,  1 },   // case-insensitive

{ OK,  "a*$",          "xxxaaa",       3,  0 },   // matches trailing a's
{ NOK, "a*$",          "xxx",          0,  0 },   // matches empty at end
{ OK,  ".*$",          "hello",        5,  0 },   // greedy .* consumes whole string
{ OK,  "^$",           "",             0,  0 },   // empty string matches ^$
{ NOK, "^$",           "x",            0,  0 },   // non-empty does not match ^$

    // ───────────────────────────────────────────────
    // 2. Dot (.)
    // ───────────────────────────────────────────────
    { OK,  "a.c",          "abc",           3,  0 },
    { OK,  "a.c",          "aXc",           3,  0 },
    { NOK, "a.c",          "ac",            0,  0 },
    { OK,  "...",          "xyz",           3,  0 },
    { OK,  "a.b.c",        "a1b2c",         5,  0 },

    // ───────────────────────────────────────────────
    // 3. * (zero or more)
    // ───────────────────────────────────────────────
    { OK,  "a*",           "aaa",           3,  0 },
    { NOK,  "a*",          "",              0,  0 },
    { NOK,  "a*",          "bbb",           0,  0 },
    { OK,  ".*",           "anything!",     9,  0 },
    { OK,  "a.*b",         "axxxb",         5,  0 },
    { OK,  "a.*b",         "ab",            2,  0 },
    { OK,  "a.*b",         "aXbYb",         5,  0 },   // greedy → longest match

    // ───────────────────────────────────────────────
    // 4. + (one or more)
    // ───────────────────────────────────────────────
    { OK,  "a+",           "aaa",           3,  0 },
    { NOK, "a+",           "",              0,  0 },
    { NOK, "a+",           "bbb",           0,  0 },
    { OK,  "a.+b",         "axxxb",         5,  0 },
    { NOK, "a.+b",         "ab",            0,  0 },

    // ───────────────────────────────────────────────
    // 5. ? (zero or one)
    // ───────────────────────────────────────────────
    { NOK, "colou?r",      "color",         0,  0 },
    { OK,  "colou?r",      "colour",        6,  0 },
    { NOK, "ab?c",         "ac",            0,  0 },
    { OK,  "ab?c",         "abc",           3,  0 },
    { NOK, "ab?c",         "abbc",          0,  0 },

    // ───────────────────────────────────────────────
    // 6. Character classes [ ]
    // ───────────────────────────────────────────────
    { OK,  "[abc]",        "a",             1,  0 },
    { OK,  "[ABC]",        "b",             1,  1 },   // case insensitive
    { NOK, "[abc]",        "d",             0,  0 },
    { OK,  "[a-z]",        "k",             1,  0 },
    { OK,  "[A-Z0-9]",     "5",             1,  0 },
    { OK,  "[^0-9]",       "x",             1,  0 },
    { NOK, "[^0-9]",       "7",             0,  0 },
    { OK,  "[a-zA-Z]",     "Z",             1,  0 },
    { OK,  "x[0-9]+z",     "x0042z",        6,  0 },

    // ───────────────────────────────────────────────
    // 7. Anchors ^ and $
    // ───────────────────────────────────────────────
    { OK,  "^abc",         "abc",           3,  0 },
    { NOK, "^abc",         "xabc",          0,  0 },
    { OK,  "abc$",         "abc",           3,  0 },
    { NOK, "abc$",         "abcd",          0,  0 },
    { OK,  "^[0-9]+$",     "42",            2,  0 },
    { NOK, "^[0-9]+$",     "42x",           0,  0 },
    { OK,  ".*end$",       "prefix end",    10, 0 },

    // ───────────────────────────────────────────────
    // 8. Escaping special characters
    // ───────────────────────────────────────────────
    { OK,  "a\\.b",        "a.b",           3,  0 },
    { NOK, "a\\.b",        "axb",           0,  0 },
    { OK,  "x\\*y",        "x*y",           3,  0 },
    { OK,  "file\\.txt$",  "file.txt",      8,  0 },
    { OK,  "\\^important", "^important",    10, 0 },
    { OK,  "price:\\$[0-9]+", "price:$42",  9,  0 },
    { OK,  "a\\+b",        "a+b",           3,  0 },
    { OK,  "\\\\",         "\\",            1,  0 },

    // ───────────────────────────────────────────────
    // 9. Combinations & realistic patterns
    // ───────────────────────────────────────────────
    { OK,  "[a-z]+\\.[a-z]+",   "document.pdf",  12, 0 },
    { OK,  "^[0-9]{3}-[0-9]{3}-[0-9]{4}$", "123-456-7890", 12, 0 },
    { OK,  "^https?://[^/]+/",  "https://example.com/", 20, 0 },
    { OK,  "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z.]+",  "user.name@company.co.uk", 23, 0 },
    { OK,  "\\[[A-Z]+\\]",     "[ERROR]",       7,  0 },

    // ───────────────────────────────────────────────
    // 10. Edge cases & failures
    // ───────────────────────────────────────────────
    { OK,  "",             "",              0,  0 },    // empty pattern matches
    { OK,  "",             "anything",      0,  0 },
    { NOK, "a",            "",              0,  0 },
    { OK,  "a*$",          "aaa",           3,  0 },
    { NOK, "a*$",          "",              0,  0 },
    { NOK, "[a-z]+$",      "hello!",        0,  0 },
};

int main(void) {
    size_t total = sizeof(tests) / sizeof(tests[0]);
    size_t passed = 0;

    printf("Running %zu test cases...\n\n", total);

    for (size_t i = 0; i < total; i++) {
        test_t *t = &tests[i];
        int length = -1;

        char *result = match(t->pattern, t->text, &length, t->igncase, 1);

        int matched = (result != NULL);
        int len_ok  = (matched && length == t->expected_length) || (!matched && t->expected_length == 0);

        if (matched == t->expect_match && len_ok) {
            printf("[PASS] %3zu  %-28s  -  \"%s\"  len=%d\n",
                   i+1, t->pattern, t->text, length);
            passed++;
        } else {
            printf("[FAIL] %3zu  %-28s  -  \"%s\"\n",
                   i+1, t->pattern, t->text);
            printf("       Expected: %s match, length = %d\n",
                   t->expect_match ? "YES" : "NO ", t->expected_length);
            printf("       Got:      %s match, length = %d  (at %p)\n",
                   matched ? "YES" : "NO ", length, (void*)result);
            if (tre_last_error > 1) {
               if (tre_last_error == TRE_ERROR_BACKTRACK_LIMIT) {
                   printf("Regex aborted - backtrack limit reached\n");
               } else if (tre_last_error == TRE_ERROR_RECURSION_DEPTH) {
                   printf("Regex aborted - recursion too deep\n");
               } else if (tre_last_error == TRE_ERROR_MALFORMED_PATTERN) {
                   printf("Regex aborted - malformed pattern\n");
               } else {
                   printf("Regex aborted - error %d\n",tre_last_error);
               }
            }
            printf("\n");
        }
    }
    printf("\n%zu / %zu tests passed (%.1f%%)\n", passed, total, (double)passed / total * 100);
    return (passed == total) ? 0 : 1;
}

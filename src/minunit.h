#ifndef __MINUNIT_H__
#define __MINUNIT_H__

#include <signal.h>


#define mu_assert(test, message) do { if (!(test)) {printf ("(%s:%d): %s\n", __FILE__, __LINE__, message); raise (SIGABRT); exit (-1); } } while (0)
#define mu_run_test(test) do { test(); tests_run++; } while (0)
#define START_TEST(test) static char *test (void) {
#define END_TEST return 0;}

extern int tests_run;

#endif


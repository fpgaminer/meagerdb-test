#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <meagerdb/meagerdb.h>
#include <meagerdb/keyvalue.h>
#include "minunit.h"


int tests_run = 0;


START_TEST (test1)
{
	MDB db = {0};

	unlink ("test.mdb");

	mu_assert (mdb_create (&db, "test.mdb", (uint8_t const *)"foo", 3, 10) == 0, "mdb_create should succeed.");
	mu_assert (mdb_create (&db, "test.mdb", (uint8_t const *)"foo", 3, 10) == MDBE_OPEN, "mdb_create should fail with MDBE_OPEN.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"fo", 2) == MDBE_BAD_PASSWORD, "mdb_open should fail with MDBE_BAD_PASSWORD.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foof", 4) == MDBE_BAD_PASSWORD, "mdb_open should fail with MDBE_BAD_PASSWORD.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foo", 3) == 0, "mdb_open should succeed.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foo", 3) == MDBE_ALREADY_OPEN, "mdb_open should fail with MDBE_ALREADY_OPEN.");
}
END_TEST


static char *all_tests (void)
{
	mu_run_test (test1);

	return 0;
}


int main (void)
{
	char *result = all_tests ();

	if (result)
		printf ("%s\n", result);
	else
		printf ("ALL TESTS PASSED\n");
	printf ("Tests run: %d\n", tests_run);

	return result != 0;
}

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
	uint8_t tmp[8];
	uint32_t rowid, rowid2;

	unlink ("test.mdb");

	/* Create and Open */
	mu_assert (mdb_create (&db, "test.mdb", (uint8_t const *)"foo", 3, 10) == 0, "mdb_create should succeed.");
	mu_assert (mdb_create (&db, "test.mdb", (uint8_t const *)"foo", 3, 10) == MDBE_OPEN, "mdb_create should fail with MDBE_OPEN.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"fo", 2) == MDBE_BAD_PASSWORD, "mdb_open should fail with MDBE_BAD_PASSWORD.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foof", 4) == MDBE_BAD_PASSWORD, "mdb_open should fail with MDBE_BAD_PASSWORD.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foo", 3) == 0, "mdb_open should succeed.");
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foo", 3) == MDBE_ALREADY_OPEN, "mdb_open should fail with MDBE_ALREADY_OPEN.");

	/* Empty table tests */
	mu_assert (mdb_walk (&db, 0, true) == 1, "mdb_walk should return 1 on an empty table.");
	mu_assert (mdb_walk (&db, 1, true) == 1, "mdb_walk should return 1 on an empty table.");
	mu_assert (mdb_select_by_rowid (&db, 0, 0) == MDBE_ROW_NOT_FOUND, "mdb_select_by_rowid should return MDBE_ROW_NOT_FOUND on an empty table.");
	mu_assert (mdb_select_by_rowid (&db, 1, 0) == MDBE_ROW_NOT_FOUND, "mdb_select_by_rowid should return MDBE_ROW_NOT_FOUND on an empty table.");
	mu_assert (mdb_select_by_rowid (&db, 1, 1) == MDBE_ROW_NOT_FOUND, "mdb_select_by_rowid should return MDBE_ROW_NOT_FOUND on an empty table.");
	mu_assert (mdb_select_by_page (&db, 0) == -1, "mdb_select_by_page should return -1 on an empty table.");
	mu_assert (mdb_select_by_page (&db, 1) == -1, "mdb_select_by_page should return MDBE_ROW_NOT_FOUND on an empty table.");
	mu_assert (mdb_select_by_page (&db, 2) == -1, "mdb_select_by_page should return MDBE_ROW_NOT_FOUND on an empty table.");

	/* No selection */
	mu_assert (mdb_get_value (&db, tmp, sizeof (tmp)) == MDBE_NO_ROW_SELECTED, "mdb_get_value should return MDBE_NO_ROW_SELECTED if no row is selected.");

	mu_assert (mdb_read_value (&db, tmp, 0, sizeof (tmp)) == MDBE_NO_ROW_SELECTED, "mdb_read_value should return MDBE_NO_ROW_SELECTED if no row is selected.");

	mu_assert (mdb_get_rowid (&db, NULL, NULL, NULL) == MDBE_NO_ROW_SELECTED, "mdb_get_rowid should return MDBE_NO_ROW_SELECTED if no row is selected.");

	mu_assert (mdb_update (&db, tmp, sizeof (tmp)) == MDBE_NO_ROW_SELECTED, "mdb_update should return MDBE_NO_ROW_SELECTED if no row is selected.");

	mu_assert (mdb_delete (&db) == MDBE_NO_ROW_SELECTED, "mdb_delete should return MDBE_NO_ROW_SELECTED if no row is selected.");

	/* Insert */
	for (size_t i = 0; i < sizeof (tmp); ++i)
		tmp[i] = i;
	mu_assert (mdb_insert (&db, 0, tmp, sizeof (tmp)) == 0, "mdb_insert should succeed.");
	mu_assert (mdb_get_rowid (&db, NULL, NULL, &rowid) == 0, "mdb_get_rowid should succeed.");
	mu_assert (mdb_get_value (&db, NULL, 0) == sizeof (tmp), "mdb_get_value should return the previously inserted length.");
	memset (tmp, 0, sizeof (tmp));
	mu_assert (mdb_get_value (&db, tmp, sizeof (tmp)) == sizeof (tmp), "mdb_get_value should return the previously inserted length.");
	for (size_t i = 0; i < sizeof (tmp); ++i)
	{
		mu_assert (tmp[i] == i, "mdb_get_value should return the previously inserted length.");
	}

	mu_assert (mdb_walk (&db, 0, true) == 0, "mdb_walk should succeed.");
	mu_assert (mdb_get_value (&db, NULL, 0) == sizeof (tmp), "mdb_get_value should return the previously inserted length.");
	mu_assert (mdb_walk (&db, 0, false) == 1, "mdb_walk should return 1.");
	mu_assert (mdb_walk (&db, 0, true) == 0, "mdb_walk should succeed.");

	/* Update */
	mu_assert (mdb_update (&db, "nica", 4) == 0, "mdb_update should succeed.");
	mu_assert (mdb_get_value (&db, tmp, sizeof (tmp)) == 4, "mdb_get_value should return 4.");
	mu_assert (memcmp (tmp, "nica", 4) == 0, "mdb_get_value should return the updated value.");
	mu_assert (mdb_get_rowid (&db, NULL, NULL, &rowid2) == 0, "mdb_get_rowid should succeed.");
	mu_assert (rowid == rowid2, "mdb_update should not change rowid.");
	mu_assert (mdb_select_by_rowid (&db, 0, rowid) == 0, "mdb_select_by_rowid should succeed");

	/* Delete */
	mu_assert (mdb_delete (&db) == 0, "mdb_delete should succeed.");
	mu_assert (mdb_select_by_rowid (&db, 0, rowid) == MDBE_ROW_NOT_FOUND, "mdb_delete should remove the row.");
	mu_assert (mdb_walk (&db, 0, true) == 1, "mdb_delete should remove the row.");

	/* Close */
	mdb_close (&db);
	mu_assert (mdb_open (&db, "test.mdb", (uint8_t const *)"foo", 3) == 0, "mdb_open should succeed.");

	mdb_close (&db);
	mu_assert (mdb_walk (&db, 0, true) == MDBE_NOT_OPEN, "mdb_walk should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_select_by_rowid (&db, 0, 0) == MDBE_NOT_OPEN, "mdb_select_by_rowid should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_select_by_page (&db, 0) == MDBE_NOT_OPEN, "mdb_select_by_page should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_get_value (&db, NULL, 0) == MDBE_NOT_OPEN, "mdb_get_value should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_read_value (&db, NULL, 0, 0) == MDBE_NOT_OPEN, "mdb_read_value should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_get_rowid (&db, NULL, NULL, NULL) == MDBE_NOT_OPEN, "mdb_get_rowid should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_get_next_rowid (&db, 0, &rowid) == MDBE_NOT_OPEN, "mdb_get_next_rowid should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_insert (&db, 0, tmp, 0) == MDBE_NOT_OPEN, "mdb_insert should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_insert_begin (&db, 0, 0) == MDBE_NOT_OPEN, "mdb_insert_begin should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_insert_continue (&db, NULL, 0) == MDBE_NOT_OPEN, "mdb_insert_continue should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_insert_finalize (&db) == MDBE_NOT_OPEN, "mdb_insert_finalize should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_update (&db, NULL, 0) == MDBE_NOT_OPEN, "mdb_update should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_update_begin (&db, 0) == MDBE_NOT_OPEN, "mdb_update_begin should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_update_continue (&db, NULL, 0) == MDBE_NOT_OPEN, "mdb_update_continue should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_update_finalize (&db) == MDBE_NOT_OPEN, "mdb_update_finalize should return MDBE_NOT_OPEN after the database is closed.");
	mu_assert (mdb_delete (&db) == MDBE_NOT_OPEN, "mdb_delete should return MDBE_NOT_OPEN after the database is closed.");

	/* TODO: Test _begin,_continue,_finalize functions. */
}
END_TEST


int main (void)
{
	mu_run_test (test1);

	printf ("ALL TESTS PASSED\n");
	printf ("Tests run: %d\n", tests_run);

	return 0;
}

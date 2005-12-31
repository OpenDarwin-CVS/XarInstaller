/*
 * Copyright (c) 2005 Ole Guldberg Jensen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of the contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * 12-Dec-2005
 * Ole Guldberg Jensen <olegb@opendarwin.org>
 */

/*
 * Database handling for xarinstall
 */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>

/* Dependency headers */
#include <sqlite3.h>

sqlite3 *db;

/* Initially add xarchive to main-table and create xarchive table */
/* Returns the number of the index or a negative number if failure */
int 
initdb(const char* xarch)
{
	int returnvalue;
	char *db_error_msg;
	int xarchnumber = 0; /* Number to assign to xarchive */

	/* Open the xi database */
	returnvalue = sqlite3_open("/var/db/xi.db", &db);
	if (returnvalue)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Create maintable XXX this might cause grief XXX */
	char sqlstring[512];
	snprintf(
		sqlstring, 512, 
		"CREATE TABLE maintable (key INTEGER PRIMARY KEY, xarchname TEXT);"
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);

	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
	}

	/* Add xarch to maintable */
	snprintf(
		sqlstring, 512, 
		"INSERT INTO maintable (xarchname) VALUES ('%s');",
		xarch
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);

	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
	}

	/* xarchnumber = keynumber; */
	xarchnumber = sqlite3_last_insert_rowid(db);
	printf("Inserted %s into maintable at index %d\n", xarch, xarchnumber);

	/* Create xarch table */
	snprintf(
		sqlstring, 512,
		"CREATE TABLE %s-%d (key INTEGER PRIMARY KEY, filename TEXT, checksum);",
		xarch,
		xarchnumber
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);

	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
	}

	/* Close up the database */
	sqlite3_close(db);
	
	/* Return the xarchnumber */
	return xarchnumber;
}

/* Adds and entry to the DB */
int 
add_entry_to_db(const char* filename, const unsigned char* checksum)
{
	int returnvalue;
	char *db_error_msg;
	
	/* Open the xi database */
	returnvalue = sqlite3_open("/var/db/xi.db", &db);
	if (returnvalue == 0)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Execute SQL statement on DB */
	/* sqlite3_exec(db, "SQL STATEMENT", callback, NULL, &errmsg) */
	/* Add file and checksum to xarchive table */
	returnvalue = sqlite3_exec(db, "SQLSTATEMENT", NULL, NULL, &db_error_msg);
	if (returnvalue != SQLITE_OK)
	{
		fprintf(stderr, "Error executing SQL statement on xidb: %s\n", db_error_msg);
	}

	/* Close up and return */
	sqlite3_close(db);
	return 0;
}

/* Removes an xarchive from the DB */
int
remove_xarchive_from_db(const char* xarch)
{
	return 0;
}

/* Returns a list of installed xarchives */
char**
list_installed_xarchives(void)
{
	return NULL;
}

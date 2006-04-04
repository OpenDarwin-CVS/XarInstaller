/*
 * $Id$
 * 
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
#include <unistd.h>

/* Dependency headers */
#include <sqlite3.h>

sqlite3 *db;
const char *dbpath = "/Users/olegb/xi.db";

/* Initially add xarchive to main-table and create xarchive table */
/* Returns the number of the index or a negative number if failure */
int 
initdb(const char* xarch)
{
	int returnvalue;
	char *db_error_msg;
	int xarchnumber = 0; /* Number to assign to xarchive */

	/* Check if we are able to write to the DB - if not fail */
	/* XXX This will fail if the xi.db doesnt exist XXX */
	returnvalue = access(dbpath, W_OK);
	if (returnvalue == -1)
	{
		fprintf(stderr, "FAILURE: Couldnt read and/or write to xidb\n");
		exit(1);
	}

	/* Open the xi database */
	returnvalue = sqlite3_open(dbpath, &db);
	if (returnvalue)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Create maintable XXX this will fail if the table allready exists XXX */
	char sqlstring[512];
	snprintf(
		sqlstring, 512, 
		"CREATE TABLE maintable (key INTEGER PRIMARY KEY, xarchname TEXT);"
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);

	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
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
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
	}

	/* xarchnumber = keynumber; */
	xarchnumber = sqlite3_last_insert_rowid(db);
	printf("Inserted %s into maintable at index %d\n", xarch, xarchnumber);

	/* Create xarch table */
	snprintf(
		sqlstring, 512,
		"CREATE TABLE %s_%d (key INTEGER PRIMARY KEY, filename TEXT, checksum);",
		xarch,
		xarchnumber
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);

	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
	}

	/* Close up the database */
	sqlite3_close(db);
	
	/* Return the xarchnumber */
	return xarchnumber;
}

/* Adds and entry to the DB */
int 
add_entry_to_db(const char *xarch, const char *filename, const unsigned char* checksum)
{
	int returnvalue;
	char *db_error_msg;

	char sqlstring[512];
	
	/* Open the xi database */
	returnvalue = sqlite3_open(dbpath, &db);
	if (returnvalue)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Add file and checksum to xarchive table */
	snprintf(
		sqlstring, 512,
		"INSERT INTO %s (filename, checksum) VALUES ('%s','%s');",
		xarch,
		filename,
		checksum
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);
	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
	}

	/* Close up and return */
	sqlite3_close(db);
	return 0;
}

/* Removes an xarchive from the DB */
int
remove_xarchive_from_db(const char* xarch)
{
	int returnvalue;
	char *db_error_msg;

	char sqlstring[512];
	
	/* Open the xi database */
	returnvalue = sqlite3_open(dbpath, &db);
	if (returnvalue)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Add file and checksum to xarchive table */
	snprintf(
		sqlstring, 512,
		""
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);
	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
	}

	/* Close up and return */
	sqlite3_close(db);
	return 0;
}

/* Callback function */
char*
callback_list_installed_xarchives(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	for (i = 0; i < argc; i++)
	{
		printf("%s %s\n", argv[i], azColName[i] );
	}
	return 0;
}

/* Returns installed xarchives */
char*
list_installed_xarchives(void)
{
	int returnvalue;
	char *db_error_msg;

	char sqlstring[512];
	
	/* Open the xi database */
	returnvalue = sqlite3_open(dbpath, &db);
	if (returnvalue)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Get installed xarchives */
	snprintf(
		sqlstring, 512,
		"SELECT * from maintable;"
	);

	returnvalue = sqlite3_exec(db, sqlstring, callback_list_installed_xarchives, NULL, &db_error_msg);
	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
	}

	/* Close up and return */
	sqlite3_close(db);
	return NULL;
}

/* Returns the files installed in a xarchive */
char*
list_installed_in_xarchive(const char *xarch)
{
	int returnvalue;
	char *db_error_msg;

	char sqlstring[512];
	
	/* Open the xi database */
	returnvalue = sqlite3_open(dbpath, &db);
	if (returnvalue)
	{
		fprintf(stderr, "Could not open the xi database: %s.\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		exit(1);
	}

	/* Add file and checksum to xarchive table */
	snprintf(
		sqlstring, 512,
		""
	);

	returnvalue = sqlite3_exec(db, sqlstring, NULL, NULL, &db_error_msg);
	if (returnvalue != SQLITE_OK)
	{
		/* Something went wrong */
		fprintf(stderr, "SQL statement: %s FAILED\nREASON: %s\n", sqlstring, db_error_msg);
	}

	/* Close up and return */
	sqlite3_close(db);
	return NULL;
}

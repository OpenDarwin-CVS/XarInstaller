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
 * Ole Guldberg Jensen <olegb@@opendarwin.org>
 */

/* Standard headers */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>

/* Dependency headers */
#include <xar/xar.h>
#include <openssl/evp.h>

/* Own headers */
#include <xidb.h>

/* BUFFERSIZE when reading the file for digesting */
#define BUFFERSIZE 1024*16

unsigned char checksum[EVP_MAX_MD_SIZE];

/* Strips the path from a filename */
char *
strip_path(const char *string)
{
	char *tmp1, *tmp2;

	if( (tmp1 = strdup(string)) == NULL)
	{
		fprintf(stderr, "Could not duplicate string?!\n");
		exit(1);
	}

	/* Get the filename */
	char *tmp3; 
	while ( (tmp2 = strsep(&tmp1, "/"))  != NULL)
	{
		tmp3 = tmp2;
	}
	tmp1 = tmp3;

	/* Free() tmp2 and tmp3 we don't need them anymore */
	free(tmp2); free(tmp3);

	char *helper;
	while ( (helper = strchr(tmp1, '.')) != NULL)
	{
		/* Replace '.' with '+' */
		*helper = '_';
		
	}

	while ( (helper = strchr(tmp1, '-')) != NULL)
	{
		/* Replace '-' with '_' */
		*helper = '_';
	} 

	return tmp1;
}

/* Returns the ripemd digest of a file */
unsigned char*
get_ripemd_digest(const char* filename)
{
	/* Initialize the RIPEMD160 structures */
	const EVP_MD *md = EVP_ripemd160();
	if (md == NULL)
	{
		fprintf(stderr, "ERROR: OpenSSL doesnt understand RIPEMD160.\n");
		exit(1);
	}

	EVP_MD_CTX ctx;
	EVP_MD_CTX_init(&ctx);

	EVP_DigestInit(&ctx, md);

	unsigned int digestlength = 0;
	
	/* Get file descriptor */
	int fd;
	fd = open(filename, O_RDONLY);

	if (fd == -1)
	{
		fprintf(stderr, "Could not open %s to create ripemd160 checksum\n", filename);
	}

	/* Check if file is a directory */
	struct stat sb;
	if (fstat(fd, &sb) == -1)
	{
		fprintf(stderr, "fstat() failed!\n");
		exit(1);
	}

	/* The file is a directory .. chicken out */
	if (S_ISDIR(sb.st_mode))
	{
		return NULL;
	}

	unsigned char buffer[BUFFERSIZE];
	size_t i = 0; 
	
	do 
	{
		i = read(fd, buffer, BUFFERSIZE); 
		EVP_DigestUpdate(&ctx, buffer, i);
	}
	while (i > 0);

	EVP_DigestFinal(&ctx, checksum, &digestlength);

	EVP_MD_CTX_cleanup(&ctx);

	if (close(fd) == -1) 
	{
		fprintf(stderr, "Could not close file descriptor %d for file %s\n", fd, filename);
	}

	return NULL;
}

/* Adds a xarchive from a filename */
int
add_xarchive_with_name(const char* xarchname)
{
	/* Init database for writing entries */
	if (initdb(strip_path(xarchname) ) < 0)
	{
		fprintf(stderr, "Could not initialize xidb!!\n");
	}

	xar_t xarch;
	xar_iter_t xarch_i;
	xar_file_t xarch_f;

	xarch = xar_open(xarchname, READ);
	if (xarch == NULL )
	{
		fprintf(stderr, "Could not open xarchive: %s\n", xarchname);
		exit(1);
	}

	/* Get an xar_iter from the open xarchive */
	xarch_i = xar_iter_new(xarch);

	if (xarch_i == NULL)
	{
		fprintf(stderr, "Couldn't create xar iterator!\n");
		exit(1);
	}

	/* Loop while there are any files in the xarchive */
	for (xarch_f = xar_file_first(xarch, xarch_i); xarch_f ; xarch_f = xar_file_next(xarch_i) )
	{
		/* Process xarch_f */
		printf("Processing %s\n", xar_get_path(xarch_f));

		/* Extract the file */
		xar_extract(xarch, xarch_f);

		/* Get ripemd160 digest of file */
		get_ripemd_digest(xar_get_path(xarch_f)); 

		int i;
		printf("Digest recieved: ");
		for (i = 0; i < strlen(checksum); i++) 
		{
			/* Print checksum */
			printf("%02x", checksum[i]);
		}
		printf("\n");

		/* Clear checksum */
		for (i = 0; i < strlen(checksum); i++)
		{
			checksum[i] = 0;
		}

		/* Add the file to the Database */
		

	}

	/* Close up and return */
	xar_iter_free(xarch_i);

	if (xar_close(xarch) != 0)
	{
		fprintf(stderr, "Problem close xarchive %s\n", xarchname);
	}

	return 0;
}

int
remove_xarchive_with_name(const char* xarchivename)
{
	return 0;
}

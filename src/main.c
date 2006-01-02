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

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <xiarchive.h>

char *
str_strip(const char * string)
{
	char *tmp;

	if( (tmp = strdup(string)) == NULL)
	{
		fprintf(stderr, "Could not duplicate string?!\n");
		exit(1);
	}

	int i;
	for (i = 0; i <= strlen(&tmp); i++)
	{
		/* Remove "." */
		/* Remove "-" */
	}

	return tmp;
}

void 
print_usage(const char* progname)
{
	fprintf(stderr, "Usage: %s -arlc xarchive\n", progname);
	fprintf(stderr, "\t-a <xarball>\n");
	fprintf(stderr, "\t-r <xarchivename>\n");
	fprintf(stderr, "\t-l\n");
	fprintf(stderr, "\t-c <xarchivename>\n");
	fprintf(stderr, "\n");
}

int 
main (int argc, char *argv[]) 
{
	/* getopt parsing */
	int c; 

	if (argc < 2)
	{
		print_usage(argv[0]);
		exit(1);
	}

	while ( (c = getopt(argc, argv, "a:r:l")) != -1 )
	{
		switch (c)
		{

			case 'a':
				fprintf(stdout, 
						"You are trying to add the archive: %s\n",
						optarg);

				char* filename_stripped;

				filename_stripped = str_strip(optarg);

				add_xarchive_with_name(filename_stripped);

				break;

			case 'r':
				fprintf(stdout, 
						"You are trying to remove the archive: %s\n",
						optarg);
				break;

			case 'l':
				fprintf(stdout, "You are trying to list archives\n");
				break;

			case '?':
			default:
				print_usage(argv[0]);
				exit(1);
		}
	}

	argc -= optind;	
	argv += optind;

	return(0);
}

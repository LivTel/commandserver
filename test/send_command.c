/**
 * Test program to send a command using Command_Server.
 */

/**
 * This hash define is needed before including source files give us
 * POSIX.4/IEEE1003.1b-1993 prototypes for time.
 */
#define _POSIX_SOURCE 1

/**
 * This hash define is needed before including source files give us
 * POSIX.4/IEEE1003.1b-1993 prototypes for time.
 */
#define _POSIX_C_SOURCE 199309L

/**
 * This enables the 'strdup' prototype in 'string.h', which is not enabled in
 * POSIX.
 */
#define _GNU_SOURCE    1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "command_server.h"

static int Output_To_File(char *filename,char *string);
static void help(void);

/**
 * Main program.
 * @param argc The number of arguments.
 * @param argv The array of strings.
 * @return Main returns 0 on success and non-zero on failure.
 */
int main(int argc, char* argv[])
{
	int c;
	extern char *optarg;
	int port;     
	char *hostname; 
	char *command_string = NULL;
	char *output_filename = NULL;
	Command_Server_Handle_T handle;
	char *reply_string;
	int retval;

	port = -1;
	hostname = "localhost";
	while ((c = getopt(argc, argv, "h:p:c:f:")) != EOF)
	{
		switch(c)
		{
			case 'c':
				command_string = strdup(optarg);
				break;
			case 'f':
				output_filename = strdup(optarg);
				break;
			case 'h':
				hostname = strdup(optarg);
				break;
			case 'p':
				if (c == 'p')
					port = atoi(optarg);
				break;
			default:
				printf("unknown flag -%c\n", (char)c);
				help();
				return 1;
		}
	}

	if (port <= 0)
	{
		printf("provide a portnumber!\n");
		help();
		return 2;
	}

	/* Establish a TCP connection */
	fprintf(stderr,"client: trying to connect to %s:%d\n", hostname, port);
	retval = Command_Server_Open_Client(hostname, port, &handle);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 4;
	}
	fprintf(stderr,"client: about to send '%s' to server\n", command_string);
	retval = Command_Server_Write_Message(handle, command_string);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 5;
	}
	fprintf(stderr,"client: sent '%s' to server\n", command_string);

	fprintf(stderr,"client: about to get reply...\n");
	retval = Command_Server_Read_Message(handle, &reply_string);
	if(retval == FALSE)
	{
		Command_Server_Error();
		return 6;
	}
	fprintf(stderr,"client: reply: %s\n", reply_string);
	fprintf(stdout,"%s\n", reply_string);
	/* close and quit*/
	retval = Command_Server_Close_Client(&handle);
	if(retval == FALSE)
	{
		Command_Server_Error();
		free(reply_string);
		return 7;
	}
	/* output to file */
	if(output_filename != NULL)
	{
		Output_To_File(output_filename,reply_string);
	}
	/* free reply */
	free(reply_string);
	return 0;
}/* main */

/**
 * Output the string to the specified filename.
 * @param filename The output filename.
 * @param string The string to put into the file.
 * @return The routine returns TRUE on success and FALSE on failure.
 */
static int Output_To_File(char *filename,char *string)
{
	FILE *fp = NULL;
	int retval;

	if(filename == NULL)
	{
		fprintf(stderr,"send_command:Output filename was NULL.\n");
		return FALSE;
	}
	if(string == NULL)
	{
		fprintf(stderr,"send_command:Output string was NULL.\n");
		return FALSE;
	}
	fp = fopen(filename,"w");
	if(fp == NULL)
	{
		fprintf(stderr,"send_command:Failed to open output filename '%s'.\n",filename);
		return FALSE;
	}
	/* output string. Add newlinw as returned string usually doesn't have one */
	fprintf(fp,"%s\n",string);
	retval = fflush(fp);
	if(retval  != 0)
	{
		fprintf(stderr,"send_command:Failed to flush output filename '%s'.\n",filename);
		return FALSE;
	}
	retval = fclose(fp);
	if(retval  != 0)
	{
		fprintf(stderr,"send_command:Failed to close output filename '%s'.\n",filename);
		return FALSE;
	}
	return TRUE;
}

/**
 * Print out help message to stdout.
 */
static void help(void)
{
	printf("send_command help:\n");
	printf("send_command -h <hostname> -p <port number> -c \"<command string>\" [-f \"<output filename>\"]\n");
}
